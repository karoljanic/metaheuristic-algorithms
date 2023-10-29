#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <iostream>
#include <vector>
#include <queue>


/*
Graph class represents weighted graph.
The size of the graph must be specified at initialization.
The edge weight type is T.
The vertices are numbered with numbers from 1 to n
*/
template<typename T>
class Graph {
public:
    struct Edge { size_t sourceVertex; size_t targetVertex; T weight; };

    class EdgeComparator { public: bool operator()(const Edge &firstEdge, const Edge &secondEdge){ return firstEdge.weight > secondEdge.weight; }};

private:
    size_t verticesNum;
    std::vector<std::vector<Edge>> adjacencyList;

public:
    Graph(size_t verticesNumber) : verticesNum{verticesNumber}, adjacencyList(verticesNumber + 1) { }

    void addEdge(size_t firstVertex, size_t secondVertex, T weight) {
        adjacencyList[firstVertex].emplace_back(Edge{firstVertex, secondVertex, weight});
        adjacencyList[secondVertex].emplace_back(Edge{secondVertex, firstVertex, weight});
    }

    std::vector<Edge> getEdges() const {
        std::vector<Edge> edges{};
        for(size_t vertex = 1; vertex <= verticesNum; vertex++) {
            for(const auto& neighbour: adjacencyList[vertex]) {
                edges.push_back(neighbour);
            }
        }

        return edges;
    }

    size_t getEdgesNumber() const {
        size_t edgesNumber{0};
        for(size_t vertex = 1; vertex <= verticesNum; vertex++) {
            edgesNumber += adjacencyList[vertex].size();
        }

        return edgesNumber;
    }

    T minimumSpanningTree(Graph<T>& mst, size_t startVertex) const {// Prim's Algorithm (O(|V|^2))
        T totalWeight{0};
        std::vector<bool> visited(verticesNum + 1, false);
        std::vector<T> cost(verticesNum + 1, std::numeric_limits<T>::max());
        std::vector<size_t> parent(verticesNum + 1, 0);
        std::vector<T> weight(verticesNum + 1, 0);

        cost[startVertex] = 0;

        for(size_t i = 0; i < verticesNum; i++) {
            size_t minKey{1};
            T minVal{std::numeric_limits<T>::max()};
            for(size_t k = 1; k <= verticesNum; k++) {
                if(!visited[k] && cost[k] < minVal) {
                    minKey = k;
                    minVal = cost[k];
                }
            }

            visited[minKey] = true;
            totalWeight += minVal;

            for(const auto& neighbour: adjacencyList[minKey]) {
                if(!visited[neighbour.targetVertex] && neighbour.weight < cost[neighbour.targetVertex]) {
                    parent[neighbour.targetVertex] = minKey;
                    weight[neighbour.targetVertex] = neighbour.weight;
                    cost[neighbour.targetVertex] = neighbour.weight;
                }
            }
        }

        for(size_t i = 1; i <= verticesNum; i++) {
            if(parent[i] != 0) {
                mst.addEdge(parent[i], i, weight[i]);
            }
        }

        return totalWeight;
    }

/*
    T minimumSpanningTree(Graph& mst, size_t startVertex) const {// Prim's Algorithm O(|E|log|V|)
        T totalWeight{0};
        std::vector<bool> visited(verticesNum + 1, false);

        std::priority_queue<Edge, std::vector<Edge>, EdgeComparator> queue;

        visited[startVertex] = true;
        for(const auto& neighbour: adjacencyList[startVertex]) {
            queue.push(neighbour);
        }

        while(!queue.empty()) {
            Edge edgeWithMinWeight = queue.top();
            queue.pop();
            if(!visited[edgeWithMinWeight.targetVertex]) {
                totalWeight += edgeWithMinWeight.weight;
                mst.addEdge(edgeWithMinWeight.sourceVertex, edgeWithMinWeight.targetVertex, edgeWithMinWeight.weight);
                visited[edgeWithMinWeight.targetVertex] = true;
                for(const auto& neighbour: adjacencyList[edgeWithMinWeight.targetVertex]) {
                    if(!visited[neighbour.targetVertex]) {
                        queue.push(neighbour);
                    }
                }
            }
        }

        return totalWeight;
    }
*/

    void generateSalesmanCycleBasedOnMst(Graph<T> mst, std::vector<size_t>& salesmanCycle, size_t startVertex) const {
        salesmanCycle.clear();

        std::vector<bool> visited(mst.verticesNum + 1, false);
        std::stack<size_t> dfsStack{};

        dfsStack.push(startVertex);
        while(!dfsStack.empty()) {
            size_t currentVertex = dfsStack.top();
            dfsStack.pop();

            if(!visited[currentVertex]) {
                visited[currentVertex] = true;
                salesmanCycle.push_back(currentVertex);

                for(const auto& neighbour: mst.adjacencyList[currentVertex]) {
                    if(!visited[neighbour.targetVertex]) {
                        dfsStack.push(neighbour.targetVertex);
                    }
                }
            }
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const Graph& graph) {
        for(size_t vertex = 1; vertex <= graph.verticesNum; vertex++) {
            for(const auto& neighbour: graph.adjacencyList[vertex]) {
                if(vertex <= neighbour.targetVertex) {
                    os << "(" << vertex << ", " << neighbour.targetVertex << "): " << neighbour.weight << std::endl;
                }
            }
        }

        return os;
    }
};

#endif // GRAPH_HPP
