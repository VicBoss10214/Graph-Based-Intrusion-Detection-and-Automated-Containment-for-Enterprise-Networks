#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

#include "Traversal.h"
#include "Dijkstra.h"
#include "MinCut.h"
#include "PriorityQueue.h"

using namespace std;


// ============================================================
// PRINT PATH
// ============================================================

void printNodeList(
    const Graph& graph,
    const vector<int>& nodes) {

    for (size_t i = 0;
         i < nodes.size();
         i++) {

        cout << graph.getNode(nodes[i]).name;

        if (i + 1 < nodes.size())
            cout << " -> ";
    }

    cout << '\n';
}


// ============================================================
// SIMULATE ONE COMPROMISED COMPUTER
// ============================================================

void simulateAttack(
    const Graph& graph,
    int compromisedNode) {

    cout << "\n\n";
    cout << "============================================================\n";

    cout << " SIMULATION: "
         << graph.getNode(compromisedNode).name
         << " HAS BEEN COMPROMISED\n";

    cout << "============================================================\n";


    // --------------------------------------------------------
    // 1. BFS
    // --------------------------------------------------------

    GraphTraversal traversal;

    vector<int> distance;

    vector<int> reachable =
        traversal.BFS(
            graph,
            compromisedNode,
            distance
        );

    cout << "\n[1] BFS - LATERAL MOVEMENT ANALYSIS\n";

    cout << "Reachable computers:\n";

    for (int node : reachable) {

        cout << "  "
             << graph.getNode(node).name;

        cout << " ("
             << distance[node]
             << " hops)";

        if (graph.getNode(node).critical)
            cout << "  <-- HIGH-VALUE TARGET";

        cout << '\n';
    }


    // DFS
    bool cycle =
        traversal.hasCycle(graph);

    cout << "\nDFS network cycle analysis: ";

    if (cycle)
        cout << "Cycle(s) detected";
    else
        cout << "No cycle detected";

    cout << '\n';


    // --------------------------------------------------------
    // 2. DIJKSTRA
    // --------------------------------------------------------

    cout << "\n[2] DIJKSTRA - ATTACK PATH ANALYSIS\n";

    DijkstraAlgorithm dijkstra;

    auto pathResult =
        dijkstra.findBestAttackPath(
            graph,
            compromisedNode
        );

    dijkstra.printPath(
        graph,
        pathResult
    );


    // --------------------------------------------------------
    // 3. MIN-CUT
    // --------------------------------------------------------

    cout << "\n[3] MIN-CUT - SURGICAL CONTAINMENT\n";

    MinCutAlgorithm minCut;

    auto cutResult =
        minCut.findMinimumCut(
            graph,
            compromisedNode
        );

    minCut.printResult(
        graph,
        cutResult
    );


    // --------------------------------------------------------
    // 4. TRADITIONAL METHOD
    // --------------------------------------------------------

    double traditionalDamage =
        graph.totalBusinessCost();


    cout << "\n[4] TRADITIONAL CONTAINMENT\n";

    cout << "Traditional method:\n";
    cout << "  Disable the entire network.\n";

    cout << "Traditional operational damage: "
         << fixed
         << setprecision(2)
         << traditionalDamage
         << '\n';


    // --------------------------------------------------------
    // 5. COMPARISON
    // --------------------------------------------------------

    double intelligentDamage =
        cutResult.totalCost;

    double savedDamage =
        traditionalDamage -
        intelligentDamage;

    double reductionPercentage = 0;

    if (traditionalDamage > 0) {

        reductionPercentage =
            (savedDamage /
             traditionalDamage) * 100.0;
    }


    cout << "\n[5] DAMAGE COMPARISON\n";

    cout << "------------------------------------------------------------\n";

    cout << left
         << setw(35)
         << "Containment method"
         << "Damage\n";

    cout << "------------------------------------------------------------\n";

    cout << left
         << setw(35)
         << "Traditional full shutdown"
         << fixed
         << setprecision(2)
         << traditionalDamage
         << '\n';

    cout << left
         << setw(35)
         << "Graph-based Min-Cut"
         << intelligentDamage
         << '\n';

    cout << "------------------------------------------------------------\n";

    cout << "Operational damage avoided: "
         << savedDamage
         << '\n';

    cout << "Percentage of traditional damage avoided: "
         << reductionPercentage
         << "%\n";

    cout << "============================================================\n";
}


// ============================================================
// MAIN
// ============================================================

int main() {

    ios::sync_with_stdio(false);
    cin.tie(nullptr);


    cout << "============================================================\n";
    cout << " GRAPH-BASED INTRUSION DETECTION & CONTAINMENT\n";
    cout << "============================================================\n";


    // --------------------------------------------------------
    // INPUT NETWORK
    // --------------------------------------------------------

    int n;

    cout << "\nEnter number of computers/devices: ";
    cin >> n;

    if (n <= 0) {

        cout << "Invalid number of nodes.\n";
        return 0;
    }


    Graph graph;

    graph.initialize(n);


    cout << "\nEnter information for each node.\n";

    cout << "Criticality should be between 1 and 100.\n";

    for (int i = 0; i < n; i++) {

        string name;
        int criticality;
        int critical;

        cout << "\nNode " << i << " name: ";

        cin >> name;

        cout << "Criticality (1-100): ";
        cin >> criticality;

        cout << "Is this a High-Value Target? (1 = Yes, 0 = No): ";
        cin >> critical;

        graph.addNode(
            i,
            name,
            criticality,
            critical == 1
        );
    }


    // --------------------------------------------------------
    // INPUT EDGES
    // --------------------------------------------------------

    int m;

    cout << "\nEnter number of directed connections: ";
    cin >> m;


    cout << "\nFor every connection enter:\n";

    cout << "FROM TO BaseCost FirewallPenalty "
            "AnomalyScore BusinessCost\n";

    cout << "\nExample:\n";
    cout << "0 1 5 2 1 10\n";


    for (int i = 0; i < m; i++) {

        int from;
        int to;

        double baseCost;
        double firewallPenalty;
        double anomalyScore;
        double businessCost;


        cout << "\nConnection " << i << ": ";

        cin >> from
            >> to
            >> baseCost
            >> firewallPenalty
            >> anomalyScore
            >> businessCost;


        if (from < 0 ||
            from >= n ||
            to < 0 ||
            to >= n) {

            cout << "Invalid node ID. Try again.\n";

            i--;
            continue;
        }


        graph.addEdge(
            i,
            from,
            to,
            baseCost,
            firewallPenalty,
            anomalyScore,
            businessCost
        );
    }


    // --------------------------------------------------------
    // DISPLAY GRAPH
    // --------------------------------------------------------

    graph.printGraph();


    // --------------------------------------------------------
    // PRIORITY QUEUE
    // --------------------------------------------------------

    IncidentPriorityQueue incidentQueue;


    /*
       We simulate every NON-CRITICAL node as a possible
       compromised computer.

       Severity:
           Based on how important the compromised node is.

       Asset Value:
           Based on the highest critical target.

       Confidence:
           For simulation we use 0.90.

       In a real system these values would come from
       IDS/EDR telemetry.
    */

    int highestCriticality = 1;

    for (const Node& node :
         graph.getNodes()) {

        if (node.critical) {

            highestCriticality =
                max(
                    highestCriticality,
                    node.criticality
                );
        }
    }


    for (const Node& node :
         graph.getNodes()) {

        if (!node.critical) {

            double severity =
                node.criticality;

            double assetValue =
                highestCriticality;

            double confidence =
                0.90;


            incidentQueue.addIncident(
                node.id,
                severity,
                assetValue,
                confidence
            );
        }
    }


    incidentQueue.printPriorityOrder(graph);


    // --------------------------------------------------------
    // RUN SIMULATION FOR EVERY POSSIBLE COMPROMISED PC
    // --------------------------------------------------------

    cout << "\n\n";
    cout << "############################################################\n";
    cout << "#              ATTACK SIMULATION PHASE                    #\n";
    cout << "############################################################\n";


    for (const Node& node :
         graph.getNodes()) {

        // We only simulate ordinary PCs as
        // initial compromise points.

        if (!node.critical) {

            simulateAttack(
                graph,
                node.id
            );
        }
    }


    cout << "\n\nSimulation completed.\n";

    return 0;
}