#pragma once
#include <limits>
#include <vector>
#include <queue>
#include <pthread.h>
#include "MinHeap.h"
#include <list>
#include <map>


static const double INFINITY_VALUE_DOUBLE = std::numeric_limits<double>::infinity();
static const int INFINITY_VALUE_INT = -1;


struct Edge {
    int node1, node2;

    double weight;

    Edge* next_out = nullptr;
    Edge* next_in = nullptr;

};

using Path = std::list<Edge*>;
using CondensedEdgeMapper = std::map<Edge*, Path>;

struct Node {
    double latitude;
    double longitude;
};
struct PathInfo {
    Edge* incoming_edge;
    double dv;
};

using LightestPathInfo = PathInfo;
using ShortestPathInfo = LightestPathInfo;

///note :: cannot remove an edge and re-add it
class Graph { //only works for graphs with no parallel edges, if we wanna fix : each edge must contain an id. complex work.
public:

    Graph(int numberOfNodes, int maxEdges);
    Graph(Graph&& other) noexcept; // move constructor
    void setNode(int index, double latitude, double longitude);
    const Node& getNode(int index) const;
    Edge *addEdge(int node1_index, int node2_index, double weight);
    virtual void removeEdge(int node1_index, int node2_index);
    Edge* getOutEdges(int index) const;
    Edge* getInEdges(int index) const;
    Edge* getEdge(int node1, int node2) const;  //////////CRUCIAL :: only assumes there is one edge from node1 to node2, no parallel edges.
    int getEdgeCount() const;

    Graph(const Graph&) = delete;
    Graph& operator=(const Graph&) = delete;

    virtual ~Graph();

    virtual LightestPathInfo* findLightestPath(int startingNode);//DIJKSTRA
    virtual ShortestPathInfo* findShortestPath(int startingNode);//BFS

    Path getPath(PathInfo* pathInfo, int src, int dest);

    Graph condenseGraph(std::vector<int> vector, double filter, CondensedEdgeMapper& mapper);

    int findClosestNode(double lat, double lng, double delta = 0.05) const;

private:
    void removeEdgeFromInEdges(int node1_index, int node2_index);
    void removeEdgeFromOutEdges(int node1_index, int node2_index);
    int getEdgeIndex(Edge* edge) const;

protected:
    Node* nodes;
    Edge** in_edges;
    Edge** out_edges;
    Edge* edges_pool;

    int numberOfNodes;
    int maxEdges;
    int edge_count;

};

