#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, M, H;
    if (!(cin >> N >> M >> H)) return 0;
    vector<int> A(N);
    for (int i = 0; i < N; ++i) cin >> A[i];
    vector<vector<int>> adj(N);
    for (int i = 0; i < M; ++i) {
        int u, v;
        cin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    // read and ignore coordinates
    for (int i = 0; i < N; ++i) {
        int x, y;
        cin >> x >> y;
    }

    vector<int> depth(N, -1);
    vector<int> parent(N, -1);
    vector<char> assigned(N, false);
    using P = pair<int, int>; // (beauty, vertex)
    priority_queue<P, vector<P>, greater<P>> roots_pq;
    for (int v = 0; v < N; ++v) {
        roots_pq.push({A[v], v});
    }

    vector<char> inFrontier(N);

    while (!roots_pq.empty()) {
        int r = -1;
        while (!roots_pq.empty()) {
            auto [val, v] = roots_pq.top();
            roots_pq.pop();
            if (!assigned[v]) {
                r = v;
                break;
            }
        }
        if (r == -1) break;

        depth[r] = 0;
        parent[r] = -1;
        assigned[r] = true;

        fill(inFrontier.begin(), inFrontier.end(), 0);
        priority_queue<P, vector<P>, greater<P>> frontier_pq;

        for (int u : adj[r]) {
            if (!assigned[u] && !inFrontier[u]) {
                frontier_pq.push({A[u], u});
                inFrontier[u] = 1;
            }
        }

        while (!frontier_pq.empty()) {
            auto [val, v] = frontier_pq.top();
            frontier_pq.pop();
            inFrontier[v] = 0;
            if (assigned[v]) continue;

            int best_depth = -1;
            int best_parent = -1;
            for (int u : adj[v]) {
                if (assigned[u] && depth[u] <= H - 1) {
                    if (depth[u] > best_depth) {
                        best_depth = depth[u];
                        best_parent = u;
                    }
                }
            }

            if (best_parent != -1) {
                depth[v] = best_depth + 1;
                parent[v] = best_parent;
                assigned[v] = true;
                for (int w : adj[v]) {
                    if (!assigned[w] && !inFrontier[w]) {
                        frontier_pq.push({A[w], w});
                        inFrontier[w] = 1;
                    }
                }
            } else {
                // cannot add to this component; will be considered later as a root
            }
        }
    }

    for (int v = 0; v < N; ++v) {
        if (v) cout << ' ';
        cout << parent[v];
    }
    cout << '\n';
    return 0;
}