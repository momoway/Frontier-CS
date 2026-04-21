#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int t;
    if (!(cin >> t)) return 0;
    while (t--) {
        int n;
        cin >> n;
        vector<vector<int>> adj(n + 1);
        for (int i = 0; i < n - 1; i++) {
            int u, v;
            cin >> u >> v;
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
        vector<long long> f(n + 1);
        // Query f for each node
        for (int i = 1; i <= n; i++) {
            cout << "? 1 1 " << i << endl;
            cout.flush();
            cin >> f[i];
        }
        // Find root and values
        vector<int> ans(n + 1);
        bool found = false;
        for (int r = 1; r <= n && !found; r++) {
            // BFS to get parent
            vector<int> parent(n + 1, -1);
            queue<int> q;
            q.push(r);
            parent[r] = 0;
            while (!q.empty()) {
                int u = q.front(); q.pop();
                for (int v : adj[u]) {
                    if (parent[v] == -1) {
                        parent[v] = u;
                        q.push(v);
                    }
                }
            }
            // Compute values
            vector<int> a(n + 1);
            bool ok = true;
            for (int i = 1; i <= n; i++) {
                if (i == r) a[i] = (int)f[i];
                else {
                    long long diff = f[i] - f[parent[i]];
                    if (diff != 1 && diff != -1) {
                        ok = false;
                        break;
                    }
                    a[i] = (int)diff;
                }
            }
            if (ok) {
                // Verify that for the root itself, a[r] is ±1 (already checked)
                ans = a;
                found = true;
                break;
            }
        }
        // Output answer
        cout << "! ";
        for (int i = 1; i <= n; i++) {
            cout << ans[i] << " \n"[i == n];
        }
        cout.flush();
    }
    return 0;
}