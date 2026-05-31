#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>
#include "FlowNetwork.h"
#include "MapLoader.h"
#include "Graph.h"
#include "EVRoutePlanner.h"
#include "FleetDispatcher.h"
#include "SwarmController.h"
#include <filesystem>


namespace fs = std::filesystem;

int main() {

    Graph* g = new Graph(REGION_MAX_NODES, REGION_MAX_EDGES);
    MapLoader mapLoader;
    mapLoader.loadGraphBinary3Part(g, "map");


    std::cout << "\n[B.A.S.E.L. Engine] Engine is LIVE. Waiting for UI clicks...\n";

    while (true) {
        if (fs::exists("request.json")) {
            std::cout << "[B.A.S.E.L. Engine] New order ticket detected!\n";
            json req;
            std::ifstream inFile("request.json");
            inFile >> req;
            string mode = req["mode"];
            if (mode == "f1") {
                EVRoutePlanner routePlanner(g, req);
                routePlanner.execute();
            } else if (mode == "f2") {
                FleetDispatcher fleetDispatcher(g, req);
                fleetDispatcher.execute();
            }
            else if (mode == "f3") {
                SwarmController swarmController(g, req);
                swarmController.execute();
            }

            fs::remove("request.json");

        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); ///CPU REST
    }
    
    delete g;
    return 0;
}