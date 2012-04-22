#pragma once

namespace fr {

// Thanks to Scott Meyers for this.
// Effective C++, 3rd Edition
// Item #6
class Uncopyable {
protected:
    Uncopyable() {}
    ~Uncopyable() {}

private:
    Uncopyable(const Uncopyable &);
    const Uncopyable& operator=(const Uncopyable& other);
};

} // namespace fr
