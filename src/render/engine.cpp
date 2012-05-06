#include "engine.hpp"

#include <cassert>
#include <sstream>
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>

#include "uv.h"

#include "scripting.hpp"
#include "types.hpp"
#include "utils.hpp"

/// How long to wait for more data before flushing the send buffer.
#define FR_FLUSH_TIMEOUT_MS 10

using std::string;
using std::stringstream;

namespace fr {

/// The library for the current render.
static Library* lib = nullptr;

/// The number of workers that we've connected to.
static size_t num_workers_connected = 0;

/// The number of workers that are syncing.
static size_t num_workers_syncing = 0;

/// The number of workers that are ready to render.
static size_t num_workers_ready = 0;

/// The number of workers that have sent and merged their images.
static size_t num_workers_complete = 0;

/// The timer for ensuring we flush the send buffer.
static uv_timer_t flush_timer;

/// Synchronization primitives for synchronizing the synchronization.
/// "We need to go deeper..."
static sem_t mesh_read;
static sem_t mesh_synced;

/// The ID of the mesh we're currently syncing over the network.
static uint64_t current_mesh_id = 0;

/// The scene file we're rendering.
static string scene;

// Callbacks, handlers, and helpers for client functionality.
namespace client {

void Init();
void DispatchMessage(NetNode* node);
void StartSync();
uint64_t SyncMesh(Mesh* mesh);
void StartRender();
void StopRender();

void OnConnect(uv_connect_t* req, int status);
uv_buf_t OnAlloc(uv_handle_t* handle, size_t suggested_size);
void OnRead(uv_stream_t* stream, ssize_t nread, uv_buf_t buf);
void OnClose(uv_handle_t* handle);
void OnFlushTimeout(uv_timer_t* timer, int status);
void OnSyncStart(uv_work_t* req);
void AfterSync(uv_work_t* req);
void OnSyncIdle(uv_idle_t* handle, int status);

void OnOK(NetNode* node);
void OnSyncImage(NetNode* node);

}

void EngineInit(const string& config_file, const string& scene_file) {
    lib = new Library;

    // Parse the config file.
    ConfigScript config_script;
    TOUTLN("Loading config from " << config_file << ".");
    if (!config_script.Parse(config_file, lib)) {
        TERRLN("Can't continue with bad config.");
        exit(EXIT_FAILURE);
    }
    TOUTLN("Config loaded.");

    scene = scene_file;

    client::Init();
}

void EngineRun() {
    uv_run(uv_default_loop());
}

void client::Init() {
    int result = 0;
    struct sockaddr_in addr;

    Config* config = lib->LookupConfig();

    TOUTLN("Connecting to " << config->workers.size() << " workers...");

    for (size_t i = 0; i < config->workers.size(); i++) {
        const auto& worker = config->workers[i];

        NetNode* node = new NetNode(DispatchMessage, worker);

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

    // Initialize the flush timeout timer.
    result = uv_timer_init(uv_default_loop(), &flush_timer);
    CheckUVResult(result, "timer_init");
    result = uv_timer_start(&flush_timer, OnFlushTimeout, FR_FLUSH_TIMEOUT_MS,
     FR_FLUSH_TIMEOUT_MS);
    CheckUVResult(result, "timer_start");
}

void client::DispatchMessage(NetNode* node) {
    switch (node->message.kind) {
        case Message::Kind::OK:
            OnOK(node);
            break;

        case Message::Kind::SYNC_IMAGE:
            OnSyncImage(node);
            break;

        default:
            TERRLN("Received unexpected message.");
            TERRLN(ToString(node->message));
            break;
    }
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
    if (num_workers_connected < lib->LookupConfig()->workers.size()) {
        return;
    }

    // Send init messages to each server.
    lib->ForEachNetNode([](uint64_t id, NetNode* node) {
        Message request(Message::Kind::INIT);
        request.size = sizeof(uint64_t);
        request.body = &id;
        node->state = NetNode::State::INITIALIZING;
        node->Send(request);
    });
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
    // TODO: halt rendering if it wasn't done
}

void client::OnFlushTimeout(uv_timer_t* timer, int status) {
    assert(timer == &flush_timer);
    assert(status == 0);

    lib->ForEachNetNode([](uint64_t id, NetNode* node) {
        if (!node->flushed && node->nwritten > 0) {
            node->Flush();
        }
        node->flushed = false;
    });
}

void client::OnOK(NetNode* node) {
    Config* config = lib->LookupConfig();
    assert(config != nullptr);

    switch (node->state) {
        case NetNode::State::INITIALIZING:
            node->state = NetNode::State::CONFIGURING;
            TOUTLN("[" << node->ip << "] Configuring worker.");
            node->SendConfig(lib);
            break;

        case NetNode::State::CONFIGURING:
            node->state = NetNode::State::SYNCING_ASSETS;
            TOUTLN("[" << node->ip << "] Ready to sync.");
            num_workers_syncing++;
            if (num_workers_syncing == config->workers.size()) {
                StartSync();
            }
            break;

        case NetNode::State::SYNCING_ASSETS:
            // Delete the current mesh from the library.
            lib->StoreMesh(current_mesh_id, nullptr);
            if (sem_post(&mesh_synced) < 0) {
                perror("sem_post");
                exit(EXIT_FAILURE);
            }
            break;

        case NetNode::State::SYNCING_CAMERA:
            node->state = NetNode::State::READY;
            TOUTLN("[" << node->ip << "] Ready to render.");
            num_workers_ready++;
            if (num_workers_ready == config->workers.size()) {
                StartRender();
            }
            break;

        default:
            TERRLN("Received OK in unexpected state.");
    }
}

void client::OnSyncImage(NetNode* node) {
    assert(node != nullptr);
    assert(lib != nullptr);

    int result = 0;

    Config* config = lib->LookupConfig();
    assert(config != nullptr);

    Image* final = lib->LookupImage();
    assert(final != nullptr);

    Image* component = node->ReceiveImage();
    assert(component != nullptr);

    // Write the component image out as name-worker.exr.
    stringstream component_file;
    component_file << config->name << "-" << node->ip << "_" << node->port <<
     ".exr";
    TOUTLN("Writing " << component_file.str() << "...");
    component->ToEXRFile(component_file.str());

    // Merge the component image with the final image.
    final->Merge(component);
    TOUTLN("[" << node->ip << "] Merged image.");

    // Done with the component image.
    delete component;

    // Done for now if this wasn't the last worker.
    num_workers_complete++;
    if (num_workers_complete < config->workers.size()) {
        return;
    }

    // Write out the final image.
    final->ToEXRFile(config->name + ".exr");
    TOUTLN("Wrote " << config->name << ".exr.");

    // Disconnect from each worker.
    lib->ForEachNetNode([config](uint64_t id, NetNode* node) {
        uv_close(reinterpret_cast<uv_handle_t*>(&node->socket), OnClose);
    });

    // Shutdown the flush timer.
    result = uv_timer_stop(&flush_timer);
    CheckUVResult(result, "timer_stop");
    uv_close(reinterpret_cast<uv_handle_t*>(&flush_timer), nullptr);
}

void client::StartSync() {
    int result = 0;

    // Build the spatial index.
    lib->BuildSpatialIndex();

    Config* config = lib->LookupConfig();
    assert(config != nullptr);

    // Create the image with all the requested buffers.
    Image* image = new Image(config->width, config->height);
    for (const auto& buffer : config->buffers) {
        image->AddBuffer(buffer);
    }
    lib->StoreImage(image);

    // Initialize the semaphores for ping-ponging between threads.
    if (sem_init(&mesh_read, 0, 0) < 0) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }
    if (sem_init(&mesh_synced, 0, 1) < 0) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    // Queue up the scene parsing to happen on the thread pool.
    uv_work_t* req = reinterpret_cast<uv_work_t*>(malloc(sizeof(uv_work_t)));
    result = uv_queue_work(uv_default_loop(), req, OnSyncStart, AfterSync);
    CheckUVResult(result, "queue_work");

    // Start up the idle callback for handling networking.
    uv_idle_t* idler = reinterpret_cast<uv_idle_t*>(malloc(sizeof(uv_idle_t)));
    result = uv_idle_init(uv_default_loop(), idler);
    CheckUVResult(result, "idle_init");
    result = uv_idle_start(idler, OnSyncIdle);
    CheckUVResult(result, "idle_start");
}

void client::StartRender() {
    Config* config = lib->LookupConfig();

    // Send render start messages to each server.
    lib->ForEachNetNode([config](uint64_t id, NetNode* node) {
        uint16_t chunk_size = config->width / config->workers.size();
        int16_t offset = (id - 1) * chunk_size;
        if (id == config->workers.size()) {
            chunk_size = config->width - (id - 1) * chunk_size;
        }
        uint32_t payload = (offset << 16) | chunk_size;

        Message request(Message::Kind::RENDER_START);
        request.size = sizeof(uint32_t);
        request.body = &payload;
        node->Send(request);

        node->state = NetNode::State::RENDERING;
        TOUTLN("[" << node->ip << "] Starting render.");
    });

    TOUTLN("Rendering has started.");

    // TODO: remove this, just testing
    StopRender();
}

void client::StopRender() {
    // Send render stop messages to each server.
    lib->ForEachNetNode([](uint64_t id, NetNode* node) {
        Message request(Message::Kind::RENDER_STOP);
        node->Send(request);
        node->state = NetNode::State::SYNCING_IMAGES;
        TOUTLN("[" << node->ip << "] Stopping render.");
    });

    TOUTLN("Rendering has stopped, syncing images.");
}

void client::OnSyncStart(uv_work_t* req) {
    // !!! WARNING !!!
    // Everything this function does and calls must be thread-safe. This
    // function will NOT run in the main thread, it runs on the thread pool.

    assert(req != nullptr);

    // Parse and distribute the scene.
    SceneScript scene_script(SyncMesh);
    TOUTLN("Loading scene from " << scene << ".");
    if (!scene_script.Parse(scene, lib)) {
        TERRLN("Can't continue with bad scene.");
        exit(EXIT_FAILURE);
    }

    // Signal that we're finished.
    SyncMesh(nullptr);
    TOUTLN("Scene distributed.");
}

void client::AfterSync(uv_work_t* req) {
    assert(req != nullptr);
    free(req);
}

uint64_t client::SyncMesh(Mesh* mesh) {
    // !!! WARNING !!!
    // Everything this function does and calls must be thread-safe. This
    // function will NOT run in the main thread, it runs on the thread pool.

    // Wait for the main thread to be finished with the network.
    if (sem_wait(&mesh_synced) < 0) {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }

    uint64_t id = 0;
    if (mesh != nullptr) {
        // Store the mesh in the library and get back its ID.
        id = lib->NextMeshID();
        mesh->id = id;
        lib->StoreMesh(id, mesh);
    }

    // Tell the main thread which mesh we'd like to sync over the network.
    current_mesh_id = id;

    // Wake up the main thread to do the networking.
    if (sem_post(&mesh_read) < 0) {
        perror("sem_post");
        exit(EXIT_FAILURE);
    }

    return id;
}

void client::OnSyncIdle(uv_idle_t* handle, int status) {
    assert(handle != nullptr);
    assert(status == 0);

    int result = 0;

    // Are we done reading in a new mesh?
    if (sem_trywait(&mesh_read) < 0) {
        if (errno == EAGAIN) {
            // Nope.
            return;
        }
        perror("sem_trywait");
        exit(EXIT_FAILURE);
    }

    // Are we done syncing assets?
    if (current_mesh_id == 0) {
        // Shut off the idle handler.
        result = uv_idle_stop(handle);
        CheckUVResult(result, "idle_stop");
        uv_close(reinterpret_cast<uv_handle_t*>(handle), nullptr);

        // Sync the camera with everyone.
        lib->ForEachNetNode([](uint64_t id, NetNode* node) {
            node->state = NetNode::State::SYNCING_CAMERA;
            TOUTLN("[" << node->ip << "] Syncing camera.");
            node->SendCamera(lib);
        });
        return;
    }

    Mesh* mesh = lib->LookupMesh(current_mesh_id);
    assert(mesh != nullptr);
    Config* config = lib->LookupConfig();
    assert(config != nullptr);

    uint64_t spacecode = SpaceEncode(mesh->centroid, config->min, config->max);
    uint64_t id = lib->LookupNetNodeBySpaceCode(spacecode);
    NetNode* node = lib->LookupNetNode(id);

    TOUTLN("[" << node->ip << "] Sending mesh " << current_mesh_id << " to worker " << id << ".");

    node->SendMesh(lib, current_mesh_id);
}

} // namespace fr
