#pragma once
#include "Graph.h"
#include <atomic>


class FlowNetwork : public Graph {
public:
    FlowNetwork(int s, int t, Graph&& graph);
    LightestPathInfo* findLightestPath(int startingNode) override;
    ShortestPathInfo* findShortestPath(int startingNode) override;
    int setCapacity(Edge* edge, int newCapacity);
    int applyMaxFlow(bool min_cost = false);
    void resetFlow();
    double getCost();

    Edge *addEdge(int node1, int node2, double weight, int cap = INFINITY_VALUE_INT);
    void removeEdge(int node1_index, int node2_index) override;
    int getFlow(Edge* edge);

    std::list<Edge*> getEdgesByFlow(int requestedFlow);

    std::vector<Edge *> getEdgeswithFlow();

    ~FlowNetwork();
private:
    int augmentPath(Path path);
    int getEdgeIndex(Edge* edge);
    bool isBackEdge(Edge* edge);
    bool edgeExistsInResidualGraph(Edge* edge);
    int findBottleneck(Path path); //////////returns the maximum flow we can increase on this path

    int s;
    int t;

    Edge* back_edges_pool;

    double* potential;

    int* flow; //size : the number of edges
    int* capacity;

    bool infinite_flow = false;

};

