#ifndef TRAVERSAL_H
#define TRAVERSAL_H

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <unordered_map>
#include <algorithm>

using namespace std;

// Represents a computer/server/network device
struct Node {
    int id;
    string name;
    int criticality;       // 1 - 100
    bool critical;         // Is this a high-value target?
};

// Represents a directed network connection
struct Edge {
    int id;
    int from;
    int to;

    // Used by Dijkstra
    double baseCost;
    double firewallPenalty;
    double anomalyScore;
    double attackWeight;

    // Used by Min-Cut
    double businessCost;
};

// Complete network graph
class Graph {
private:
    vector<Node> nodes;
    vector<Edge> edges;
    vector<vector<int>> adjacency;

public:

    Graph() {}

    void initialize(int n) {
        nodes.clear();
        edges.clear();
        adjacency.assign(n, {});
    }

    void addNode(int id, const string& name,
                 int criticality, bool critical) {

        if (id >= (int)nodes.size()) {
            nodes.resize(id + 1);
        }

        nodes[id] = {id, name, criticality, critical};
    }

    void addEdge(int id, int from, int to,
                 double baseCost,
                 double firewallPenalty,
                 double anomalyScore,
                 double businessCost) {

        double attackWeight =
            baseCost + firewallPenalty - anomalyScore;

        // Edge weights must remain positive
        attackWeight = max(0.1, attackWeight);

        Edge e{
            id,
            from,
            to,
            baseCost,
            firewallPenalty,
            anomalyScore,
            attackWeight,
            businessCost
        };

        edges.push_back(e);
        adjacency[from].push_back((int)edges.size() - 1);
    }

    const vector<Node>& getNodes() const {
        return nodes;
    }

    const vector<Edge>& getEdges() const {
        return edges;
    }

    const vector<vector<int>>& getAdjacency() const {
        return adjacency;
    }

    const Node& getNode(int id) const {
        return nodes[id];
    }

    const Edge& getEdge(int index) const {
        return edges[index];
    }

    int size() const {
        return nodes.size();
    }

    vector<int> getCriticalNodes() const {
        vector<int> result;

        for (const auto& node : nodes) {
            if (node.critical) {
                result.push_back(node.id);
            }
        }

        return result;
    }

    double totalBusinessCost() const {
        double total = 0;

        for (const auto& edge : edges) {
            total += edge.businessCost;
        }

        return total;
    }

    void printGraph() const {
        cout << "\n========== NETWORK GRAPH ==========\n";

        cout << "\nNodes:\n";

        for (const auto& node : nodes) {
            cout << "  [" << node.id << "] "
                 << node.name
                 << " | Criticality: " << node.criticality
                 << " | ";

            if (node.critical)
                cout << "HIGH-VALUE TARGET";
            else
                cout << "Normal";

            cout << '\n';
        }

        cout << "\nConnections:\n";

        for (const auto& edge : edges) {
            cout << "  [" << edge.id << "] "
                 << nodes[edge.from].name
                 << " -> "
                 << nodes[edge.to].name
                 << "\n";

            cout << "      Attack weight: "
                 << edge.attackWeight
                 << " | Business cost: "
                 << edge.businessCost << '\n';
        }

        cout << "===================================\n";
    }
};


// ============================================================
// BFS / DFS CLASS
// ============================================================

class GraphTraversal {

public:

    // BFS: Finds every node reachable from the compromised PC
    vector<int> BFS(const Graph& graph, int source,
                    vector<int>& distance) {

        int n = graph.size();

        distance.assign(n, -1);

        vector<int> visited(n, false);
        vector<int> order;

        queue<int> q;

        q.push(source);
        visited[source] = true;
        distance[source] = 0;

        while (!q.empty()) {

            int u = q.front();
            q.pop();

            order.push_back(u);

            for (int edgeIndex :
                 graph.getAdjacency()[u]) {

                const Edge& edge =
                    graph.getEdge(edgeIndex);

                int v = edge.to;

                if (!visited[v]) {

                    visited[v] = true;
                    distance[v] = distance[u] + 1;

                    q.push(v);
                }
            }
        }

        return order;
    }


    // DFS cycle detection
    bool DFSUtil(const Graph& graph,
                 int u,
                 vector<int>& state) {

        // 1 = currently visiting
        // 2 = completely visited

        state[u] = 1;

        for (int edgeIndex :
             graph.getAdjacency()[u]) {

            int v =
                graph.getEdge(edgeIndex).to;

            if (state[v] == 1) {
                return true;
            }

            if (state[v] == 0) {

                if (DFSUtil(graph, v, state))
                    return true;
            }
        }

        state[u] = 2;

        return false;
    }


    bool hasCycle(const Graph& graph) {

        int n = graph.size();

        vector<int> state(n, 0);

        for (int i = 0; i < n; i++) {

            if (state[i] == 0) {

                if (DFSUtil(graph, i, state))
                    return true;
            }
        }

        return false;
    }
};

#endif