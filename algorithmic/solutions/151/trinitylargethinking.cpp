#include <bits/stdc++.h>
using namespace std;

struct DSU {
    vector<int> parent, rank;
    DSU(int n) {
        parent.resize(n);
        rank.resize(n, 0);
        for (int i = 0; i < n; ++i) parent[i] = i;
    }
    int find(int x) {
        if (parent[x] != x) parent[x] = find(parent[x]);
        return parent[x];
    }
    bool unite(int x, int y) {
        x = find(x); y = find(y);
        if (x == y) return false;
        if (rank[x] < rank[y]) swap(x, y);
        parent[y] = x;
        if (rank[x] == rank[y]) rank[x]++;
        return true;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int N, si, sj;
    if (!(cin >> N >> si >> sj)) return 0;
    vector<string> grid(N);
    for (int i = 0; i < N; ++i) {
        cin >> grid[i];
    }

    vector<int> row, col, cost;
    vector<vector<int>> idx(N, vector<int>(N, -1));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (grid[i][j] != '#') {
                idx[i][j] = row.size();
                row.push_back(i);
                col.push_back(j);
                cost.push_back(grid[i][j] - '0');
            }
        }
    }
    int r = row.size();

    if (r == 0) {
        cout << "\n";
        return 0;
    }

    struct Edge {
        int u, v;
        int weight;
    };
    vector<Edge> edges;
    const int di[4] = {-1, 1, 0, 0};
    const int dj[4] = {0, 0, -1, 1};
    for (int i = 0; i < r; ++i) {
        int ci = row[i], cj = col[i];
        for (int d = 0; d < 4; ++d) {
            int ni = ci + di[d], nj = cj + dj[d];
            if (ni < 0 || ni >= N || nj < 0 || nj >= N) continue;
            int j = idx[ni][nj];
            if (j == -1) continue;
            if (j > i) edges.push_back({i, j, cost[i] + cost[j]});
        }
    }

    sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) {
        return a.weight < b.weight;
    });
    DSU dsu(r);
    vector<vector<int>> tree(r);
    int edges_used = 0;
    for (const auto& e : edges) {
        if (dsu.unite(e.u, e.v)) {
            tree[e.u].push_back(e.v);
            tree[e.v].push_back(e.u);
            if (++edges_used == r - 1) break;
        }
    }

    int start_idx = idx[si][sj];
    vector<vector<int>> children(r);
    vector<int> parent(r, -1);
    function<void(int, int)> build_tree = [&](int v, int p) {
        parent[v] = p;
        for (int w : tree[v]) {
            if (w == p) continue;
            children[v].push_back(w);
            build_tree(w, v);
        }
    };
    build_tree(start_idx, -1);

    auto get_dir = [&](int r1, int c1, int r2, int c2) -> char {
        if (r2 == r1 - 1 && c2 == c1) return 'U';
        if (r2 == r1 + 1 && c2 == c1) return 'D';
        if (r2 == r1 && c2 == c1 - 1) return 'L';
        if (r2 == r1 && c2 == c1 + 1) return 'R';
        if (r2 < r1) return 'U';
        if (r2 > r1) return 'D';
        if (c2 < c1) return 'L';
        return 'R';
    };

    string moves;
    function<void(int)> traverse = [&](int v) {
        for (int u : children[v]) {
            moves.push_back(get_dir(row[v], col[v], row[u], col[u]));
            traverse(u);
            moves.push_back(get_dir(row[u], col[u], row[v], col[v]));
        }
    };
    traverse(start_idx);

    cout << moves << "\n";
    return 0;
}