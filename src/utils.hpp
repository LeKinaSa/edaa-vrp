
#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include "types.hpp"
#include <chrono>

inline double degToRad(double deg) {
    return deg * M_PI / 180.0;
}

template <typename T>
u64 interval(const std::chrono::high_resolution_clock::time_point& start,
        const std::chrono::high_resolution_clock::time_point& end) {
    return std::chrono::duration_cast<T>(end - start).count();
}

#endif // UTILS_H
