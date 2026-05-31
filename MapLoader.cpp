#include "MapLoader.h"
#include <iostream>
#include <fstream>
#include "Utilities.h"



MapLoader::MapLoader() : g_temp(nullptr), current_node_index(0) {}
MapLoader::~MapLoader() {}


void MapLoader::loadGraphBinary3Part(Graph* g, const std::string& baseName) {
    std::cout << "[B.A.S.E.L. Engine] Loading Binary Map...\n";

    idToGraphIndex.clear();

    std::ifstream inN(baseName + "_nodes.bin", std::ios::binary);
    int count; inN.read((char*)&count, sizeof(int));
    for (int i = 0; i < count; i++) {
        int old_id; double la, lo;
        inN.read((char*)&old_id, sizeof(int));
        inN.read((char*)&la, sizeof(double));
        inN.read((char*)&lo, sizeof(double));

        idToGraphIndex[old_id] = i;
        g->setNode(i, la, lo);
    }
    inN.close();

    std::ifstream inE1(baseName + "_edges_1.bin", std::ios::binary);
    std::ifstream inE2(baseName + "_edges_2.bin", std::ios::binary);
    int total; inE1.read((char*)&total, sizeof(int));

    auto rd = [&](std::ifstream& in) {
        while (in.peek() != EOF) {
            int n1, n2; double w;
            in.read((char*)&n1, sizeof(int));
            in.read((char*)&n2, sizeof(int));
            in.read((char*)&w, sizeof(double));

            if(idToGraphIndex.count(n1) && idToGraphIndex.count(n2)) {
                g->addEdge(idToGraphIndex[n1], idToGraphIndex[n2], w);
            }
        }
    };
    rd(inE1); rd(inE2);
    inE1.close(); inE2.close();
}

