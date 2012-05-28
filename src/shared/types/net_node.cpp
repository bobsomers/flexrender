#include "types/net_node.hpp"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <fstream>

#include "types.hpp"
#include "utils/library.hpp"
#include "utils/network.hpp"

using std::stringstream;
using std::string;
using std::unordered_map;
using std::ofstream;
using std::endl;

namespace fr {

NetNode::NetNode(DispatchCallback dispatcher, const string& address,
 RenderStats* stats) :
 me(0),
 state(State::NONE),
 mode(ReadMode::HEADER),
 message(),
 nread(0),
 nwritten(0),
 flushed(false),
 _dispatcher(dispatcher),
 _materials(),
 _textures(),
 _shaders(),
 _stats_log(),
 _current_stats(stats),
 _num_uninteresting(0),
 _last_progress(0.0f) {
    size_t pos = address.find(':');
    if (pos == string::npos) {
        ip = address;
        port = 19400;
    } else {
        ip = address.substr(0, pos);
        stringstream stream(address.substr(pos + 1));
        stream >> port;
    }
}

NetNode::NetNode(DispatchCallback dispatcher, RenderStats* stats) :
 me(0),
 state(State::NONE),
 ip(""),
 port(0),
 mode(ReadMode::HEADER),
 message(),
 nread(0),
 nwritten(0),
 flushed(false),
 _dispatcher(dispatcher),
 _materials(),
 _textures(),
 _shaders(),
 _stats_log(),
 _current_stats(stats),
 _num_uninteresting(0),
 _last_progress(0.0f) {}

void NetNode::Receive(const char* buf, ssize_t len) {
    if (buf == nullptr || len <= 0) return;

    size_t header_size = sizeof(message.kind) + sizeof(message.size);

    const char* from = buf;
    ssize_t remaining = len;

    while (true) {
        if (mode == ReadMode::HEADER) {
            char* to = reinterpret_cast<char*>(
             reinterpret_cast<uintptr_t>(&message) +
             static_cast<uintptr_t>(nread));

            ssize_t bytes_to_go = header_size - nread;

            if (bytes_to_go <= remaining) {
                memcpy(to, from, bytes_to_go);

                remaining -= bytes_to_go;

                from = reinterpret_cast<const char*>(
                 reinterpret_cast<uintptr_t>(from) +
                 static_cast<uintptr_t>(bytes_to_go));

                nread = 0;
                if (message.size > 0) {
                    message.body = malloc(message.size);
                } else {
                    message.body = nullptr;
                    message.size = 0;
                }

                mode = ReadMode::BODY;
                continue;
            } else {
                memcpy(to, from, remaining);
                nread += remaining;
                break;
            }
        } else {
            char* to = reinterpret_cast<char*>(
             reinterpret_cast<uintptr_t>(message.body) +
             static_cast<uintptr_t>(nread));

            ssize_t bytes_to_go = message.size - nread;

            if (bytes_to_go <= remaining) {
                memcpy(to, from, bytes_to_go);

                remaining -= bytes_to_go;

                from = reinterpret_cast<const char*>(
                 reinterpret_cast<uintptr_t>(from) +
                 static_cast<uintptr_t>(bytes_to_go));

                nread = 0;
                _dispatcher(this);

                free(message.body);
                mode = ReadMode::HEADER;
                continue;
            } else {
                memcpy(to, from, remaining);
                nread += remaining;
                break;
            }
        }
    }
}

void NetNode::Send(const Message& msg) {
    ssize_t bytes_sent = 0;
    ssize_t space_left = 0;

    size_t header_size = sizeof(msg.kind) + sizeof(msg.size);

    ssize_t bytes_remaining = header_size;
    if (nwritten + bytes_remaining > FR_WRITE_BUFFER_SIZE) {
        Flush();
    }

    char* to = reinterpret_cast<char*>(
     reinterpret_cast<uintptr_t>(&buffer) +
     static_cast<uintptr_t>(nwritten));

    const char* from = reinterpret_cast<const char*>(&msg);

    memcpy(to, from, header_size);
    nwritten += header_size;

    while (true) {
        bytes_remaining = msg.size - bytes_sent;
        space_left = FR_WRITE_BUFFER_SIZE - nwritten;

        to = reinterpret_cast<char*>(
         reinterpret_cast<uintptr_t>(&buffer) +
         static_cast<uintptr_t>(nwritten));

        from = reinterpret_cast<const char*>(
         reinterpret_cast<uintptr_t>(msg.body) +
         static_cast<uintptr_t>(bytes_sent));

        if (bytes_remaining <= space_left) {
            memcpy(to, from, bytes_remaining);
            nwritten += bytes_remaining;
            bytes_sent += bytes_remaining;
            break;
        }

        if (space_left > 0) {
            memcpy(to, from, space_left);
            nwritten += space_left;
            bytes_sent += space_left;
        }

        Flush();
    }

}

void NetNode::Flush() {
    int result = 0;

    if (nwritten <= 0) return;

    uv_write_t* req = reinterpret_cast<uv_write_t*>(malloc(sizeof(uv_write_t)));
    req->data = malloc(FR_WRITE_BUFFER_SIZE);
    memcpy(req->data, &buffer, nwritten);

    uv_buf_t buf;
    buf.base = reinterpret_cast<char*>(req->data);
    buf.len = nwritten;

    result = uv_write(req, reinterpret_cast<uv_stream_t*>(&socket),
     &buf, 1, AfterFlush);
    CheckUVResult(result, "write");

    flushed = true;
    nwritten = 0;
}

void NetNode::AfterFlush(uv_write_t* req, int status) {
    assert(req != nullptr);
    assert(req->data != nullptr);
    assert(status == 0);

    free(req->data);
    free(req);
}

void NetNode::ReceiveConfig(Library* lib) {
    assert(message.size > 0);

    // Deserialize the payload.
    msgpack::unpacked mp_msg;
    msgpack::unpack(&mp_msg, reinterpret_cast<const char*>(message.body), message.size);

    // Unpack the config.
    Config *config = new Config;
    msgpack::object mp_obj = mp_msg.get();
    mp_obj.convert(config);

    // Save it in the library.
    lib->StoreConfig(config);
}

void NetNode::SendConfig(const Library* lib) {
    assert(lib != nullptr);

    Config* config = lib->LookupConfig();
    assert(config != nullptr);

    Message request(Message::Kind::SYNC_CONFIG);

    // Serialize the payload.
    msgpack::sbuffer buffer;
    msgpack::pack(buffer, *config);

    // Pack the message body.
    request.size = buffer.size();
    request.body = buffer.data();

    Send(request);
}

void NetNode::ReceiveCamera(Library* lib) {
    assert(message.size > 0);

    // Deserialize the payload.
    msgpack::unpacked mp_msg;
    msgpack::unpack(&mp_msg, reinterpret_cast<const char*>(message.body), message.size);

    // Unpack the camera.
    Camera *camera = new Camera;
    msgpack::object mp_obj = mp_msg.get();
    mp_obj.convert(camera);

    // Hook up the config.
    camera->SetConfig(lib->LookupConfig());

    // Save it in the library.
    lib->StoreCamera(camera);
}

void NetNode::SendCamera(const Library* lib) {
    assert(lib != nullptr);

    Camera* camera = lib->LookupCamera();
    assert(camera != nullptr);

    Message request(Message::Kind::SYNC_CAMERA);

    // Serialize the payload.
    msgpack::sbuffer buffer;
    msgpack::pack(buffer, *camera);

    // Pack the message body.
    request.size = buffer.size();
    request.body = buffer.data();

    Send(request);
}

void NetNode::ReceiveLightList(Library* lib) {
    assert(message.size > 0);

    // Deserialize the payload.
    msgpack::unpacked mp_msg;
    msgpack::unpack(&mp_msg, reinterpret_cast<const char*>(message.body), message.size);

    // Unpack the light list.
    LightList* lights = new LightList;
    msgpack::object mp_obj = mp_msg.get();
    mp_obj.convert(lights);

    // Save it in the library.
    lib->StoreLightList(lights);
}

void NetNode::SendLightList(const Library* lib) {
    assert(lib != nullptr);

    LightList* lights = lib->LookupLightList();
    assert(lights != nullptr);

    Message request(Message::Kind::SYNC_EMISSIVE);

    // Serialize the payload.
    msgpack::sbuffer buffer;
    msgpack::pack(buffer, *lights);

    // Pack the message body.
    request.size = buffer.size();
    request.body = buffer.data();

    Send(request);
}

void NetNode::ReceiveWBVH(Library *lib) {
    assert(message.size > 0);

    // Deserialize the payload.
    msgpack::unpacked mp_msg;
    msgpack::unpack(&mp_msg, reinterpret_cast<const char*>(message.body), message.size);

    // Unpack the light list.
    BVH* wbvh = new BVH;
    msgpack::object mp_obj = mp_msg.get();
    mp_obj.convert(wbvh);

    // Save it in the library.
    lib->StoreWBVH(wbvh);
}

void NetNode::SendWBVH(BVH* wbvh) {
    assert(wbvh != nullptr);

    Message request(Message::Kind::SYNC_WBVH);

    // Serialize the payload.
    msgpack::sbuffer buffer;
    msgpack::pack(buffer, *wbvh);

    // Pack the message body.
    request.size = buffer.size();
    request.body = buffer.data();

    Send(request);
}

Image* NetNode::ReceiveImage() {
    assert(message.size > 0);

    // Deserialize the payload.
    msgpack::unpacked mp_msg;
    msgpack::unpack(&mp_msg, reinterpret_cast<const char*>(message.body), message.size);

    // Unpack the image.
    Image *image = new Image;
    msgpack::object mp_obj = mp_msg.get();
    mp_obj.convert(image);

    return image;
}

void NetNode::SendImage(const Library* lib) {
    assert(lib != nullptr);

    Image* image = lib->LookupImage();
    assert(image != nullptr);

    Message request(Message::Kind::SYNC_IMAGE);

    // Serialize the payload.
    msgpack::sbuffer buffer;
    msgpack::pack(buffer, *image);

    // Pack the message body.
    request.size = buffer.size();
    request.body = buffer.data();

    Send(request);
}

uint32_t NetNode::ReceiveMesh(Library *lib) {
    assert(message.size > 0);

    // Deserialize the payload.
    msgpack::unpacked mp_msg;
    msgpack::unpack(&mp_msg, reinterpret_cast<const char*>(message.body), message.size);

    // Unpack the mesh.
    Mesh *mesh = new Mesh;
    msgpack::object mp_obj = mp_msg.get();
    mp_obj.convert(mesh);

    // Recompute transformation matrices.
    mesh->ComputeMatrices();

    // Save it in the library.
    lib->StoreMesh(mesh->id, mesh);

    return mesh->id;
}

void NetNode::SendMesh(const Library* lib, uint32_t id) {
    assert(lib != nullptr);
    assert(id > 0);

    Mesh* mesh = lib->LookupMesh(id);
    assert(mesh != nullptr);

    // Send the material first.
    SendMaterial(lib, mesh->material);

    Message request(Message::Kind::SYNC_MESH);

    // Serialize the payload.
    msgpack::sbuffer buffer;
    msgpack::pack(buffer, *mesh);

    // Pack the message body.
    request.size = buffer.size();
    request.body = buffer.data();

    Send(request);
}

uint32_t NetNode::ReceiveMaterial(Library* lib) {
    assert(message.size > 0);

    // Deserialize the payload.
    msgpack::unpacked mp_msg;
    msgpack::unpack(&mp_msg, reinterpret_cast<const char*>(message.body), message.size);

    // Unpack the material.
    Material *material = new Material;
    msgpack::object mp_obj = mp_msg.get();
    mp_obj.convert(material);

    // Save it in the library.
    // (Material names are inaccessible (and irrelevant) by this point.
    lib->StoreMaterial(material->id, material, "");

    return material->id;
}

void NetNode::SendMaterial(const Library* lib, uint32_t id) {
    assert(lib != nullptr);
    assert(id > 0);

    // Don't send the material if this node already has it.
    if (_materials.find(id) != _materials.end()) return;

    Material* material = lib->LookupMaterial(id);
    assert(material != nullptr);

    // Send the shader first.
    SendShader(lib, material->shader);

    // Send each of the textures first.
    for (const auto& kv_pair : material->textures) {
        SendTexture(lib, kv_pair.second);
    }

    // Is this material emissive? If so, this worker should be in the light
    // list.
    if (material->emissive) {
        LightList* lights = lib->LookupLightList();
        lights->AddEmissiveWorker(me);
    }

    Message request(Message::Kind::SYNC_MATERIAL);

    // Serialize the payload.
    msgpack::sbuffer buffer;
    msgpack::pack(buffer, *material);

    // Pack the message body.
    request.size = buffer.size();
    request.body = buffer.data();

    Send(request);

    // Mark that this node has this material.
    _materials[id] = true;
}

uint32_t NetNode::ReceiveTexture(Library *lib) {
    assert(message.size > 0);

    // Deserialize the payload.
    msgpack::unpacked mp_msg;
    msgpack::unpack(&mp_msg, reinterpret_cast<const char*>(message.body), message.size);

    // Unpack the texture.
    Texture *texture = new Texture;
    msgpack::object mp_obj = mp_msg.get();
    mp_obj.convert(texture);

    // Save it in the library.
    lib->StoreTexture(texture->id, texture);

    return texture->id;
}

void NetNode::SendTexture(const Library* lib, uint32_t id) {
    assert(lib != nullptr);
    assert(id > 0);

    // Don't send the texture if this node already has it.
    if (_textures.find(id) != _textures.end()) return;

    Texture* texture = lib->LookupTexture(id);
    assert(texture != nullptr);

    Message request(Message::Kind::SYNC_TEXTURE);

    // Serialize the payload.
    msgpack::sbuffer buffer;
    msgpack::pack(buffer, *texture);

    // Pack the message body.
    request.size = buffer.size();
    request.body = buffer.data();

    Send(request);

    // Mark that this node has this texture.
    _textures[id] = true;

}

uint32_t NetNode::ReceiveShader(Library *lib) {
    assert(message.size > 0);

    // Deserialize the payload.
    msgpack::unpacked mp_msg;
    msgpack::unpack(&mp_msg, reinterpret_cast<const char*>(message.body), message.size);

    // Unpack the shader.
    Shader *shader = new Shader;
    msgpack::object mp_obj = mp_msg.get();
    mp_obj.convert(shader);

    // Save it in the library.
    lib->StoreShader(shader->id, shader);

    return shader->id;
}

void NetNode::SendShader(const Library* lib, uint32_t id) {
    assert(lib != nullptr);
    assert(id > 0);

    // Don't send the shader if this node already has it.
    if (_shaders.find(id) != _shaders.end()) return;

    Shader* shader = lib->LookupShader(id);
    assert(shader != nullptr);

    Message request(Message::Kind::SYNC_SHADER);

    // Serialize the payload.
    msgpack::sbuffer buffer;
    msgpack::pack(buffer, *shader);

    // Pack the message body.
    request.size = buffer.size();
    request.body = buffer.data();

    Send(request);

    // Mark that this node has this shader.
    _shaders[id] = true;
}

FatRay* NetNode::ReceiveRay() {
    assert(message.size == sizeof(FatRay));

    // Forgo safe deserialization for speed.
    FatRay* ray = new FatRay;
    memcpy(ray, message.body, sizeof(FatRay));

    if (_current_stats != nullptr) {
        _current_stats->rays_rx++;
    }

    return ray;
}

void NetNode::SendRay(FatRay* ray) {
    Message msg(Message::Kind::RAY);

    // Forgo safe serialization for speed.
    msg.size = sizeof(FatRay);
    msg.body = ray;

    if (_current_stats != nullptr) {
        _current_stats->rays_tx++;
    }

    Send(msg);
}

void NetNode::ReceiveRenderStats() {
    assert(message.size > 0);

    // Deserialize the payload.
    msgpack::unpacked mp_msg;
    msgpack::unpack(&mp_msg, reinterpret_cast<const char*>(message.body), message.size);

    // Unpack the render stats.
    RenderStats* stats = new RenderStats;
    msgpack::object mp_obj = mp_msg.get();
    mp_obj.convert(stats);

    // Store them in the stats log.
    _stats_log.push_back(stats);

    // Are they interesting?
    if (stats->intersect_queue > 0 ||
        stats->illuminate_queue > 0 ||
        stats->light_queue > 0 ||
        stats->intersects_produced > 0 ||
        stats->illuminates_produced > 0 ||
        stats->lights_killed > 0 ||
        stats->intersects_killed > 0 ||
        stats->illuminates_killed > 0 ||
        stats->lights_killed > 0) {
        _num_uninteresting = 0;
    } else {
        _num_uninteresting++;
    }

    // Save the progress.
    _last_progress = stats->primary_progress;
}

void NetNode::SendRenderStats(RenderStats* stats) {
    assert(stats != nullptr);

    Message request(Message::Kind::RENDER_STATS);

    // Serialize the payload.
    msgpack::sbuffer buffer;
    msgpack::pack(buffer, *stats);

    // Pack the message body.
    request.size = buffer.size();
    request.body = buffer.data();

    Send(request);
}

bool NetNode::IsInteresting(uint32_t intervals) {
    return _num_uninteresting < intervals;
}

uint64_t NetNode::RaysProduced(uint32_t intervals) {
    uint64_t rays = 0;

    uint32_t count = 0;
    auto iter = _stats_log.rbegin();
    while (iter != _stats_log.rend() && count < intervals) {
        rays += (*iter)->intersects_produced + (*iter)->illuminates_produced +
         (*iter)->lights_produced;
        iter++;
        count++;
    }

    return rays;
}

uint64_t NetNode::RaysKilled(uint32_t intervals) {
    uint64_t rays = 0;

    uint32_t count = 0;
    auto iter = _stats_log.rbegin();
    while (iter != _stats_log.rend() && count < intervals) {
        rays += (*iter)->intersects_killed + (*iter)->illuminates_killed +
         (*iter)->lights_killed;
        iter++;
        count++;
    }

    return rays;
}

uint64_t NetNode::RaysQueued(uint32_t intervals) {
    uint64_t rays = 0;

    uint32_t count = 0;
    auto iter = _stats_log.rbegin();
    while (iter != _stats_log.rend() && count < intervals) {
        rays += (*iter)->intersect_queue + (*iter)->illuminate_queue +
         (*iter)->light_queue;
        iter++;
        count++;
    }

    return rays;
}

void NetNode::StatsToCSVFile(const string& filename) const {
    ofstream file;
    file.open(filename);

    // Write header.
    file << "Tick," <<
     "Primary Ray Casting Progress," <<
     "Intersection Rays Produced," <<
     "Illumination Rays Produced," <<
     "Light Rays Produced," <<
     "Intersection Rays Killed," <<
     "Illumination Rays Killed," <<
     "Light Rays Killed," <<
     "Intersection Queue Size," <<
     "Illumination Queue Size," <<
     "Light Queue Size," <<
     "Total Rays Received," <<
     "Total Rays Sent," <<
     "Total Rays Produced," <<
     "Total Rays Killed," <<
     "Total Rays Queued," <<
     "Total Bytes Received" << endl;

    // Write stats log.
    uint64_t tick = 1;
    for (const auto& record : _stats_log) {
        file << tick << "," <<
         record->primary_progress << "," <<
         record->intersects_produced << "," <<
         record->illuminates_produced << "," <<
         record->lights_produced << "," <<
         record->intersects_killed << "," <<
         record->illuminates_killed << "," <<
         record->lights_killed << "," <<
         record->intersect_queue << "," <<
         record->illuminate_queue << "," <<
         record->light_queue << "," <<
         record->rays_rx << "," <<
         record->rays_tx << "," <<
         (record->intersects_produced + record->illuminates_produced +
          record->lights_produced) << "," <<
         (record->intersects_killed + record->illuminates_killed +
          record->lights_killed) << "," <<
         (record->intersect_queue + record->illuminate_queue +
          record->light_queue) << "," <<
         record->bytes_rx << endl;
        tick++;
    }

    file.close();
}

} // namespace fr
