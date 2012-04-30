#include "engine.hpp"

#include <cassert>

#include "uv.h"

#include "scripting.hpp"
#include "types.hpp"
#include "utils.hpp"

using std::string;

namespace fr {

static Library* lib = nullptr;
static size_t num_workers_connected = 0;

// Callbacks, handlers, and helpers for client functionality.
namespace client {

void Init(const Config& config);
void OnConnect(uv_connect_t* req, int status);

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

    client::Init(*lib->LookupConfig());

    // Parse and distribute the scene.
    // TODO: do this in a a uv_work thread after connected to all workers
    //SceneScript scene_script;
    //TOUTLN("Loading scene from " << scene_file << ".");
    //if (!scene_script.Parse(scene_file, lib)) {
    //    TERRLN("Can't continue with bad scene.");
    //    exit(EXIT_FAILURE);
    //}
    //TOUTLN("Scene loaded.");
}

void EngineRun() {
    uv_run(uv_default_loop());
}

void client::Init(const Config& config) {
    int result = 0;
    struct sockaddr_in server_addr;

    TOUTLN("Connecting to " << config.workers.size() << " workers...");

    for (const auto& worker : config.workers) {
        Server* server = new Server(worker);

        // Initialize the TCP socket.
        result = uv_tcp_init(uv_default_loop(), &server->socket);
        CheckUVResult(result, "tcp_init");
        
        // Squirrel away the server in the data baton.
        server->socket.data = server;

        // Connect to the server.
        server_addr = uv_ip4_addr(server->ip.c_str(), server->port);
        uv_connect_t* req = reinterpret_cast<uv_connect_t*>(malloc(sizeof(uv_connect_t)));
        result = uv_tcp_connect(req, &server->socket, server_addr,
         client::OnConnect);
        CheckUVResult(result, "tcp_connect");
    }
}

void client::OnConnect(uv_connect_t* req, int status) {
    assert(req != nullptr);
    assert(req->handle != nullptr);
    assert(req->handle->data != nullptr);

    // Pull the server out of the data baton.
    Server* server = reinterpret_cast<Server*>(req->handle->data);
    free(req);

    if (status != 0) {
        TERRLN("Failed connecting to " << server->ip << ".");
        exit(EXIT_FAILURE);
    }

    TOUTLN("[" << server->ip << "] Connected on port " << server->port << ".");

    // If we've connected to all the workers, move on to the next step.
    num_workers_connected++;
    if (num_workers_connected == lib->LookupConfig()->workers.size()) {
        TOUTLN("Everyone's here! Let's get this party started!");
    }
}

} // namespace fr
