#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int N, M;
    if(!(cin >> N >> M)) return 0;
    vector<vector<int>> adj(N + 1);
    for (int i = 0; i < M; ++i) {
        int u, v;
        cin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    // Compute degrees
    vector<int> deg(N + 1);
    for (int i = 1; i <= N; ++i) deg[i] = adj[i].size();

    // Heuristic 1: static greedy (sort by original degree)
    vector<int> order(N);
    iota(order.begin(), order.end(), 1);
    sort(order.begin(), order.end(), [&](int a, int b) {
        return deg[a] < deg[b];
    });
    vector<char> selected1(N + 1, 0), excluded(N + 1, 0);
    for (int v : order) {
        if (!excluded[v]) {
            selected1[v] = 1;
            for (int u : adj[v]) excluded[u] = 1;
        }
    }
    int size1 = 0;
    for (int i = 1; i <= N; ++i) size1 += selected1[i];

    // Heuristic 2: dynamic greedy (min-heap of current degree)
    vector<char> selected2(N + 1, 0), removed(N + 1, 0);
    vector<int> curDeg(N + 1);
    for (int i = 1; i <= N; ++i) curDeg[i] = deg[i];
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    for (int i = 1; i <= N; ++i) pq.push({curDeg[i], i});
    while (!pq.empty()) {
        auto [d, v] = pq.top(); pq.pop();
        if (removed[v]) continue;
        if (d != curDeg[v]) continue; // stale entry
        selected2[v] = 1;
        vector<int> toRemove;
        toRemove.push_back(v);
        for (int u : adj[v]) {
            if (!removed[u]) toRemove.push_back(u);
        }
        for (int x : toRemove) {
            if (removed[x]) continue;
            removed[x] = 1;
            for (int y : adj[x]) {
                if (!removed[y]) {
                    --curDeg[y];
                    pq.push({curDeg[y], y});
                }
            }
        }
    }
    int size2 = 0;
    for (int i = 1; i <= N; ++i) size2 += selected2[i];

    // Choose the better solution
    const vector<char>* outSelected = &selected1;
    if (size2 > size1) outSelected = &selected2;

    // Output
    for (int i = 1; i <= N; ++i) {
        cout << static_cast<int>((*outSelected)[i]) << '\n';
    }
    return 0;
}