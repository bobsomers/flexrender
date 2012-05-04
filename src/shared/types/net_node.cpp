#include "types/net_node.hpp"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include "types.hpp"
#include "utils/library.hpp"
#include "utils/network.hpp"

using std::stringstream;
using std::string;
using std::unordered_map;

namespace fr {

NetNode::NetNode(DispatchCallback dispatcher, const string& address) :
 state(State::NONE),
 mode(ReadMode::HEADER),
 message(),
 nread(0),
 nwritten(0),
 flushed(false),
 _dispatcher(dispatcher),
 _materials(),
 _textures(),
 _shaders() {
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

NetNode::NetNode(DispatchCallback dispatcher) :
 state(State::NONE),
 ip(""),
 port(0),
 mode(ReadMode::HEADER),
 message(),
 nread(0),
 nwritten(0),
 flushed(false),
 _dispatcher(dispatcher) {}

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

uint64_t NetNode::ReceiveMesh(Library *lib) {
    assert(message.size > 0);

    // Deserialize the payload.
    msgpack::unpacked mp_msg;
    msgpack::unpack(&mp_msg, reinterpret_cast<const char*>(message.body), message.size);

    // Unpack the mesh.
    Mesh *mesh = new Mesh;
    msgpack::object mp_obj = mp_msg.get();
    mp_obj.convert(mesh);

    // Save it in the library.
    lib->StoreMesh(mesh->id, mesh);

    return mesh->id;
}

void NetNode::SendMesh(const Library* lib, uint64_t id) {
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

uint64_t NetNode::ReceiveMaterial(Library* lib) {
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

void NetNode::SendMaterial(const Library* lib, uint64_t id) {
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

uint64_t NetNode::ReceiveTexture(Library *lib) {
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

void NetNode::SendTexture(const Library* lib, uint64_t id) {
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

uint64_t NetNode::ReceiveShader(Library *lib) {
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

void NetNode::SendShader(const Library* lib, uint64_t id) {
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

} // namespace fr
