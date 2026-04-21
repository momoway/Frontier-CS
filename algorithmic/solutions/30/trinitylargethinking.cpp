#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;
        vector<vector<int>> adj(n + 1);
        for (int i = 0; i < n - 1; ++i) {
            int u, v;
            cin >> u >> v;
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
        vector<int> parent(n + 1), tin(n + 1), tout(n + 1);
        int timer = 0;
        function<void(int, int)> dfs = [&](int u, int p) {
            tin[u] = ++timer;
            for (int v : adj[u]) {
                if (v == p) continue;
                parent[v] = u;
                dfs(v, u);
            }
            tout[u] = timer;
        };
        dfs(1, 0);
        parent[1] = 1; // parent of root is itself for convenience

        vector<bool> possible(n + 1, true);
        int queryCount = 0;
        while (true) {
            vector<int> subPossible(n + 1, 0);
            function<int(int, int)> dfsSub = [&](int u, int p) {
                int cnt = possible[u] ? 1 : 0;
                for (int v : adj[u]) {
                    if (v == p) continue;
                    cnt += dfsSub(v, u);
                }
                subPossible[u] = cnt;
                return cnt;
            };
            int total = dfsSub(1, 0);
            if (total == 1) {
                int ansNode = -1;
                for (int i = 1; i <= n; ++i) {
                    if (possible[i]) {
                        ansNode = i;
                        break;
                    }
                }
                cout << "! " << ansNode << endl;
                cout.flush();
                break;
            }
            // Find node that minimizes max(subtree size, complement size)
            int bestNode = -1;
            int bestMax = total + 1;
            for (int i = 1; i <= n; ++i) {
                int curMax = max(subPossible[i], total - subPossible[i]);
                if (curMax < bestMax) {
                    bestMax = curMax;
                    bestNode = i;
                }
            }
            // Query the chosen node
            cout << "? " << bestNode << endl;
            cout.flush();
            int ans;
            cin >> ans;
            ++queryCount;
            // Update possible set based on answer
            vector<bool> new_possible(n + 1, false);
            if (ans == 1) {
                for (int i = 1; i <= n; ++i) {
                    if (possible[i] && tin[bestNode] <= tin[i] && tin[i] <= tout[bestNode]) {
                        new_possible[i] = true;
                    }
                }
            } else {
                for (int i = 1; i <= n; ++i) {
                    if (possible[i] && !(tin[bestNode] <= tin[i] && tin[i] <= tout[bestNode])) {
                        int p = parent[i];
                        new_possible[p] = true;
                    }
                }
            }
            possible = move(new_possible);
        }
    }
    return 0;
}