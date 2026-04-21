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

    // Check if graph is bipartite
    vector<int> side(n, -1);
    bool is_bipartite = true;
    for (int i = 0; i < n && is_bipartite; ++i) {
        if (side[i] != -1) continue;
        queue<int> q;
        q.push(i);
        side[i] = 0;
        while (!q.empty() && is_bipartite) {
            int v = q.front(); q.pop();
            for (int u : adj[v]) {
                if (side[u] == -1) {
                    side[u] = side[v] ^ 1;
                    q.push(u);
                } else if (side[u] == side[v]) {
                    is_bipartite = false;
                    break;
                }
            }
        }
    }

    vector<int> color(n);

    if (is_bipartite) {
        for (int i = 0; i < n; ++i) {
            color[i] = side[i] + 1; // 1 or 2
        }
    } else {
        // Greedy initialization with vertices ordered by descending degree
        color.assign(n, 0);
        vector<int> order(n);
        iota(order.begin(), order.end(), 0);
        sort(order.begin(), order.end(), [&](int a, int b) {
            return adj[a].size() > adj[b].size();
        });
        for (int v : order) {
            int cnt[4] = {0};
            for (int u : adj[v]) {
                if (color[u] != 0) {
                    cnt[color[u]]++;
                }
            }
            int best = 1;
            for (int c = 2; c <= 3; ++c) {
                if (cnt[c] < cnt[best]) best = c;
            }
            color[v] = best;
        }

        // Local search to reduce conflicts
        const int MAX_ITER = 300;
        int iter = 0;
        bool improved = true;
        while (improved && iter < MAX_ITER) {
            improved = false;
            for (int v = 0; v < n; ++v) {
                int old = color[v];
                int best_color = old;
                int best_delta = 0; // we want negative
                for (int c = 1; c <= 3; ++c) {
                    if (c == old) continue;
                    int count_new = 0;
                    int count_old = 0;
                    for (int u : adj[v]) {
                        if (color[u] == c) count_new++;
                        if (color[u] == old) count_old++;
                    }
                    int delta = count_new - count_old;
                    if (delta < best_delta) {
                        best_delta = delta;
                        best_color = c;
                    }
                }
                if (best_delta < 0) {
                    color[v] = best_color;
                    improved = true;
                }
            }
            ++iter;
        }
    }

    for (int i = 0; i < n; ++i) {
        if (i) cout << ' ';
        cout << color[i];
    }
    cout << '\n';
    return 0;
}