#pragma once

#include <cstdint>
#include <cassert>
#include <vector>
#include <unordered_map>
#include <string>
#include <stdexcept>

#include "utils/uncopyable.hpp"

namespace fr {

struct Config;
struct Camera;
struct Shader;
struct Texture;
struct Material;
struct Mesh;

class Library : private Uncopyable {
public:
    explicit Library();
    ~Library();

    // Configs...
    void StoreConfig(Config* config);
    inline Config* LookupConfig() const { return _config; }

    // Cameras...
    void StoreCamera(Camera* camera);
    inline Camera* LookupCamera() const { return _camera; }

    // Shaders...
    inline uint64_t NextShaderID() const { return _shaders.size(); }
    void StoreShader(uint64_t id, Shader* shader);
    inline Shader* LookupShader(uint64_t id) const {
        assert(id > 0);
        assert(id < _shaders.size());
        return _shaders[id];    
    };

    // Textures...
    inline uint64_t NextTextureID() const { return _textures.size(); }
    void StoreTexture(uint64_t id, Texture* texture);
    inline Texture* LookupTexture(uint64_t id) const {
        assert(id > 0);
        assert(id < _textures.size());
        return _textures[id];
    }

    // Materials...
    inline uint64_t NextMaterialID() const { return _materials.size(); }
    void StoreMaterial(uint64_t id, Material* material, const std::string& name);
    inline Material* LookupMaterial(uint64_t id) const {
        assert(id > 0);
        assert(id < _materials.size());
        return _materials[id];
    }
    inline uint64_t LookupMaterial(const std::string& name) const {
        uint64_t id = 0;
        try {
            id = _material_name_index.at(name);
        } catch (std::out_of_range& e) {
            id = 0;
        }
        return id;
    }

    // Meshes...
    inline uint64_t NextMeshID() const { return _meshes.size(); }
    void StoreMesh(uint64_t id, Mesh* mesh);
    inline Mesh* LookupMesh(uint64_t id) const {
        assert(id > 0);
        assert(id < _meshes.size());
        return _meshes[id];
    }

private:
    Config *_config;
    Camera* _camera;
    std::vector<Shader*> _shaders;
    std::vector<Texture*> _textures;
    std::vector<Material*> _materials;
    std::vector<Mesh*> _meshes;
    std::unordered_map<std::string, uint64_t> _material_name_index;
};

} // namespace fr
