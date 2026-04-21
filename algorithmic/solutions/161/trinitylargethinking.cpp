#include <bits/stdc++.h>
using namespace std;

using ll = long long;
const ll INF = (ll)1e15;
const ll MAX_RADIUS_SQ = 5000LL * 5000; // 25,000,000

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, M, K;
    if (!(cin >> N >> M >> K)) return 0;

    vector<ll> x(N+1), y(N+1);
    for (int i = 1; i <= N; ++i) {
        cin >> x[i] >> y[i];
    }

    vector<int> u(M), v(M);
    vector<ll> w(M);
    vector<vector<int>> edge_idx(N+1, vector<int>(N+1, -1));
    for (int j = 0; j < M; ++j) {
        cin >> u[j] >> v[j] >> w[j];
        edge_idx[u[j]][v[j]] = edge_idx[v[j]][u[j]] = j;
    }

    vector<ll> a(K), b(K);
    for (int k = 0; k < K; ++k) {
        cin >> a[k] >> b[k];
    }

    // Precompute squared Euclidean distances between residents and vertices
    vector<vector<ll>> sq_dist(K, vector<ll>(N+1, 0));
    for (int k = 0; k < K; ++k) {
        for (int i = 1; i <= N; ++i) {
            ll dx = a[k] - x[i];
            ll dy = b[k] - y[i];
            sq_dist[k][i] = dx*dx + dy*dy;
        }
    }

    // Floyd-Warshall for all-pairs shortest paths and next matrix for path reconstruction
    vector<vector<ll>> d(N+1, vector<ll>(N+1, INF));
    vector<vector<int>> next_mat(N+1, vector<int>(N+1, -1));
    for (int i = 1; i <= N; ++i) {
        d[i][i] = 0;
        next_mat[i][i] = i;
    }
    for (int j = 0; j < M; ++j) {
        int uu = u[j], vv = v[j];
        if (w[j] < d[uu][vv]) {
            d[uu][vv] = d[vv][uu] = w[j];
            next_mat[uu][vv] = vv;
            next_mat[vv][uu] = uu;
        }
    }
    for (int k = 1; k <= N; ++k) {
        for (int i = 1; i <= N; ++i) {
            if (d[i][k] == INF) continue;
            for (int j = 1; j <= N; ++j) {
                if (d[k][j] == INF) continue;
                if (d[i][k] + d[k][j] < d[i][j]) {
                    d[i][j] = d[i][k] + d[k][j];
                    next_mat[i][j] = next_mat[i][k];
                }
            }
        }
    }

    // Precompute cover lists: for each resident, vertices within distance 5000
    vector<vector<int>> cover_vertices(K); // vertices that cover resident k
    vector<int> count_cover(N+1, 0); // number of uncovered residents each vertex covers
    for (int k = 0; k < K; ++k) {
        for (int i = 1; i <= N; ++i) {
            if (sq_dist[k][i] <= MAX_RADIUS_SQ) {
                cover_vertices[k].push_back(i);
                count_cover[i]++;
            }
        }
    }

    // Greedy set cover with priority queue
    vector<bool> covered(K, false);
    vector<bool> inS(N+1, false);
    vector<int> S; // selected vertices
    priority_queue<pair<int,int>> pq; // (count, vertex)
    for (int i = 1; i <= N; ++i) {
        pq.push({count_cover[i], i});
    }
    int covered_cnt = 0;
    while (covered_cnt < K) {
        int v = -1;
        while (!pq.empty()) {
            auto top = pq.top(); pq.pop();
            int cnt = top.first, node = top.second;
            if (cnt != count_cover[node]) continue; // stale
            if (cnt <= 0) continue;
            v = node;
            break;
        }
        if (v == -1) break; // should not happen

        S.push_back(v);
        inS[v] = true;
        count_cover[v] = -1; // mark as removed

        // Cover all residents that v covers
        for (int k : cover_vertices[v]) {
            if (!covered[k]) {
                covered[k] = true;
                covered_cnt++;
                // For each vertex that also covers k, decrement its count
                for (int u : cover_vertices[k]) {
                    if (count_cover[u] > 0) {
                        count_cover[u]--;
                        pq.push({count_cover[u], u});
                    }
                }
            }
        }
    }

    // Assign residents to vertices in S (nearest Euclidean distance)
    vector<ll> max_sq(N+1, -1); // max squared distance for each vertex, -1 if none
    vector<int> assign(K, -1);
    for (int k = 0; k < K; ++k) {
        ll best_d = LLONG_MAX;
        int best_v = -1;
        for (int v : S) {
            if (sq_dist[k][v] < best_d) {
                best_d = sq_dist[k][v];
                best_v = v;
            }
        }
        assign[k] = best_v;
        if (best_d > max_sq[best_v]) max_sq[best_v] = best_d;
    }

    // Build terminal set T_final: include vertex 1 and vertices in S that have assigned residents
    vector<int> T_final;
    T_final.push_back(1);
    for (int v : S) {
        if (v == 1) continue;
        if (max_sq[v] != -1) {
            T_final.push_back(v);
        }
    }

    // Compute MST on T_final using shortest path distances d[i][j]
    int T_sz = T_final.size();
    vector<ll> minDist(T_sz, INF);
    vector<int> parent(T_sz, -1);
    vector<bool> inMST(T_sz, false);
    minDist[0] = 0; // start from vertex 1 (T_final[0] is 1)
    for (int i = 0; i < T_sz; ++i) {
        int u = -1;
        for (int v = 0; v < T_sz; ++v) {
            if (!inMST[v] && (u == -1 || minDist[v] < minDist[u])) {
                u = v;
            }
        }
        inMST[u] = true;
        for (int v = 0; v < T_sz; ++v) {
            if (!inMST[v]) {
                ll dist_uv = d[T_final[u]][T_final[v]];
                if (dist_uv < minDist[v]) {
                    minDist[v] = dist_uv;
                    parent[v] = u;
                }
            }
        }
    }

    // Collect MST edges (as vertex pairs)
    vector<pair<int,int>> mst_edges;
    for (int v = 1; v < T_sz; ++v) {
        int u = parent[v];
        mst_edges.push_back({T_final[u], T_final[v]});
    }

    // Reconstruct edge set E' by taking union of shortest paths for each MST edge
    vector<bool> edge_used(M, false);
    for (auto &e : mst_edges) {
        int a = e.first, b = e.second;
        while (a != b) {
            int c = next_mat[a][b];
            int idx = edge_idx[a][c];
            if (idx == -1) {
                // Should not happen, but safety
                // Continue anyway
                a = c;
                continue;
            }
            edge_used[idx] = true;
            a = c;
        }
    }

    // Compute P_i for each vertex
    vector<int> P(N+1, 0);
    for (int i = 1; i <= N; ++i) {
        if (max_sq[i] != -1) {
            ll sq = max_sq[i];
            ll r = (ll) sqrt((double)sq);
            while (r * r < sq) ++r;
            while (r > 0 && (r-1)*(r-1) >= sq) --r;
            P[i] = (int) r;
        } else {
            P[i] = 0;
        }
    }

    // Compute total cost
    ll cost_edges = 0;
    for (int j = 0; j < M; ++j) {
        if (edge_used[j]) cost_edges += w[j];
    }
    ll cost_vertices = 0;
    for (int i = 1; i <= N; ++i) {
        cost_vertices += 1LL * P[i] * P[i];
    }
    ll total_cost = cost_edges + cost_vertices;

    // Output
    for (int i = 1; i <= N; ++i) {
        if (i > 1) cout << ' ';
        cout << P[i];
    }
    cout << '\n';
    for (int j = 0; j < M; ++j) {
        if (j > 0) cout << ' ';
        cout << (edge_used[j] ? 1 : 0);
    }
    cout << '\n';

    return 0;
}