#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <deque>

#include "uv.h"

#include "types/message.hpp"

/// The size of the static write buffer (for this node).
#define FR_WRITE_BUFFER_SIZE 65536

namespace fr {

class Library;
class Image;
class BVH;
struct FatRay;
struct RenderStats;

class NetNode {
public:
    enum class State {
        NONE,
        INITIALIZING,
        CONFIGURING,
        SYNCING_ASSETS,
        SYNCING_CAMERA,
        SYNCING_EMISSIVE,
        BUILDING_BVH,
        SYNCING_WBVH,
        READY,
        RENDERING,
        PAUSED,
        SYNCING_IMAGES,
    };

    enum class ReadMode {
        HEADER,
        BODY
    };

    typedef void (*DispatchCallback)(NetNode* node);

    explicit NetNode(DispatchCallback dispatcher, const std::string& address,
     RenderStats* stats = nullptr);
    explicit NetNode(DispatchCallback dispatcher, RenderStats* stats = nullptr);

    /// The resource ID of this net node.
    uint32_t me;

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

    /// Sends the config to this node.
    void SendConfig(const Library* lib);

    /// Receives the message in the net node's buffer as a camera.
    void ReceiveCamera(Library* lib);

    /// Sends the camera to this node.
    void SendCamera(const Library* lib);

    /// Receives the message in the net node's buffer as a light list.
    void ReceiveLightList(Library* lib);

    /// Sends the light list to this node.
    void SendLightList(const Library* lib);

    /// Receives the message in the net node's buffer as a WBVH.
    void ReceiveWBVH(Library* lib);

    /// Sends the WBVH to this node.
    void SendWBVH(BVH* wbvh);

    /// Receives the message in the net node's buffer as a freshly allocated
    /// image.
    Image* ReceiveImage();

    /// Sends the given image to this node.
    void SendImage(const Library* lib);

    /// Receives the message in the net node's buffer as a mesh.
    uint32_t ReceiveMesh(Library* lib);

    /// Sends the given mesh (and its dependent assets) to this node.
    void SendMesh(const Library* lib, uint32_t id);

    /// Receives the message in the net node's buffer as a material.
    uint32_t ReceiveMaterial(Library* lib);

    /// Sends the given material (and its dependent assets) to this node.
    void SendMaterial(const Library* lib, uint32_t id);

    /// Receives the message in the net node's buffer as a texture.
    uint32_t ReceiveTexture(Library* lib);

    /// Sends the given texture to this node.
    void SendTexture(const Library* lib, uint32_t id);

    /// Receives the message in the net node's buffer as a shader.
    uint32_t ReceiveShader(Library* lib);

    /// Sends the given shader to this node.
    void SendShader(const Library* lib, uint32_t id);

    /// Receives the message in the net node's buffer as a freshly allocated ray.
    FatRay* ReceiveRay();

    /// Sends the given ray to this node.
    void SendRay(FatRay* ray);

    /// Receives the message in the net node's buffer as a freshly allocated 
    /// render stats.
    void ReceiveRenderStats();

    /// Sends the given render stats to this node.
    void SendRenderStats(RenderStats* stats);

    /// Flushes the send buffer, forcing all buffered messages to be written.
    void Flush();

    /// Has this net node been interesting in the last intervals intervals?
    bool IsInteresting(uint32_t intervals);

    /// Returns the number of rays produced at this node (from the stats logs)
    /// in the last intervals intervals.
    uint64_t RaysProduced(uint32_t intervals);

    /// Returns the number of rays killed at this node (from the stats logs)
    /// in the last intervals intervals.
    uint64_t RaysKilled(uint32_t intervals);

    /// Returns the number of rays currently queued at this node (from the
    /// stats logs) in the last intervals intervals.
    uint64_t RaysQueued(uint32_t intervals);

    /// Dumps the stats log to a CSV file with the given filename.
    void StatsToCSVFile(const std::string& filename) const;

    inline float Progress() const { return _last_progress; }

private:
    DispatchCallback _dispatcher;
    std::unordered_map<uint32_t, bool> _materials;
    std::unordered_map<uint32_t, bool> _textures;
    std::unordered_map<uint32_t, bool> _shaders;
    std::deque<RenderStats*> _stats_log;
    RenderStats* _current_stats;
    uint32_t _num_uninteresting;
    float _last_progress;

    /// Post-write callback from libuv.
    static void AfterFlush(uv_write_t* req, int status);
};

} // namespace fr
