
#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include "types.hpp"
#include <chrono>
#include <mutex>

inline double degToRad(double deg) {
    return deg * M_PI / 180.0;
}

template <typename T>
u64 interval(const std::chrono::high_resolution_clock::time_point& start,
        const std::chrono::high_resolution_clock::time_point& end) {
    return std::chrono::duration_cast<T>(end - start).count();
}

class AtomicOStream {
    public:
        AtomicOStream(std::ostream& os) : os(os) {}

        template <typename T>
        AtomicOStream& operator<<(const T& x) {
            std::lock_guard<std::mutex> lock(mut);
            os << x;
            return *this;
        }

        AtomicOStream& flush() {
            os.flush();
            return *this;
        }
    private:
        std::ostream& os;
        std::mutex mut;
};

#endif // UTILS_H
