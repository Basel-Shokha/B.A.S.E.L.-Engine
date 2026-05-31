# B.A.S.E.L. Engine
**Bot for Autonomous Search and Efficient Logistics**  
*C++ · Graph Theory · Fleet Optimization · Algorithms*

---

A fleet intelligence and routing engine built entirely from scratch in C++. Loads a full road network of **4.3M nodes and 9.1M edges** from a custom binary format and solves three real-world autonomous logistics problems — battery-aware EV routing, robot fleet dispatch, and swarm coordination — all on the same graph.

🔗 **[View Live Demo](https://basel-shokha.github.io/basel-engine-hosting-v/)**  
📄 **[Algorithm Whitepapers](https://drive.google.com/drive/folders/1kWGzsBIuERVGgthmQftyYQX7H_oXqllH)**

---

## How do you want to run it?

### Option A — Just use the browser (no setup)

Go to the live demo link above. Pick a solver, fill in the map UI, and submit. Nothing to install.

---

### Option B — Run it locally on your machine

The local version runs the full C++ engine natively — same algorithm, direct access, no browser in the way.

#### Step 1 — Prerequisites

You need a C++ compiler:

- **Linux:** `sudo apt install g++`
- **macOS:** `xcode-select --install`
- **Windows:** Use WSL (recommended) or install [MinGW-w64](https://www.mingw-w64.org/)

No other dependencies. The only external library is `json.hpp` (nlohmann), already bundled in `External Libraries/`.

#### Step 2 — Clone the repo

```bash
git clone https://github.com/Basel-Shokha/B.A.S.E.L.-Engine.git
cd B.A.S.E.L.-Engine
```

#### Step 3 — Build and start the engine

```bash
make
```

Compiles everything and launches the engine immediately. You'll see:

```
[B.A.S.E.L. Engine] Engine is LIVE. Waiting for UI clicks...
```

The engine is now running and watching for a `request.json` file in the same folder.

#### Step 4 — Send it a request

Drop a `request.json` file in the project folder. The engine detects it automatically, runs the solver, writes `response.json`, and goes back to waiting.

**EV Route (f1):**
```json
{
  "mode": "f1",
  "start": { "lat": 32.08, "lng": 34.78 },
  "end":   { "lat": 31.76, "lng": 35.23 },
  "battery_meters": 80000,
  "charging_stations": [{ "lat": 31.90, "lng": 35.00 }]
}
```

**Fleet Dispatch (f2):**
```json
{
  "mode": "f2",
  "robots": [...],
  "vehicles": [...]
}
```

**Swarm Control (f3):**
```json
{
  "mode": "f3",
  "start": { "lat": 32.08, "lng": 34.78 },
  "end":   { "lat": 31.76, "lng": 35.23 },
  "units": 4,
  "battery_capacity": 50000,
  "batteries": [...]
}
```

Full request/response specs are in the whitepapers linked above.

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

**Swarm coordination** — multiple units routed simultaneously with global optimality guarantees across the shared road graph.

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
| `main.cpp` | Entry point — loads the graph, polls for `request.json`, dispatches to the right solver |
| `index.html` | Browser frontend |

---

## Makefile Targets

| Command | What it does |
|---|---|
| `make` | Build and launch the engine |
| `make clean` | Remove compiled objects and binary |
