#include "utils/cmdline.hpp"

#include <algorithm>

using std::string;
using std::find;

namespace fr {

string FlagValue(int argc, char* argv[], const string& flag_short,
 const string& flag_long) {
    char **begin = argv;
    char **end = argv + argc;
    char **iter = nullptr;

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

bool FlagExists(int argc, char* argv[], const string &flag_short,
 const string& flag_long) {
    char **begin = argv;
    char **end = argv + argc;
    char **iter = nullptr;

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

string ArgumentValue(int argc, char* argv[], int i) {
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

} // namespace flexrender
