#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int N, M;
    if (!(cin >> N >> M)) return 0;
    const int MAXN = 1000;
    vector<bitset<MAXN>> neighborMask(N);
    vector<int> degree(N, 0);
    for (int i = 0; i < M; ++i) {
        int u, v;
        cin >> u >> v;
        --u; --v;
        if (u == v) continue;
        neighborMask[u][v] = 1;
        neighborMask[v][u] = 1;
        ++degree[u];
        ++degree[v];
    }
    vector<int> order(N);
    iota(order.begin(), order.end(), 0);
    sort(order.begin(), order.end(), [&](int a, int b) {
        return degree[a] < degree[b];
    });
    bitset<MAXN> selected;
    selected.reset();
    for (int idx : order) {
        if ((neighborMask[idx] & selected).any()) continue;
        selected.set(idx);
    }
    for (int i = 0; i < N; ++i) {
        cout << (selected[i] ? 1 : 0) << "\n";
    }
    return 0;
}