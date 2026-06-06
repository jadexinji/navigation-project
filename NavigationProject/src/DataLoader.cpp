#include "DataLoader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace {
std::vector<std::string> splitCsvLine(const std::string& line) {
    std::vector<std::string> cells;
    std::stringstream stream(line);
    std::string cell;

    while (std::getline(stream, cell, ',')) {
        cells.push_back(cell);
    }

    return cells;
}
}

std::vector<Node> DataLoader::loadNodes(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file) {
        throw std::runtime_error("Cannot open nodes file: " + filePath);
    }

    std::vector<Node> nodes;
    std::string line;
    bool firstLine = true;

    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        if (firstLine) {
            firstLine = false;
            if (line.find("id") != std::string::npos) {
                continue;
            }
        }

        std::vector<std::string> cells = splitCsvLine(line);
        if (cells.size() < 3) {
            throw std::runtime_error("Invalid node row: " + line);
        }

        nodes.push_back(Node{
            std::stoi(cells[0]),
            std::stod(cells[1]),
            std::stod(cells[2])
        });
    }

    return nodes;
}

std::vector<Edge> DataLoader::loadEdges(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file) {
        throw std::runtime_error("Cannot open edges file: " + filePath);
    }

    std::vector<Edge> edges;
    std::string line;
    bool firstLine = true;

    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        if (firstLine) {
            firstLine = false;
            if (line.find("from") != std::string::npos) {
                continue;
            }
        }

        std::vector<std::string> cells = splitCsvLine(line);
        if (cells.size() < 3) {
            throw std::runtime_error("Invalid edge row: " + line);
        }

        edges.push_back(Edge{
            std::stoi(cells[0]),
            std::stoi(cells[1]),
            std::stod(cells[2])
        });
    }

    return edges;
}

Graph DataLoader::loadGraph(
    const std::string& nodesPath,
    const std::string& edgesPath,
    bool bidirectional
) {
    Graph graph;

    for (const Node& node : loadNodes(nodesPath)) {
        graph.addNode(node);
    }

    for (const Edge& edge : loadEdges(edgesPath)) {
        graph.addEdge(edge, bidirectional);
    }

    return graph;
}
