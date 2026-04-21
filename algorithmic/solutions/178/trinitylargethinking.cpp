#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m;
    if (!(cin >> n >> m)) return 0;
    vector<array<int,3>> clauses;
    clauses.reserve(m);
    vector<int> posCount(n+1,0), negCount(n+1,0);
    for (int i = 0; i < m; ++i) {
        int a, b, c;
        cin >> a >> b >> c;
        clauses.push_back({a, b, c});
        if (a > 0) posCount[a]++; else negCount[-a]++;
        if (b > 0) posCount[b]++; else negCount[-b]++;
        if (c > 0) posCount[c]++; else negCount[-c]++;
    }
    vector<vector<int>> varClauses(n+1);
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < 3; ++j) {
            int lit = clauses[i][j];
            int var = abs(lit);
            varClauses[var].push_back(i);
        }
    }
    vector<int> assign(n+1);
    for (int v = 1; v <= n; ++v) {
        assign[v] = (posCount[v] >= negCount[v]) ? 1 : 0;
    }
    vector<char> sat(m, 0);
    int total = 0;
    for (int i = 0; i < m; ++i) {
        bool satisfied = false;
        for (int j = 0; j < 3; ++j) {
            int lit = clauses[i][j];
            int var = abs(lit);
            int val = (lit > 0) ? assign[var] : (1 - assign[var]);
            if (val) { satisfied = true; break; }
        }
        sat[i] = satisfied ? 1 : 0;
        total += sat[i];
    }
    const int MAX_ITER = 10000;
    for (int iter = 0; iter < MAX_ITER; ++iter) {
        int bestDelta = 0;
        int bestVar = -1;
        for (int v = 1; v <= n; ++v) {
            int delta = 0;
            for (int c : varClauses[v]) {
                int pre = sat[c];
                bool post = false;
                for (int j = 0; j < 3; ++j) {
                    int lit = clauses[c][j];
                    int var = abs(lit);
                    int val;
                    if (var == v) val = 1 - assign[var];
                    else val = (lit > 0) ? assign[var] : (1 - assign[var]);
                    if (val) { post = true; break; }
                }
                delta += (post ? 1 : 0) - pre;
            }
            if (delta > bestDelta) {
                bestDelta = delta;
                bestVar = v;
            }
        }
        if (bestDelta > 0) {
            assign[bestVar] ^= 1;
            total += bestDelta;
            for (int c : varClauses[bestVar]) {
                bool post = false;
                for (int j = 0; j < 3; ++j) {
                    int lit = clauses[c][j];
                    int var = abs(lit);
                    int val = (lit > 0) ? assign[var] : (1 - assign[var]);
                    if (val) { post = true; break; }
                }
                sat[c] = post ? 1 : 0;
            }
        } else {
            break;
        }
    }
    for (int v = 1; v <= n; ++v) {
        if (v > 1) cout << ' ';
        cout << assign[v];
    }
    cout << '\n';
    return 0;
}