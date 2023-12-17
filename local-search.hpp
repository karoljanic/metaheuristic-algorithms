#ifndef LOCAL_SEARCH_HPP
#define LOCAL_SEARCH_HPP

#include "utils.hpp"

struct LSParallelRunResult {
    double minWeight;
    double avgWeight;
    double timePerRepetition;
    size_t improvements;
};

size_t localSearchWithFullNeighbourhood(std::vector<size_t> &cycle, const std::vector<Coordinate> &points) {
    const size_t n{cycle.size()};
    size_t improvements{0};

    bool foundNewCandidate{true};
    while (foundNewCandidate) {
        foundNewCandidate = false;
        int_fast32_t newCandidateWeightChange{0};
        size_t besti, bestj;

        for (size_t indexi = 0; indexi < (n - 1); indexi++) {
            for (size_t indexj = indexi + 1; indexj < n; indexj++) {
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

                if (change < newCandidateWeightChange) {
                    newCandidateWeightChange = change;
                    foundNewCandidate = true;
                    besti = indexi;
                    bestj = indexj;
                }
            }
        }

        if (foundNewCandidate) {
            std::reverse(cycle.begin() + besti, cycle.begin() + bestj + 1);
            improvements++;
        }
    }

    return improvements;
}

LSParallelRunResult
localSearchWithFullNeighbourhoodOnThreads(const std::vector<Coordinate> &points, size_t repetitions) {

}


size_t localSearchWithRandomPartNeighbourhood(std::vector<size_t> &cycle, const std::vector<Coordinate> &points) {
    const size_t n{cycle.size()};
    size_t improvements{0};

    std::mt19937 gen{std::random_device{}()};
    std::uniform_int_distribution<size_t> dist(0, n - 1);

    bool foundNewCandidate{true};
    while (foundNewCandidate) {
        foundNewCandidate = false;
        int_fast32_t newCandidateWeightChange{0};
        size_t besti, bestj;

        for (size_t invNum = 0; invNum < n; invNum++) {
            size_t indexi = dist(gen);
            size_t indexj = dist(gen);

            if (indexi == 0 && indexj == (n - 1)) {
                continue;
            } else if (indexi == indexj) {
                continue;
            } else if (indexi > indexj) {
                const size_t tmp{indexi};
                indexi = indexj;
                indexj = tmp;
            }

            int_fast32_t change{0};

            size_t prevIndexi = (indexi == 0) ? (n - 1) : indexi - 1;
            size_t nextIndexj = (indexj == (n - 1)) ? 0 : indexj + 1;

            change -= distance(points[cycle[prevIndexi] - 1], points[cycle[indexi] - 1]);
            change -= distance(points[cycle[indexj] - 1], points[cycle[nextIndexj] - 1]);

            change += distance(points[cycle[indexi] - 1], points[cycle[nextIndexj] - 1]);
            change += distance(points[cycle[prevIndexi] - 1], points[cycle[indexj] - 1]);

            if (change < newCandidateWeightChange) {
                newCandidateWeightChange = change;
                foundNewCandidate = true;
                besti = indexi;
                bestj = indexj;
            }
        }

        if (foundNewCandidate) {
            std::reverse(cycle.begin() + besti, cycle.begin() + bestj + 1);
            improvements++;
        }
    }

    return improvements;
}

LSParallelRunResult
localSearchWithRandomPartNeighbourhoodOnThreads(const std::vector<Coordinate> &points, size_t repetitions) {

}

#endif // LOCAL_SEARCH_HPP