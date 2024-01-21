#ifndef UTILS_HPP
#define UTILS_HPP

#include <cmath>
#include <random>
#include <algorithm>
#include <utility>
#include <chrono>
#include <thread>

#include "graph.hpp"

typedef Graph<int_fast32_t> ProblemGraph;

struct Coordinate {
    int_fast32_t x;
    int_fast32_t y;
};

enum InitialSolutionType {
    RANDOM,
    MST_BASED
};

int_fast32_t distance(const Coordinate &firstPoint, const Coordinate &secondPoint) {
    return static_cast<int_fast32_t>(sqrt(pow(firstPoint.x - secondPoint.x, 2) + pow(firstPoint.y - secondPoint.y, 2)) +
                                     0.5);
}

int_fast32_t cycleWeight(const std::vector<Coordinate> &points, const std::vector<size_t> &cycle) {
    int_fast32_t cycleWeight{0};

    for (size_t i = 0; i < cycle.size(); i++) {
        cycleWeight += distance(points[cycle[i] - 1], points[cycle[(i + 1) % cycle.size()] - 1]);
    }

    return cycleWeight;
}

ProblemGraph initializeGraph(const std::vector<Coordinate> &points) {
    ProblemGraph graph{points.size()};

    for (size_t firstVertex = 0; firstVertex < points.size(); firstVertex++) {
        for (size_t secondVertex = firstVertex + 1; secondVertex < points.size(); secondVertex++) {
            graph.addEdge(firstVertex + 1, secondVertex + 1, distance(points[firstVertex], points[secondVertex]));
        }
    }

    return graph;
}

#endif // UTILS_HPP