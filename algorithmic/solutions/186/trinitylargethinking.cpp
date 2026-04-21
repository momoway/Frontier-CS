#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, M;
    if (!(cin >> N >> M)) return 0;

    const int MAXN = 500;
    bool adjMat[MAXN+1][MAXN+1] = {};

    for (int i = 0; i < M; ++i) {
        int u, v;
        cin >> u >> v;
        if (u > v) swap(u, v);
        adjMat[u][v] = adjMat[v][u] = true;
    }

    vector<int> adj[MAXN+1];
    for (int i = 1; i <= N; ++i) {
        for (int j = 1; j <= N; ++j) {
            if (adjMat[i][j]) {
                adj[i].push_back(j);
            }
        }
    }

    vector<int> degree(N+1);
    for (int i = 1; i <= N; ++i) {
        degree[i] = adj[i].size();
    }

    vector<int> color(N+1, 0);
    vector<int> saturation(N+1, 0);
    const int MAX_COLOR = 501;
    vector<bitset<MAX_COLOR>> neighborUsed(N+1);
    for (int i = 1; i <= N; ++i) {
        neighborUsed[i].reset();
    }

    set<tuple<int,int,int>> S;
    for (int i = 1; i <= N; ++i) {
        S.insert(make_tuple(0, degree[i], i));
    }

    bool usedColor[MAXN+2];

    while (!S.empty()) {
        auto it = prev(S.end());
        int v = get<2>(*it);
        S.erase(it);

        for (int i = 1; i <= N; ++i) usedColor[i] = false;
        for (int u : adj[v]) {
            if (color[u] != 0) {
                usedColor[color[u]] = true;
            }
        }
        int c = 1;
        while (c <= N && usedColor[c]) ++c;
        color[v] = c;

        for (int u : adj[v]) {
            if (color[u] == 0) {
                if (!neighborUsed[u][c]) {
                    neighborUsed[u].set(c);
                    auto oldKey = make_tuple(saturation[u], degree[u], u);
                    auto it_u = S.find(oldKey);
                    if (it_u != S.end()) {
                        S.erase(it_u);
                    }
                    saturation[u]++;
                    S.insert(make_tuple(saturation[u], degree[u], u));
                }
            }
        }
    }

    for (int i = 1; i <= N; ++i) {
        cout << color[i] << "\n";
    }

    return 0;
}