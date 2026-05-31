#include "Graph.h"
#include "Utilities.h"

#define CHECK_OUT_OF_BOUNDS(index) \
do { \
if ((index) >= numberOfNodes || (index) < 0) { \
is_out_of_bounds_message(index); \
} \
} while (0)


Graph::Graph(int numberOfNodes, int maxEdges) : numberOfNodes(numberOfNodes), maxEdges(maxEdges), edge_count(0) {
    try {
        nodes = new Node[numberOfNodes];
        in_edges = new Edge*[numberOfNodes]();
        out_edges = new Edge*[numberOfNodes]();
        edges_pool = new Edge[maxEdges]();

    } catch (std::bad_alloc& e) {
        failed_allocation_message(e);
    }
}

Graph::Graph(Graph&& other) noexcept
    : nodes(other.nodes),
      in_edges(other.in_edges),
      out_edges(other.out_edges),
      edges_pool(other.edges_pool),
      numberOfNodes(other.numberOfNodes),
      maxEdges(other.maxEdges),
      edge_count(other.edge_count) {
    other.nodes = nullptr;
    other.in_edges = nullptr;
    other.out_edges = nullptr;
    other.edges_pool = nullptr;

    other.numberOfNodes = 0;
    other.maxEdges = 0;
    other.edge_count = 0;
}


void Graph::setNode(int index, double latitude, double longitude) {
    CHECK_OUT_OF_BOUNDS(index);

    nodes[index].latitude = latitude;
    nodes[index].longitude = longitude;

}

const Node& Graph::getNode(int index) const {
    CHECK_OUT_OF_BOUNDS(index);

    return nodes[index];
}

int Graph::getEdgeIndex(Edge* edge) const {
    return edge - edges_pool;
}

Edge *Graph::addEdge(int node1_index, int node2_index, double weight) {
    CHECK_OUT_OF_BOUNDS(node1_index);
    CHECK_OUT_OF_BOUNDS(node2_index);

    Edge* curr = out_edges[node1_index];
    while (curr != nullptr) {
        if (curr->node2 == node2_index) {
            int index = getEdgeIndex(curr);
            if (weight < edges_pool[index].weight) {
                edges_pool[index].weight = weight;
            }

            return curr;
        }
        curr = curr->next_out;
    }


    if (edge_count >= maxEdges) {
        throw std::runtime_error("Max edges reached");
    }

    edges_pool[edge_count] = {node1_index, node2_index, weight, nullptr, nullptr};
    edges_pool[edge_count].next_out = out_edges[node1_index];
    out_edges[node1_index] = &edges_pool[edge_count];

    edges_pool[edge_count].next_in = in_edges[node2_index];
    in_edges[node2_index] = &edges_pool[edge_count];
    ////crtical check the logic
    edge_count++;
    return &edges_pool[edge_count - 1];
}

void Graph::removeEdgeFromInEdges(int node1_index, int node2_index) {

    int index_in;
    Edge* currentEdge = in_edges[node2_index];
    Edge* previousEdge = nullptr;

    index_in = in_edges[node2_index]->node2;
    while (index_in != node1_index && currentEdge != nullptr) {
        previousEdge = currentEdge;
        currentEdge = currentEdge->next_in;
        index_in = in_edges[node2_index]->node1;
    }
    if (currentEdge == nullptr) {
        return;
    }
    if (previousEdge != nullptr) {
        previousEdge->next_in = currentEdge->next_in;
    } else {
        in_edges[node2_index] = in_edges[node2_index]->next_in;
    }
}

void Graph::removeEdgeFromOutEdges(int node1_index, int node2_index) {
    int index_out;
    Edge* currentEdge = out_edges[node1_index];
    Edge* previousEdge = nullptr;

    index_out = out_edges[node1_index]->node2;
    while (index_out != node2_index && currentEdge != nullptr) {
        previousEdge = currentEdge;
        currentEdge = currentEdge->next_out;
        index_out = out_edges[node1_index]->node2;
    }
    if (currentEdge == nullptr) {
        return;
    }
    if (previousEdge != nullptr) {
        previousEdge->next_out = currentEdge->next_out;
    } else {
        out_edges[node1_index] = out_edges[node1_index]->next_out;
    }

}

void Graph::removeEdge(int node1_index, int node2_index) {
    CHECK_OUT_OF_BOUNDS(node1_index);
    CHECK_OUT_OF_BOUNDS(node2_index);
    removeEdgeFromInEdges(node1_index, node2_index);
    removeEdgeFromOutEdges(node1_index, node2_index);
}
Edge* Graph::getOutEdges(int index) const {
    return out_edges[index];
}

Edge *Graph::getInEdges(int index) const {
    return in_edges[index];
}

int Graph::getEdgeCount() const {
    return edge_count;
}

Edge* Graph::getEdge(int node1, int node2) const {
    Edge* current = getOutEdges(node1);
    while (current != nullptr && current->node2 != node2) {
        current = current->next_out;
    }
    if (current == nullptr) {
        std::cerr << "EDGE " << node1 << "," << node2 << "NOT FOUND!" << std::endl;
        exit(1);
    }
    return current;
}

Graph::~Graph() {
    delete[] nodes;
    delete[] in_edges;
    delete[] out_edges;
    delete[] edges_pool;
}

LightestPathInfo* Graph::findLightestPath(int startingNode) {
    LightestPathInfo* info = nullptr;
    HeapNode* heap = nullptr;
    try {
        info = new LightestPathInfo[numberOfNodes];
        heap = new HeapNode[numberOfNodes];
    } catch (std::bad_alloc& e) {
        failed_allocation_message(e);
    }

    for (int i = 0; i < numberOfNodes; i++) {
        info[i].dv = INFINITY_VALUE_DOUBLE;
        info[i].incoming_edge = nullptr;
        heap[i].first = INFINITY_VALUE_DOUBLE;
        heap[i].second = i;
    }

    heap[startingNode].first = 0;
    MinHeap minHeap(heap, numberOfNodes);

    int counter = 0;
    int currentNode = startingNode;
    HeapNode heapNode ;
    Edge* edge = nullptr;
    while (counter < numberOfNodes) {

        heapNode = minHeap.getMin();
        if (heapNode.first == INFINITY_VALUE_DOUBLE) {
            break;
        }
        minHeap.delMin();
        currentNode = heapNode.second;
        info[currentNode].dv = heapNode.first;
        counter++;

        edge = out_edges[currentNode];

        while (edge != nullptr) {
            if (minHeap[currentNode] + edge->weight < minHeap[edge->node2]) {
                minHeap.decreaseKey(edge->node2, minHeap[currentNode] + edge->weight);
                info[edge->node2].incoming_edge = edge;            }
            edge = edge->next_out;
        }

    }

    delete[] heap;
    return info;
}


ShortestPathInfo* Graph::findShortestPath(int startingNode) {
    ShortestPathInfo* info = nullptr;
    try {
        info = new ShortestPathInfo[numberOfNodes];
    } catch (std::bad_alloc& e) {
        failed_allocation_message(e); // and exits
    }

    for (int i = 0; i < numberOfNodes; i++) {
        info[i].dv = INFINITY_VALUE_DOUBLE;
        info[i].incoming_edge = nullptr;
    }
    info[startingNode].dv = 0;

    std::queue<int> queue;
    queue.push(startingNode);
    Edge* current = nullptr;

    while (!queue.empty()) {
        int node = queue.front();
        queue.pop();
        current = out_edges[node];
        while (current != nullptr) {
            if (info[current->node2].dv == INFINITY_VALUE_DOUBLE) {
                info[current->node2].incoming_edge = current;
                info[current->node2].dv = info[node].dv + 1;
                queue.push(current->node2);
            }
            current = current->next_out;
        }
    }
    return info;
}

Graph Graph::condenseGraph(std::vector<int> list, double filter, CondensedEdgeMapper& mapper) {
    int size = list.size();
    Graph condensedGraph(size, size*(size - 1));

    LightestPathInfo* info = nullptr;

    int i = 0;

        while (i < size) {
            const Node& originalNode = getNode(list[i]);
            condensedGraph.setNode(i, originalNode.latitude, originalNode.longitude);
            info = findLightestPath(list[i]);
            for (int j = 0; j < size; j++) {
                if (j != i && info[list[j]].dv != INFINITY_VALUE_DOUBLE && info[list[j]].dv <= filter) {
                    condensedGraph.addEdge(i, j, info[list[j]].dv);
                    Edge* edge = condensedGraph.getEdge(i, j);
                    mapper[edge] = this->getPath(info, list[i], list[j]);
                }
            }
            delete[] info;
            i++;
        }
    return condensedGraph;
}


Path Graph::getPath(PathInfo* pathInfo, int src, int dest) {
    CHECK_OUT_OF_BOUNDS(src);
    CHECK_OUT_OF_BOUNDS(dest);
    int current = dest;
    Path path;
    if (pathInfo[dest].dv != INFINITY_VALUE_DOUBLE) {
        while (current != src) {
            Edge* edge = pathInfo[current].incoming_edge;
            path.emplace_front(edge);
            current = edge->node1;
        }
    }
    return path;
}

int Graph::findClosestNode(double lat, double lng, double delta) const {
    double min_dist = INFINITY_VALUE_DOUBLE;
    int closest_id = -1;
    double n_lat;
    double n_lon;

    for (int i = 0; i < numberOfNodes; i++) {

        n_lat = nodes[i].latitude;
        n_lon = nodes[i].longitude;

        if (n_lat == 0.0 && n_lon == 0.0) {
            continue;
        }

        if ((n_lat > lat - delta) && (n_lat < lat + delta) && (n_lon > lng - delta) && (n_lon < lng + delta)) {

            double d = calculateDistance(lat, lng, n_lat, n_lon);
            if (d < min_dist) {
                min_dist = d;
                closest_id = i;
            }
        }
    }
    return closest_id;
}

