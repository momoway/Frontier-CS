#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m;
    if (!(cin >> n >> m)) return 0;
    vector<int> init(n), targ(n);
    for (int i = 0; i < n; i++) cin >> init[i];
    for (int i = 0; i < n; i++) cin >> targ[i];
    vector<vector<int>> adj(n);
    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        u--; v--;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    vector<vector<int>> states;
    states.push_back(init);
    while (states.back() != targ) {
        const vector<int>& cur = states.back();
        vector<int> nxt(n);
        for (int v = 0; v < n; v++) {
            if (cur[v] == targ[v]) {
                nxt[v] = cur[v];
            } else {
                bool can = false;
                for (int u : adj[v]) {
                    if (cur[u] == targ[v]) {
                        can = true;
                        break;
                    }
                }
                if (can) nxt[v] = targ[v];
                else nxt[v] = cur[v];
            }
        }
        states.push_back(nxt);
        if (states.size() > 20000) break; // safety
    }

    cout << states.size() - 1 << "\n";
    for (const auto& state : states) {
        for (int i = 0; i < n; i++) {
            if (i) cout << ' ';
            cout << state[i];
        }
        cout << "\n";
    }
    return 0;
}