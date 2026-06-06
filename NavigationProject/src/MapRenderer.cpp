#include "MapRenderer.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include <unordered_set>

namespace {
struct Bounds {
    double minX{};
    double maxX{};
    double minY{};
    double maxY{};
};

Bounds findBounds(const Graph& graph) {
    Bounds bounds{
        std::numeric_limits<double>::infinity(),
        -std::numeric_limits<double>::infinity(),
        std::numeric_limits<double>::infinity(),
        -std::numeric_limits<double>::infinity()
    };

    for (const auto& pair : graph.getNodes()) {
        const Node& node = pair.second;
        bounds.minX = std::min(bounds.minX, node.x);
        bounds.maxX = std::max(bounds.maxX, node.x);
        bounds.minY = std::min(bounds.minY, node.y);
        bounds.maxY = std::max(bounds.maxY, node.y);
    }

    return bounds;
}

std::string edgeKey(int from, int to) {
    return std::to_string(from) + "-" + std::to_string(to);
}

double roadWidth(double distance) {
    if (distance >= 170.0) {
        return 8.0;
    }

    if (distance >= 120.0) {
        return 6.0;
    }

    return 4.0;
}
}

void MapRenderer::saveSvg(
    const Graph& graph,
    const PathResult& path,
    const std::string& filePath,
    int startId,
    int goalId,
    int width,
    int height
) {
    if (graph.getNodes().empty()) {
        throw std::runtime_error("Cannot render an empty graph.");
    }

    std::ofstream file(filePath);
    if (!file) {
        throw std::runtime_error("Cannot write SVG output: " + filePath);
    }

    const Bounds bounds = findBounds(graph);
    const double padding = 56.0;
    const double mapWidth = std::max(1.0, bounds.maxX - bounds.minX);
    const double mapHeight = std::max(1.0, bounds.maxY - bounds.minY);
    const double scale = std::min(
        (width - padding * 2.0) / mapWidth,
        (height - padding * 2.0) / mapHeight
    );

    auto screenX = [&](double x) {
        return padding + (x - bounds.minX) * scale;
    };

    auto screenY = [&](double y) {
        return padding + (y - bounds.minY) * scale;
    };

    std::unordered_set<std::string> pathEdges;
    for (std::size_t i = 1; i < path.nodes.size(); ++i) {
        pathEdges.insert(edgeKey(path.nodes[i - 1], path.nodes[i]));
        pathEdges.insert(edgeKey(path.nodes[i], path.nodes[i - 1]));
    }

    file << std::fixed << std::setprecision(2);
    file << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << width
         << "\" height=\"" << height << "\" viewBox=\"0 0 " << width << " " << height << "\">\n";
    file << "  <rect width=\"100%\" height=\"100%\" fill=\"#eef2ee\"/>\n";
    file << "  <rect x=\"32\" y=\"44\" width=\"210\" height=\"118\" rx=\"16\" fill=\"#d9ead7\" opacity=\"0.75\"/>\n";
    file << "  <rect x=\"474\" y=\"72\" width=\"148\" height=\"124\" rx=\"14\" fill=\"#dbeafe\" opacity=\"0.65\"/>\n";
    file << "  <rect x=\"96\" y=\"362\" width=\"180\" height=\"82\" rx=\"14\" fill=\"#fef3c7\" opacity=\"0.7\"/>\n";
    file << "  <rect x=\"430\" y=\"352\" width=\"132\" height=\"88\" rx=\"14\" fill=\"#dcfce7\" opacity=\"0.7\"/>\n";
    file << "  <text x=\"24\" y=\"30\" fill=\"#1f2937\" font-size=\"18\" font-family=\"Arial\">Sample City Road Map</text>\n";

    for (const Edge& edge : graph.getEdges()) {
        const Node& from = graph.getNode(edge.from);
        const Node& to = graph.getNode(edge.to);
        const bool highlighted = pathEdges.find(edgeKey(edge.from, edge.to)) != pathEdges.end();

        file << "  <line x1=\"" << screenX(from.x) << "\" y1=\"" << screenY(from.y)
             << "\" x2=\"" << screenX(to.x) << "\" y2=\"" << screenY(to.y)
             << "\" stroke=\"#ffffff\" stroke-width=\"" << (roadWidth(edge.distance) + 4.0)
             << "\" stroke-linecap=\"round\"/>\n";
        file << "  <line x1=\"" << screenX(from.x) << "\" y1=\"" << screenY(from.y)
             << "\" x2=\"" << screenX(to.x) << "\" y2=\"" << screenY(to.y)
             << "\" stroke=\"" << (highlighted ? "#dc2626" : "#9ca3af")
             << "\" stroke-width=\"" << (highlighted ? 8 : roadWidth(edge.distance))
             << "\" stroke-linecap=\"round\"/>\n";
    }

    for (const auto& pair : graph.getNodes()) {
        const Node& node = pair.second;
        const bool isStart = node.id == startId;
        const bool isGoal = node.id == goalId;

        if (isStart || isGoal) {
            file << "  <circle cx=\"" << screenX(node.x) << "\" cy=\"" << screenY(node.y)
                 << "\" r=\"16\" fill=\"" << (isStart ? "#16a34a" : "#2563eb")
                 << "\" stroke=\"#ffffff\" stroke-width=\"4\"/>\n";
            file << "  <text x=\"" << screenX(node.x) << "\" y=\"" << (screenY(node.y) + 5)
                 << "\" fill=\"#ffffff\" font-size=\"13\" text-anchor=\"middle\" font-family=\"Arial\" font-weight=\"bold\">"
                 << (isStart ? "S" : "E") << "</text>\n";
        } else {
            file << "  <circle cx=\"" << screenX(node.x) << "\" cy=\"" << screenY(node.y)
                 << "\" r=\"4\" fill=\"#475569\"/>\n";
        }
    }

    if (path.found) {
        file << "  <text x=\"24\" y=\"" << (height - 24)
             << "\" fill=\"#334155\" font-size=\"16\" font-family=\"Arial\">Shortest distance: "
             << path.distance << "</text>\n";
    }

    file << "</svg>\n";
}
