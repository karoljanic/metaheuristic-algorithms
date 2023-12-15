#ifndef SIMULATED_ANNEALING_HPP
#define SIMULATED_ANNEALING_HPP

#include "utils.hpp"


struct SaParallelRunResult {
    double minWeight;
    double avgWeight;
    double timePerRepetition;
};

void simulatedAnnealing(std::vector<size_t> &cycle, const std::vector<Coordinate> &points,
                        double temperature, double alpha, double beta, double gamma) {
    const size_t n{cycle.size()};
    const size_t maxEpochSize{static_cast<size_t>(static_cast<double>(n) * beta)};
    const size_t maxIterationsWithoutImprovement{static_cast<size_t>(static_cast<double>(n) * gamma)};

    std::vector<std::pair<size_t, size_t>> inversions;
    for (size_t i = 0; i < n - 1; i++) {
        for (size_t j = i + 1; j < n; j++) {
            inversions.emplace_back(i, j);
        }
    }

    std::mt19937 gen{std::random_device{}()};
    std::uniform_int_distribution<size_t> intDist(0, inversions.size() - 1);

    int_fast32_t currentCycleChange{0};
    int_fast32_t bestCycleChange{0};
    std::vector<size_t> bestCycle{cycle};

    size_t iterationsWithoutImprovement{0};
    while (iterationsWithoutImprovement < maxIterationsWithoutImprovement) {
        iterationsWithoutImprovement++;
        size_t currentIteration{0};
        while (currentIteration < maxEpochSize) {
            const auto neighbour = inversions[intDist(gen)];
            size_t indexi = neighbour.first;
            size_t indexj = neighbour.second;

            if (indexi == 0 && indexj == (n - 1)) {
                continue;
            }

            int_fast32_t change{0};

            size_t prevIndexi = (indexi == 0) ? (n - 1) : indexi - 1;
            size_t nextIndexj = (indexj == (n - 1)) ? 0 : indexj + 1;

            change -= distance(points[cycle[prevIndexi] - 1], points[cycle[indexi] - 1]);
            change -= distance(points[cycle[indexj] - 1], points[cycle[nextIndexj] - 1]);

            change += distance(points[cycle[indexi] - 1], points[cycle[nextIndexj] - 1]);
            change += distance(points[cycle[prevIndexi] - 1], points[cycle[indexj] - 1]);

            if (change < 0) {
                std::reverse(cycle.begin() + indexi, cycle.begin() + indexj + 1);
                iterationsWithoutImprovement = 0;
                currentIteration++;
                currentCycleChange += change;

                if (currentCycleChange < bestCycleChange) {
                    bestCycleChange = currentCycleChange;
                    bestCycle = cycle;
                }
            } else {
                std::uniform_real_distribution<double> realDist(0.0, 1.0);
                if (realDist(gen) < std::exp(-change / temperature)) {
                    std::reverse(cycle.begin() + indexi, cycle.begin() + indexj + 1);
                    currentIteration++;
                    currentCycleChange += change;
                }
            }
        }

        temperature *= alpha;
    }

    cycle = bestCycle;
}

SaParallelRunResult
simulatedAnnealingOnThreads(const std::vector<Coordinate> &points, InitialSolutionType initialSolution,
                            double temperature, double alpha, double beta, double gamma, size_t repetitions,
                            std::mt19937 &generator, const ProblemGraph &graph) {
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    std::vector<std::vector<size_t>> cycles(repetitions, std::vector<size_t>());
    if (initialSolution == InitialSolutionType::RANDOM) {
        for (size_t rep = 0; rep < repetitions; rep++) {
            for (size_t vertex = 1; vertex <= points.size(); vertex++) {
                cycles[rep].push_back(vertex);
            }
            std::shuffle(cycles[rep].begin(), cycles[rep].end(), generator);
        }
    } else {
        std::uniform_int_distribution<size_t> dist(1, points.size());
        for (size_t rep = 0; rep < repetitions; rep++) {
            size_t mstStartVertex = dist(generator);
            ProblemGraph mst{points.size()};
            graph.minimumSpanningTree(mst, mstStartVertex);
            mst.generateSalesmanCycleBasedOnMst(mst, cycles[rep], mstStartVertex);
        }
    }

    for (size_t rep = 0; rep < repetitions; rep++) {
        threads.emplace_back(simulatedAnnealing, std::ref(cycles[rep]), std::ref(points), temperature,
                             alpha, beta, gamma);
    }

    for (auto &thread: threads) {
        thread.join();
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::vector<int_fast32_t> results(repetitions);
    for (size_t rep = 0; rep < repetitions; rep++) {
        results[rep] = cycleWeight(points, cycles[rep]);
    }

    std::chrono::duration<double> elapsed = end - start;

    return {static_cast<double>(*std::min_element(results.begin(), results.end())),
            std::accumulate(results.begin(), results.end(), 0.0) / static_cast<double>(repetitions),
            static_cast<double>(elapsed.count()) / static_cast<double>(repetitions)};

}

#endif // SIMULATED_ANNEALING_HPP