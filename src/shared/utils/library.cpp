#include "utils/library.hpp"

#include "types.hpp"
#include "utils.hpp"

using std::string;

namespace fr {

Library::Library() :
 _config(nullptr),
 _camera(nullptr),
 _shaders(),
 _textures(),
 _materials(),
 _meshes(),
 _servers(),
 _material_name_index() {
    // ID #0 is always reserved.
    _shaders.push_back(nullptr);
    _textures.push_back(nullptr);
    _materials.push_back(nullptr);
    _meshes.push_back(nullptr);
    _servers.push_back(nullptr);
}

Library::~Library() {
    if (_config != nullptr) delete _config;
    if (_camera != nullptr) delete _camera;
}

void Library::StoreConfig(Config* config) {
    if (_config != nullptr) delete _config;
    _config = config;
}

void Library::StoreCamera(Camera* camera) {
    if (_camera != nullptr) delete _camera;
    _camera = camera;
}

void Library::StoreShader(uint64_t id, Shader* shader) {
    if (id < _shaders.size()) {
        if (_shaders[id] != nullptr) {
            delete _shaders[id];
            _shaders[id] = nullptr;
        }
    } else {
        _shaders.resize(id, nullptr);
    }
    _shaders.insert(_shaders.begin() + id, shader);
}

void Library::StoreTexture(uint64_t id, Texture* texture) {
    if (id < _textures.size()) {
        if (_textures[id] != nullptr) {
            delete _textures[id];
            _textures[id] = nullptr;
        }
    } else {
        _textures.resize(id, nullptr);
    }
    _textures.insert(_textures.begin() + id, texture);
}

void Library::StoreMaterial(uint64_t id, Material* material, const string& name) {
    if (id < _materials.size()) {
        if (_materials[id] != nullptr) {
            delete _materials[id];
            _materials[id] = nullptr;
        }
    } else {
        _materials.resize(id, nullptr);
    }
    _materials.insert(_materials.begin() + id, material);
    _material_name_index[name] = id;
}

void Library::StoreMesh(uint64_t id, Mesh* mesh) {
    if (id < _meshes.size()) {
        if (_meshes[id] != nullptr) {
            delete _meshes[id];
            _meshes[id] = nullptr;
        }
    } else {
        _meshes.resize(id, nullptr);
    }
    _meshes.insert(_meshes.begin() + id, mesh);
}

void Library::StoreServer(uint64_t id, Server* server) {
    if (id < _servers.size()) {
        if (_servers[id] != nullptr) {
            delete _servers[id];
            _servers[id] = nullptr;
        }
    } else {
        _servers.resize(id, nullptr);
    }
    _servers.insert(_servers.begin() + id, server);
}

}
