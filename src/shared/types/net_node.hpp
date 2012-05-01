#pragma once

#include <cstdint>
#include <string>

#include "uv.h"

#include "types/message.hpp"

/// The size of the static write buffer (for this node).
#define FR_WRITE_BUFFER_SIZE 65536

namespace fr {

class NetNode {
public:
    enum class State {
        NONE,
        INIT_SENT,
        READY_TO_SYNC,
        SYNC_SENT,
        SYNC_LIMBO
    };

    enum class ReadMode {
        HEADER,
        BODY
    };

    typedef void (*DispatchCallback)(NetNode* node);

    explicit NetNode(DispatchCallback dispatcher, const std::string& address);
    explicit NetNode(DispatchCallback dispatcher);

    /// Receives the given chunk of bytes, parses out messages, and dispatches
    /// them using the dispatcher callback.
    void Receive(const char* buf, ssize_t len);

    /// Appends the given message to the send buffer.
    void Send(const Message& msg);

    /// Flushes the send buffer, forcing all buffered messages to be written.
    void Flush();

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

private:
    DispatchCallback _dispatcher;

    /// Post-write callback from libuv.
    static void AfterFlush(uv_write_t* req, int status);
};

} // namespace fr
