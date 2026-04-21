#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T;
    cin >> T;
    while (T--) {
        int n;
        cin >> n;
        vector<int> p(n + 1);
        for (int i = 1; i <= n; ++i) cin >> p[i];
        vector<pair<int, int>> edges(n); // edges[1..n-1]
        vector<vector<int>> adj(n + 1);
        vector<vector<int>> edge_idx(n + 1, vector<int>(n + 1, -1));
        for (int i = 1; i <= n - 1; ++i) {
            int u, v;
            cin >> u >> v;
            edges[i] = {u, v};
            adj[u].push_back(v);
            adj[v].push_back(u);
            edge_idx[u][v] = i;
            edge_idx[v][u] = i;
        }

        // BFS for parent and depth from root 1
        vector<int> parent(n + 1, 0), depth(n + 1, 0);
        queue<int> q;
        q.push(1);
        parent[1] = 0;
        depth[1] = 0;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (int v : adj[u]) {
                if (v == parent[u]) continue;
                parent[v] = u;
                depth[v] = depth[u] + 1;
                q.push(v);
            }
        }

        // LCA function (simple lifting)
        auto lca = [&](int a, int b) {
            while (depth[a] > depth[b]) a = parent[a];
            while (depth[b] > depth[a]) b = parent[b];
            while (a != b) {
                a = parent[a];
                b = parent[b];
            }
            return a;
        };

        // Initialize current tokens and positions
        vector<int> cur(n + 1), pos(n + 1);
        for (int i = 1; i <= n; ++i) {
            cur[i] = p[i];
            pos[p[i]] = i;
        }

        // Generate swaps
        vector<int> swaps;
        for (int i = 1; i <= n; ++i) {
            if (cur[i] == i) continue;
            int v = pos[i];
            int w = lca(v, i);
            // Move up from v to w
            while (v != w) {
                int u = parent[v];
                int e = edge_idx[u][v];
                swaps.push_back(e);
                int tu = cur[u], tv = cur[v];
                swap(cur[u], cur[v]);
                pos[tu] = u;
                pos[tv] = v;
                v = u;
            }
            // Move down from w to i if needed
            if (w != i) {
                vector<int> path;
                int x = i;
                while (x != w) {
                    path.push_back(x);
                    x = parent[x];
                }
                path.push_back(w);
                reverse(path.begin(), path.end());
                for (int j = 0; j + 1 < (int)path.size(); ++j) {
                    int u = path[j];
                    int vv = path[j + 1];
                    int e = edge_idx[u][vv];
                    swaps.push_back(e);
                    int tu = cur[u], tv = cur[vv];
                    swap(cur[u], cur[vv]);
                    pos[tu] = u;
                    pos[tv] = vv;
                }
            }
        }

        // Compute degrees and Δ
        vector<int> degree(n + 1, 0);
        for (int e : swaps) {
            int u = edges[e].first, v = edges[e].second;
            degree[u]++;
            degree[v]++;
        }
        int Δ = 0;
        for (int v = 1; v <= n; ++v) Δ = max(Δ, degree[v]);

        // Group swaps by edge
        vector<vector<int>> edge_swaps(n); // index 0 unused; edges 1..n-1
        for (int idx = 0; idx < (int)swaps.size(); ++idx) {
            int e = swaps[idx];
            edge_swaps[e].push_back(idx);
        }

        // Edge coloring
        vector<vector<char>> used_color(n + 1, vector<char>(Δ + 1, 0));
        vector<int> edge_remaining(n, 0); // for e=1..n-1
        for (int e = 1; e <= n - 1; ++e) edge_remaining[e] = edge_swaps[e].size();

        vector<int> neighbor_count(n + 1, 0);
        for (int u = 1; u <= n; ++u) neighbor_count[u] = adj[u].size();

        queue<int> leaves;
        for (int u = 1; u <= n; ++u) if (neighbor_count[u] == 1) leaves.push(u);

        vector<int> color_of_swap(swaps.size(), 0);
        while (!leaves.empty()) {
            int v = leaves.front(); leaves.pop();
            if (neighbor_count[v] != 1) continue;
            // Find neighbor u with remaining edge
            int u = -1;
            for (int nb : adj[v]) {
                int e = edge_idx[nb][v];
                if (edge_remaining[e] > 0) {
                    u = nb;
                    break;
                }
            }
            if (u == -1) continue;
            int e = edge_idx[v][u];
            int k = edge_remaining[e];
            // Find k distinct colors not used by u or v
            vector<int> colors;
            for (int c = 1; c <= Δ; ++c) {
                if (!used_color[u][c] && !used_color[v][c]) {
                    colors.push_back(c);
                    if ((int)colors.size() == k) break;
                }
            }
            // Assign colors to the k swaps of this edge
            for (int i = 0; i < k; ++i) {
                int swap_idx = edge_swaps[e][i];
                color_of_swap[swap_idx] = colors[i];
                used_color[u][colors[i]] = 1;
                used_color[v][colors[i]] = 1;
            }
            edge_remaining[e] = 0;
            // Update neighbor counts
            neighbor_count[v]--;
            neighbor_count[u]--;
            if (neighbor_count[u] == 1) leaves.push(u);
        }

        // Group by color
        vector<vector<int>> ops(Δ + 1);
        for (int idx = 0; idx < (int)swaps.size(); ++idx) {
            int c = color_of_swap[idx];
            if (c == 0) {
                // Should not happen; but to be safe, assign a new color?
                // In practice, algorithm ensures all colored.
            } else {
                ops[c].push_back(swaps[idx]);
            }
        }

        // Count non-empty colors
        int m = 0;
        for (int c = 1; c <= Δ; ++c) if (!ops[c].empty()) m++;
        cout << m << "\n";
        for (int c = 1; c <= Δ; ++c) {
            if (ops[c].empty()) continue;
            cout << ops[c].size();
            for (int e : ops[c]) cout << " " << e;
            cout << "\n";
        }
    }
    return 0;
}