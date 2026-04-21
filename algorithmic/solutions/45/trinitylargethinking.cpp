#include <bits/stdc++.h>
using namespace std;

inline uint64_t custom_hash(uint64_t x) {
    return x * 1469113000698558691ULL;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m, k;
    double eps;
    if (!(cin >> n >> m >> k >> eps)) return 0;
    vector<pair<int,int>> edges;
    edges.reserve(m);
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        --u; --v;
        if (u == v) continue;
        edges.emplace_back(min(u,v), max(u,v));
    }
    sort(edges.begin(), edges.end());
    edges.erase(unique(edges.begin(), edges.end()), edges.end());
    vector<vector<int>> adj(n);
    for (auto &e : edges) {
        int u = e.first, v = e.second;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    vector<int> deg(n);
    for (int i = 0; i < n; ++i) deg[i] = (int)adj[i].size();
    long long ideal = (static_cast<long long>(n) + k - 1) / k;
    long long max_size_ll = static_cast<long long>(floor((1.0 + eps) * ideal + 1e-9));
    int max_size = static_cast<int>(max_size_ll);
    vector<int> order(n);
    iota(order.begin(), order.end(), 0);
    sort(order.begin(), order.end(), [&](int a, int b) {
        if (deg[a] != deg[b]) return deg[a] < deg[b];
        return custom_hash(a) < custom_hash(b);
    });
    vector<int> part(n, -1);
    vector<int> part_size(k, 0);
    vector<int> partCount(k, 0);
    vector<int> touchedList;
    touchedList.reserve(10);
    using P = pair<int,int>;
    priority_queue<P, vector<P>, greater<P>> pq;
    for (int p = 0; p < k; ++p) pq.emplace(0, p);
    for (int v : order) {
        touchedList.clear();
        for (int w : adj[v]) {
            int p = part[w];
            if (p != -1) {
                if (partCount[p] == 0) touchedList.push_back(p);
                partCount[p]++;
            }
        }
        int bestPart = -1;
        if (!touchedList.empty()) {
            int bestCount = -1;
            for (int p : touchedList) {
                if (part_size[p] < max_size) {
                    if (partCount[p] > bestCount ||
                        (partCount[p] == bestCount &&
                         (bestPart == -1 || part_size[p] < part_size[bestPart] ||
                          (part_size[p] == part_size[bestPart] && p < bestPart)))) {
                        bestCount = partCount[p];
                        bestPart = p;
                    }
                }
            }
            if (bestPart == -1) {
                while (!pq.empty()) {
                    auto [sz, p] = pq.top(); pq.pop();
                    if (sz != part_size[p]) continue;
                    if (sz < max_size) {
                        bestPart = p;
                        break;
                    }
                }
            }
        } else {
            while (!pq.empty()) {
                auto [sz, p] = pq.top(); pq.pop();
                if (sz != part_size[p]) continue;
                if (sz < max_size) {
                    bestPart = p;
                    break;
                }
            }
        }
        if (bestPart == -1) bestPart = 0;
        part[v] = bestPart;
        part_size[bestPart]++;
        pq.emplace(part_size[bestPart], bestPart);
        for (int p : touchedList) partCount[p] = 0;
    }
    for (int i = 0; i < n; ++i) {
        if (i) cout << ' ';
        cout << (part[i] + 1);
    }
    cout << '\n';
    return 0;
}