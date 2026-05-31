#pragma once
#include "FlowNetwork.h"
#include <string>
#include <unordered_map>
#include <filesystem>
#include "External Libraries/json.hpp"

using json = nlohmann::json;
using string = std::string;

static const int TWO_WHEELERS_NODE_CAPACITY = 1;
static const int PASSENGER_CARS_NODE_CAPACITY = 2;
static const int HEAVY_TRANSPORT_NODE_CAPACITY = 3;
static const int COMPLEX_VEHICLE_NODE_CAPACITY = 4;

////feat 2 handler

class FleetDispatcher {
public:
    FleetDispatcher(Graph* g, json& req);
    ~FleetDispatcher();
    void execute();

private:

    Graph* g;
    json req;
    std::vector<int> fixerRobots;
    std::vector<std::pair<int,int>> brokenVehicles; // maps index in g to the robots needed
    CondensedEdgeMapper condensedEdgeMapper;
    int networkNodesCount = 2 + fixerRobots.size() + brokenVehicles.size(); //// 2 for the super nodes at indices 0 and 1
    int networkEdgesCount = fixerRobots.size() + brokenVehicles.size() + fixerRobots.size()*brokenVehicles.size();
    //reminder : 0,1 super nodes.
    int fixerRobotsStartIndex = 2;
    int brokenVehiclesStartIndex = 2 + fixerRobots.size();

    void dumpJson(const string& mode, const string& status, const json& correctedVehicles, double totalDistance, const json& assignments);
    void loadReq();
    void buildFlowNetwork(FlowNetwork& network);
    void loadCorrectedVehicles(json& correctedVehicles);
    double loadAssignments(json& assignments, FlowNetwork& network);
};
