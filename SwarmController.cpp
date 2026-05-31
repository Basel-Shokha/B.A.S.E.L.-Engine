#include "SwarmController.h"
#include <iostream>
#include <fstream>
#include "Utilities.h"

SwarmController::SwarmController(Graph *g, json &req) : g(g), req(req) {}

SwarmController::~SwarmController() {}

std::vector<std::pair<int, int>> SwarmController::loadReq() {

    startIndex = g->findClosestNode(req["start"]["lat"], req["start"]["lng"]);
    endIndex = g->findClosestNode(req["end"]["lat"], req["end"]["lng"]);
    BATTERY_CAPACITY = req["capacity_km"].get<double>() * 1000;

    if (startIndex == endIndex) {
        collision_indices.push_back(-1);
    }

    std::unordered_map<int, int> consolidated_batteries;
    std::unordered_map<int, std::vector<int>> node_to_ui_index;

    int ui_index = 0;
    for (auto& battery : req["batteries"]) {
        int closest_node = g->findClosestNode(battery["lat"], battery["lng"]);

        if (closest_node == startIndex || closest_node == endIndex) {
            collision_indices.push_back(ui_index);
        }

        consolidated_batteries[closest_node] += (int)battery["count"];
        node_to_ui_index[closest_node].push_back(ui_index);
        ui_index++;
    }

    for (auto& pair : node_to_ui_index) {
        if (pair.second.size() > 1) {
            for (int idx : pair.second) collision_indices.push_back(idx);
        }
    }

    return std::vector<std::pair<int, int>> (consolidated_batteries.begin(), consolidated_batteries.end());
}

void SwarmController::dumpJson(const string& status, const json& data) {
    json response;
    response["mode"] = "f3";
    response["status"] = status;

    if (status == "collision") {
        response["collision_data"] = data;
    }
    else if (status == "infinite") {
        response["path"] = data["path"];
        response["distance_km"] = data["distance_km"];
    } else if (status == "success") {
        json official_bats = json::array();
        for (auto& pair : batteryIndex_to_count) {
            const Node& node = g->getNode(pair.first);
            official_bats.push_back({{"lat", node.latitude}, {"lng", node.longitude}, {"count", pair.second}});
        }
        response["official_batteries"] = official_bats;
        response["swarm_info"] = data;
    }

    std::ofstream outFile("response.json");
    outFile << response.dump(4);
    outFile.close();

    std::cout << "[F3] Status: " << status << " saved to response.json.\n";

}

bool SwarmController::checkAndHandleInfinitePath(LightestPathInfo* info) {
    if (info[endIndex].dv <= BATTERY_CAPACITY) {
        Path infinity_path = g->getPath(info, startIndex, endIndex);

        json data;
        json path_coords = json::array();
        double dist = 0;
        int flag = 0;

        for(auto e : infinity_path) {
            if(flag == 0) {
                path_coords.push_back({{"lat", g->getNode(e->node1).latitude}, {"lng", g->getNode(e->node1).longitude}});
                flag = 1;
            }
            path_coords.push_back({{"lat", g->getNode(e->node2).latitude}, {"lng", g->getNode(e->node2).longitude}});
            dist += e->weight;
        }

        data["path"] = path_coords;
        data["distance_km"] = dist / 1000.0;

        dumpJson("infinite", data);
        return true;
    }
    return false;
}

void SwarmController::buildFlowNetwork(FlowNetwork& network, CondensedEdgeMapper& mapper, LightestPathInfo* info, int s, int t) {
    for (int i = 0 ; i < batteryIndex_to_count.size(); i++) {
        int k = batteryIndex_to_count[i].first;
        if (info[k].dv <= BATTERY_CAPACITY) {
            mapper[network.addEdge(s, 2*i, info[k].dv)] = g->getPath(info, startIndex, k);
        }
    }

    for (int i = 0 ; i < batteryIndex_to_count.size() ; i++) {
        network.addEdge(2*i, 2*i + 1, 0, batteryIndex_to_count[i].second);

        LightestPathInfo* local_info = g->findLightestPath(batteryIndex_to_count[i].first);

        for (int j = 0; j < batteryIndex_to_count.size() ; j++) {
            int k = batteryIndex_to_count[j].first;
            if (j != i &&
                local_info[k].dv != INFINITY_VALUE_DOUBLE &&
                local_info[k].dv <= BATTERY_CAPACITY) {

                mapper[network.addEdge(2*i + 1, 2*j, local_info[k].dv)] =
                    g->getPath(local_info, batteryIndex_to_count[i].first, batteryIndex_to_count[j].first);
            }
        }

        if (local_info[endIndex].dv <= BATTERY_CAPACITY) {
            mapper[network.addEdge(2*i+1, t, local_info[endIndex].dv)] =
                g->getPath(local_info, batteryIndex_to_count[i].first, endIndex);
        }
        delete[] local_info;
    }
}

void SwarmController::loadSwarmInfo(json& swarm_info) {
    int id = 0;
    for (auto& pair : unitPath_And_BatteriesItCollected) {
        json leg;
        json batteries_it_passed = json::array();
        leg["unit_id"] = id;

        for (std::list<int>::iterator it = pair.second.begin(); it != pair.second.end(); it++) {
            int g_index = batteryIndex_to_count[*it].first;
            const Node& node = g->getNode(g_index);
            batteries_it_passed.push_back({{"lat", node.latitude}, {"lng", node.longitude}});
        }

        leg["batteries_it_passed"] = batteries_it_passed;

        double totalDistance = 0;
        json path = json::array();
        int flag = 0;
        for (Path::iterator it = pair.first.begin(); it != pair.first.end(); it++) {
            if (flag == 0) {
                const Node& node = g->getNode((*it)->node1);
                path.push_back({{"lat", node.latitude}, {"lng", node.longitude}});
            }
            const Node& node = g->getNode((*it)->node2);
            path.push_back({{"lat", node.latitude}, {"lng", node.longitude}});
            totalDistance += (*it)->weight;
        }

        leg["path"] = path;
        leg["distance_km"] = totalDistance/1000.0;
        id++;
        swarm_info.push_back(leg);
    }
}

void SwarmController::load_unitPath_And_BatteriesItCollected(FlowNetwork& network, CondensedEdgeMapper& mapper, int s, int t) {
    std::vector<Edge*> edgesWithFlow = network.getEdgeswithFlow();
    std::vector<int> artificalFlow(edgesWithFlow.size());

    for (int i = 0; i < edgesWithFlow.size(); i++) {
        artificalFlow[i] = network.getFlow(edgesWithFlow[i]);
    }

    bool found_flow = true;

    while (found_flow) {
        found_flow = false;
        Edge* current = nullptr;
        Path network_path;

        for (int i = 0; i < edgesWithFlow.size(); i++) {
            if ((edgesWithFlow[i]->node1) == s && (artificalFlow[i] > 0)) {
                current = edgesWithFlow[i];
                artificalFlow[i]--;
                found_flow = true;
                break;
            }
        }
        if (!found_flow) {
            break;
        }
        network_path.emplace_back(current);
        int node2 = current->node2;
        while (node2 != t) {
            for (int i = 0; i < edgesWithFlow.size(); i++) {
                if ((edgesWithFlow[i]->node1 == node2) && (artificalFlow[i] > 0)) {
                    current = edgesWithFlow[i];
                    artificalFlow[i]--;
                    node2 = current->node2;
                    network_path.emplace_back(current);
                }
            }
        }

        std::pair<Path, std::list<int>> pathAndBatteries;
        for (Path::iterator it = network_path.begin(); it != network_path.end(); it++) {
            if (((*it)->node1%2 == 0) && ((*it)->node2%2 == 1) && ((*it)->node1 == ((*it)->node2) - 1)) {
                pathAndBatteries.second.emplace_back((*it)->node1/2);
                continue;
            }
            Path mapped_path = mapper[*it];
            pathAndBatteries.first.insert(pathAndBatteries.first.end(), mapped_path.begin(), mapped_path.end());
        }
        unitPath_And_BatteriesItCollected.push_back(pathAndBatteries);
    }
}

void SwarmController::execute() {

    batteryIndex_to_count = std::move(loadReq());

    if (!collision_indices.empty()) {
        json data;
        data["collided_indices"] = collision_indices;
        data["message"] = "Collision detected! Due to inaccuracies and lack of info in the map, these items mapped to the exact same location. Please make them further apart.";
        dumpJson("collision", data);
        return;
    }

    int nodesNum = 2 + 2 * batteryIndex_to_count.size();
    int s = nodesNum - 2;
    int t = nodesNum - 1;

    Graph temp(nodesNum, nodesNum*(nodesNum-1));
    FlowNetwork network(s, t, std::move(temp));
    CondensedEdgeMapper mapper;

    LightestPathInfo* info = g->findLightestPath(startIndex);

    if (checkAndHandleInfinitePath(info)) {
        delete[] info;
        return;
    }

    buildFlowNetwork(network, mapper, info, s, t);
    delete[] info;

    units_count = network.applyMaxFlow();

    load_unitPath_And_BatteriesItCollected(network, mapper, s, t);

    json swarm_info = json::array();
    loadSwarmInfo(swarm_info);
    dumpJson("success", swarm_info);
}
