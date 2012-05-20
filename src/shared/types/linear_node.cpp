#include "types/linear_node.hpp"

namespace fr {

LinearNode::LinearNode(const BoundingBox& bounds, size_t primitive) :
 bounds(bounds),
 primitive(primitive),
 offset(0) {}

} // namespace fr
