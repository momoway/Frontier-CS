#include <bits/stdc++.h>
using namespace std;

const int MAXN = 2000;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    if (!(cin >> n >> m)) return 0;

    vector<bitset<MAXN>> adj1(n), adj2(n);
    vector<vector<int>> neigh2(n);

    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        --u; --v;
        adj1[u][v] = adj1[v][u] = 1;
    }
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        --u; --v;
        adj2[u][v] = adj2[v][u] = 1;
        neigh2[u].push_back(v);
        neigh2[v].push_back(u);
    }

    vector<int> deg1(n), deg2(n);
    for (int i = 0; i < n; ++i) {
        deg1[i] = adj1[i].count();
        deg2[i] = neigh2[i].size();
    }

    vector<int> order1(n), order2(n);
    iota(order1.begin(), order1.end(), 0);
    iota(order2.begin(), order2.end(), 0);
    sort(order1.begin(), order1.end(), [&](int a, int b) {
        if (deg1[a] != deg1[b]) return deg1[a] < deg1[b];
        return a < b;
    });
    sort(order2.begin(), order2.end(), [&](int a, int b) {
        if (deg2[a] != deg2[b]) return deg2[a] < deg2[b];
        return a < b;
    });

    vector<int> mp(n);
    for (int i = 0; i < n; ++i) {
        mp[order2[i]] = order1[i];
    }

    vector<bitset<MAXN>> B(n);
    for (int i = 0; i < n; ++i) {
        for (int v : neigh2[i]) {
            B[i].set(mp[v]);
        }
    }

    const int MAX_PASSES = 10;
    bool improved = true;
    int passes = 0;
    while (improved && passes < MAX_PASSES) {
        improved = false;
        for (int i = 0; i < n; ++i) {
            int best_j = -1;
            int best_delta = 0;
            for (int j = i + 1; j < n; ++j) {
                int a = mp[i];
                int b = mp[j];
                if (a == b) continue;
                bitset<MAXN> Si_ex = B[i];
                if (adj2[i][j]) Si_ex.reset(b);
                bitset<MAXN> Sj_ex = B[j];
                if (adj2[i][j]) Sj_ex.reset(a);
                int C_i_a = (adj1[a] & Si_ex).count();
                int C_i_b = (adj1[b] & Si_ex).count();
                int C_j_a = (adj1[a] & Sj_ex).count();
                int C_j_b = (adj1[b] & Sj_ex).count();
                int delta = (C_i_b - C_i_a) + (C_j_a - C_j_b);
                if (delta > best_delta) {
                    best_delta = delta;
                    best_j = j;
                }
            }
            if (best_j != -1 && best_delta > 0) {
                int j = best_j;
                int a = mp[i];
                int b = mp[j];
                mp[i] = b;
                mp[j] = a;
                for (int k : neigh2[i]) {
                    if (k == j) continue;
                    if (adj2[j][k]) continue;
                    B[k].reset(a);
                    B[k].set(b);
                }
                for (int k : neigh2[j]) {
                    if (k == i) continue;
                    if (adj2[i][k]) continue;
                    B[k].reset(b);
                    B[k].set(a);
                }
                if (adj2[i][j]) {
                    B[i].reset(b);
                    B[i].set(a);
                    B[j].reset(a);
                    B[j].set(b);
                }
                improved = true;
            }
        }
        ++passes;
    }

    for (int i = 0; i < n; ++i) {
        if (i) cout << ' ';
        cout << mp[i] + 1;
    }
    cout << '\n';
    return 0;
}