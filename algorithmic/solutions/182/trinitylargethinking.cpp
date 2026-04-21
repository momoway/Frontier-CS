#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, M;
    if (!(cin >> N >> M)) return 0;

    vector<vector<int>> adj(N + 1);
    vector<pair<int, int>> edges;
    edges.reserve(M);

    for (int i = 0; i < M; ++i) {
        int u, v;
        cin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u);
        edges.emplace_back(u, v);
    }

    // Greedy max-degree vertex cover
    vector<char> cover1(N + 1, 0);
    vector<int> curDeg(N + 1);
    for (int i = 1; i <= N; ++i) {
        curDeg[i] = static_cast<int>(adj[i].size());
    }
    priority_queue<pair<int, int>> pq;
    for (int i = 1; i <= N; ++i) {
        pq.emplace(curDeg[i], i);
    }
    while (!pq.empty()) {
        auto [d, v] = pq.top(); pq.pop();
        if (d != curDeg[v]) continue;
        if (d == 0) break;
        cover1[v] = 1;
        curDeg[v] = 0;
        for (int u : adj[v]) {
            if (cover1[u]) continue;
            --curDeg[u];
            pq.emplace(curDeg[u], u);
        }
    }

    // Maximal matching vertex cover
    vector<char> cover2(N + 1, 0);
    vector<char> matched(N + 1, 0);
    for (auto &e : edges) {
        int u = e.first, v = e.second;
        if (!matched[u] && !matched[v]) {
            matched[u] = matched[v] = 1;
        }
    }
    cover2 = matched;

    // Reduction to minimal vertex cover
    auto reduce = [&](vector<char>& cover) {
        int n = static_cast<int>(cover.size()) - 1;
        vector<int> cnt(n + 1, 0);
        for (int v = 1; v <= n; ++v) {
            if (cover[v]) {
                for (int u : adj[v]) {
                    if (!cover[u]) ++cnt[v];
                }
            }
        }
        queue<int> q;
        for (int v = 1; v <= n; ++v) {
            if (cover[v] && cnt[v] == 0) q.push(v);
        }
        while (!q.empty()) {
            int v = q.front(); q.pop();
            if (!cover[v] || cnt[v] != 0) continue;
            cover[v] = 0;
            for (int u : adj[v]) {
                if (cover[u]) {
                    ++cnt[u];
                }
            }
        }
    };

    reduce(cover1);
    reduce(cover2);

    int size1 = 0, size2 = 0;
    for (int i = 1; i <= N; ++i) {
        if (cover1[i]) ++size1;
        if (cover2[i]) ++size2;
    }
    const vector<char>& finalCover = (size1 <= size2) ? cover1 : cover2;

    for (int i = 1; i <= N; ++i) {
        cout << static_cast<int>(finalCover[i]) << '\n';
    }

    return 0;
}