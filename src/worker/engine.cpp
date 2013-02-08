#include "engine.hpp"

#include <iostream>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <utility>

#include "uv.h"

#include "scripting.hpp"
#include "types.hpp"
#include "utils.hpp"
#include "ray_queue.hpp"

/// How long to wait for more data before flushing the send buffer.
#define FR_FLUSH_TIMEOUT_MS 10

using std::string;
using std::flush;
using std::cout;
using std::endl;
using std::vector;
using std::pair;
using std::make_pair;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::normalize;
using glm::distance;
using glm::dot;

namespace fr {

/// The number of vertices on this worker.
uint64_t num_verts = 0;

/// The number of faces on this worker.
uint64_t num_faces = 0;

/// The size of the BVH data on this worker.
float bvh_size_mb;

/// The library for the current render.
static Library* lib = nullptr;

/// The ray queue for the current render.
static RayQueue* rayq = nullptr;

/// The libuv socket for the server we're running.
static uv_tcp_t host;

/// The server ID of us from everyone else's perspective.
static uint32_t me = 0;

/// The connected renderer who's running the show.
static NetNode* renderer = nullptr;

/// The timer for ensuring we flush the send buffer.
static uv_timer_t flush_timer;

/// The timer for sending stats during rendering.
static uv_timer_t stats_timer;

/// The number of other workers we're connected to.
static uint32_t num_workers_connected = 0;

/// The maximum number of jobs "in flight" at any given time.
static uint32_t max_jobs = 0;

/// The number of jobs currently available for running on libuv's thread pool.
static uint32_t active_jobs = 0;

/// Render stats for this worker.
static RenderStats stats;

/// Traversal stats for this worker.
static TraversalStats trav_stats;

// Callbacks, handlers, and helpers for server functionality.
namespace server {

void Init(const string& ip, uint16_t port);
void DispatchMessage(NetNode* node);
void ScheduleJob();
void ProcessRay(FatRay* ray, WorkResults* results);
void ProcessIntersect(FatRay* ray, WorkResults* results);
void ProcessIlluminate(FatRay* ray, WorkResults* results);
void ProcessLight(FatRay* ray, WorkResults* results);
void ForwardRay(FatRay* ray, WorkResults* results, uint32_t id);
void IlluminateIntersection(FatRay* ray, WorkResults* results);
void ShadeIntersection(FatRay* ray, WorkResults* results);
void IntersectWBVH(FatRay* ray, WorkResults* results, BVH* wbvh);
void IntersectLinear(FatRay* ray, WorkResults* results);
void LightWBVH(FatRay* ray, WorkResults* results, BVH* wbvh);
void LightLinear(FatRay* ray, WorkResults* results);

void OnConnection(uv_stream_t* stream, int status);
uv_buf_t OnAlloc(uv_handle_t* handle, size_t suggested_size);
void OnRead(uv_stream_t* stream, ssize_t nread, uv_buf_t buf);
void OnWork(uv_work_t* req);
void AfterWork(uv_work_t* req, int status);
void OnStatsTimeout(uv_timer_t* timer, int status);
void OnClose(uv_handle_t* handle);

void OnRay(NetNode* node);
void OnInit(NetNode* node);
void OnSyncConfig(NetNode* node);
void OnSyncMesh(NetNode* node);
void OnSyncMaterial(NetNode* node);
void OnSyncTexture(NetNode* node);
void OnSyncShader(NetNode* node);
void OnSyncCamera(NetNode* node);
void OnSyncEmissive(NetNode* node);
void OnBuildBVH(NetNode* node);
void OnSyncWBVH(NetNode* node);
void OnRenderStart(NetNode* node);
void OnRenderStop(NetNode* node);
void OnRenderPause(NetNode* node);
void OnRenderResume(NetNode* node);

} // namespace server

// Callbacks, handlers, and helpers for client functionality.
namespace client {

void Init();
void DispatchMessage(NetNode* node);

void OnConnect(uv_connect_t* req, int status);
uv_buf_t OnAlloc(uv_handle_t* handle, size_t suggested_size);
void OnRead(uv_stream_t* stream, ssize_t nread, uv_buf_t buf);
void OnClose(uv_handle_t* handle);

} // namespace client

void OnFlushTimeout(uv_timer_t* timer, int status);

void EngineInit(const string& ip, uint16_t port, uint32_t jobs) {
    int result = 0;

    max_jobs = jobs;

    // Randomize the world.
    srand(time(0));

    server::Init(ip, port);

    // Initialize the flush timeout timer.
    result = uv_timer_init(uv_default_loop(), &flush_timer);
    CheckUVResult(result, "timer_init");
    result = uv_timer_start(&flush_timer, OnFlushTimeout, FR_FLUSH_TIMEOUT_MS,
     FR_FLUSH_TIMEOUT_MS);
    CheckUVResult(result, "timer_start");

    // Initialize the stats timeout timer.
    result = uv_timer_init(uv_default_loop(), &stats_timer);
    CheckUVResult(result, "timer_init");
}

void EngineRun() {
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

void server::Init(const string& ip, uint16_t port) {
    int result = 0;
    struct sockaddr_in addr;

    // Initialize the listening TCP socket.
    result = uv_tcp_init(uv_default_loop(), &host);
    CheckUVResult(result, "tcp_init");

    // Attempt to bind to the desired port.
    addr = uv_ip4_addr(ip.c_str(), port);
    result = uv_tcp_bind(&host, addr);
    CheckUVResult(result, "bind");

    // Attempt to listen on the bound port.
    result = uv_listen(reinterpret_cast<uv_stream_t*>(&host), 128, OnConnection);
    CheckUVResult(result, "listen");
}

void server::OnConnection(uv_stream_t* stream, int status) {
    assert(reinterpret_cast<uv_tcp_t*>(stream) == &host);
    assert(status == 0);

    int result = 0;

    // Create a net node to track the machine that just connected.
    NetNode* node = new NetNode(DispatchMessage, &stats);
    result = uv_tcp_init(uv_default_loop(), &node->socket);
    CheckUVResult(result, "tcp_init");

    // Squirrel away the net node in the data baton.
    node->socket.data = node;

    // Accept the connection.
    result = uv_accept(reinterpret_cast<uv_stream_t*>(&host),
     reinterpret_cast<uv_stream_t*>(&node->socket));
    CheckUVResult(result, "accept");

    // Ensure the client doesn't die due to inactivity on the socket.
    result = uv_tcp_keepalive(&node->socket, 1, 60);
    CheckUVResult(result, "tcp_keepalive");

    // Who connected?
    struct sockaddr_in addr;
    int addr_len = sizeof(addr);
    result = uv_tcp_getpeername(&node->socket,
     reinterpret_cast<struct sockaddr*>(&addr), &addr_len);
    CheckUVResult(result, "getpeername");

    char ip[32];
    result = uv_ip4_name(&addr, ip, sizeof(ip));
    CheckUVResult(result, "ip4_name");
    node->ip = string(ip);
    node->port = ntohs(addr.sin_port);

    TOUTLN("[" << node->ip << "] Connected on port " << node->port << ".");

    // Start reading from the client.
    result = uv_read_start(reinterpret_cast<uv_stream_t*>(&node->socket),
     OnAlloc, OnRead);
    CheckUVResult(result, "read_start");
}

uv_buf_t server::OnAlloc(uv_handle_t* handle, size_t suggested_size) {
    assert(handle != nullptr);
    assert(handle->data != nullptr);

    // Just allocate a buffer of the suggested size.
    uv_buf_t buf;
    buf.base = reinterpret_cast<char*>(malloc(suggested_size));
    buf.len = suggested_size;

    return buf;
}

void server::OnRead(uv_stream_t* stream, ssize_t nread, uv_buf_t buf) {
    assert(stream != nullptr);
    assert(stream->data != nullptr);

    // Pull the net node out of the data baton.
    NetNode* node = reinterpret_cast<NetNode*>(stream->data);

    if (nread < 0) {
        // No data was read.
        uv_err_t err = uv_last_error(uv_default_loop());
        if (err.code == UV_EOF) {
            uv_close(reinterpret_cast<uv_handle_t*>(&node->socket), OnClose);
        } else {
            TERRLN("read: " << uv_strerror(err));
        }
    } else if (nread > 0) {
        // Data is available, parse any new messages out.
        node->Receive(buf.base, nread);

        stats.bytes_rx += nread;
    }

    if (buf.base) {
        free(buf.base);
    }
}

void server::OnClose(uv_handle_t* handle) {
    assert(handle != nullptr);
    assert(handle->data != nullptr);

    // Pull the net node out of the data baton.
    NetNode* node = reinterpret_cast<NetNode*>(handle->data);

    TOUTLN("[" << node->ip << "] Disconnected.");

    // Net nodes in the library will be deleted when the library is deleted.
    if (node == renderer) {
        delete node;
        renderer = nullptr;

        // Disconnect all clients.
        lib->ForEachNetNode([](uint32_t id, NetNode* node) {
            uv_close(reinterpret_cast<uv_handle_t*>(&node->socket), client::OnClose);
        });
    }
}

void server::DispatchMessage(NetNode* node) {
    switch (node->message.kind) {
        case Message::Kind::RAY:
            OnRay(node);
            break;

        case Message::Kind::INIT:
            OnInit(node);
            break;

        case Message::Kind::SYNC_CONFIG:
            OnSyncConfig(node);
            break;

        case Message::Kind::SYNC_MESH:
            OnSyncMesh(node);
            break;

        case Message::Kind::SYNC_MATERIAL:
            OnSyncMaterial(node);
            break;

        case Message::Kind::SYNC_TEXTURE:
            OnSyncTexture(node);
            break;

        case Message::Kind::SYNC_SHADER:
            OnSyncShader(node);
            break;

        case Message::Kind::SYNC_CAMERA:
            OnSyncCamera(node);
            break;

        case Message::Kind::SYNC_EMISSIVE:
            OnSyncEmissive(node);
            break;

        case Message::Kind::BUILD_BVH:
            OnBuildBVH(node);
            break;

        case Message::Kind::SYNC_WBVH:
            OnSyncWBVH(node);
            break;

        case Message::Kind::RENDER_START:
            OnRenderStart(node);
            break;

        case Message::Kind::RENDER_STOP:
            OnRenderStop(node);
            break;

        case Message::Kind::RENDER_PAUSE:
            OnRenderPause(node);
            break;

        case Message::Kind::RENDER_RESUME:
            OnRenderResume(node);
            break;

        default:
            TERRLN("Received unexpected message.");
            TERRLN(ToString(node->message));
            break;
    }
}

void server::ScheduleJob() {
    assert(rayq != nullptr);

    int result = 0;

    // Don't schedule anything if we're maxed out.
    if (active_jobs >= max_jobs) {
        return;
    }

    // Attempt to queue some work.
    FatRay* ray = rayq->Pop();
    if (ray != nullptr) {
        uv_work_t* req = reinterpret_cast<uv_work_t*>(malloc(sizeof(uv_work_t)));
        req->data = ray;
        result = uv_queue_work(uv_default_loop(), req, OnWork, AfterWork);
        CheckUVResult(result, "queue_work");
        active_jobs++;
    }
}

void server::ProcessRay(FatRay* ray, WorkResults* results) {
    // !!! WARNING !!!
    // Everything this function does and calls must be thread-safe. This
    // function will NOT run in the main thread, it runs on the thread pool.
    switch (ray->kind) {
        case FatRay::Kind::INTERSECT:
            ProcessIntersect(ray, results);
            break;

        case FatRay::Kind::ILLUMINATE:
            ProcessIlluminate(ray, results);
            break;

        case FatRay::Kind::LIGHT:
            ProcessLight(ray, results);
            break;

        default:
            TERRLN("Unknown ray kind " << ray->kind << ".");
            break;
    }
}

void server::IntersectWBVH(FatRay* ray, WorkResults* results, BVH* wbvh) {
    // !!! WARNING !!!
    // Everything this function does and calls must be thread-safe. This
    // function will NOT run in the main thread, it runs on the thread pool.
    
    // Common continuation for suspending traversal upon intersection.
    auto suspender = [ray](uint32_t worker_index, const SlimRay& r,
     HitRecord* hit, bool* request_suspend) {
        // Save the current worker and request suspension.
        ray->current_worker = worker_index;
        *request_suspend = true;
        return false; // Didn't hit anything... yet.
    };

    // Is this a suspended ray?
    if (ray->traversal.state != TraversalState::State::NONE) {
        // Yes, it is. Is the traversal complete?
        if (ray->traversal.current == 0) {
            // Yes it is. Illuminate the nearest intersection and kill the ray.
            IlluminateIntersection(ray, results);
            results->workers_touched[ray->workers_touched]++;
            delete ray;
            results->intersects_killed++;
            return;
        } else {
            // No it's not. Test local geometry and resume traversal.
            ray->traversal.hit = lib->Intersect(ray, me);
            ray->traversal = wbvh->Traverse(ray->traversal, ray->slim, &ray->hit, suspender);
        }
    } else {
        // No, it's not. Kick off the initial traversal.
        ray->traversal = wbvh->Traverse(ray->slim, &ray->hit, suspender);
    }

    // Did the traversal complete?
    if (ray->traversal.current == 0) {
        // Yes it did. Did it hit anything?
        if (ray->hit.worker > 0) {
            // Yes it did. Forward this ray to the winning worker.
            ForwardRay(ray, results, ray->hit.worker);
        } else {
            // No it did not. Kill the ray.
            results->workers_touched[ray->workers_touched]++;
            delete ray;
            results->intersects_killed++;
        }
    } else {
        // No it did not. Forward this ray to the current worker.
        ForwardRay(ray, results, ray->current_worker);
    }
}

void server::IntersectLinear(FatRay* ray, WorkResults* results) {
    // !!! WARNING !!!
    // Everything this function does and calls must be thread-safe. This
    // function will NOT run in the main thread, it runs on the thread pool.

    Config* config = lib->LookupConfig();

    // Our turn to check for a hit?
    if (ray->current_worker == me) {
        lib->Intersect(ray, me);
    }

    // Move the ray to the next worker.
    ray->current_worker++;

    // Have we checked every worker?
    if (ray->current_worker > config->workers.size()) {
        // Yes, are we the nearest hit?
        if (ray->hit.worker == me) {
            // Yes, let's illuminate the intersection and kill the ray.
            IlluminateIntersection(ray, results);
            results->workers_touched[ray->workers_touched]++;
            delete ray;
            results->intersects_killed++;
        } else if (ray->hit.worker != 0) {
            // No, forward the ray to the hit worker.
            ForwardRay(ray, results, ray->hit.worker);
        } else {
            // There was no hit. Kill the ray.
            results->workers_touched[ray->workers_touched]++;
            delete ray;
            results->intersects_killed++;
        }
    } else {
        // Forward it on.
        ForwardRay(ray, results, ray->current_worker);
    }
}

void server::ProcessIntersect(FatRay* ray, WorkResults* results) {
    // !!! WARNING !!!
    // Everything this function does and calls must be thread-safe. This
    // function will NOT run in the main thread, it runs on the thread pool.

    BVH* wbvh = lib->LookupWBVH();
    if (wbvh != nullptr) {
        // Using worker BVH for network traversal.
        IntersectWBVH(ray, results, wbvh);
    } else {
        // Using linear scan for network traversal.
        IntersectLinear(ray, results);
    }
}

void server::ProcessIlluminate(FatRay* ray, WorkResults* results) {
    // !!! WARNING !!!
    // Everything this function does and calls must be thread-safe. This
    // function will NOT run in the main thread, it runs on the thread pool.

    Config* config = lib->LookupConfig();

    lib->ForEachEmissiveMesh([ray, results, config](uint32_t id, Mesh* mesh) {
        // The target we're trying to hit is the original intersection.
        vec3 target = ray->EvaluateAt(ray->hit.t);

        // Find the shader.
        Material* mat = lib->LookupMaterial(mesh->material);
        assert(mat != nullptr);

        Shader* shader = lib->LookupShader(mat->shader);
        assert(shader != nullptr);
        assert(shader->script != nullptr);

        for (const auto& tri : mesh->faces) {
            for (uint16_t i = 0; i < config->samples; i++) {
                // Sample the triangle.
                vec3 position, normal;
                vec2 texcoord;
                tri.Sample(mesh->vertices, &position, &normal, &texcoord);

                // Transform the position and normal into world space.
                position = vec3(mesh->xform * vec4(position, 1.0f));
                normal = vec3(mesh->xform_inv_tr * vec4(normal, 0.0f));

                // The direction is toward the target.
                vec3 direction = normalize(target - position);

                // Lights only emit in the hemisphere that their normal defines.
                if (dot(normal, direction) < 0) {
                    continue;
                }

                // Create a new light ray that inherits the source <x, y> pixel.
                FatRay* light = new FatRay(FatRay::Kind::LIGHT, ray->x, ray->y);
                results->lights_produced++;

                // The origin is at the sample position.
                light->slim.origin = position;

                light->slim.direction = direction;
                light->target = target;

                // Run the shader's emissive() function.
                light->emission = shader->script->Emissive(texcoord);

                // Scale the transmittance by the number of samples.
                light->transmittance = ray->transmittance / config->samples;

                // Send it on it's way!
                ProcessLight(light, results);
            }
        }
    });

    // Kill the ray.
    delete ray;
    results->illuminates_killed++;
}

void server::LightWBVH(FatRay* ray, WorkResults* results, BVH* wbvh) {
    // !!! WARNING !!!
    // Everything this function does and calls must be thread-safe. This
    // function will NOT run in the main thread, it runs on the thread pool.

    // Common continuation for suspending traversal upon intersection.
    auto suspender = [ray](uint32_t worker_index, const SlimRay& r,
     HitRecord* hit, bool* request_suspend) {
        // Save the current worker and request suspension.
        ray->current_worker = worker_index;
        *request_suspend = true;
        return false; // Didn't hit anything... yet.
    };

    // Is this a suspended ray?
    if (ray->traversal.state != TraversalState::State::NONE) {
        // Yes, it is. Is the traversal complete?
        if (ray->traversal.current == 0) {
            // Yes it is. Shade the nearest intersection and kill the ray.
            ShadeIntersection(ray, results);
            results->workers_touched[ray->workers_touched]++;
            delete ray;
            results->lights_killed++;
            return;
        } else {
            // No it's not. Test local geometry and resume traversal.
            ray->traversal.hit = lib->Intersect(ray, me);
            ray->traversal = wbvh->Traverse(ray->traversal, ray->slim, &ray->hit, suspender);
        }
    } else {
        // No, it's not. Kick off the initial traversal.
        ray->traversal = wbvh->Traverse(ray->slim, &ray->hit, suspender);
    }

    // Did the traversal complete?
    if (ray->traversal.current == 0) {
        // Yes it did. Did it hit anything?
        if (ray->hit.worker > 0) {
            // Yes it did. Forward this ray to the winning worker.
            ForwardRay(ray, results, ray->hit.worker);
        } else {
            // No it did not. Kill the ray.
            results->workers_touched[ray->workers_touched]++;
            delete ray;
            results->lights_killed++;
        }
    } else {
        // No it did not. Forward this ray to the current worker.
        ForwardRay(ray, results, ray->current_worker);
    }
}

void server::LightLinear(FatRay* ray, WorkResults* results) {
    // !!! WARNING !!!
    // Everything this function does and calls must be thread-safe. This
    // function will NOT run in the main thread, it runs on the thread pool.

    Config* config = lib->LookupConfig();

    // Our turn to check for a hit?
    if (ray->current_worker == me) {
        lib->Intersect(ray, me);
    }

    // Move the ray to the next worker.
    ray->current_worker++;

    // Have we checked every worker?
    if (ray->current_worker > config->workers.size()) {
        // Yes, are we the nearest hit?
        if (ray->hit.worker == me) {
            // Yes, shade the intersection and kill the ray.
            ShadeIntersection(ray, results);
            results->workers_touched[ray->workers_touched]++;
            delete ray;
            results->lights_killed++;
        } else if (ray->hit.worker != 0) {
            // No, forward the ray to the hit worker.
            ForwardRay(ray, results, ray->hit.worker);
        } else {
            // There was no hit. Kill the ray.
            results->workers_touched[ray->workers_touched]++;
            delete ray;
            results->lights_killed++;
        }
    } else {
        // Forward it on.
        ForwardRay(ray, results, ray->current_worker);
    }
}

void server::ProcessLight(FatRay* ray, WorkResults* results) {
    // !!! WARNING !!!
    // Everything this function does and calls must be thread-safe. This
    // function will NOT run in the main thread, it runs on the thread pool.

    BVH* wbvh = lib->LookupWBVH();
    if (wbvh != nullptr) {
        // Using worker BVH for network traversal.
        LightWBVH(ray, results, wbvh);
    } else {
        // Using linear scan for network traversal.
        LightLinear(ray, results);
    }
}

void server::ForwardRay(FatRay* ray, WorkResults* results, uint32_t id) {
    // !!! WARNING !!!
    // Everything this function does and calls must be thread-safe. This
    // function will NOT run in the main thread, it runs on the thread pool.

    if (id == me) {
        // Push the ray back through the processing stack to save thread and
        // network overhead.
        ProcessRay(ray, results);
    } else {
        // Register the forwarding request.
        NetNode* node = lib->LookupNetNode(id);
        results->forwards.emplace_back(ray, node);
    }
}

void server::IlluminateIntersection(FatRay* ray, WorkResults* results) {
    // !!! WARNING !!!
    // Everything this function does and calls must be thread-safe. This
    // function will NOT run in the main thread, it runs on the thread pool.

    // Where did we hit?
    vec3 hit = ray->EvaluateAt(ray->hit.t);

    // Find the shader and run the indirect() function.
    Mesh* mesh = lib->LookupMesh(ray->hit.mesh);
    assert(mesh != nullptr);

    Material* mat = lib->LookupMaterial(mesh->material);
    assert(mat != nullptr);

    Shader* shader = lib->LookupShader(mat->shader);
    assert(shader != nullptr);
    assert(shader->script != nullptr);

    shader->script->Indirect(ray, hit, results);

    // Create ILLUMINATE rays and send them to each emissive node.
    LightList* lights = lib->LookupLightList();
    lights->ForEachEmissiveWorker([ray, results](uint32_t id) {
        FatRay* illum = new FatRay(*ray);
        illum->kind = FatRay::Kind::ILLUMINATE;
        results->illuminates_produced++;
        ForwardRay(illum, results, id);
    });
}

void server::ShadeIntersection(FatRay* ray, WorkResults* results) {
    // !!! WARNING !!!
    // Everything this function does and calls must be thread-safe. This
    // function will NOT run in the main thread, it runs on the thread pool.

    // Did it hit close enough to the target?
    vec3 hit = ray->EvaluateAt(ray->hit.t);
    if (distance(hit, ray->target) > TARGET_INTERSECT_EPSILON) {
        // Nope, ignore.
        return;
    }

    // Find the shader and run the direct() function.
    Mesh* mesh = lib->LookupMesh(ray->hit.mesh);
    assert(mesh != nullptr);

    Material* mat = lib->LookupMaterial(mesh->material);
    assert(mat != nullptr);

    Shader* shader = lib->LookupShader(mat->shader);
    assert(shader != nullptr);
    assert(shader->script != nullptr);

    shader->script->Direct(ray, hit, results);
}

void server::OnWork(uv_work_t* req) {
    // !!! WARNING !!!
    // Everything this function does and calls must be thread-safe. This
    // function will NOT run in the main thread, it runs on the thread pool.

    // Pull the ray out of the data baton.
    FatRay *ray = reinterpret_cast<FatRay*>(req->data);

    // Allocate results of this work.
    WorkResults* results = new WorkResults;

    // Dispatch the ray.
    ProcessRay(ray, results);

    // Pass the work results back through the data baton.
    req->data = results;
}

void server::AfterWork(uv_work_t* req, int status) {
    assert(req != nullptr);
    assert(req->data != nullptr);

    // Pull the work result out of the data baton.
    WorkResults* results = reinterpret_cast<WorkResults*>(req->data);

    // Do buffer operations.
    Image* image = lib->LookupImage();
    assert(image != nullptr);
    for (auto& op : results->ops) {
        switch (op.kind) {
            case BufferOp::Kind::WRITE:
                image->Write(op.buffer, op.x, op.y, op.value);
                break;

            case BufferOp::Kind::ACCUMULATE:
                image->Accumulate(op.buffer, op.x, op.y, op.value);
                break;
        }
    }
    
    // Forward rays and kill them off.
    for (auto& forward : results->forwards) {
        if (forward.node == nullptr) {
            // We didn't know where to send it, so queue it for processing.
            rayq->Push(forward.ray);
        } else {
            // Send it and kill the local copy.
            forward.ray->workers_touched++;
            forward.node->SendRay(forward.ray);
            delete forward.ray;
        }
    }

    // Merge render stats.
    stats.intersects_produced += results->intersects_produced;
    stats.illuminates_produced += results->illuminates_produced;
    stats.lights_produced += results->lights_produced;
    stats.intersects_killed += results->intersects_killed;
    stats.illuminates_killed += results->illuminates_killed;
    stats.lights_killed += results->lights_killed;
    for (const auto& kv : results->workers_touched) {
        trav_stats.workers_touched[kv.first] += kv.second;
    }

    delete results;
    free(req);

    // This job is done. Schedule more work.
    active_jobs--;
    ScheduleJob();
}

void server::OnRay(NetNode* node) {
    assert(node != nullptr);
    assert(rayq != nullptr);
    
    // Unpack the ray and push it into the queue.
    rayq->Push(node->ReceiveRay());

    stats.rays_rx++;

    // Try to schedule a job.
    ScheduleJob();
}

void server::OnInit(NetNode* node) {
    assert(node->message.size == sizeof(uint32_t));

    // Who am I?
    me = *(reinterpret_cast<uint32_t*>(node->message.body));
    TOUTLN("[" << node->ip << "] Joining the render as worker " << me << ".");

    // Create a fresh library.
    if (lib != nullptr) delete lib;
    lib = new Library;
    
    // Hang on to the renderer.
    renderer = node;

    // No workers connected yet.
    num_workers_connected = 0;

    // No active jobs yet.
    active_jobs = 0;

    // Reply with OK.
    Message reply(Message::Kind::OK);
    node->Send(reply);
}

void server::OnSyncConfig(NetNode* node) {
    assert(node != nullptr);
    assert(lib != nullptr);

    // Unpack the config.
    node->ReceiveConfig(lib);

    TOUTLN("[" << node->ip << "] Received configuration.");

    Config* config = lib->LookupConfig();
    assert(config != nullptr);

    // Create the image with all the requested buffers.
    Image* image = new Image(config->width, config->height);
    for (const auto& buffer : config->buffers) {
        image->AddBuffer(buffer);
    }
    lib->StoreImage(image);

    // Connect to all the other workers.
    client::Init();
}

void server::OnSyncMesh(NetNode* node) {
    assert(node != nullptr);
    assert(lib != nullptr);

    // Unpack the mesh.
    uint32_t id = node->ReceiveMesh(lib);

    Mesh* mesh = lib->LookupMesh(id);
    assert(mesh != nullptr);
    num_verts += mesh->vertices.size();
    num_faces += mesh->faces.size();

    // Reply with OK.
    Message reply(Message::Kind::OK);
    node->Send(reply);

    TOUTLN("[" << node->ip << "] Received mesh " << id << ".");
}

void server::OnSyncMaterial(NetNode* node) {
    assert(node != nullptr);
    assert(lib != nullptr);

    // Unpack the material.
    uint32_t id = node->ReceiveMaterial(lib);

    TOUTLN("[" << node->ip << "] Received material " << id << ".");
}

void server::OnSyncTexture(NetNode* node) {
    assert(node != nullptr);
    assert(lib != nullptr);

    // Unpack the texture.
    uint32_t id = node->ReceiveTexture(lib);

    // Prepare the texture for execution (if it's procedural).
    Texture* tex = lib->LookupTexture(id);
    if (tex->kind == Texture::Kind::PROCEDURAL) {
        tex->script = new TextureScript(tex->code);
    }

    TOUTLN("[" << node->ip << "] Received texture " << id << ".");
}

void server::OnSyncShader(NetNode* node) {
    assert(node != nullptr);
    assert(lib != nullptr);

    // Unpack the shader.
    uint32_t id = node->ReceiveShader(lib);

    // Prepare the shader for execution.
    Shader* shader = lib->LookupShader(id);
    shader->script = new ShaderScript(shader->code, lib);

    TOUTLN("[" << node->ip << "] Received shader " << id << ".");
}

void server::OnSyncCamera(NetNode* node) {
    assert(node != nullptr);
    assert(lib != nullptr);

    // Unpack the camera.
    node->ReceiveCamera(lib);

    // Create a fresh ray queue.
    if (rayq != nullptr) delete rayq;
    rayq = new RayQueue(lib->LookupCamera(), &stats);

    // Reply with OK.
    Message reply(Message::Kind::OK);
    node->Send(reply);

    TOUTLN("[" << node->ip << "] Received camera.");
}

void server::OnSyncEmissive(NetNode* node) {
    assert(node != nullptr);
    assert(lib != nullptr);

    // Unpack the light list.
    node->ReceiveLightList(lib);

    // Reply with OK.
    Message reply(Message::Kind::OK);
    node->Send(reply);

    TOUTLN("[" << node->ip << "] Received list of emissive workers.");
}

void server::OnBuildBVH(NetNode* node) {
    assert(node != nullptr);

    vector<pair<uint32_t, BoundingBox>> mesh_bounds;

    TOUT("Building local BVH" << flush);
    lib->ForEachMesh([&mesh_bounds](uint32_t id, Mesh* mesh) {
        mesh->bvh = new BVH(mesh);
        bvh_size_mb += mesh->bvh->GetSizeInMB();
        mesh_bounds.emplace_back(make_pair(id, mesh->bvh->Extents()));
        cout << "." << flush;
    });

    // Build the mesh BVH from the mesh extents.
    BVH* mbvh = new BVH(mesh_bounds);
    bvh_size_mb += mbvh->GetSizeInMB();
    lib->StoreMBVH(mbvh);
    cout << "." << endl;

    // This worker's bounding box is the mesh BVH extents.
    BoundingBox worker_bounds = mbvh->Extents();

    // Reply with OK and worker bounds.
    Message reply(Message::Kind::OK);
    reply.size = sizeof(BoundingBox);
    reply.body = &worker_bounds;
    node->Send(reply);

    TOUTLN("Local BVH ready.");
}

void server::OnSyncWBVH(NetNode* node) {
    assert(node != nullptr);
    assert(lib != nullptr);

    // Unpack the worker BVH.
    node->ReceiveWBVH(lib);

    bvh_size_mb += lib->LookupWBVH()->GetSizeInMB();

    // Reply with OK.
    Message reply(Message::Kind::OK);
    node->Send(reply);

    TOUTLN("[" << node->ip << "] Received WBVH.");
}

void server::OnRenderStart(NetNode* node) {
    assert(node != nullptr);
    assert(lib != nullptr);
    assert(node->message.size == sizeof(uint32_t));

    int result = 0;

    // Unpack the offset and chunk size.
    uint32_t payload = *(reinterpret_cast<uint32_t*>(node->message.body));
    int16_t offset = (payload & 0xffff0000) >> 16;
    uint16_t chunk_size = payload & 0xffff;

    // Set the camera range.
    Camera* camera = lib->LookupCamera();
    assert(camera != nullptr);
    camera->SetRange(offset, chunk_size);

    TOUTLN("Starting render with range " << offset << " -> " << (offset + chunk_size - 1) << ".");

    // Start the stats timer.
    result = uv_timer_start(&stats_timer, OnStatsTimeout, FR_STATS_TIMEOUT_MS,
     FR_STATS_TIMEOUT_MS);
    CheckUVResult(result, "timer_start");

    // Queue up some jobs.
    for (uint32_t i = 0; i < max_jobs; i++) {
        ScheduleJob();
    }
}

void server::OnRenderStop(NetNode* node) {
    assert(node != nullptr);
    assert(lib != nullptr);

    int result = 0;

    // Stop the stats timer.
    result = uv_timer_stop(&stats_timer);
    CheckUVResult(result, "timer_stop");

    node->SendImage(lib);

    TOUTLN("[" << node->ip << "] Sending image to renderer.");

    TOUTLN("Traversal stats:");
    for (const auto& kv : trav_stats.workers_touched) {
        TOUTLN("\t" << kv.first << " worker(s): " << kv.second);
    }

    TOUTLN("Scene stats:");
    TOUTLN("\tNumber of vertices: " << num_verts);
    TOUTLN("\tNumber of faces: " << num_faces);
    TOUTLN("\tSize of geometry: " << ((num_verts * sizeof(Vertex) + num_faces * sizeof(Triangle)) / (1024.0f * 1024.0f)) << " MB");
    TOUTLN("\tBVH size: " << bvh_size_mb << " MB");
}

void server::OnRenderPause(NetNode* node) {
    assert(rayq != nullptr);

    rayq->Pause();
    TOUTLN("Pausing work generation.");
}

void server::OnRenderResume(NetNode* node) {
    assert(rayq != nullptr);

    rayq->Resume();
    TOUTLN("Resuming work generation.");

    // Reschedule new jobs if we're not at our max capacity.
    uint32_t num_jobs = max_jobs - active_jobs;
    for (uint32_t i = 0; i < num_jobs; i++) {
        ScheduleJob();
    }
}

void OnFlushTimeout(uv_timer_t* timer, int status) {
    assert(timer == &flush_timer);
    assert(status == 0);

    // Flush the server's connection to the renderer.
    if (renderer != nullptr) {
        if (!renderer->flushed && renderer->nwritten > 0) {
            renderer->Flush();
        }
        renderer->flushed = false;
    }

    // Flush all the client connections.
    if (lib != nullptr) {
        lib->ForEachNetNode([](uint32_t id, NetNode* node) {
            if (!node->flushed && node->nwritten > 0) {
                node->Flush();
            }
            node->flushed = false;
        });
    }

    // Note: This does NOT flush the server's connection to any of its
    // clients, except for the renderer. This is because when we want to write
    // back to one of the server's clients, we write back through our client
    // to that server. If you actually want to write from the server, you
    // need to flush that manually.
}

void server::OnStatsTimeout(uv_timer_t* timer, int status) {
    assert(timer == &stats_timer);
    assert(status == 0);
    assert(renderer != nullptr);
    assert(rayq != nullptr);

    stats.intersect_queue = rayq->IntersectSize();
    stats.illuminate_queue = rayq->IlluminateSize();
    stats.light_queue = rayq->LightSize();

    Camera* cam = lib->LookupCamera();
    assert(cam != nullptr);
    stats.primary_progress = cam->Progress();

    renderer->SendRenderStats(&stats);
    stats.Reset();
}

void client::Init() {
    int result = 0;
    struct sockaddr_in addr;

    Config* config = lib->LookupConfig();

    TOUTLN("Connecting to " << (config->workers.size() - 1) << " other workers...");

    for (size_t i = 0; i < config->workers.size(); i++) {
        // Don't connect to ourselves.
        if (i == me - 1) continue;

        const auto& worker = config->workers[i];

        NetNode* node = new NetNode(DispatchMessage, worker, &stats);

        // Initialize the TCP socket.
        result = uv_tcp_init(uv_default_loop(), &node->socket);
        CheckUVResult(result, "tcp_init");
        
        // Squirrel away the node in the data baton.
        node->socket.data = node;

        // Connect to the server.
        addr = uv_ip4_addr(node->ip.c_str(), node->port);
        uv_connect_t* req = reinterpret_cast<uv_connect_t*>(malloc(sizeof(uv_connect_t)));
        result = uv_tcp_connect(req, &node->socket, addr, OnConnect);
        CheckUVResult(result, "tcp_connect");

        // Add the node to the library.
        lib->StoreNetNode(i + 1, node); // +1 because 0 is reserved
    }
}

void client::DispatchMessage(NetNode* node) {
    // Nothing yet.
}

void client::OnConnect(uv_connect_t* req, int status) {
    assert(req != nullptr);
    assert(req->handle != nullptr);
    assert(req->handle->data != nullptr);

    int result = 0;

    // Pull the net node out of the data baton.
    NetNode* node = reinterpret_cast<NetNode*>(req->handle->data);
    free(req);

    if (status != 0) {
        TERRLN("Failed connecting to " << node->ip << ".");
        exit(EXIT_FAILURE);
    }

    TOUTLN("[" << node->ip << "] Connected on port " << node->port << ".");

    // Start reading replies from the server.
    result = uv_read_start(reinterpret_cast<uv_stream_t*>(&node->socket),
     OnAlloc, OnRead);
    CheckUVResult(result, "read_start");

    // Nothing else to do if we're still waiting for everyone to connect.
    num_workers_connected++;
    if (num_workers_connected < lib->LookupConfig()->workers.size() - 1) {
        return;
    }

    // Reply with OK to the renderer.
    Message reply(Message::Kind::OK);
    assert(renderer != nullptr);
    renderer->Send(reply);
}

uv_buf_t client::OnAlloc(uv_handle_t* handle, size_t suggested_size) {
    assert(handle != nullptr);
    assert(handle->data != nullptr);

    // Just allocate a buffer of the suggested size.
    uv_buf_t buf;
    buf.base = reinterpret_cast<char*>(malloc(suggested_size));
    buf.len = suggested_size;

    return buf;
}

void client::OnRead(uv_stream_t* stream, ssize_t nread, uv_buf_t buf) {
    assert(stream != nullptr);
    assert(stream->data != nullptr);

    // Pull the net node out of the data baton.
    NetNode* node = reinterpret_cast<NetNode*>(stream->data);

    if (nread < 0) {
        // No data was read.
        uv_err_t err = uv_last_error(uv_default_loop());
        if (err.code == UV_EOF) {
            uv_close(reinterpret_cast<uv_handle_t*>(&node->socket), OnClose);
        } else {
            TERRLN("read: " << uv_strerror(err));
        }
    } else if (nread > 0) {
        // Data is available, parse any new messages out.
        node->Receive(buf.base, nread);
    }

    if (buf.base) {
        free(buf.base);
    }
}

void client::OnClose(uv_handle_t* handle) {
    assert(handle != nullptr);
    assert(handle->data != nullptr);

    // Pull the net node out of the data baton.
    NetNode* node = reinterpret_cast<NetNode*>(handle->data);

    TOUTLN("[" << node->ip << "] Disconnected.");

    // Net node will be deleted with library.
}

} // namespace fr
