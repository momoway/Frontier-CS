#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    if (!(cin >> n)) return 0;
    vector<int> D(n * n), F(n * n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cin >> D[i * n + j];
        }
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cin >> F[i * n + j];
        }
    }

    vector<long long> flow_sum(n, 0), dist_sum(n, 0);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            flow_sum[i] += F[i * n + j];
            dist_sum[j] += D[i * n + j];
        }
    }

    vector<int> fac_idx(n), loc_idx(n);
    iota(fac_idx.begin(), fac_idx.end(), 0);
    iota(loc_idx.begin(), loc_idx.end(), 0);
    sort(fac_idx.begin(), fac_idx.end(), [&](int a, int b) {
        return flow_sum[a] > flow_sum[b];
    });
    sort(loc_idx.begin(), loc_idx.end(), [&](int a, int b) {
        return dist_sum[a] < dist_sum[b];
    });

    vector<int> assignment(n);
    for (int k = 0; k < n; ++k) {
        assignment[fac_idx[k]] = loc_idx[k];
    }

    long long cost = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cost += (long long)F[i * n + j] * D[assignment[i] * n + assignment[j]];
        }
    }

    mt19937 rng(random_device{}());

    const int MAX_ITER = 50000;
    for (int iter = 0; iter < MAX_ITER; ++iter) {
        int u = uniform_int_distribution<int>(0, n - 1)(rng);
        int v = uniform_int_distribution<int>(0, n - 1)(rng);
        if (u == v) continue;
        int a = assignment[u];
        int b = assignment[v];
        long long delta = 0;
        for (int i = 0; i < n; ++i) {
            if (i == u || i == v) continue;
            int pi = assignment[i];
            delta += (long long)F[u * n + i] * (D[b * n + pi] - D[a * n + pi]);
            delta += (long long)F[i * n + u] * (D[pi * n + b] - D[pi * n + a]);
            delta += (long long)F[v * n + i] * (D[a * n + pi] - D[b * n + pi]);
            delta += (long long)F[i * n + v] * (D[pi * n + a] - D[pi * n + b]);
        }
        delta += (long long)F[u * n + u] * (D[b * n + b] - D[a * n + a]);
        delta += (long long)F[u * n + v] * (D[b * n + a] - D[a * n + b]);
        delta += (long long)F[v * n + v] * (D[a * n + a] - D[b * n + b]);
        delta += (long long)F[v * n + u] * (D[a * n + b] - D[b * n + a]);
        if (delta < 0) {
            assignment[u] = b;
            assignment[v] = a;
            cost += delta;
        }
    }

    for (int i = 0; i < n; ++i) {
        if (i) cout << ' ';
        cout << assignment[i] + 1;
    }
    cout << '\n';
    return 0;
}