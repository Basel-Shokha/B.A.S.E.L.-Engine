#include "FleetDispatcher.h"
#include <iostream>
#include <fstream>
#include "FlowNetwork.h"
#include "Utilities.h"

FleetDispatcher::FleetDispatcher(Graph* g, json& req) : g(g), req(req) {}
FleetDispatcher::~FleetDispatcher() {}

void FleetDispatcher::loadReq() {

    for (const auto& station : req["robot_fixers"]) {
        fixerRobots.push_back(g->findClosestNode(station["lat"], station["lng"]));
    }


    for (const auto& station : req["broken_vehicles"]) {
        std::string type = station["type"];
        int capacity = (type == "two-wheeler") ? TWO_WHEELERS_NODE_CAPACITY :
        ((type == "passengers-car") ? PASSENGER_CARS_NODE_CAPACITY :
            ((type == "heavy-transport") ? HEAVY_TRANSPORT_NODE_CAPACITY :
                ((type == "complex-vehicle") ? COMPLEX_VEHICLE_NODE_CAPACITY : -1)));

        brokenVehicles.push_back({g->findClosestNode(station["lat"], station["lng"]), capacity});
    }

    networkNodesCount = 2 + fixerRobots.size() + brokenVehicles.size(); //// 2 for the super nodes at indices 0 and 1
    networkEdgesCount = fixerRobots.size() + brokenVehicles.size() + fixerRobots.size()*brokenVehicles.size();
    fixerRobotsStartIndex = 2;
    brokenVehiclesStartIndex = 2 + fixerRobots.size();
}

void FleetDispatcher::buildFlowNetwork(FlowNetwork& network) {

    for (int i = 0; i < fixerRobots.size(); i++) {
        LightestPathInfo* info = g->findLightestPath(fixerRobots[i]);
        for (int j = 0 ; j < brokenVehicles.size(); j++) {
            if (info[brokenVehicles[j].first].dv != INFINITY_VALUE_DOUBLE) {
                condensedEdgeMapper[network.addEdge(fixerRobotsStartIndex + i, brokenVehiclesStartIndex + j,
                    info[brokenVehicles[j].first].dv, 1)] =
                        g->getPath(info, fixerRobots[i], brokenVehicles[j].first);
            }
            if (i == 0) {
                network.addEdge( brokenVehiclesStartIndex + j, 1, 0, brokenVehicles[j].second);
            }
        }
        delete[] info;
        //on the way add the edges from super source to the robots
        network.addEdge(0, fixerRobotsStartIndex + i,0, 1);
    }

}

void FleetDispatcher::dumpJson(const string& mode, const string& status, const json& correctedVehicles, double totalDistance, const json& assignments) {
    json response;
    response["mode"] = mode;
    response["status"] = status;
    response["corrected_vehicles"] = correctedVehicles;
    response["total_fleet_distance"] = totalDistance/1000.0;
    response["assignments"] = assignments;


    std::ofstream outFile("response.json");
    outFile << response.dump(4);
    outFile.close();
    std::cout << "[F2] Routes calculated and saved to response.json.\n";

}

void FleetDispatcher::loadCorrectedVehicles(json& correctedVehicles) {
    for (int i = 0; i < brokenVehicles.size(); i++) {
        json leg;
        leg["id"] = i;
        Node node = g->getNode(brokenVehicles[i].first);
        leg["lat"] = node.latitude;
        leg["lng"] = node.longitude;
        correctedVehicles.push_back(leg);
    }
}

double FleetDispatcher::loadAssignments(json& assignments, FlowNetwork& network) {
    std::list<Edge*> edges = network.getEdgesByFlow(1);
    double totalDistance = 0;
    for (auto& edge : edges) {
        if (edge->node1 == 0 || edge->node2 == 1) {
            continue;
        }
        json leg;
        json json_path = json::array();
        leg["robot_id"] = edge->node1 - fixerRobotsStartIndex;
        leg["target_vehicle_id"] = edge->node2 - brokenVehiclesStartIndex;
        leg["distance_km"] = edge->weight/1000.0;

        Path path = condensedEdgeMapper[edge];
        int index_in_g = fixerRobots[edge->node1 - 2];
        json_path.push_back({{"lat" , g->getNode(index_in_g).latitude},
            {"lng", g->getNode(index_in_g).longitude}});

        for (Path::iterator it = path.begin(); it != path.end(); ++it) {
            json_path.push_back({{"lat" , g->getNode((*it)->node2).latitude},
            {"lng", g->getNode((*it)->node2).longitude}});
        }
        leg["path"] = json_path;

        assignments.push_back(leg);
        totalDistance += edge->weight;
    }

    return totalDistance;
}

void FleetDispatcher::execute() {
    loadReq();

    Graph temp(networkNodesCount, networkEdgesCount);
    FlowNetwork network(0,1, std::move(temp));
    buildFlowNetwork(network);

    int achieved_flow = network.applyMaxFlow(true);

    int totalRequired = 0;
    for (auto& v : brokenVehicles) {
        totalRequired += v.second;
    }

    if (achieved_flow < totalRequired) {
        json response;
        response["mode"] = "f2";
        response["error"] = "Not all vehicles could be reached. Some robots may be unreachable from their target vehicles.";
        std::ofstream outFile("response.json");
        outFile << response.dump(4);
        outFile.close();
        return;
    }

    json correctedVehicles = json::array();
    loadCorrectedVehicles(correctedVehicles);

    json assignments = json::array();

    double totalDistance = loadAssignments(assignments, network);

    dumpJson("f2", "success", correctedVehicles, totalDistance, assignments);

}