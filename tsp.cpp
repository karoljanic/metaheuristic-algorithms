#include <iostream>
#include <fstream>
#include <thread>
#include <functional>

#include "utils.hpp"
#include "local-search.hpp"
#include "simulated-annealing.hpp"
#include "tabu-search.hpp"

constexpr bool GENERATE_RANDOM_CYCLE_TEST = false;
constexpr bool GENERATE_MST_BASE_CYCLE_TEST = false;

constexpr bool GENERATE_LOCAL_SEARCH_BEGIN_MST_TEST = false;
constexpr bool GENERATE_LOCAL_SEARCH_BEGIN_RANDOM_TEST = false;
constexpr bool GENERATE_LOCAL_SEARCH_BEGIN_RANDOM_NEIGHBOURHOOD_RANDOM_TEST = false;

constexpr bool TUNE_SIMULATED_ANNEALING = false;
constexpr bool GENERATE_SIMULATED_ANNEALING_TEST = true;

constexpr bool TUNE_TABU_SEARCH = false;
constexpr bool GENERATE_TABU_SEARCH_TEST = false;

const std::vector<std::string> testCases{
        /*"xqf131", "xqg237", "pma343",*/ "pka379", "bcl380",
                                          "pbl395", "pbk411", "pbn423", "pbm436", "xql662",
                                          "xit1083", "icw1483", "djc1785", "dcb2086", "pds2566"};

std::string readTest(const std::string &testCase, std::vector<Coordinate> &points) {
    std::ifstream inputFile{"examples/" + testCase + ".tsp"};

    std::string fileName{};
    std::string token;
    inputFile >> token >> token >> fileName;

    while (token != "NODE_COORD_SECTION") {
        inputFile >> token;
    }

    inputFile >> token;
    while (token != "EOF") {
        int_fast32_t pointX, pointY;
        inputFile >> pointX >> pointY;
        points.push_back({pointX, pointY});

        inputFile >> token;
    }

    return fileName;
}

void testRandomTsp(const std::vector<Coordinate> &points, const std::string &testCaseName) {
    constexpr size_t REPS{1000};

    std::mt19937 gen{std::random_device{}()};

    std::vector<int_fast32_t> randomCycleWeights{};
    for (size_t rep = 0; rep < REPS; rep++) {
        std::vector<size_t> randomCycle;
        for (size_t vertex = 1; vertex <= points.size(); vertex++) {
            randomCycle.push_back(vertex);
        }

        std::shuffle(randomCycle.begin(), randomCycle.end(), gen);
        randomCycleWeights.push_back(cycleWeight(points, randomCycle));
    }

    std::ofstream randomOutputFile{"results/" + testCaseName + "-rand.txt"};
    for (const auto &cycleWeight: randomCycleWeights) {
        randomOutputFile << cycleWeight << std::endl;
    }
}

void testMstTsp(const std::vector<Coordinate> &points, const std::string &testCaseName) {
    constexpr size_t startVertex{1};

    const ProblemGraph graph = initializeGraph(points);

    ProblemGraph mst{points.size()};
    int_fast32_t mstWeight = graph.minimumSpanningTree(mst, startVertex);

    std::vector<size_t> salesmanCycle{};
    graph.generateSalesmanCycleBasedOnMst(mst, salesmanCycle, startVertex);
    int_fast32_t salesmanCycleWeight{cycleWeight(points, salesmanCycle)};

    std::ofstream mstOutputFile{"results/" + testCaseName + "-mst.txt"};
    mstOutputFile << "weight: " << mstWeight << std::endl
                  << "edges: " << std::endl;

    for (const auto &edge: mst.getEdges()) {
        mstOutputFile << edge.sourceVertex << " " << edge.targetVertex << std::endl;
    }
    mstOutputFile << std::endl;

    std::ofstream tspMstOutputFile{"results/" + testCaseName + "-tsp-mst.txt"};
    tspMstOutputFile << "weight: " << salesmanCycleWeight << std::endl
                     << "vertices-order: " << std::endl;
    for (size_t vertexPos: salesmanCycle) {
        tspMstOutputFile << vertexPos << std::endl;
    }
    tspMstOutputFile << std::endl;
}

void testLocalSearchBasedOnMstInitTsp(const std::vector<Coordinate> &points, const std::string &testCaseName) {
    constexpr size_t mstStartVertex{1};

    const ProblemGraph graph = initializeGraph(points);

    ProblemGraph mst{points.size()};
    int_fast32_t mstWeight = graph.minimumSpanningTree(mst, mstStartVertex);

    std::ofstream lsMstOutputFile{"results/" + testCaseName + "-local-search-mst.txt"};
    lsMstOutputFile << "mst-weight: " << mstWeight << std::endl;

    const size_t reps = static_cast<size_t>(ceil(sqrt(points.size())));

    std::mt19937 gen{std::random_device{}()};
    std::uniform_int_distribution<size_t> dist(1, points.size());

    for (size_t rep = 0; rep < reps; rep++) {
        const size_t startVertex = dist(gen);
        std::vector<size_t> salesmanCycle{};
        graph.generateSalesmanCycleBasedOnMst(mst, salesmanCycle, startVertex);
        int_fast32_t initialSalesmanCycleWeight = cycleWeight(points, salesmanCycle);
        size_t improvements = localSearchWithFullNeighbourhood(salesmanCycle, points);
        int_fast32_t finalSalesmanCycleWeight = cycleWeight(points, salesmanCycle);

        lsMstOutputFile << "improvements: " << improvements << std::endl;
        lsMstOutputFile << "initial-cycle-weight: " << initialSalesmanCycleWeight << std::endl;
        lsMstOutputFile << "final-cycle-weight: " << finalSalesmanCycleWeight << std::endl;
        for (size_t vertexPos: salesmanCycle) {
            lsMstOutputFile << vertexPos << std::endl;
        }
        lsMstOutputFile << std::endl;
    }
}

void testLocalSearchBasedOnRandomInitTsp(const std::vector<Coordinate> &points, const std::string &testCaseName) {
    std::mt19937 gen{std::random_device{}()};

    const size_t reps = points.size();

    std::ofstream lsRandomOutputFile{"results/" + testCaseName + "-local-search-rand.txt"};

    for (size_t rep = 0; rep < reps; rep++) {
        std::vector<size_t> salesmanCycle;
        for (size_t vertex = 1; vertex <= points.size(); vertex++) {
            salesmanCycle.push_back(vertex);
        }
        std::shuffle(salesmanCycle.begin(), salesmanCycle.end(), gen);

        int_fast32_t initialSalesmanCycleWeight = cycleWeight(points, salesmanCycle);
        size_t improvements = localSearchWithFullNeighbourhood(salesmanCycle, points);
        int_fast32_t finalSalesmanCycleWeight = cycleWeight(points, salesmanCycle);

        lsRandomOutputFile << "improvements: " << improvements << std::endl;
        lsRandomOutputFile << "initial-cycle-weight: " << initialSalesmanCycleWeight << std::endl;
        lsRandomOutputFile << "final-cycle-weight: " << finalSalesmanCycleWeight << std::endl;
        for (size_t vertexPos: salesmanCycle) {
            lsRandomOutputFile << vertexPos << std::endl;
        }
        lsRandomOutputFile << std::endl;
    }
}

void testLocalSearchBasedOnRandomInitWithRandomNeighbourhoodTsp(const std::vector<Coordinate> &points,
                                                                const std::string &testCaseName) {
    std::mt19937 gen{std::random_device{}()};

    const size_t reps = points.size();

    std::ofstream lsRandomOutputFile{"results/" + testCaseName + "-local-search-rand-rand-neighbourhood.txt"};

    for (size_t rep = 0; rep < reps; rep++) {
        std::vector<size_t> salesmanCycle;
        for (size_t vertex = 1; vertex <= points.size(); vertex++) {
            salesmanCycle.push_back(vertex);
        }
        std::shuffle(salesmanCycle.begin(), salesmanCycle.end(), gen);

        int_fast32_t initialSalesmanCycleWeight = cycleWeight(points, salesmanCycle);
        size_t improvements = localSearchWithRandomPartNeighbourhood(salesmanCycle, points);
        int_fast32_t finalSalesmanCycleWeight = cycleWeight(points, salesmanCycle);

        lsRandomOutputFile << "improvements: " << improvements << std::endl;
        lsRandomOutputFile << "initial-cycle-weight: " << initialSalesmanCycleWeight << std::endl;
        lsRandomOutputFile << "final-cycle-weight: " << finalSalesmanCycleWeight << std::endl;
        for (size_t vertexPos: salesmanCycle) {
            lsRandomOutputFile << vertexPos << std::endl;
        }
        lsRandomOutputFile << std::endl;
    }
}

void testSimulatedAnnealing(const std::vector<Coordinate> &points, const std::string &testCaseName,
                            InitialSolutionType initialSolution) {

    constexpr double alpha{0.5};
    constexpr double beta{0.95};
    constexpr double gamma{0.2};
    constexpr double delta{0.1};
    constexpr size_t reps{100};
    const ProblemGraph graph = initializeGraph(points);

    std::mt19937 gen{std::random_device{}()};

    std::string outputFileName;
    if (initialSolution == InitialSolutionType::RANDOM) {
        outputFileName = testCaseName + "-sa.txt";
    } else {
        outputFileName = testCaseName + "-sa-mst.txt";
    }

    std::ofstream outputFile{"results/" + outputFileName};

    SaParallelRunResult result = simulatedAnnealingOnThreads(points, initialSolution,
                                                             alpha, beta, gamma, delta,
                                                             reps, gen, graph);

    outputFile << alpha << " " << beta << " " << gamma << " " << delta << " " << result.minWeight << " "
               << result.avgWeight << " " << result.timePerRepetition << std::endl;

    for (size_t vertexPos: result.cycleWithMinWeight) {
        outputFile << vertexPos << std::endl;
    }
}

void tuneTabuSearch(const std::vector<Coordinate> &points, const std::string &testCaseName,
                    InitialSolutionType initialSolution) {
    const size_t reps{100};
    const ProblemGraph graph = initializeGraph(points);

    std::mt19937 gen{std::random_device{}()};

    std::string outputFileName;
    if (initialSolution == InitialSolutionType::RANDOM) {
        outputFileName = testCaseName + "-ts-tuning2.txt";
    } else {
        outputFileName = testCaseName + "-ts-mst-tuning2.txt";
    }

    std::ofstream outputFile{"results/" + outputFileName};

    for (const double alpha: {0.01, 0.03, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0}) {
        for (const double beta: {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0}) {
            auto result = tabuSearchWithFullNeighbourhoodOnThreads(points, initialSolution,
                                                                   alpha, beta, reps, gen, graph);
            outputFile << alpha << " " << beta << " " << result.minWeight << " " << result.avgWeight << " "
                       << result.timePerRepetition << std::endl;

            std::cout << "Alpha: " << alpha << " Beta: " << beta << std::endl;
        }
    }
}

void testTabuSearch(const std::vector<Coordinate> &points, const std::string &testCaseName,
                    InitialSolutionType initialSolution, bool fullNeighbourhood) {

    constexpr double alpha{0.6};
    constexpr double beta{0.3};
    const size_t reps{100};
    const ProblemGraph graph = initializeGraph(points);

    std::mt19937 gen{std::random_device{}()};

    std::string outputFileName;
    if (initialSolution == InitialSolutionType::RANDOM) {
        if (fullNeighbourhood) {
            outputFileName = testCaseName + "-ts-full-neighbourhood.txt";
        } else {
            outputFileName = testCaseName + "-ts-rand-neighbourhood.txt";
        }
    } else {
        if (fullNeighbourhood) {
            outputFileName = testCaseName + "-ts-mst-full-neighbourhood.txt";
        } else {
            outputFileName = testCaseName + "-ts-mst-rand-neighbourhood.txt";
        }
    }

    std::ofstream outputFile{"results/" + outputFileName};

    TSParallelRunResult result;
    if (fullNeighbourhood) {
        result = tabuSearchWithFullNeighbourhoodOnThreads(points, initialSolution,
                                                          alpha, beta, reps, gen, graph);
    } else {
        result = tabuSearchWithRandomPartNeighbourhoodOnThreads(points, initialSolution,
                                                                alpha, beta, reps, gen, graph);
    }

    outputFile << alpha << " " << beta << " " << result.minWeight << " " << result.avgWeight << " "
               << result.timePerRepetition << std::endl;

    for (size_t vertexPos: result.cycleWithMinWeight) {
        outputFile << vertexPos << std::endl;
    }
}

int main(int /*arg*/, char ** /*argv*/) {
    for (const auto &testCase: testCases) {
        std::vector<Coordinate> points; // point with id ID is points[ID - 1]
        const std::string testCaseName = readTest(testCase, points);

        std::cout << testCaseName << std::endl;

        if (GENERATE_RANDOM_CYCLE_TEST) {
            testRandomTsp(points, testCaseName);
        }
        if (GENERATE_MST_BASE_CYCLE_TEST) {
            testMstTsp(points, testCaseName);
        }
        if (GENERATE_LOCAL_SEARCH_BEGIN_MST_TEST) {
            testLocalSearchBasedOnMstInitTsp(points, testCaseName);
        }
        if (GENERATE_LOCAL_SEARCH_BEGIN_RANDOM_TEST) {
            testLocalSearchBasedOnRandomInitTsp(points, testCaseName);
        }
        if (GENERATE_LOCAL_SEARCH_BEGIN_RANDOM_NEIGHBOURHOOD_RANDOM_TEST) {
            testLocalSearchBasedOnRandomInitWithRandomNeighbourhoodTsp(points, testCaseName);
        }
        if (TUNE_SIMULATED_ANNEALING) {
            if (points.size() == 343) {
                tuneSimulatedAnnealing(points, testCaseName, InitialSolutionType::RANDOM);
            }
        }
        if (GENERATE_SIMULATED_ANNEALING_TEST) {
            testSimulatedAnnealing(points, testCaseName, InitialSolutionType::RANDOM);
        }
        if (TUNE_TABU_SEARCH) {
            if (points.size() <= 1000) {
                tuneTabuSearch(points, testCaseName, InitialSolutionType::MST_BASED);
            }
        }
        if (GENERATE_TABU_SEARCH_TEST) {
            testTabuSearch(points, testCaseName, InitialSolutionType::RANDOM, true);
            testTabuSearch(points, testCaseName, InitialSolutionType::RANDOM, false);
            testTabuSearch(points, testCaseName, InitialSolutionType::MST_BASED, true);
            testTabuSearch(points, testCaseName, InitialSolutionType::MST_BASED, false);
        }
    }

    return 0;
}
