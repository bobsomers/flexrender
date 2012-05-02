#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include "uv.h"

#include "types/message.hpp"

/// The size of the static write buffer (for this node).
#define FR_WRITE_BUFFER_SIZE 65536

namespace fr {

class Library;

class NetNode {
public:
    enum class State {
        NONE,
        INITIALIZING,
        CONFIGURING,
        SYNCING_ASSETS,
        SYNCING_CAMERA,
        READY,
        RENDERING
    };

    enum class ReadMode {
        HEADER,
        BODY
    };

    typedef void (*DispatchCallback)(NetNode* node);

    explicit NetNode(DispatchCallback dispatcher, const std::string& address);
    explicit NetNode(DispatchCallback dispatcher);

    /// The TCP socket this net node is connected on.
    uv_tcp_t socket;

    /// The state this net node is currently in (one of the the above).
    State state;

    /// A string representation of the net node's IP address.
    std::string ip;

    /// The net node's port.
    uint16_t port;

    /// The current read mode of the net node.
    ReadMode mode;

    /// The message we're in the middle of reading.
    Message message;

    /// The amount of data this net node has read.
    ssize_t nread;

    /// The amount of data this net node has written.
    ssize_t nwritten;

    /// True if the write buffer has been flushed recently.
    bool flushed;

    /// The static write buffer for sending data to this net node.
    char buffer[FR_WRITE_BUFFER_SIZE];

    /// Receives the given chunk of bytes, parses out messages, and dispatches
    /// them using the dispatcher callback.
    void Receive(const char* buf, ssize_t len);

    /// Appends the given message to the send buffer.
    void Send(const Message& msg);

    /// Receives the message in the net node's buffer as a config.
    void ReceiveConfig(Library* lib);

    /// Sends the given config to this node.
    void SendConfig(const Library* lib);

    /// Receives the message in the net node's buffer as a camera.
    void ReceiveCamera(Library* lib);

    /// Sends the given camera to this node.
    void SendCamera(const Library* lib);

    /// Receives the message in the net node's buffer as a mesh.
    uint64_t ReceiveMesh(Library* lib);

    /// Sends the given mesh (and its dependent assets) to this node.
    void SendMesh(const Library* lib, uint64_t id);

    /// Receives the message in the net node's buffer as a material.
    uint64_t ReceiveMaterial(Library* lib);

    /// Sends the given material (and its dependent assets) to this node.
    void SendMaterial(const Library* lib, uint64_t id);

    /// Receives the message in the net node's buffer as a texture.
    uint64_t ReceiveTexture(Library* lib);

    /// Sends the given texture to this node.
    void SendTexture(const Library* lib, uint64_t id);

    /// Receives the message in the net node's buffer as a shader.
    uint64_t ReceiveShader(Library* lib);

    /// Sends the given shader to this node.
    void SendShader(const Library* lib, uint64_t id);

    /// Flushes the send buffer, forcing all buffered messages to be written.
    void Flush();

private:
    DispatchCallback _dispatcher;
    std::unordered_map<uint64_t, bool> _materials;
    std::unordered_map<uint64_t, bool> _textures;
    std::unordered_map<uint64_t, bool> _shaders;

    /// Post-write callback from libuv.
    static void AfterFlush(uv_write_t* req, int status);
};

} // namespace fr
