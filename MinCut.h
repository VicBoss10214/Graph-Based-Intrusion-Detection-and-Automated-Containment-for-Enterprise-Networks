#ifndef MINCUT_H
#define MINCUT_H

#include "Traversal.h"
#include <queue>
#include <vector>
#include <cmath>
#include <iomanip>

using namespace std;

class MinCutAlgorithm {

private:

    struct FlowEdge {

        int to;
        int reverseIndex;

        double capacity;

        // Original network edge ID
        int originalEdgeId;

        // Original capacity before residual changes
        double originalCapacity;
    };


    vector<vector<FlowEdge>> network;

    void addFlowEdge(
        int from,
        int to,
        double capacity,
        int originalEdgeId) {

        FlowEdge forward{
            to,
            (int)network[to].size(),
            capacity,
            originalEdgeId,
            capacity
        };

        FlowEdge reverse{
            from,
            (int)network[from].size(),
            0,
            -1,
            0
        };

        network[from].push_back(forward);
        network[to].push_back(reverse);
    }


    double bfs(
        int source,
        int sink,
        vector<int>& parentNode,
        vector<int>& parentEdge) {

        fill(
            parentNode.begin(),
            parentNode.end(),
            -1
        );

        fill(
            parentEdge.begin(),
            parentEdge.end(),
            -1
        );

        queue<int> q;

        q.push(source);
        parentNode[source] = source;

        while (!q.empty()) {

            int u = q.front();
            q.pop();

            for (int i = 0;
                 i < (int)network[u].size();
                 i++) {

                const FlowEdge& edge =
                    network[u][i];

                if (parentNode[edge.to] == -1 &&
                    edge.capacity > 1e-9) {

                    parentNode[edge.to] = u;
                    parentEdge[edge.to] = i;

                    if (edge.to == sink)
                        break;

                    q.push(edge.to);
                }
            }
        }

        if (parentNode[sink] == -1)
            return 0;

        double pathFlow =
            1e18;

        int current = sink;

        while (current != source) {

            int previous =
                parentNode[current];

            int edgeIndex =
                parentEdge[current];

            pathFlow =
                min(
                    pathFlow,
                    network[previous][edgeIndex]
                        .capacity
                );

            current = previous;
        }

        current = sink;

        while (current != source) {

            int previous =
                parentNode[current];

            int edgeIndex =
                parentEdge[current];

            FlowEdge& forward =
                network[previous][edgeIndex];

            FlowEdge& reverse =
                network[current]
                       [forward.reverseIndex];

            forward.capacity -= pathFlow;
            reverse.capacity += pathFlow;

            current = previous;
        }

        return pathFlow;
    }


    vector<bool> findReachable(
        int source) {

        vector<bool> visited(
            network.size(),
            false
        );

        queue<int> q;

        q.push(source);
        visited[source] = true;

        while (!q.empty()) {

            int u = q.front();
            q.pop();

            for (const auto& edge :
                 network[u]) {

                if (!visited[edge.to] &&
                    edge.capacity > 1e-9) {

                    visited[edge.to] = true;
                    q.push(edge.to);
                }
            }
        }

        return visited;
    }


public:

    struct CutEdge {

        int edgeId;

        int from;
        int to;

        double cost;
    };


    struct CutResult {

        double totalCost = 0;

        double maxFlow = 0;

        vector<CutEdge> cutEdges;
    };


    CutResult findMinimumCut(
        const Graph& graph,
        int compromisedNode) {

        int originalNodes =
            graph.size();

        int superSource =
            originalNodes;

        int superSink =
            originalNodes + 1;

        int totalNodes =
            originalNodes + 2;

        network.clear();
        network.resize(totalNodes);


        const double INF =
            1e9;


        // Add original network edges
        for (const Edge& edge :
             graph.getEdges()) {

            addFlowEdge(
                edge.from,
                edge.to,
                edge.businessCost,
                edge.id
            );
        }


        // Connect compromised PC to super-source
        addFlowEdge(
            superSource,
            compromisedNode,
            INF,
            -1
        );


        // Connect all critical nodes to super-sink
        for (int criticalNode :
             graph.getCriticalNodes()) {

            // Do not allow the compromised node
            // itself to become source and sink.
            if (criticalNode == compromisedNode)
                continue;

            addFlowEdge(
                criticalNode,
                superSink,
                INF,
                -1
            );
        }


        double maxFlow = 0;


        vector<int> parentNode(totalNodes);
        vector<int> parentEdge(totalNodes);


        while (true) {

            double flow =
                bfs(
                    superSource,
                    superSink,
                    parentNode,
                    parentEdge
                );

            if (flow <= 1e-9)
                break;

            maxFlow += flow;
        }


        vector<bool> reachable =
            findReachable(superSource);


        CutResult result;

        result.maxFlow = maxFlow;


        // Find original graph edges crossing
        // from reachable to unreachable.

        for (const Edge& edge :
             graph.getEdges()) {

            if (reachable[edge.from] &&
                !reachable[edge.to]) {

                CutEdge cut;

                cut.edgeId = edge.id;
                cut.from = edge.from;
                cut.to = edge.to;
                cut.cost = edge.businessCost;

                result.cutEdges.push_back(cut);

                result.totalCost +=
                    edge.businessCost;
            }
        }

        return result;
    }


    void printResult(
        const Graph& graph,
        const CutResult& result) {

        cout << "\nContainment decision:\n";

        if (result.cutEdges.empty()) {

            cout << "  No network edge needs to be cut.\n";
            return;
        }

        for (const CutEdge& edge :
             result.cutEdges) {

            cout << "  BLOCK: "
                 << graph.getNode(edge.from).name
                 << " -> "
                 << graph.getNode(edge.to).name
                 << " | Business cost: "
                 << fixed
                 << setprecision(2)
                 << edge.cost
                 << '\n';
        }

        cout << "\nMinimum containment damage: "
             << fixed
             << setprecision(2)
             << result.totalCost
             << '\n';
    }
};

#endif