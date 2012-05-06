#pragma once

#include <cstring>

#include "utils/uncopyable.hpp"

namespace fr {

struct FatRay;
struct Camera;

class RayQueue : private Uncopyable {
public:
    explicit RayQueue(Camera* camera);

    ~RayQueue();

    /// Pushes the given ray into the queue and assumes ownership of its memory.
    void Push(FatRay* ray);

    /// Pops a ray out of the queue and relinquishes control of its memory.
    FatRay* Pop();

    /// Returns the size of the internal intersection ray queue.
    inline size_t IntersectSize() const { return _intersect_size; }

    /// Returns the size of the internal light ray queue.
    inline size_t LightSize() const { return _light_size; }

private:
    Camera* _camera;
    FatRay* _intersect_front;
    FatRay* _intersect_back;
    size_t _intersect_size;
    FatRay* _light_front;
    FatRay* _light_back;
    size_t _light_size;
};

} // namespace fr
