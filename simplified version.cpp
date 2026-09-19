#include <bits/stdc++.h>
using namespace std;

using ll = long long;

const ll INF = LLONG_MAX / 4;

struct Incident{
    int id;
    double severity;
    double assetValue;
    double confidence;
    double risk;
    bool operator<(const Incident& other) const{
        if (risk == other.risk)
            return id > other.id;

        return risk < other.risk;
    }
};

vector<int> bfs(int deviceNum,const vector<vector<int>>& adj,const vector<int>& compromisedDev){
    vector<int> distance(deviceNum, -1);
    queue<int> q;
    for (int device : compromisedDev){
        if (distance[device] == -1){
            distance[device] = 0;
            q.push(device);
        }
    }

    while (!q.empty()){
        int current = q.front();
        q.pop();

        for (int next : adj[current])
        {
            if (distance[next] == -1)
            {
                distance[next] = distance[current] + 1;
                q.push(next);
            }
        }
    }

    return distance;
}

void dijkstra(
    int deviceNum,
    const vector<vector<int>>& adj,
    const vector<vector<ll>>& attackCost,
    const vector<int>& compromisedDev,
    vector<ll>& distance,
    vector<int>& parent)
{
    distance.assign(deviceNum, INF);
    parent.assign(deviceNum, -1);

    priority_queue<
        pair<ll, int>,
        vector<pair<ll, int>>,
        greater<pair<ll, int>>
    > pq;

    for (int device : compromisedDev)
    {
        distance[device] = 0;
        parent[device] = -2;
        pq.push({0, device});
    }

    while (!pq.empty())
    {
        ll currentDistance = pq.top().first;
        int current = pq.top().second;
        pq.pop();

        if (currentDistance != distance[current])
            continue;

        for (int next : adj[current])
        {
            ll newDistance =
                currentDistance + attackCost[current][next];

            if (newDistance < distance[next])
            {
                distance[next] = newDistance;
                parent[next] = current;

                pq.push({newDistance, next});
            }
        }
    }
}

vector<int> getPath(
    int target,
    const vector<int>& parent)
{
    vector<int> path;

    if (parent[target] == -1)
        return path;

    int current = target;

    while (current >= 0)
    {
        path.push_back(current);
        current = parent[current];
    }

    reverse(path.begin(), path.end());

    return path;
}

ll flowBfs(
    int s,
    int t,
    vector<int>& parent,
    const vector<vector<ll>>& capacity,
    const vector<vector<int>>& adj)
{
    fill(parent.begin(), parent.end(), -1);
    parent[s] = -2;

    queue<pair<int, ll>> q;
    q.push({s, INF});

    while (!q.empty())
    {
        int current = q.front().first;
        ll flow = q.front().second;
        q.pop();

        for (int next : adj[current])
        {
            if (parent[next] == -1 &&
                capacity[current][next] > 0)
            {
                parent[next] = current;

                ll newFlow = min(
                    flow,
                    capacity[current][next]
                );

                if (next == t)
                    return newFlow;

                q.push({next, newFlow});
            }
        }
    }

    return 0;
}

ll edmonds_karp(
    int n,
    int s,
    int t,
    vector<vector<ll>>& capacity,
    const vector<vector<int>>& adj)
{
    ll flow = 0;
    vector<int> parent(n);

    while (true)
    {
        ll newFlow = flowBfs(
            s,
            t,
            parent,
            capacity,
            adj
        );

        if (newFlow == 0)
            break;

        flow += newFlow;

        int current = t;

        while (current != s)
        {
            int previous = parent[current];

            capacity[previous][current] -= newFlow;
            capacity[current][previous] += newFlow;

            current = previous;
        }
    }

    return flow;
}

vector<bool> minCutBfs(
    int source,
    const vector<vector<ll>>& capacity,
    const vector<vector<int>>& adj)
{
    int n = adj.size();

    vector<bool> visited(n, false);
    queue<int> q;

    visited[source] = true;
    q.push(source);

    while (!q.empty())
    {
        int current = q.front();
        q.pop();

        for (int next : adj[current])
        {
            if (!visited[next] &&
                capacity[current][next] > 0)
            {
                visited[next] = true;
                q.push(next);
            }
        }
    }

    return visited;
}

int main(){
    int deviceNum, edges;
    cout << "Enter the number of devices and network connections (edges): ";
    if (!(cin >> deviceNum >> edges)) return 0;

    vector<pair<int, int>> edgesAll;
    vector<vector<int>> adj(deviceNum);
    vector<vector<int>> adj1(deviceNum + 2);
    vector<vector<ll>> attackCost(
        deviceNum,
        vector<ll>(deviceNum, INF)
    );
    vector<vector<ll>> disruptionCost(
        deviceNum + 2,
        vector<ll>(deviceNum + 2, 0)
    );

    ll maximumPossibleFlow = 0;

    cout << "Enter connection details (format: source destination attackCost disruptionCost):\n";
    for (int i = 0; i < edges; i++)
    {
        int u, v;
        ll attackC, disrpC;

        cout << "  Connection " << i + 1 << ": ";
        cin >> u >> v >> attackC >> disrpC;

        u--;
        v--;

        adj[u].push_back(v);

        adj1[u].push_back(v);
        adj1[v].push_back(u);

        attackCost[u][v] =
            min(attackCost[u][v], attackC);

        disruptionCost[u][v] += disrpC;

        edgesAll.push_back({u, v});
        maximumPossibleFlow += disrpC;
    }

    vector<int> compromisedDev;

    int compDevNum;
    cout << "\nEnter the number of compromised devices: ";
    cin >> compDevNum;

    if (compDevNum > 0) {
        cout << "Enter " << compDevNum << " compromised device ID(s) (1-based index): ";
    }
    for (int i = 0; i < compDevNum; i++)
    {
        int device;
        cin >> device;

        compromisedDev.push_back(device - 1);
    }

    vector<int> criticalDev;

    int criticalDevNum;
    cout << "\nEnter the number of critical assets: ";
    cin >> criticalDevNum;

    if (criticalDevNum > 0) {
        cout << "Enter " << criticalDevNum << " critical asset ID(s) (1-based index): ";
    }
    for (int i = 0; i < criticalDevNum; i++)
    {
        int device;
        cin >> device;

        criticalDev.push_back(device - 1);
    }

    int incidentNum, responseLimit;
    cout << "\nEnter the number of active incident alerts and the response action limit: ";
    cin >> incidentNum >> responseLimit;

    priority_queue<Incident> incidentQueue;

    if (incidentNum > 0) {
        cout << "Enter details for each incident alert (format: ID severity assetValue confidence):\n";
    }
    for (int i = 0; i < incidentNum; i++)
    {
        Incident incident;

        cout << "  Incident " << i + 1 << ": ";
        cin >> incident.id
            >> incident.severity
            >> incident.assetValue
            >> incident.confidence;

        incident.risk =
            incident.severity *
            incident.assetValue *
            incident.confidence;

        incidentQueue.push(incident);
    }

    // --- Start of analysis output ---
    cout << "\n=============================================\n";
    cout << "           SECURITY ANALYSIS RESULTS         \n";
    cout << "=============================================\n\n";

    vector<int> exposedDistance =
        bfs(deviceNum, adj, compromisedDev);

    cout << "EXPOSED DEVICES\n";

    bool foundExposed = false;

    for (int i = 0; i < deviceNum; i++)
    {
        if (exposedDistance[i] != -1)
        {
            foundExposed = true;

            cout << "Device "
                 << i + 1
                 << ", distance = "
                 << exposedDistance[i]
                 << " hop(s)\n";
        }
    }

    if (!foundExposed)
        cout << "None\n";

    vector<ll> shortestAttackCost;
    vector<int> dijkstraParent;

    dijkstra(
        deviceNum,
        adj,
        attackCost,
        compromisedDev,
        shortestAttackCost,
        dijkstraParent
    );

    cout << "\nEASIEST ATTACK PATHS\n";

    for (int target : criticalDev)
    {
        cout << "Critical device "
             << target + 1
             << ": ";

        if (shortestAttackCost[target] == INF)
        {
            cout << "unreachable\n";
            continue;
        }

        vector<int> path =
            getPath(target, dijkstraParent);

        for (int i = 0; i < (int)path.size(); i++)
        {
            if (i > 0)
                cout << " -> ";

            cout << path[i] + 1;
        }

        cout << "\nAttack cost: "
             << shortestAttackCost[target]
             << "\n";
    }

    const int source = deviceNum;
    const int sink = deviceNum + 1;

    ll infiniteCapacity = maximumPossibleFlow + 1;

    for (int i = 0; i < compDevNum; i++)
    {
        int device = compromisedDev[i];

        adj1[source].push_back(device);
        adj1[device].push_back(source);

        disruptionCost[source][device] +=
            infiniteCapacity;
    }

    for (int i = 0; i < criticalDevNum; i++)
    {
        int device = criticalDev[i];

        adj1[device].push_back(sink);
        adj1[sink].push_back(device);

        disruptionCost[device][sink] +=
            infiniteCapacity;
    }

    ll disruptionCostMin = edmonds_karp(
        deviceNum + 2,
        source,
        sink,
        disruptionCost,
        adj1
    );

    vector<bool> sourceSide =
        minCutBfs(
            source,
            disruptionCost,
            adj1
        );

    vector<pair<int, int>> removeEdges;

    for (int i = 0; i < edges; i++)
    {
        int u = edgesAll[i].first;
        int v = edgesAll[i].second;

        if (sourceSide[u] && !sourceSide[v])
        {
            removeEdges.push_back({u, v});
        }
    }

    cout << "\nCONNECTIONS TO DISABLE\n";

    if (removeEdges.empty())
    {
        cout << "None\n";
    }
    else
    {
        for (auto edge : removeEdges)
        {
            cout << edge.first + 1
                 << " -> "
                 << edge.second + 1
                 << "\n";
        }
    }

    cout << "Minimum disruption cost: "
         << disruptionCostMin
         << "\n";

    cout << "\nINCIDENT RESPONSE PRIORITY\n";

    int handled = 0;

    while (!incidentQueue.empty() &&
           handled < responseLimit)
    {
        Incident incident = incidentQueue.top();
        incidentQueue.pop();

        cout << "Incident "
             << incident.id
             << ", risk score = "
             << fixed
             << setprecision(2)
             << incident.risk
             << "\n";

        handled++;
    }

    if (handled == 0)
        cout << "None\n";

    return 0;
}
