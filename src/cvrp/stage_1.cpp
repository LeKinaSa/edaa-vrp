
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <mutex>
#include <queue>
#include <thread>
#include "stage_1.hpp"
#include "../algorithms/a_star.hpp"
#include "../data_structures/quadtree.hpp"
#include "../data_structures/kd_tree.hpp"
#include "../utils.hpp"

using namespace std;
using chrono::high_resolution_clock;
using chrono::microseconds;
using chrono::nanoseconds;

MapMatchingResult matchLocations(const OsmXmlData& osmData,
        const CvrpInstance& problem, MapMatchingDataStructure dataStructure,
        bool printLogs) {
    static const char* filePath = "matching.txt";
    ofstream ofs(filePath);

    u64 originNode = 0;
    vector<u64> deliveryNodes;
    const u32 numDeliveries = problem.getDeliveries().size();
    deliveryNodes.reserve(numDeliveries);

    // Log-related variables
    const u32 nnIterations = 1 + numDeliveries;
    u64 timeElapsed = 0;
    high_resolution_clock::time_point start, end;
    u64 intv;

    if (dataStructure == QUADTREE) {
        Quadtree tree(AABB(osmData.minCoords, osmData.maxCoords));
        for (const pair<u64, const OsmNode&>& p : osmData.graph.getNodes()) {
            if (p.second.mapMatch) {
                tree.insert(p.second);
            }
        }

        start = high_resolution_clock::now();
        originNode = tree.nearestNeighbor(problem.getOrigin())->id;
        end = high_resolution_clock::now();
        intv = interval<nanoseconds>(start, end);
        timeElapsed += intv;
        if (printLogs) ofs << intv << " ";

        for (const CvrpDelivery& delivery : problem.getDeliveries()) {
            start = high_resolution_clock::now();
            const OsmNode* node = tree.nearestNeighbor(delivery.coordinates);
            end = high_resolution_clock::now();
            intv = interval<nanoseconds>(start, end);
            timeElapsed += intv;
            if (printLogs) ofs << intv << " ";

            if (node) {
                deliveryNodes.push_back(node->id);
            }
            else {
                deliveryNodes.push_back(0);
            }
        }
    }
    else {
        vector<reference_wrapper<const OsmNode>> v;
        v.reserve(osmData.graph.getNodes().size());

        for (const pair<u64, const OsmNode&>& p : osmData.graph.getNodes()) {
            if (p.second.mapMatch) {
                v.push_back(p.second);
            }
        }

        KDTree tree(v);

        start = high_resolution_clock::now();
        originNode = tree.nearestNeighbor(problem.getOrigin())->id;
        end = high_resolution_clock::now();
        intv = interval<nanoseconds>(start, end);
        timeElapsed += intv;
        if (printLogs) ofs << intv << " ";

        for (const CvrpDelivery& delivery : problem.getDeliveries()) {
            start = high_resolution_clock::now();
            const OsmNode* node = tree.nearestNeighbor(delivery.coordinates);
            end = high_resolution_clock::now();
            intv = interval<nanoseconds>(start, end);
            timeElapsed += intv;
            if (printLogs) ofs << intv << " ";

            if (node) {
                deliveryNodes.push_back(node->id);
            }
            else {
                deliveryNodes.push_back(0);
            }
        }
    }

    if (printLogs) {
        cout << "Map matched " << nnIterations << " nodes in "
            << timeElapsed / 1000 << "us (average of " << timeElapsed / nnIterations
            << "ns per iteration)\n";
    }
    ofs.close();

    return {originNode, deliveryNodes};
}

struct DijkstraThreadData {
    DijkstraThreadData(const OsmXmlData& osmData, CvrpInstance& problem,
            const MapMatchingResult& mmResult, bool printLogs, ofstream& ofs) : osmData(osmData),
            problem(problem), mmResult(mmResult), printLogs(printLogs),
            aStdOut(cout), aOfs(ofs) {}

    // Concurrency
    bool terminated = false;
    condition_variable cond;
    mutex queueMutex;
    queue<u64> jobQueue;
    AtomicOStream aStdOut, aOfs;

    // Data
    const OsmXmlData& osmData;
    CvrpInstance& problem;
    const MapMatchingResult& mmResult;
    bool printLogs;
};

size_t matchedPoint(const MapMatchingResult& mmResult, size_t idx) {
    if (idx == 0) {
        return mmResult.originNode;
    }
    return mmResult.deliveryNodes[idx - 1];
}

void dijkstraThreadJob(DijkstraThreadData* data, ShortestPathDataStructure dataStructure) {
    u64 from;
    size_t n = 1 + data->problem.getDeliveries().size();

    while (true) {
        {
            unique_lock<mutex> lock(data->queueMutex);

            data->cond.wait(lock, [data] {
                return !data->jobQueue.empty() || data->terminated;
            });

            if (data->jobQueue.empty()) {
                // Pool terminated and no more jobs to run
                return;
            }

            from = data->jobQueue.front();
            data->jobQueue.pop();
        }

        vector<u64> endVec;
        for (size_t to = 0; to < n; ++to) {
            if (from != to) {
                endVec.push_back(matchedPoint(data->mmResult, to));
            }
        }

        auto start = high_resolution_clock::now();
        vector<ShortestPathResult> resultVec = dijkstra(
            data->osmData.graph,
            matchedPoint(data->mmResult, from),
            endVec,
            dataStructure
        );
        auto end = high_resolution_clock::now();
        if (data->printLogs) {
            auto us = interval<chrono::microseconds>(start, end);
            data->aStdOut << "Finished Dijkstra for location " << from << " in " << us << "us." << "\n";
            data->aStdOut.flush();
            data->aOfs << us << " ";
        }

        for (size_t idx = 0; idx < resultVec.size(); ++idx) {
            size_t to = idx >= from ? idx + 1 : idx;
            const auto& result = resultVec[idx];

            if (result.path.size() == 0) {
                // Couldn't find a path...
                data->problem.setDistance(from, to, DBL_MAX);
            }
            else {
                data->problem.setDistance(from, to, result.distance);
            }
        }
    }
}

void calculateShortestPaths(const OsmXmlData& osmData, CvrpInstance& problem,
        const MapMatchingResult& mmResult, ShortestPathDataStructure dataStructure,
        bool printLogs, u32 numThreads, const string& filePath) {
    ofstream ofs(filePath);

    // Multithreading support
    DijkstraThreadData threadData(osmData, problem, mmResult, printLogs, ofs);

    vector<thread> threads;
    threads.reserve(numThreads);
    for (u32 _ = 0; _ < numThreads; ++_) {
        threads.push_back(thread(dijkstraThreadJob, &threadData, dataStructure));
    }

    size_t n = 1 + problem.getDeliveries().size();

    // We can't assume d[from, to] == d[to, from] since there are directed edges
    for (size_t from = 0; from < n; ++from) {
        {
            unique_lock<mutex> lock(threadData.queueMutex);
            threadData.jobQueue.push(from);
        }
        threadData.cond.notify_one();
    } 

    // Shutdown thread pool and join all threads
    threadData.terminated = true;
    threadData.cond.notify_all();
    for (thread& t : threads) {
        t.join();
    }
    threads.clear();

    ofs.close();
}