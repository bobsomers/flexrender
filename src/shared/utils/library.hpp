#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <utility>
#include <string>

namespace fr {

struct Camera;
struct Shader;
struct Texture;
struct Material;
struct Mesh;

class Library {
public:
    enum class Kind {
        NONE,
        SHADER,
        TEXTURE,
        MATERIAL,
        MESH
    };

    explicit Library();
    ~Library();

    void StoreCamera(Camera* camera);
    Camera* LookupCamera();

    void StoreShader(uint64_t id, Shader* shader);
    Shader* LookupShader(uint64_t id);

    void StoreTexture(uint64_t id, Texture* texture);
    Texture* LookupTexture(uint64_t id);

    void StoreMaterial(uint64_t id, Material* material, const std::string& name);
    Material* LookupMaterial(uint64_t id);
    uint64_t LookupMaterial(const std::string& name);

    void StoreMesh(uint64_t id, Mesh* mesh);
    Mesh* LookupMesh(uint64_t id);

    inline uint64_t GetNextResourceID() { return _resources.size(); }

private:
    std::vector<std::pair<Kind, void*>> _resources;
    std::unordered_map<std::string, uint64_t> _material_name_index;
    Camera* _camera;

    void DestroyResource(Kind kind, void* data);
};

} // namespace fr
