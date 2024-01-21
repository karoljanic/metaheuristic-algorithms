#ifndef GENETIC_HPP
#define GENETIC_HPP

#include <vector>
#include <utility>
#include <map>

#include "utils.hpp"
#include "tabu-search.hpp"
#include "local-search.hpp"

enum class CrossType {
    PARTIALLY_MAPPED_CROSSOVER,
    ORDER_CROSSOVER,
    CYCLE_CROSSOVER
};

typedef std::pair<std::vector<size_t>, int_fast32_t> Individual;

std::pair<Individual, Individual>
pmx(const Individual &individual1, const Individual &individual2, std::mt19937 &generator) {
    std::uniform_int_distribution<size_t> dist(1, individual1.first.size() - 2);
    size_t index1 = dist(generator);
    size_t index2 = dist(generator);
    while (index1 == index2) {
        index2 = dist(generator);
    }
    if (index1 > index2) {
        std::swap(index1, index2);
    }
    index1 = 3;
    index2 = 5;

    std::vector<size_t> child1(individual1.first.size(), 0);
    std::vector<size_t> child2(individual1.first.size(), 0);

    std::map<size_t, size_t> mapping1{};
    std::map<size_t, size_t> mapping2{};
    for (size_t i = index1; i <= index2; i++) {
        child1[i] = individual2.first[i];
        child2[i] = individual1.first[i];

        mapping2[individual1.first[i]] = individual2.first[i];
        mapping1[individual2.first[i]] = individual1.first[i];
    }

    for (size_t i = 0; i < index1; i++) {
        size_t index = individual1.first[i];
        if (mapping1.find(index) == mapping1.end()) {
            child1[i] = index;
        } else {
            while (mapping1.find(index) != mapping1.end()) {
                index = mapping1[index];
            }
            child1[i] = index;
        }

        index = individual2.first[i];
        if (mapping2.find(index) == mapping2.end()) {
            child2[i] = index;
        } else {
            while (mapping2.find(index) != mapping2.end()) {
                index = mapping2[index];
            }
            child2[i] = index;
        }
    }


    for (size_t i = index2 + 1; i < individual1.first.size(); i++) {
        size_t index = individual1.first[i];
        if (mapping1.find(index) == mapping1.end()) {
            child1[i] = index;
        } else {
            while (mapping1.find(index) != mapping1.end()) {
                index = mapping1[index];
            }
            child1[i] = index;
        }

        index = individual2.first[i];
        if (mapping2.find(index) == mapping2.end()) {
            child2[i] = index;
        } else {
            while (mapping2.find(index) != mapping2.end()) {
                index = mapping2[index];
            }
            child2[i] = index;
        }
    }

    return std::make_pair(Individual{child1, 0}, Individual{child2, 0});
}

std::pair<Individual, Individual>
ox(const Individual &individual1, const Individual &individual2, std::mt19937 &generator) {
    std::uniform_int_distribution<size_t> dist(1, individual1.first.size() - 2);
    size_t index1 = dist(generator);
    size_t index2 = dist(generator);
    while (index1 == index2) {
        index2 = dist(generator);
    }

    if (index1 > index2) {
        std::swap(index1, index2);
    }

    std::vector<size_t> child1(individual1.first.size(), 0);
    std::vector<size_t> child2(individual1.first.size(), 0);

    std::map<size_t, bool> mapping1{};
    std::map<size_t, bool> mapping2{};
    for (size_t i = index1; i <= index2; i++) {
        child1[i] = individual1.first[i];
        child2[i] = individual2.first[i];

        mapping1[individual1.first[i]] = true;
        mapping2[individual2.first[i]] = true;
    }

    std::vector<size_t> missing1{};
    std::vector<size_t> missing2{};

    for (size_t i = 0; i < individual1.first.size(); i++) {
        size_t index = (index2 + i + 1) % individual1.first.size();
        if (mapping1.find(individual2.first[index]) == mapping1.end()) {
            missing1.push_back(individual2.first[index]);
        }

        if (mapping2.find(individual1.first[index]) == mapping2.end()) {
            missing2.push_back(individual1.first[index]);
        }
    }

    for (size_t i = 0; i < missing1.size(); i++) {
        size_t index = (index2 + i + 1) % individual1.first.size();
        child1[index] = missing1[i];
        child2[index] = missing2[i];
    }

    return std::make_pair(Individual{child1, 0}, Individual{child2, 0});
}

std::pair<Individual, Individual>
cx(const Individual &individual1, const Individual &individual2) {
    std::vector<size_t> child1(individual1.first.size(), 0);
    std::vector<size_t> child2(individual1.first.size(), 0);

    size_t index1 = 1;
    size_t value1 = individual2.first[index1 - 1];
    do {
        for (size_t i = 0; i < individual1.first.size(); i++) {
            if (individual1.first[i] == value1) {
                index1 = i + 1;
                break;
            }
        }
        child1[index1 - 1] = value1;
        value1 = individual2.first[index1 - 1];
    } while (index1 != 1);

    for (size_t i = 0; i < individual1.first.size(); i++) {
        if (child1[i] == 0) {
            child1[i] = individual2.first[i];
        }
    }

    size_t index2 = 1;
    size_t value2 = individual1.first[index2 - 1];
    do {
        for (size_t i = 0; i < individual2.first.size(); i++) {
            if (individual2.first[i] == value2) {
                index2 = i + 1;
                break;
            }
        }
        child2[index2 - 1] = value2;
        value2 = individual1.first[index2 - 1];
    } while (index2 != 1);

    for (size_t i = 0; i < individual1.first.size(); i++) {
        if (child2[i] == 0) {
            child2[i] = individual1.first[i];
        }
    }

    return std::make_pair(Individual{child1, 0}, Individual{child2, 0});
}

void geneticThread(std::vector<Individual> &population, const std::vector<Coordinate> &points,
                   size_t iterationsNumber, CrossType crossType, float crossProbability, float mutationProbability,
                   std::mt19937 &generator) {

    for (size_t iteration = 0; iteration < iterationsNumber; iteration++) {
        std::sort(population.begin(), population.end(),
                  [](const Individual &a, const Individual &b) { return a.second < b.second; });

        std::uniform_real_distribution<float> dist(0.0f, 1.0f);

        // crossing - choosing parents
        std::vector<Individual> parents{};
        size_t parentsNumber{population.size() / 2};
        while (parents.size() < parentsNumber) {
            for (size_t individual = 0; individual < population.size(); individual++) {
                if (parents.size() >= parentsNumber) {
                    break;
                }

                if (dist(generator) < (crossProbability / (1.0 + static_cast<float>(individual)))) {
                    parents.push_back(population[individual]);
                }
            }
        }

        // crossing - crossing parents
        std::vector<Individual> newPopulation{};
        for (size_t i = 0; i < population.size() / 2; i++) {
            newPopulation.push_back(population[i]);
        }

        std::uniform_int_distribution<size_t> parentDist(0, parents.size() - 1);
        for (size_t i = 0; i < population.size() / 4; i++) {
            size_t parent1Index = parentDist(generator);
            size_t parent2Index = parentDist(generator);
            while (parent1Index == parent2Index) {
                parent2Index = parentDist(generator);
            }

            if (crossType == CrossType::PARTIALLY_MAPPED_CROSSOVER) {
                auto children = pmx(parents[parent1Index], parents[parent2Index], generator);
                children.first.second = cycleWeight(points, children.first.first);
                children.second.second = cycleWeight(points, children.second.first);
                newPopulation.push_back(children.first);
                newPopulation.push_back(children.second);
            } else if (crossType == CrossType::ORDER_CROSSOVER) {
                auto children = ox(parents[parent1Index], parents[parent2Index], generator);
                children.first.second = cycleWeight(points, children.first.first);
                children.second.second = cycleWeight(points, children.second.first);
                newPopulation.push_back(children.first);
                newPopulation.push_back(children.second);
            } else if (crossType == CrossType::CYCLE_CROSSOVER) {
                auto children = cx(parents[parent1Index], parents[parent2Index]);
                children.first.second = cycleWeight(points, children.first.first);
                children.second.second = cycleWeight(points, children.second.first);
                newPopulation.push_back(children.first);
                newPopulation.push_back(children.second);
            } else {
                throw std::runtime_error("Unknown cross type");
            }
        }

        // mutation
        for (auto &individual: newPopulation) {
            std::uniform_real_distribution<float> mutDist(0.0f, 1.0f);
            std::uniform_int_distribution<size_t> indexDist(1, individual.first.size() - 2);

            // reverse sequence mutation
            if (mutDist(generator) < mutationProbability) {
                size_t index1 = indexDist(generator);
                size_t index2 = indexDist(generator);

                if (index1 > index2) {
                    std::swap(index1, index2);
                }

                std::reverse(individual.first.begin() + index1, individual.first.begin() + index2 + 1);
                individual.second = cycleWeight(points, individual.first);
            }

            // single swap mutation
            if (mutDist(generator) < mutationProbability) {
                size_t index1 = indexDist(generator);
                size_t index2 = indexDist(generator);

                if (index1 > index2) {
                    std::swap(index1, index2);
                }

                std::swap(individual.first[index1], individual.first[index2]);
                individual.second = cycleWeight(points, individual.first);
            }
        }

        for (auto &individual: newPopulation) {
//            tabuSearchWithRandomPartNeighbourhood(individual.first, points);
            localSearchWithRandomPartNeighbourhood(individual.first, points);
            individual.second = cycleWeight(points, individual.first);
        }

        population = newPopulation;
    }
}

void genetic(Individual &cycle, const std::vector<Coordinate> &points,
             size_t populationSize, size_t islandsNumber, size_t epochsNumber, size_t iterationPerEpochNumber,
             CrossType crossType, float crossProbability, float mutationProbability, std::mt19937 &generator) {

    // random initial population
    std::vector<std::vector<Individual>> islandsPopulations{};
    for (size_t island = 0; island < islandsNumber; island++) {
        std::vector<Individual> population{};
        for (size_t i = 0; i < populationSize; i++) {
            population.push_back(cycle);
            std::shuffle(population.back().first.begin(), population.back().first.end(), generator);
            population.back().second = cycleWeight(points, population.back().first);
        }
        islandsPopulations.push_back(population);
    }

    // run genetic algorithm for epochsNumber epochs
    for (size_t epoch = 0; epoch < epochsNumber; epoch++) {
        std::vector<std::thread> threads{};
        for (size_t island = 0; island < islandsNumber; island++) {
            threads.emplace_back(geneticThread, std::ref(islandsPopulations[island]), std::ref(points),
                                 iterationPerEpochNumber, crossType, crossProbability, mutationProbability,
                                 std::ref(generator));
        }
        for (auto &thread: threads) {
            thread.join();
        }

        // sort populations by weigh
        for (size_t island = 0; island < islandsNumber; island++) {
            std::sort(islandsPopulations[island].begin(), islandsPopulations[island].end(),
                      [](const Individual &a, const Individual &b) { return a.second < b.second; });
        }

        // remove worst half of population
        for (size_t island = 0; island < islandsNumber; island++) {
            islandsPopulations[island].erase(islandsPopulations[island].begin() + populationSize / 2,
                                             islandsPopulations[island].end());
        }

        // move some best cycles to other islands
        std::uniform_int_distribution<size_t> dist1(0, islandsNumber - 1);
        std::uniform_int_distribution<size_t> dist2(0, populationSize / 2 - 1);

        for (size_t island = 0; island < islandsNumber; island++) {
            for (size_t i = 0; i < populationSize / 2; i++) {
                size_t randomIsland = dist1(generator);
                while (randomIsland == island) {
                    randomIsland = dist1(generator);
                }

                size_t randomIndex = dist2(generator);
                islandsPopulations[island].push_back(islandsPopulations[randomIsland][randomIndex]);
            }
        }
    }

    // find best cycle
    Individual bestCycle{};
    int_fast32_t bestCycleWeight{std::numeric_limits<int_fast32_t>::max()};
    for (size_t island = 0; island < islandsNumber; island++) {
        for (size_t i = 0; i < populationSize; i++) {
            if (islandsPopulations[island][i].second < bestCycleWeight) {
                bestCycleWeight = islandsPopulations[island][i].second;
                bestCycle = islandsPopulations[island][i];
            }
        }
    }

    cycle = bestCycle;
}

#endif // GENETIC_HPP