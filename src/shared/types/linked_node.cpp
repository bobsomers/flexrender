#include "types/linked_node.hpp"

#include <iostream>
#include <sstream>

using std::string;
using std::stringstream;
using std::endl;

namespace fr {

LinkedNode::LinkedNode(size_t index, const BoundingBox& bounds) :
 bounds(bounds),
 index(index) {
    children[0] = nullptr;
    children[1] = nullptr;
}

LinkedNode::LinkedNode(LinkedNode* left, LinkedNode* right,
 BoundingBox::Axis split) :
 index(0),
 split(split) {
    assert(left != nullptr);
    assert(right != nullptr);

    children[0] = left;
    children[1] = right;

    bounds = left->bounds.Union(right->bounds);
}

string ToString(const LinkedNode* node, const string& indent) {
    stringstream stream;
    string pad = indent + "| ";
    stream << "LinkedNode {" << endl <<
     indent << "| bounds = " << ToString(node->bounds, pad) << endl;
    if (node->children[0] != nullptr) {
        // Interior node.
        stream << indent << "| split = " <<
         ((node->split == BoundingBox::Axis::X) ? "X" :
         ((node->split == BoundingBox::Axis::Y) ? "Y" : "Z")) << endl <<
         indent << "| children[0] = " << ToString(node->children[0], pad) << endl <<
         indent << "| children[1] = " << ToString(node->children[1], pad) << endl;
    } else {
        // Leaf node.
        stream << indent << "| index = " << node->index << endl;
    }
    stream << indent << "}";
    return stream.str();
}

} // namespace fr
