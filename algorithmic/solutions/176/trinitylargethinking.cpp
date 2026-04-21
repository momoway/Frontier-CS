#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m;
    if (!(cin >> n >> m)) return 0;
    vector<array<int,3>> clauses;
    clauses.reserve(m);
    vector<vector<int>> var_clauses(n+1);
    for (int i = 0; i < m; i++) {
        int a, b, c;
        cin >> a >> b >> c;
        clauses.push_back({a, b, c});
        int vars[3] = {abs(a), abs(b), abs(c)};
        for (int v : vars) {
            var_clauses[v].push_back(i);
        }
    }
    vector<char> val(n+1, 0);
    vector<char> satisfied(m, 0);
    for (int i = 0; i < m; i++) {
        bool sat = false;
        for (int lit : clauses[i]) {
            if (lit > 0) {
                if (val[lit]) { sat = true; break; }
            } else {
                if (!val[-lit]) { sat = true; break; }
            }
        }
        satisfied[i] = sat;
    }
    auto compute_delta = [&](int v) {
        int d = 0;
        for (int cls_idx : var_clauses[v]) {
            bool old_sat = satisfied[cls_idx];
            bool new_sat = false;
            for (int lit : clauses[cls_idx]) {
                int var = abs(lit);
                if (var == v) {
                    if (lit > 0) {
                        if (!val[var]) { new_sat = true; break; }
                    } else {
                        if (val[var]) { new_sat = true; break; }
                    }
                } else {
                    if (lit > 0) {
                        if (val[var]) { new_sat = true; break; }
                    } else {
                        if (!val[var]) { new_sat = true; break; }
                    }
                }
            }
            d += (new_sat ? 1 : 0) - (old_sat ? 1 : 0);
        }
        return d;
    };
    vector<int> delta(n+1, 0);
    for (int v = 1; v <= n; v++) {
        delta[v] = compute_delta(v);
    }
    while (true) {
        int best_v = -1;
        int best_delta = 0;
        for (int v = 1; v <= n; v++) {
            if (delta[v] > best_delta) {
                best_delta = delta[v];
                best_v = v;
            }
        }
        if (best_delta <= 0) break;
        val[best_v] = !val[best_v];
        for (int cls_idx : var_clauses[best_v]) {
            bool old_sat = satisfied[cls_idx];
            bool new_sat = false;
            for (int lit : clauses[cls_idx]) {
                if (lit > 0) {
                    if (val[lit]) { new_sat = true; break; }
                } else {
                    if (!val[-lit]) { new_sat = true; break; }
                }
            }
            if (new_sat == old_sat) continue;
            satisfied[cls_idx] = new_sat;
            for (int lit : clauses[cls_idx]) {
                int u = abs(lit);
                delta[u] = compute_delta(u);
            }
        }
    }
    for (int i = 1; i <= n; i++) {
        cout << (int)val[i];
        if (i < n) cout << ' ';
    }
    cout << '\n';
    return 0;
}