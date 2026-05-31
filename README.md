# B.A.S.E.L. Engine
**Bot for Autonomous Search and Efficient Logistics**  
*C++ · Graph Theory · Fleet Optimization · Algorithms*

---

A fleet intelligence and routing engine built entirely from scratch in C++. Loads a full road network of **4.3M nodes and 9.1M edges** from a custom binary format and solves three real-world autonomous logistics problems — battery-aware EV routing, robot fleet dispatch, and swarm coordination — all on the same graph.

🔗 **[View Live Demo](https://basel-shokha.github.io/basel-engine-hosting-v/)**  
📄 **[Algorithm Whitepapers](https://drive.google.com/drive/folders/1kWGzsBIuERVGgthmQftyYQX7H_oXqllH)**

---

## The Three Solvers

**🔋 EV Route Planner** — plans a battery-aware route between any two points on the map. Given a battery range in meters, it finds the optimal path and snaps to charging stations along the way when needed, returning a full leg-by-leg breakdown with distances and charging stops.

**🤖 Fleet Dispatcher** — assigns a fleet of fixer robots to broken vehicles across the map. Each vehicle type carries a different capacity cost; the dispatcher solves the globally optimal robot-to-vehicle assignment that minimizes total travel distance using a min-cost max-flow network.

**🐝 Swarm Controller** — routes multiple autonomous units from a shared start to a shared end, collecting battery packs scattered along the way. Handles collision detection between unit paths and distributes battery collection across the swarm without conflicts.

---

## Technical Highlights

**Custom binary map format** — raw OSM `.pbf` files require expensive parsing on every load. Converted once into flat binary files: nodes (ID + lat/lng) and edges (node pairs + weights). Zero-overhead instant loading at startup.

**Custom MinHeap** — dynamic position tracking for strict O(log V) `decreaseKey` operations during Dijkstra traversals. No STL priority queue shortcuts.

**Manual memory pools** — eliminates OS-level heap allocations during intensive compute cycles. The edge pool is pre-allocated once; no `new` inside the hot path.

**Min-Cost Max-Flow** — battery constraints and robot capacity limits are modeled as flow capacities via node-splitting, transforming physical dispatch problems into graph optimization problems.

**Swarm coordination** — multiple EVs routed simultaneously with global optimality guarantees across the shared road graph.

---

## Architecture

| File | Role |
|---|---|
| `Graph.cpp/h` | Core directed graph — adjacency lists, Dijkstra, BFS, path reconstruction, graph condensation, spatial nearest-node search |
| `MinHeap.cpp/h` | Custom priority queue with O(log V) decreaseKey |
| `MapLoader.cpp/h` | Binary map data loader |
| `FlowNetwork.cpp/h` | Min-cost max-flow with condensed edge mapper |
| `EVRoutePlanner.cpp/h` | Feature 1 — battery-aware EV routing |
| `FleetDispatcher.cpp/h` | Feature 2 — robot-to-vehicle assignment |
| `SwarmController.cpp/h` | Feature 3 — multi-unit swarm routing |
| `Utilities.cpp/h` | Shared helpers |
| `main.cpp` | Entry point — loads the graph, reads JSON request, dispatches to the right solver |
| `index.html` | Browser frontend |

---

## Input / Output

All solvers communicate via JSON. Send a request, get a response. The `mode` field routes to the correct solver.

```json
{
  "mode": "ev_route",
  "start": { "lat": 32.08, "lng": 34.78 },
  "end":   { "lat": 31.76, "lng": 35.23 },
  "battery_meters": 80000,
  "charging_stations": [{ "lat": 31.90, "lng": 35.00 }]
}
```

Full API contracts and edge case handling are documented in the whitepapers.

---

## Building

```bash
make
```

Requires g++ with C++17 and pthreads. No external dependencies beyond the bundled `json.hpp` (nlohmann) in `External Libraries/`.
