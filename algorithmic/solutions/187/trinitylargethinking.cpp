#include <bits/stdc++.h>
using namespace std;

const int MAXN = 500;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, M;
    if (!(cin >> N >> M)) return 0;

    static bitset<MAXN> adj[MAXN];
    for (int i = 0; i < N; ++i) adj[i].reset();

    for (int i = 0; i < M; ++i) {
        int u, v;
        cin >> u >> v;
        --u; --v;
        adj[u].set(v);
        adj[v].set(u);
    }

    vector<int> color(N, -1);
    int coloredCount = 0;

    while (coloredCount < N) {
        int bestV = -1;
        int bestSat = -1;

        for (int v = 0; v < N; ++v) {
            if (color[v] != -1) continue;

            bitset<MAXN> forbid;
            forbid.reset();
            for (int u = 0; u < N; ++u) {
                if (color[u] != -1 && !adj[v].test(u)) {
                    forbid.set(color[u]);
                }
            }
            int sat = (int)forbid.count();
            if (sat > bestSat || (sat == bestSat && v < bestV)) {
                bestSat = sat;
                bestV = v;
            }
        }

        bitset<MAXN> forbid;
        forbid.reset();
        for (int u = 0; u < N; ++u) {
            if (color[u] != -1 && !adj[bestV].test(u)) {
                forbid.set(color[u]);
            }
        }
        int c = 0;
        while (c < N && forbid.test(c)) ++c;
        color[bestV] = c;
        ++coloredCount;
    }

    for (int i = 0; i < N; ++i) {
        cout << color[i] + 1 << "\n";
    }

    return 0;
}