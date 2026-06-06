#pragma once

#include "Graph.h"

#include <string>

class MapRenderer {
public:
    static void saveSvg(
        const Graph& graph,
        const PathResult& path,
        const std::string& filePath,
        int startId = -1,
        int goalId = -1,
        int width = 720,
        int height = 520
    );
};
