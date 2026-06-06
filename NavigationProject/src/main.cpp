#include "DataLoader.h"
#include "MapRenderer.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
void printResult(const std::string& algorithm, const PathResult& result) {
    std::cout << algorithm << "\n";

    if (!result.found) {
        std::cout << "  No path found.\n";
        return;
    }

    std::cout << "  Distance: " << result.distance << "\n";
    std::cout << "  Path: ";

    for (std::size_t i = 0; i < result.nodes.size(); ++i) {
        if (i > 0) {
            std::cout << " -> ";
        }
        std::cout << result.nodes[i];
    }

    std::cout << "\n";
}

void savePathCsv(const std::string& filePath, const PathResult& result) {
    std::ofstream file(filePath);
    if (!file) {
        throw std::runtime_error("Cannot write CSV output: " + filePath);
    }

    file << "order,node_id\n";
    for (std::size_t i = 0; i < result.nodes.size(); ++i) {
        file << (i + 1) << "," << result.nodes[i] << "\n";
    }
}

void savePathJson(const std::string& filePath, const PathResult& result) {
    std::ofstream file(filePath);
    if (!file) {
        throw std::runtime_error("Cannot write JSON output: " + filePath);
    }

    file << std::fixed << std::setprecision(2);
    file << "{\n";
    file << "  \"found\": " << (result.found ? "true" : "false") << ",\n";
    file << "  \"distance\": " << result.distance << ",\n";
    file << "  \"path\": [";

    for (std::size_t i = 0; i < result.nodes.size(); ++i) {
        if (i > 0) {
            file << ", ";
        }
        file << result.nodes[i];
    }

    file << "]\n";
    file << "}\n";
}

std::vector<int> sortedNodeIds(const Graph& graph) {
    std::vector<int> ids;
    ids.reserve(graph.getNodes().size());

    for (const auto& pair : graph.getNodes()) {
        ids.push_back(pair.first);
    }

    std::sort(ids.begin(), ids.end());
    return ids;
}

void printAvailableNodes(const Graph& graph) {
    std::cout << "Available nodes: ";
    const std::vector<int> ids = sortedNodeIds(graph);

    for (std::size_t i = 0; i < ids.size(); ++i) {
        if (i > 0) {
            std::cout << ", ";
        }
        std::cout << ids[i];
    }

    std::cout << "\n";
}

void saveOutputs(const Graph& graph, const PathResult& result, int startId, int goalId) {
    if (!result.found) {
        return;
    }

    savePathCsv("path_result.csv", result);
    savePathJson("path_result.json", result);
    MapRenderer::saveSvg(graph, result, "path_preview.svg", startId, goalId);
    std::cout << "Saved outputs: path_result.csv, path_result.json, path_preview.svg\n";
}

bool readNodeId(const Graph& graph, const std::string& label, int& value) {
    std::cout << label;

    if (!(std::cin >> value)) {
        return false;
    }

    if (!graph.hasNode(value)) {
        std::cout << "Node " << value << " does not exist.\n";
        return false;
    }

    return true;
}

void runInteractive(const Graph& graph) {
    std::cout << "Interactive navigation mode\n";
    printAvailableNodes(graph);

    while (true) {
        int startId = 0;
        int goalId = 0;

        if (!readNodeId(graph, "Start node: ", startId)) {
            break;
        }

        if (!readNodeId(graph, "Goal node: ", goalId)) {
            break;
        }

        PathResult dijkstraResult = graph.dijkstra(startId, goalId);
        PathResult aStarResult = graph.aStar(startId, goalId);

        printResult("Dijkstra", dijkstraResult);
        printResult("A*", aStarResult);
        saveOutputs(graph, aStarResult, startId, goalId);

        std::cout << "Try another route? (y/n): ";
        char answer = 'n';
        std::cin >> answer;

        if (answer != 'y' && answer != 'Y') {
            break;
        }
    }
}
}

int main(int argc, char* argv[]) {
    const bool interactive = argc > 1 && std::string(argv[1]) == "--interactive";
    const int offset = interactive ? 1 : 0;
    const std::string nodesPath = argc > 1 + offset ? argv[1 + offset] : "resources/nodes.csv";
    const std::string edgesPath = argc > 2 + offset ? argv[2 + offset] : "resources/edges.csv";
    const int startId = argc > 3 + offset ? std::stoi(argv[3 + offset]) : 1;
    const int goalId = argc > 4 + offset ? std::stoi(argv[4 + offset]) : 24;

    try {
        Graph graph = DataLoader::loadGraph(nodesPath, edgesPath);

        if (interactive) {
            runInteractive(graph);
            return 0;
        }

        PathResult dijkstraResult = graph.dijkstra(startId, goalId);
        PathResult aStarResult = graph.aStar(startId, goalId);

        printResult("Dijkstra", dijkstraResult);
        printResult("A*", aStarResult);
        saveOutputs(graph, aStarResult, startId, goalId);
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << "\n";
        return 1;
    }

    return 0;
}
