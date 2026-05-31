#pragma once
#include "FlowNetwork.h"
#include <string>
#include "External Libraries/json.hpp"

using string = std::string;
using json = nlohmann::json;

class SwarmController {
public:
    SwarmController(Graph* g, json& req);
    ~SwarmController();
    void execute();
private:
    class Graph* g;
    json& req;
    std::vector<std::pair<int, int>> batteryIndex_to_count;
    std::vector<std::pair<Path, std::list<int>>> unitPath_And_BatteriesItCollected;
    std::vector<int> collision_indices;

    int startIndex;
    int endIndex;
    double BATTERY_CAPACITY;
    int units_count;

    std::vector<std::pair<int, int>> loadReq();
    void dumpJson(const string& status, const json& data);
    bool checkAndHandleInfinitePath(LightestPathInfo* info);
    void buildFlowNetwork(FlowNetwork& network, CondensedEdgeMapper& mapper, LightestPathInfo* info, int s, int t);
    void loadSwarmInfo(json& swarm_info);
    void load_unitPath_And_BatteriesItCollected(FlowNetwork& network, CondensedEdgeMapper& mapper, int s, int t);
};
