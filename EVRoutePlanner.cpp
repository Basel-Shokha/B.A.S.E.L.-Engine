#include "EVRoutePlanner.h"
#include <iostream>
#include <fstream>
#include "Utilities.h"


EVRoutePlanner::EVRoutePlanner(Graph* g, json& req) : g(g), req(req) {}

EVRoutePlanner::~EVRoutePlanner() {}

void EVRoutePlanner::dumpJson(const string& mode, double total_distance_km, const json& snapped_stations,
    const json& legs, const json& used_stations, bool error) {
    json response;
    response["mode"] = mode;
    if (error) {
        response["error"] = "B.A.S.E.L. unit cannot make it! Battery capacity is too low.";
        std::ofstream outFile("response.json");
        outFile << response.dump(4);
        outFile.close();
        return;
    }

    response["total_distance_km"] = total_distance_km;
    response["snapped_start"] = {{"lat", g->getNode(targetNodes[0]).latitude}, {"lng", g->getNode(targetNodes[0]).longitude}};
    response["snapped_end"] = {{"lat", g->getNode(targetNodes[1]).latitude}, {"lng", g->getNode(targetNodes[1]).longitude}};
    response["snapped_stations"] = snapped_stations;
    response["legs"] = legs;
    response["used_stations"] = used_stations;

    std::ofstream outFile("response.json");
    outFile << response.dump(4);
    outFile.close();
    std::cout << "[F1] Route calculated and saved to response.json.\n";
}

void EVRoutePlanner::loadReq() {
    battery_meters = req["battery_capacity_km"].get<double>() * 1000.0;
    targetNodes.push_back(g->findClosestNode(req["start"]["lat"], req["start"]["lng"]));
    targetNodes.push_back(g->findClosestNode(req["end"]["lat"], req["end"]["lng"]));

    for (const auto& station : req["stations"]) {
        targetNodes.push_back(g->findClosestNode(station["lat"], station["lng"]));
    }

}

void EVRoutePlanner::execute() {
    loadReq();

    CondensedEdgeMapper mapper;
    Graph condensed_g = g->condenseGraph(targetNodes, battery_meters, mapper);
    LightestPathInfo* condensed_g_info = condensed_g.findLightestPath(0);


    if (condensed_g_info[1].dv == INFINITY_VALUE_DOUBLE) {
        json dummy;
        dumpJson("f1", -1.0, dummy, dummy, dummy, true);
        return;
    }


    json snapped_stations = json::array();
    for(size_t i = 2; i < targetNodes.size(); i++) {
        snapped_stations.push_back({{"lat", g->getNode(targetNodes[i]).latitude}, {"lng", g->getNode(targetNodes[i]).longitude}});
    }

    json legs = json::array();
    json used_stations = json::array();

    Path condensed_path = condensed_g.getPath(condensed_g_info, 0, 1);
    for (Path::iterator it = condensed_path.begin(); it != condensed_path.end(); ++it) {
        Edge* c_edge = *it;
        int from_idx = c_edge->node1;
        int to_idx = c_edge->node2;

        if (to_idx != 1) {
            Node sn = g->getNode(targetNodes[to_idx]);
            used_stations.push_back({{"lat", sn.latitude}, {"lng", sn.longitude}});
        }

        json leg;
        leg["distance_km"] = c_edge->weight / 1000.0;
        json leg_coords = json::array();

        Node fn = g->getNode(targetNodes[from_idx]);
        leg_coords.push_back({{"lat", fn.latitude}, {"lng", fn.longitude}});

        Path micro_path = mapper[c_edge];
        for (auto m_edge : micro_path) {
            Node n = g->getNode(m_edge->node2);
            leg_coords.push_back({{"lat", n.latitude}, {"lng", n.longitude}});
        }

        leg["path"] = leg_coords;
        legs.push_back(leg);
    }

    dumpJson("f1", condensed_g_info[1].dv / 1000.0, snapped_stations, legs, used_stations);
    delete[] condensed_g_info;
}
