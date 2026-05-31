#include "FlowNetwork.h"

#include "Utilities.h"

FlowNetwork::FlowNetwork(int s, int t, Graph&& graph)
    : s(s), t(t), Graph(std::move(graph)) {
    try {
        flow = new int[maxEdges]{0};
        capacity = new int[maxEdges]{0};
        potential = new double[numberOfNodes]{0.0};
        back_edges_pool = new Edge[maxEdges];

    } catch (std::bad_alloc& e) {
        failed_allocation_message(e);
    }

    std::fill_n(capacity, maxEdges, INFINITY_VALUE_INT);

    for (int i = 0; i < edge_count; i++) {
        int newNode1 = edges_pool[i].node2;
        int newNode2 = edges_pool[i].node1;
        back_edges_pool[i].node1 = newNode1;
        back_edges_pool[i].node2 = newNode2;
        back_edges_pool[i].weight = -edges_pool[i].weight;

        back_edges_pool[i].next_out = out_edges[newNode1];
        out_edges[newNode1] = &back_edges_pool[i];

        back_edges_pool[i].next_in = in_edges[newNode2];
        in_edges[newNode2] = &back_edges_pool[i];
    }
}



FlowNetwork::~FlowNetwork() {
    delete[] back_edges_pool;
    delete[] flow;
    delete[] potential;
    delete[] capacity;
}

int FlowNetwork::getFlow(Edge* edge) {
   if (isBackEdge(edge)) {
       return -1;
   }
    return flow[getEdgeIndex(edge)];
}

std::list<Edge*> FlowNetwork::getEdgesByFlow(int requestedFlow) {
    std::list<Edge*> edges;
    for (int i = 0; i < edge_count; i++) {
        if (flow[i] == requestedFlow) {
            edges.emplace_back(&edges_pool[i]);
        }
    }
    return edges;
}

std::vector<Edge*> FlowNetwork::getEdgeswithFlow() {
    std::vector<Edge*> edges;
    for (int i = 0; i < edge_count; i++) {
        if (flow[i] > 0) {
            edges.push_back(&edges_pool[i]);
        }
    }
    return edges;
}

Edge *FlowNetwork::addEdge(int node1_index, int node2_index, double weight, int cap) {
    Edge* curr = out_edges[node1_index];
    while (curr != nullptr) {
        if (curr->node2 == node2_index && !isBackEdge(curr)) {
            int index = getEdgeIndex(curr);
            if (weight < edges_pool[index].weight) {
                edges_pool[index].weight = weight;
            }
            back_edges_pool[index].weight = -weight;

            flow[index] = 0;
            capacity[index] = cap;

            return curr;
        }
        curr = curr->next_out;
    }

    int i = edge_count;

    Edge* toRet = Graph::addEdge(node1_index, node2_index, weight);

    back_edges_pool[i].node1 = node2_index;
    back_edges_pool[i].node2 = node1_index;
    back_edges_pool[i].weight = -weight;

    back_edges_pool[i].next_out = out_edges[node2_index];
    out_edges[node2_index] = &back_edges_pool[i];

    back_edges_pool[i].next_in = in_edges[node1_index];
    in_edges[node1_index] = &back_edges_pool[i];

    flow[i] = 0;
    capacity[i] = cap;
    return toRet;
}

void FlowNetwork::removeEdge(int node1_index, int node2_index)  {
    setCapacity(getEdge(node1_index, node2_index), 0);
}

void FlowNetwork::resetFlow() {
    std::fill_n(potential, numberOfNodes, 0.0);
    std::fill_n(flow, maxEdges, 0.0);
    infinite_flow = false;
}

double FlowNetwork::getCost() {
    if (infinite_flow) {
        return INFINITY_VALUE_DOUBLE;
    }
    double totalCost = 0.0;

    for (int i = 0; i < edge_count; i++) {
        if (flow[i] > 0) {
            totalCost += flow[i] * edges_pool[i].weight;
        }
    }
    return totalCost;
}

int FlowNetwork::getEdgeIndex(Edge* edge) {
    if (edge == nullptr ||
        ((edge < edges_pool || edge >= (edges_pool + edge_count)) &&
        (edge < back_edges_pool || edge >= (back_edges_pool + edge_count)))) {
        return -1;
    }
    if (isBackEdge(edge)) {
        return (edge - back_edges_pool);
    } else {
        return (edge - edges_pool);

    }
}

bool FlowNetwork::isBackEdge(Edge* edge) {
    if (edge >= back_edges_pool && edge < (back_edges_pool + edge_count)) {
        return true;
    } else {
        return false;
    }
}

int FlowNetwork::setCapacity(Edge* edge, int newCapacity) {
    int index = getEdgeIndex(edge);

    if (index == -1) {
        return -1;
    }
    int oldCapacity = capacity[index];
    capacity[index] = newCapacity;
    return oldCapacity;
}

bool FlowNetwork::edgeExistsInResidualGraph(Edge* edge) {
    if (edge == nullptr) {
        throw std::invalid_argument("edge is null");
    }

    int index = getEdgeIndex(edge);

    return isBackEdge(edge) ?
    (flow[index] > 0) :
    ((capacity[index] - flow[index] > 0) || (capacity[index] == INFINITY_VALUE_INT));
}

int FlowNetwork::findBottleneck(Path path) {
    Edge* current = nullptr;
    int currentIndex = -1;
    int min = -1;

    if (path.size() == 0) {
        throw std::invalid_argument("path is empty");
    }
    bool bottleNeckIsFinite = false;

    for (Path::iterator it = path.begin(); it != path.end(); ++it) {
        current = *it;
        currentIndex = getEdgeIndex(current);
        if (isBackEdge(current) || capacity[currentIndex] != INFINITY_VALUE_INT) {
            bottleNeckIsFinite = true;
        }
        if (!isBackEdge(current)) {
            if (capacity[currentIndex] == INFINITY_VALUE_INT) {
                continue;
            }
            min = min > capacity[currentIndex] - flow[currentIndex] || (min == -1)?
            capacity[currentIndex] - flow[currentIndex] : min;
        } else {
            min = min > flow[currentIndex] || (min == -1) ? flow[currentIndex] : min;
        }

    }
    return bottleNeckIsFinite ? min : INFINITY_VALUE_INT;
}

int FlowNetwork::augmentPath(Path path) {
    int bottleNeck = findBottleneck(path);
    Edge* current = nullptr;

    if (bottleNeck == INFINITY_VALUE_INT) {
        return INFINITY_VALUE_INT;
    }

    int sign;
    for (Path::iterator it = path.begin(); it != path.end(); ++it) {
        current = *it;
        sign = isBackEdge(current) ? -1 : 1;
        flow[getEdgeIndex(current)] += sign*bottleNeck;
    }
    return bottleNeck;
}



LightestPathInfo* FlowNetwork::findLightestPath(int startingNode) {
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
        info[currentNode].dv = heapNode.first - potential[startingNode] + potential[currentNode];
        counter++;

        edge = out_edges[currentNode];

        while (edge != nullptr) {
            if (!edgeExistsInResidualGraph(edge)) {
                edge = edge->next_out;
                continue;
            }
            if (minHeap[edge->node1] + edge->weight + potential[edge->node1] - potential[edge->node2]
                < minHeap[edge->node2]) {
                minHeap.decreaseKey(edge->node2, minHeap[edge->node1] + edge->weight
                    + potential[edge->node1] - potential[edge->node2]);
                info[edge->node2].incoming_edge = edge;
                }
            edge = edge->next_out;
        }

    }

    for (int i = 0; i < numberOfNodes; i++) {
        if (info[i].dv != INFINITY_VALUE_DOUBLE) {
            potential[i] += (info[i].dv + potential[startingNode] - potential[i]);
        }
    }

    delete[] heap;
    return info;
}


ShortestPathInfo* FlowNetwork::findShortestPath(int startingNode) {
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
            if (!edgeExistsInResidualGraph(current)) {
                current = current->next_out;
                continue;
            }
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

int FlowNetwork::applyMaxFlow(bool min_cost) {
    if (s==t) {
        return 0;
    }
    PathInfo* info = nullptr;

    int currentFlow = 0;
    info = min_cost ? findLightestPath(s) : findShortestPath(s);
    int augmentation;
    while (info[t].dv != INFINITY_VALUE_DOUBLE) {
        Path path = getPath(info, s, t);
        augmentation = augmentPath(path);
        if (augmentation == INFINITY_VALUE_INT) {
            infinite_flow = true;
            delete[] info;
            return INFINITY_VALUE_INT;
        }
        currentFlow += augmentation;
        delete[] info;
        info = min_cost ? findLightestPath(s) : findShortestPath(s);
    }
    delete[] info;
    return currentFlow;
}
