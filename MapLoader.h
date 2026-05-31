#pragma once
#include "Graph.h"
#include <string>
#include <unordered_map>


static const int REGION_MAX_NODES = 4292773;
static const int REGION_MAX_EDGES = 9163116;


class MapLoader {
public:
    MapLoader();
    ~MapLoader();

    void loadGraphBinary3Part(Graph* g, const std::string& baseName);


private:

    Graph* g_temp;
    std::unordered_map<long long, int> idToGraphIndex;
    std::unordered_map<long long, Node> tempCoords;
    int current_node_index = 0;
};
