#pragma once

#include <string>
#include <unordered_map>
#include <vector>

struct Node {
    int id{};
    double x{};
    double y{};
};

struct Edge {
    int from{};
    int to{};
    double distance{};
};

struct PathResult {
    bool found{false};
    double distance{0.0};
    std::vector<int> nodes;
};

class Graph {
public:
    void addNode(const Node& node);
    void addEdge(const Edge& edge, bool bidirectional = true);

    bool hasNode(int id) const;
    const Node& getNode(int id) const;
    const std::unordered_map<int, Node>& getNodes() const;
    const std::vector<Edge>& getEdges() const;

    PathResult dijkstra(int startId, int goalId) const;
    PathResult aStar(int startId, int goalId) const;

private:
    std::unordered_map<int, Node> nodes_;
    std::vector<Edge> edges_;
    std::unordered_map<int, std::vector<Edge>> adjacency_;

    double heuristic(int fromId, int toId) const;
    PathResult buildPath(
        int startId,
        int goalId,
        const std::unordered_map<int, int>& previous,
        double distance
    ) const;
};
