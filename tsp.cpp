#include <iostream>
#include <cinttypes>
#include <fstream>
#include <stack>
#include <math.h>
#include <random>
#include <algorithm>

#include "Graph.hpp"

constexpr bool GENERATE_RANDOM_CYLCE_TEST = true;
constexpr bool GENERATE_MST_BASE_CYCLE_TEST = true;

const std::vector<std::string> testCases {
    "xqf131", "xqg237", "pma343", "pka379", "bcl380",
    "pbl395", "pbk411", "pbn423", "pbm436", "xql662"
};

struct Coordinate {
    int_fast32_t x;
    int_fast32_t y;
};

typedef Graph<int_fast32_t> ProblemGraph;


std::string readTest(const std::string& testCase, std::vector<Coordinate>& points) {
    std::ifstream inputFile{"examples/" + testCase + ".tsp"};

    std::string fileName{};
    std::string token;
    inputFile >> token >> token >> fileName;

    while(token != "NODE_COORD_SECTION") {
        inputFile >> token;
    }

    inputFile >> token;
    while(token != "EOF") {
        int_fast32_t pointX, pointY;
        inputFile >> pointX >> pointY;
        points.push_back({pointX, pointY});

        inputFile >> token;
    }

    return fileName;
}


int_fast32_t distance(const Coordinate& firstPoint, const Coordinate& secondPoint) {
    return static_cast<int_fast32_t>(sqrt(pow(firstPoint.x - secondPoint.x, 2) + pow(firstPoint.y - secondPoint.y, 2)) + 0.5);
}

int_fast32_t cycleWeight(const std::vector<Coordinate> points, const std::vector<size_t> cycle) {
    int_fast32_t cycleWeight{0};

    for(size_t i = 0; i < cycle.size(); i++) {
        cycleWeight += distance(points[cycle[i] - 1], points[cycle[(i + 1) % cycle.size()] - 1]);
    }

    return cycleWeight;
}

ProblemGraph initializeGraph(const std::vector<Coordinate>& points) {
    ProblemGraph graph{points.size()};

    for(size_t firstVertex = 0; firstVertex < points.size(); firstVertex++) {
        for(size_t secondVertex = firstVertex + 1; secondVertex < points.size(); secondVertex++) {
            graph.addEdge(firstVertex + 1, secondVertex + 1, distance(points[firstVertex], points[secondVertex]));
        }
    }

    return graph;
}


void testRandomTsp(const std::vector<Coordinate>& points, std::string testCaseName) {
    constexpr size_t REPS{1000};

    std::mt19937 gen{std::random_device{}()};

    std::vector<int_fast32_t> randomCycleWeights{};
    for(size_t rep = 0; rep < REPS; rep++) {
        std::vector<size_t> randomCycle;
        for(size_t vertex = 1; vertex <= points.size(); vertex++) {
            randomCycle.push_back(vertex);
        }

        std::shuffle(randomCycle.begin(), randomCycle.end(), gen);
        randomCycleWeights.push_back(cycleWeight(points, randomCycle));
    }

    std::ofstream randomOutputFile{"results/" + testCaseName + "-rand.txt"};
    for(const auto& cycleWeight: randomCycleWeights) {
        randomOutputFile << cycleWeight << std::endl;
    }
}


void testMstTsp(const std::vector<Coordinate>& points, std::string testCaseName) {
    constexpr size_t startVertex{1};

    const ProblemGraph graph = initializeGraph(points);

    ProblemGraph mst{points.size()};
    int_fast32_t mstWeight = graph.minimumSpanningTree(mst, startVertex);

    std::vector<size_t> salesmanCycle{};
    graph.generateSalesmanCycleBasedOnMst(mst, salesmanCycle, startVertex);
    int_fast32_t salesmanCycleWeight{cycleWeight(points, salesmanCycle)};


    std::ofstream mstOutputFile{"results/" + testCaseName + "-mst.txt"};
    mstOutputFile << "weight: " << mstWeight << std::endl << "edges: " << std::endl;
    for(const auto& edge: mst.getEdges()) {
        mstOutputFile << edge.sourceVertex << " " << edge.targetVertex << std::endl;;
    }
    mstOutputFile << std::endl;

    std::ofstream tspMstOutputFile{"results/" + testCaseName + "-tsp-mst.txt"};
    tspMstOutputFile << "weight: " << salesmanCycleWeight << std::endl << "vertices-order: " << std::endl;
    for(size_t vertexPos = 0; vertexPos < salesmanCycle.size(); vertexPos++) {
        tspMstOutputFile << salesmanCycle[vertexPos] << std::endl;
    }
    tspMstOutputFile << std::endl;

    std::cout << "MST weight: " << mstWeight << std::endl << "salesman cycle weight: " << salesmanCycleWeight << std::endl << std::endl;
}


int main(int /*arg*/, char** /*argv*/) {
    for(const auto& testCase: testCases) {
        std::vector<Coordinate> points; // point with id ID is points[ID - 1]
        const std::string testCaseName = readTest(testCase, points);

        std::cout << testCaseName << std::endl;

        if(GENERATE_RANDOM_CYLCE_TEST) {
            testRandomTsp(points, testCaseName);
        }
        if(GENERATE_MST_BASE_CYCLE_TEST) {
            testMstTsp(points, testCaseName);
        }
    }

    return 0;
}
