#include "utils/cmdline.hpp"

#include <algorithm>

using std::string;
using std::find;

namespace fr {
namespace cmdline {

string FlagValue(int argc, const char* argv[], const string& flag_short,
 const string& flag_long) {
    const char **begin = argv;
    const char **end = argv + argc;
    const char **iter = nullptr;

    // Look for short flag value.
    iter = find(begin, end, flag_short);
    if (iter != end && ++iter != end) {
        return string(*iter);
    }

    // Look for long flag value.
    iter = find(begin, end, flag_long);
    if (iter != end && ++iter != end) {
        return string(*iter);
    }

    // Didn't find it.
    return string("");
}

bool FlagExists(int argc, const char* argv[], const string &flag_short,
 const string& flag_long) {
    const char **begin = argv;
    const char **end = argv + argc;
    const char **iter = nullptr;

    // Look for short flag.
    iter = find(begin, end, flag_short);
    if (iter != end) {
        return true;
    }

    // Look for long option.
    iter = find(begin, end, flag_long);
    if (iter != end) {
        return true;
    }

    // Didn't find it.
    return false;
}

string Argument(int argc, const char* argv[], int i) {
    int count = 0;
    for (int j = 0; j < argc; j++) {
        const char* str = argv[j];
        if (str[0] != '-') {
            if (count == i) {
                return string(str);
            } else {
                count++;
            }
        }
    }
    return string("");
}

} // namespace cmdline
} // namespace flexrender
