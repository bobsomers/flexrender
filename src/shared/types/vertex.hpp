#pragma once

#include "glm/glm.hpp"
#include "msgpack.hpp"

#include "utils/tostring.hpp"

namespace fr {

struct Vertex {
    explicit Vertex(glm::vec3 v, glm::vec3 n, glm::vec2 t);

    explicit Vertex(glm::vec3 v, glm::vec3 n);

    explicit Vertex(glm::vec3 v);

    explicit Vertex(float x, float y, float z);

    // FOR MSGPACK ONLY!
    explicit Vertex();

    /// Position of the vertex.
    glm::vec3 v;

    /// Normal of the vertex. Unit length is not enforced.
    glm::vec3 n;

    /// Texture coordinate of the vertex. Range is not enforced.
    glm::vec2 t;

    MSGPACK_DEFINE(v, n, t);

    TOSTRINGABLE(Vertex);
};

std::string ToString(const Vertex& vert, const std::string& indent = "");

} // namespace fr
