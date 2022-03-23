
#ifndef A_STAR_H
#define A_STAR_H

#include <utility>
#include <list>
#include "../types.hpp"
#include "../graph.hpp"

template <typename T>
std::pair<std::list<u64>, double> aStarSearch(Graph<T> g, u64 start, u64 end);

#endif // A_STAR_H
