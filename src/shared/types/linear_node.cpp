#include "types/linear_node.hpp"

#include <limits>
#include <iostream>
#include <sstream>

using std::numeric_limits;
using std::string;
using std::stringstream;
using std::endl;

namespace fr {

LinearNode::LinearNode() :
 bounds(),
 parent(-1),
 right(-1) {
    leaf = numeric_limits<uint64_t>::max();
    index = numeric_limits<size_t>::max();
    axis = numeric_limits<uint64_t>::max();
}

string ToString(const LinearNode& node, const string& indent) {
    stringstream stream;
    string pad = indent + "| ";
    stream << "LinearNode {" << endl <<
     indent << "| bounds = " << ToString(node.bounds, pad) << endl <<
     indent << "| leaf = " << node.leaf << endl <<
     indent << "| index = " << node.index << endl <<
     indent << "| parent = " << node.parent << endl <<
     indent << "| right = " << node.right << endl <<
     indent << "| axis = " << node.axis << endl <<
     indent << "}";
    return stream.str();
}

} // namespace fr
