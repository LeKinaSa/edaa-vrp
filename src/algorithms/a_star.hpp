
#ifndef A_STAR_H
#define A_STAR_H

#include <utility>
#include <list>
#include "../osm/osm.hpp"
#include "../graph.hpp"
#include "../types.hpp"

std::pair<std::list<u64>, double> aStarSearch(Graph<OsmNode> g, u64 start, u64 end);

std::pair<std::list<u64>, double> simpleMemoryBoundedAStarSearch(Graph<OsmNode> g, u64 start, u64 end, int maxSize);

#endif // A_STAR_H
