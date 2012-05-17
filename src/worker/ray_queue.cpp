#include "ray_queue.hpp"

#include <cassert>

#include "types.hpp"
#include "utils.hpp"

namespace fr {

RayQueue::RayQueue(Camera* camera, RenderStats* stats) :
 _camera(camera),
 _stats(stats),
 _intersect_front(nullptr),
 _intersect_back(nullptr),
 _intersect_size(0),
 _illuminate_front(nullptr),
 _illuminate_back(nullptr),
 _illuminate_size(0),
 _light_front(nullptr),
 _light_back(nullptr),
 _light_size(0) {}

RayQueue::~RayQueue() {
    FatRay* ray = nullptr;

    while (_light_front != nullptr) {
        ray = _light_front;
        _light_front = ray->next;
        delete ray;
    }

    while (_illuminate_front != nullptr) {
        ray = _illuminate_front;
        _illuminate_front = ray->next;
        delete ray;
    }

    while (_intersect_front != nullptr) {
        ray = _intersect_front;
        _intersect_front = ray->next;
        delete ray;
    }
}

void RayQueue::Push(FatRay* ray) {
    assert(ray != nullptr);

    switch (ray->kind) {
        case FatRay::Kind::INTERSECT:
            if (_intersect_back != nullptr) {
                _intersect_back->next = ray;
            } else {
                _intersect_front = ray;
            }
            ray->next = nullptr;
            _intersect_back = ray;
            _intersect_size++;
            break;

        case FatRay::Kind::ILLUMINATE:
            if (_illuminate_back != nullptr) {
                _illuminate_back->next = ray;
            } else {
                _illuminate_front = ray;
            }
            ray->next = nullptr;
            _illuminate_back = ray;
            _illuminate_size++;
            break;

        case FatRay::Kind::LIGHT:
            if (_light_back != nullptr) {
                _light_back->next = ray;
            } else {
                _light_front = ray;
            }
            ray->next = nullptr;
            _light_back = ray;
            _light_size++;
            break;

        default:
            TERRLN("Pushed unknown ray kind into ray queue.");
            break;
    }
}

FatRay* RayQueue::Pop() {
    FatRay* ray = nullptr;

    // Pull from the light queue first.
    ray = _light_front;
    if (ray != nullptr) {
        _light_front = ray->next;
        ray->next = nullptr;
        if (_light_front == nullptr) {
            _light_back = nullptr;
        }
        _light_size--;
        return ray;
    }

    // Pull from the illumination queue if the light queue is empty.
    ray = _illuminate_front;
    if (ray != nullptr) {
        _illuminate_front = ray->next;
        ray->next = nullptr;
        if (_illuminate_front == nullptr) {
            _illuminate_back = nullptr;
        }
        _illuminate_size--;
        return ray;
    }

    // Pull from the intersection queue if the light queue is empty.
    ray = _intersect_front;
    if (ray != nullptr) {
        _intersect_front = ray->next;
        ray->next = nullptr;
        if (_intersect_front == nullptr) {
            _intersect_back = nullptr;
        }
        _intersect_size--; 
        return ray;
    }

    // Generate a primary ray if the intersection queue is empty.
    ray = new FatRay;
    if (_camera->GeneratePrimary(ray)) {
        _stats->intersects_produced++;
        return ray;
    }

    // No more primary rays.
    delete ray;
    return nullptr;
}

} // namespace fr
