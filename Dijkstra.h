#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "Traversal.h"
#include <queue>
#include <limits>
#include <iomanip>

using namespace std;

class DijkstraAlgorithm {

public:

    struct PathResult {

        bool reachable = false;

        int target = -1;

        double distance =
            numeric_limits<double>::infinity();

        vector<int> path;
    };


    PathResult findBestAttackPath(
        const Graph& graph,
        int source) {

        int n = graph.size();

        const double INF =
            numeric_limits<double>::infinity();

        vector<double> dist(n, INF);
        vector<int> previous(n, -1);

        priority_queue<
            pair<double, int>,
            vector<pair<double, int>>,
            greater<pair<double, int>>
        > pq;

        dist[source] = 0;

        pq.push({0, source});

        while (!pq.empty()) {

            auto [currentDistance, u] =
                pq.top();

            pq.pop();

            if (currentDistance > dist[u])
                continue;

            for (int edgeIndex :
                 graph.getAdjacency()[u]) {

                const Edge& edge =
                    graph.getEdge(edgeIndex);

                int v = edge.to;

                double newDistance =
                    dist[u] + edge.attackWeight;

                if (newDistance < dist[v]) {

                    dist[v] = newDistance;
                    previous[v] = u;

                    pq.push({
                        newDistance,
                        v
                    });
                }
            }
        }


        int bestTarget = -1;
        double bestDistance = INF;

        for (int target :
             graph.getCriticalNodes()) {

            if (dist[target] < bestDistance) {

                bestDistance = dist[target];
                bestTarget = target;
            }
        }

        PathResult result;

        if (bestTarget == -1)
            return result;

        result.reachable = true;
        result.target = bestTarget;
        result.distance = bestDistance;


        int current = bestTarget;

        while (current != -1) {

            result.path.push_back(current);

            current = previous[current];
        }

        reverse(
            result.path.begin(),
            result.path.end()
        );

        return result;
    }


    void printPath(
        const Graph& graph,
        const PathResult& result) {

        if (!result.reachable) {

            cout << "No critical target is reachable.\n";
            return;
        }

        cout << "\nMost likely attack path:\n";

        for (size_t i = 0;
             i < result.path.size();
             i++) {

            cout << graph.getNode(
                result.path[i]
            ).name;

            if (i + 1 < result.path.size())
                cout << " -> ";
        }

        cout << "\n";

        cout << fixed << setprecision(2);

        cout << "Attack path cost: "
             << result.distance
             << '\n';

        cout << "Target criticality: "
             << graph.getNode(
                    result.target
                ).criticality
             << "/100\n";
    }
};

#endif
