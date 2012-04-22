#include <cstdlib>

#include "glm/glm.hpp"

#include "types.hpp"
#include "utils.hpp"

static float data[12] = {
    1.0f, 2.0f, 3.0f,
    4.0f, 5.0f, 6.0f,
    7.0f, 8.0f, 9.0f,
    10.0f, 11.0f, 12.0f
};

int main(int argc, char *argv[]) {
    // TODO: check args
    
    TOUTLN("FlexWorker starting.");

    //fr::Vertex v1(glm::vec3(1.0f, 2.0f, 3.0f), glm::vec3(4.0f, 5.0f, 6.0f), glm::vec2(7.0f, 8.0f));
    //fr::Vertex v2(glm::vec3(1.0f, 2.0f, 3.0f), glm::vec3(4.0f, 5.0f, 6.0f), glm::vec2(7.0f, 8.0f));
    //fr::Vertex v3(glm::vec3(1.0f, 2.0f, 3.0f), glm::vec3(4.0f, 5.0f, 6.0f), glm::vec2(7.0f, 8.0f));
    //fr::Triangle tri(v1, v2, v3);

    //fr::Transform xform1(fr::Transform::Kind::NONE, 0.0f);
    //fr::Transform xform2(fr::Transform::Kind::ROTATE, 1.0f, glm::vec3(2.0f, 3.0f, 4.0f));
    //fr::Transform xform3(fr::Transform::Kind::SCALE, glm::vec3(5.0f, 6.0f, 7.0f));
    //fr::Transform xform4(fr::Transform::Kind::TRANSLATE, glm::vec3(8.0f, 9.0f, 10.0f));

    //TOUTLN(ToString(tri));
    //TOUTLN(ToString(xform1, "\t"));
    //TOUTLN(ToString(xform2, "\t"));
    //TOUTLN(ToString(xform3, "\t"));
    //TOUTLN(ToString(xform4, "\t"));

    //fr::Texture tex1(42, "function awesome(x)\n\tprint(x)\nend");
    //fr::Texture tex2(69, 3, 4, data);

    //TOUTLN(ToString(tex1, "\t"));
    //TOUTLN(ToString(tex2, "\t"));

    TOUTLN("FlexWorker done.");
    return EXIT_SUCCESS;
}
