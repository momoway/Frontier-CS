#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m;
    if (!(cin >> n >> m)) return 0;
    vector<vector<int>> adj(n);
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        --u; --v;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    // Bipartite check
    vector<int> color(n, -1);
    bool is_bipartite = true;
    for (int i = 0; i < n && is_bipartite; ++i) {
        if (color[i] == -1) {
            color[i] = 0;
            queue<int> q;
            q.push(i);
            while (!q.empty() && is_bipartite) {
                int v = q.front(); q.pop();
                for (int u : adj[v]) {
                    if (color[u] == -1) {
                        color[u] = 1 - color[v];
                        q.push(u);
                    } else if (color[u] == color[v]) {
                        is_bipartite = false;
                        break;
                    }
                }
            }
        }
    }
    if (is_bipartite) {
        for (int i = 0; i < n; ++i) {
            if (i) cout << ' ';
            cout << color[i];
        }
        cout << '\n';
        return 0;
    }
    // Local search for Max-Cut
    vector<int> side(n, 0);
    vector<int> same(n);
    vector<int> diff(n);
    for (int i = 0; i < n; ++i) {
        same[i] = adj[i].size();
        diff[i] = 0;
    }
    bool improved = true;
    while (improved) {
        improved = false;
        for (int v = 0; v < n; ++v) {
            int delta = same[v] - diff[v];
            if (delta > 0) {
                int old_side = side[v];
                int new_side = 1 - old_side;
                side[v] = new_side;
                for (int u : adj[v]) {
                    if (side[u] == old_side) {
                        same[u]--;
                        diff[u]++;
                    } else {
                        same[u]++;
                        diff[u]--;
                    }
                }
                swap(same[v], diff[v]);
                improved = true;
            }
        }
    }
    for (int i = 0; i < n; ++i) {
        if (i) cout << ' ';
        cout << side[i];
    }
    cout << '\n';
    return 0;
}