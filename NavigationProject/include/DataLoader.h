#pragma once

#include "Graph.h"

#include <string>
#include <vector>

class DataLoader {
public:
    static std::vector<Node> loadNodes(const std::string& filePath);
    static std::vector<Edge> loadEdges(const std::string& filePath);
    static Graph loadGraph(
        const std::string& nodesPath,
        const std::string& edgesPath,
        bool bidirectional = true
    );
};
