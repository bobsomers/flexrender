#include "types/traversal_state.hpp"

#include <iostream>
#include <sstream>

using std::string;
using std::stringstream;
using std::endl;

namespace fr {

TraversalState::TraversalState() :
 current(0),
 state(State::NONE),
 hit(0) {}

string ToString(const TraversalState& state, const string& indent) {
    stringstream stream;
    stream << "TraversalState {" << endl <<
     indent << "| current = " << state.current << endl <<
     indent << "| state = ";
    switch (state.state) {
        case TraversalState::State::NONE:
            stream << "NONE";
            break;

        case TraversalState::State::FROM_PARENT:
            stream << "FROM_PARENT";
            break;

        case TraversalState::State::FROM_SIBLING:
            stream << "FROM_SIBLING";
            break;

        case TraversalState::State::FROM_CHILD:
            stream << "FROM_CHILD";
            break;

        default:
            stream << "UNKNOWN";
            break;
    }
    stream << endl << indent << "| hit = " << (state.hit ? "YES" : "NO") << endl <<
     indent << "}";
    return stream.str();
}

} // namespace fr
