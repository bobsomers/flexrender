#include "types/linear_node.hpp"

#include <limits>

using std::numeric_limits;

namespace fr {

LinearNode::LinearNode() :
 bounds(),
 sibling(-1),
 parent(-1) {
    leaf = numeric_limits<uint64_t>::max();
    index = numeric_limits<size_t>::max();
    axis = numeric_limits<uint64_t>::max();
}

} // namespace fr
