#pragma once
#include "Graph.h"
#include <string>
#include <unordered_map>
#include "External Libraries/json.hpp"


using json = nlohmann::json;
using string = std::string;

////feat 1 handler
class EVRoutePlanner {
public:
    EVRoutePlanner(Graph* g, json& req);
    ~EVRoutePlanner();
    void execute();

private:
    Graph* g;
    double battery_meters;
    json& req;
    std::vector<int> targetNodes;
    void loadReq();
    void dumpJson(const string& mode, double total_distance_km, const json& snapped_stations, const json& legs, const json& used_stations, bool error = false);
};

