#include "utils/library.hpp"

#include <cassert>
#include <cstdlib>
#include <stdexcept>

#include "types.hpp"
#include "utils.hpp"

using std::make_pair;
using std::string;
using std::out_of_range;

namespace fr {

Library::Library() :
 _resources(),
 _material_name_index(),
 _camera(nullptr) {
    // Resource ID 0 is reserved.
    _resources.push_back(make_pair(Kind::NONE, nullptr));
}

Library::~Library() {
    for (const auto& record : _resources) {
        DestroyResource(record.first, record.second);
    }
    if (_camera != nullptr) {
        delete _camera;
    }
}

void Library::DestroyResource(Kind kind, void* data) {
    switch (kind) {
        case Kind::NONE:
            // Skip.
            break;

        case Kind::SHADER:
            {
                Shader* shader = reinterpret_cast<Shader*>(data);
                delete shader;
            }
            break;

        case Kind::TEXTURE:
            {
                Texture* texture = reinterpret_cast<Texture*>(data);
                delete texture;
            }
            break;

        case Kind::MATERIAL:
            {
                Material* material = reinterpret_cast<Material*>(data);
                delete material;
            }
            break;

        case Kind::MESH:
            {
                Mesh* mesh = reinterpret_cast<Mesh*>(data);
                delete mesh;
            }
            break;

        default:
            TERRLN("Unknown type encountered when destroying resource!");
            exit(EXIT_FAILURE);
            break;
    }
}

void Library::StoreCamera(Camera* camera) {
    _camera = camera;
}

Camera* Library::LookupCamera() {
    return _camera;
}

void Library::StoreShader(uint64_t id, Shader* shader) {
    auto entry = make_pair(Kind::SHADER, reinterpret_cast<void*>(shader));
    if (id < _resources.size()) {
        auto record = _resources[id];
        DestroyResource(record.first, record.second);
    } else {
        _resources.resize(id, make_pair(Kind::NONE, nullptr));
    }
    _resources[id] = entry;
}

Shader* Library::LookupShader(uint64_t id) {
    assert(id > 0);
    assert(id < _resources.size());

    auto record = _resources[id];
    if (record.first != Kind::SHADER) {
        TERRLN("Attempt to LookupShader() on non-shader!");
        exit(EXIT_FAILURE);
    }

    return reinterpret_cast<Shader*>(record.second);
}

void Library::StoreTexture(uint64_t id, Texture* texture) {
    auto entry = make_pair(Kind::TEXTURE, reinterpret_cast<void*>(texture));
    if (id < _resources.size()) {
        auto record = _resources[id];
        DestroyResource(record.first, record.second);
    } else {
        _resources.resize(id, make_pair(Kind::NONE, nullptr));
    }
    _resources[id] = entry;
}

Texture* Library::LookupTexture(uint64_t id) {
    assert(id > 0);
    assert(id < _resources.size());

    auto record = _resources[id];
    if (record.first != Kind::TEXTURE) {
        TERRLN("Attempt to LookupTexture() on non-texture!");
        exit(EXIT_FAILURE);
    }

    return reinterpret_cast<Texture*>(record.second);
}

void Library::StoreMaterial(uint64_t id, Material* material, const string& name) {
    auto entry = make_pair(Kind::MATERIAL, reinterpret_cast<void*>(material));
    if (id < _resources.size()) {
        auto record = _resources[id];
        DestroyResource(record.first, record.second);
    } else {
        _resources.resize(id, make_pair(Kind::NONE, nullptr));
    }
    _resources[id] = entry;
    _material_name_index[name] = id;
}

Material* Library::LookupMaterial(uint64_t id) {
    assert(id > 0);
    assert(id < _resources.size());

    auto record = _resources[id];
    if (record.first != Kind::MATERIAL) {
        TERRLN("Attempt to LookupMaterial() on non-material!");
        exit(EXIT_FAILURE);
    }

    return reinterpret_cast<Material*>(record.second);
}

uint64_t Library::LookupMaterial(const string& name) {
    int64_t id = 0;

    try {
        id = _material_name_index.at(name);
    } catch (out_of_range& e) {
        id = 0;
    }

    return id;
}

void Library::StoreMesh(uint64_t id, Mesh* mesh) {
    auto entry = make_pair(Kind::MESH, reinterpret_cast<void*>(mesh));
    if (id < _resources.size()) {
        auto record = _resources[id];
        DestroyResource(record.first, record.second);
    } else {
        _resources.resize(id, make_pair(Kind::NONE, nullptr));
    }
    _resources[id] = entry;
}

Mesh* Library::LookupMesh(uint64_t id) {
    assert(id > 0);
    assert(id < _resources.size());

    auto record = _resources[id];
    if (record.first != Kind::MESH) {
        TERRLN("Attempt to LookupMesh() on non-mesh!");
        exit(EXIT_FAILURE);
    }

    return reinterpret_cast<Mesh*>(record.second);
}

}
