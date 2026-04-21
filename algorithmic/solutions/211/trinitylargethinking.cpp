#include <bits/stdc++.h>
using namespace std;

struct Node {
    int id;
    int x, y;
    char type;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, K;
    if (!(cin >> N >> K)) return 0;
    int M = N + K;
    vector<Node> nodes(M);
    for (int i = 0; i < M; ++i) {
        cin >> nodes[i].id >> nodes[i].x >> nodes[i].y >> nodes[i].type;
    }

    const long long INF = (1LL << 60);
    vector<bool> inMST(M, false);
    vector<long long> minCost(M, INF);
    vector<int> parent(M, -1);
    vector<pair<int,int>> mstEdges;
    minCost[0] = 0;

    for (int iter = 0; iter < M; ++iter) {
        int u = -1;
        for (int i = 0; i < M; ++i) {
            if (!inMST[i] && (u == -1 || minCost[i] < minCost[u])) {
                u = i;
            }
        }
        inMST[u] = true;
        if (parent[u] != -1) {
            mstEdges.emplace_back(parent[u], u);
        }
        for (int v = 0; v < M; ++v) {
            if (!inMST[v]) {
                long long D = (long long)(nodes[u].x - nodes[v].x) * (nodes[u].x - nodes[v].x) +
                             (long long)(nodes[u].y - nodes[v].y) * (nodes[u].y - nodes[v].y);
                long long cost;
                if (nodes[u].type == 'C' && nodes[v].type == 'C') {
                    cost = INF;
                } else if (nodes[u].type == 'C' || nodes[v].type == 'C') {
                    cost = 5 * D;
                } else {
                    if (nodes[u].type == 'S' && nodes[v].type == 'S') {
                        cost = 4 * D;
                    } else if (nodes[u].type == 'R' && nodes[v].type == 'R') {
                        cost = 5 * D;
                    } else {
                        cost = 4 * D;
                    }
                }
                if (cost < minCost[v]) {
                    minCost[v] = cost;
                    parent[v] = u;
                }
            }
        }
    }

    int E = mstEdges.size();
    vector<vector<int>> adjEdgeIdx(M);
    for (int i = 0; i < E; ++i) {
        int u = mstEdges[i].first;
        int v = mstEdges[i].second;
        adjEdgeIdx[u].push_back(i);
        adjEdgeIdx[v].push_back(i);
    }

    vector<bool> removed(M, false);
    vector<bool> edgeActive(E, true);
    vector<int> deg(M);
    for (int i = 0; i < M; ++i) {
        deg[i] = adjEdgeIdx[i].size();
    }

    queue<int> q;
    for (int i = 0; i < M; ++i) {
        if (nodes[i].type == 'C' && deg[i] == 1) {
            q.push(i);
        }
    }

    while (!q.empty()) {
        int u = q.front(); q.pop();
        if (removed[u] || deg[u] != 1 || nodes[u].type != 'C') continue;
        int e = -1, v = -1;
        for (int idx : adjEdgeIdx[u]) {
            if (edgeActive[idx]) {
                e = idx;
                if (mstEdges[idx].first == u) v = mstEdges[idx].second;
                else v = mstEdges[idx].first;
                break;
            }
        }
        if (e == -1) continue;
        removed[u] = true;
        edgeActive[e] = false;
        deg[u] = 0;
        deg[v]--;
        if (deg[v] == 1 && nodes[v].type == 'C') {
            q.push(v);
        }
    }

    vector<int> relayIds;
    for (int i = 0; i < M; ++i) {
        if (nodes[i].type == 'C' && !removed[i]) {
            relayIds.push_back(nodes[i].id);
        }
    }
    sort(relayIds.begin(), relayIds.end());

    vector<string> edgeStrs;
    for (int i = 0; i < E; ++i) {
        if (edgeActive[i]) {
            int u = mstEdges[i].first;
            int v = mstEdges[i].second;
            edgeStrs.emplace_back(to_string(nodes[u].id) + "-" + to_string(nodes[v].id));
        }
    }

    if (relayIds.empty()) {
        cout << "#\n";
    } else {
        for (size_t i = 0; i < relayIds.size(); ++i) {
            if (i > 0) cout << "#";
            cout << relayIds[i];
        }
        cout << "\n";
    }

    if (edgeStrs.empty()) {
        cout << "\n";
    } else {
        for (size_t i = 0; i < edgeStrs.size(); ++i) {
            if (i > 0) cout << "#";
            cout << edgeStrs[i];
        }
        cout << "\n";
    }

    return 0;
}