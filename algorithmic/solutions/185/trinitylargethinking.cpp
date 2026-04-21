#include <bits/stdc++.h>
using namespace std;

const int MAXN = 1000;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int N, M;
    if (!(cin >> N >> M)) return 0;
    vector<bitset<MAXN>> adj(N);
    vector<int> deg(N, 0);
    for (int i = 0; i < M; ++i) {
        int u, v;
        cin >> u >> v;
        --u; --v;
        if (u == v) continue;
        adj[u].set(v);
        adj[v].set(u);
        ++deg[u];
        ++deg[v];
    }

    bitset<MAXN> allValidMask;
    for (int i = 0; i < N; ++i) allValidMask.set(i);

    auto greedy_desc = [&](const vector<int>& order) {
        bitset<MAXN> clique;
        for (int v : order) {
            if ((adj[v] & clique) == clique) {
                clique.set(v);
            }
        }
        return clique;
    };

    function<bitset<MAXN>(const bitset<MAXN>&)> expand_from = [&](const bitset<MAXN>& initialClique) {
        bitset<MAXN> clique = initialClique;
        bitset<MAXN> candidate = allValidMask;
        if (clique.any()) {
            for (int u = 0; u < N; ++u) {
                if (clique.test(u)) candidate &= adj[u];
            }
        } else {
            candidate = allValidMask;
        }
        while (candidate.any()) {
            int bestV = -1;
            int bestDeg = -1;
            for (int v = 0; v < N; ++v) {
                if (candidate.test(v) && deg[v] > bestDeg) {
                    bestDeg = deg[v];
                    bestV = v;
                }
            }
            if (bestV == -1) break;
            clique.set(bestV);
            candidate &= adj[bestV];
        }
        return clique;
    };

    vector<int> order_base(N);
    iota(order_base.begin(), order_base.end(), 0);

    bitset<MAXN> bestClique;

    vector<int> order_desc = order_base;
    sort(order_desc.begin(), order_desc.end(), [&](int a, int b) {
        return deg[a] > deg[b];
    });
    auto clique = greedy_desc(order_desc);
    clique = expand_from(clique);
    bestClique = clique;

    vector<int> order_asc = order_base;
    sort(order_asc.begin(), order_asc.end(), [&](int a, int b) {
        return deg[a] < deg[b];
    });
    clique = greedy_desc(order_asc);
    clique = expand_from(clique);
    if (clique.count() > bestClique.count()) bestClique = clique;

    clique = expand_from(bitset<MAXN>());
    if (clique.count() > bestClique.count()) bestClique = clique;

    random_device rd;
    mt19937 rng(rd());
    for (int iter = 0; iter < 5; ++iter) {
        vector<int> order = order_base;
        shuffle(order.begin(), order.end(), rng);
        clique = greedy_desc(order);
        clique = expand_from(clique);
        if (clique.count() > bestClique.count()) bestClique = clique;
    }

    for (int i = 0; i < N; ++i) {
        cout << (bestClique.test(i) ? 1 : 0) << '\n';
    }
    return 0;
}