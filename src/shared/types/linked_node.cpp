#include "types/linked_node.hpp"

namespace fr {

LinkedNode::LinkedNode(size_t first, size_t second, const BoundingBox& bounds) :
 bounds(bounds) {
    children[0] = nullptr;
    children[1] = nullptr;

    primitives[0] = first;
    primitives[1] = second;
}

LinkedNode::LinkedNode(LinkedNode* left, LinkedNode* right) {
    assert(left != nullptr);
    assert(right != nullptr);

    primitives[0] = 0;
    primitives[1] = 0;

    children[0] = left;
    children[1] = right;

    bounds = left->bounds.Union(right->bounds);
}

} // namespace fr
