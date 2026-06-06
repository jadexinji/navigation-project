# C++ Map Navigation System

This is the first runnable MVP for the navigation project. It uses handwritten CSV map data, loads the graph, runs Dijkstra and A* shortest-path search, and exports the computed path.

## Current Features

- Load nodes from `resources/nodes.csv`
- Load edges from `resources/edges.csv`
- Build a bidirectional weighted graph
- Run Dijkstra shortest path
- Run A* shortest path with Euclidean distance heuristic
- Save path output to CSV and JSON
- Render a map-like SVG preview with roads, highlighted route, and start/end markers
- Open an interactive map UI for mouse-based start/end selection

## Build And Run

From the `NavigationProject` folder:

```bash
cmake -S . -B build
cmake --build build
./build/navigate
```

If CMake is not installed, compile directly with clang:

```bash
clang++ -std=c++17 -Iinclude src/main.cpp src/DataLoader.cpp src/Graph.cpp src/MapRenderer.cpp -o navigate
./navigate
```

Optional arguments:

```bash
./build/navigate resources/nodes.csv resources/edges.csv 1 24
```

Interactive mode:

```bash
./build/navigate --interactive
```

Arguments are:

```text
nodes.csv edges.csv start_node_id goal_node_id
```

## CSV Format

`nodes.csv`

```csv
id,x,y
1,80,80
2,180,80
```

`edges.csv`

```csv
from,to,distance
1,2,100
```

## Next Step

The next practical step is adding a map renderer and mouse selection UI on top of this core graph module.
The project now includes an SVG renderer, so the next practical step is an interactive UI for selecting the start and end points.

## Interactive Map UI

![Interactive map preview](docs/map-ui-screenshot.svg)

Start a local preview server from the `NavigationProject` folder:

```bash
python3 -m http.server 4173
```

Then open:

```text
http://localhost:4173/ui/index.html
```

The UI reads `resources/nodes.csv` and `resources/edges.csv`, supports mouse selection for start and destination nodes, can switch between A* and Dijkstra, and can export the current route as CSV, JSON, or SVG.
