#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m;
    if (!(cin >> n >> m)) return 0;
    vector<vector<int>> adj(n);
    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        u--; v--;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    vector<int> deg(n);
    for (int i = 0; i < n; i++) {
        deg[i] = adj[i].size();
    }
    vector<int> order(n);
    iota(order.begin(), order.end(), 0);
    sort(order.begin(), order.end(), [&](int a, int b) {
        return deg[a] > deg[b];
    });
    vector<int> color(n, 0);
    // Greedy coloring based on degree order
    for (int v : order) {
        int cnt[4] = {0};
        for (int u : adj[v]) {
            if (color[u] != 0) cnt[color[u]]++;
        }
        int best = 1;
        for (int c = 2; c <= 3; c++) {
            if (cnt[c] < cnt[best]) best = c;
        }
        color[v] = best;
    }
    // Local improvement passes
    const int MAX_ITER = 10;
    for (int iter = 0; iter < MAX_ITER; iter++) {
        bool improved = false;
        for (int i = 0; i < n; i++) {
            int v = i;
            int same = 0;
            for (int u : adj[v]) {
                if (color[u] == color[v]) same++;
            }
            if (same == 0) continue;
            int cnt[4] = {0};
            for (int u : adj[v]) {
                cnt[color[u]]++;
            }
            int best = 1;
            for (int c = 2; c <= 3; c++) {
                if (cnt[c] < cnt[best]) best = c;
            }
            if (best != color[v]) {
                color[v] = best;
                improved = true;
            }
        }
        if (!improved) break;
    }
    // Output the coloring
    for (int i = 0; i < n; i++) {
        if (i) cout << ' ';
        cout << color[i];
    }
    cout << '\n';
    return 0;
}