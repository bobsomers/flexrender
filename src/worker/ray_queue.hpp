#pragma once

#include <cstring>

#include "utils/uncopyable.hpp"

namespace fr {

struct Ray;
struct Camera;

class RayQueue : private Uncopyable {
public:
    explicit RayQueue(Camera* camera);

    ~RayQueue();

    /// Pushes the given ray into the queue and assumes ownership of its memory.
    void Push(Ray* ray);

    /// Pops a ray out of the queue and relinquishes control of its memory.
    Ray* Pop();

    /// Returns the size of the internal intersection ray queue.
    inline size_t IntersectSize() const { return _intersect_size; }

    /// Returns the size of the internal light ray queue.
    inline size_t LightSize() const { return _light_size; }

private:
    Camera* _camera;
    Ray* _intersect_front;
    Ray* _intersect_back;
    size_t _intersect_size;
    Ray* _light_front;
    Ray* _light_back;
    size_t _light_size;
};

} // namespace fr
