
#ifndef UTILS_H
#define UTILS_H

#include "types.hpp"
#include <cmath>
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

// Adapted from https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
inline void hashCombine(std::size_t& seed) {}

template<typename T, typename... Rest>
void hashCombine(size_t& seed, const T& v, Rest... rest) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    hashCombine(seed, rest...);
}

struct PairHash {
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        size_t seed = 0;
        hashCombine(seed, p.first, p.second);
        return seed;
    }
};

#endif // UTILS_H
