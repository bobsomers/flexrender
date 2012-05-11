#pragma once

namespace fr {

struct FatRay;
class NetNode;

struct RayForward {
    explicit RayForward(FatRay* ray, NetNode* node) :
     ray(ray),
     node(node) {}

    /// What are we sending?
    FatRay* ray;

    /// Where are we sending it?
    NetNode* node;
};

} // namespace fr
