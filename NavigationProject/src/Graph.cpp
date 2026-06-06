#include "Graph.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>
#include <stdexcept>
#include <unordered_set>

namespace {
struct QueueItem {
    int nodeId{};
    double priority{};

    bool operator>(const QueueItem& other) const {
        return priority > other.priority;
    }
};
}

void Graph::addNode(const Node& node) {
    nodes_[node.id] = node;
}

void Graph::addEdge(const Edge& edge, bool bidirectional) {
    if (!hasNode(edge.from) || !hasNode(edge.to)) {
        throw std::runtime_error("Edge references a node that was not loaded.");
    }

    edges_.push_back(edge);
    adjacency_[edge.from].push_back(edge);

    if (bidirectional) {
        adjacency_[edge.to].push_back(Edge{edge.to, edge.from, edge.distance});
    }
}

bool Graph::hasNode(int id) const {
    return nodes_.find(id) != nodes_.end();
}

const Node& Graph::getNode(int id) const {
    auto it = nodes_.find(id);
    if (it == nodes_.end()) {
        throw std::out_of_range("Node not found.");
    }
    return it->second;
}

const std::unordered_map<int, Node>& Graph::getNodes() const {
    return nodes_;
}

const std::vector<Edge>& Graph::getEdges() const {
    return edges_;
}

PathResult Graph::dijkstra(int startId, int goalId) const {
    if (!hasNode(startId) || !hasNode(goalId)) {
        return {};
    }

    const double infinity = std::numeric_limits<double>::infinity();
    std::unordered_map<int, double> distances;
    std::unordered_map<int, int> previous;
    std::priority_queue<QueueItem, std::vector<QueueItem>, std::greater<QueueItem>> frontier;

    for (const auto& pair : nodes_) {
        distances[pair.first] = infinity;
    }

    distances[startId] = 0.0;
    frontier.push(QueueItem{startId, 0.0});

    while (!frontier.empty()) {
        QueueItem current = frontier.top();
        frontier.pop();

        if (current.priority > distances[current.nodeId]) {
            continue;
        }

        if (current.nodeId == goalId) {
            return buildPath(startId, goalId, previous, distances[goalId]);
        }

        auto edgesIt = adjacency_.find(current.nodeId);
        if (edgesIt == adjacency_.end()) {
            continue;
        }

        for (const Edge& edge : edgesIt->second) {
            double candidate = distances[current.nodeId] + edge.distance;
            if (candidate < distances[edge.to]) {
                distances[edge.to] = candidate;
                previous[edge.to] = current.nodeId;
                frontier.push(QueueItem{edge.to, candidate});
            }
        }
    }

    return {};
}

PathResult Graph::aStar(int startId, int goalId) const {
    if (!hasNode(startId) || !hasNode(goalId)) {
        return {};
    }

    const double infinity = std::numeric_limits<double>::infinity();
    std::unordered_map<int, double> costFromStart;
    std::unordered_map<int, int> previous;
    std::priority_queue<QueueItem, std::vector<QueueItem>, std::greater<QueueItem>> frontier;

    for (const auto& pair : nodes_) {
        costFromStart[pair.first] = infinity;
    }

    costFromStart[startId] = 0.0;
    frontier.push(QueueItem{startId, heuristic(startId, goalId)});

    while (!frontier.empty()) {
        QueueItem current = frontier.top();
        frontier.pop();

        if (current.nodeId == goalId) {
            return buildPath(startId, goalId, previous, costFromStart[goalId]);
        }

        auto edgesIt = adjacency_.find(current.nodeId);
        if (edgesIt == adjacency_.end()) {
            continue;
        }

        for (const Edge& edge : edgesIt->second) {
            double candidate = costFromStart[current.nodeId] + edge.distance;
            if (candidate < costFromStart[edge.to]) {
                costFromStart[edge.to] = candidate;
                previous[edge.to] = current.nodeId;
                double priority = candidate + heuristic(edge.to, goalId);
                frontier.push(QueueItem{edge.to, priority});
            }
        }
    }

    return {};
}

double Graph::heuristic(int fromId, int toId) const {
    const Node& from = getNode(fromId);
    const Node& to = getNode(toId);
    const double dx = from.x - to.x;
    const double dy = from.y - to.y;
    return std::sqrt(dx * dx + dy * dy);
}

PathResult Graph::buildPath(
    int startId,
    int goalId,
    const std::unordered_map<int, int>& previous,
    double distance
) const {
    std::vector<int> path;
    int current = goalId;
    path.push_back(current);

    while (current != startId) {
        auto it = previous.find(current);
        if (it == previous.end()) {
            return {};
        }
        current = it->second;
        path.push_back(current);
    }

    std::reverse(path.begin(), path.end());
    return PathResult{true, distance, path};
}
