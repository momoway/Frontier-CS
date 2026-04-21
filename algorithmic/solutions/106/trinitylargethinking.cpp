#include <bits/stdc++.h>
using namespace std;

int n;
map<vector<int>, int> memo;

vector<int> merge_sorted(const vector<int>& a, const vector<int>& b) {
    vector<int> res;
    res.reserve(a.size() + b.size());
    size_t i = 0, j = 0;
    while (i < a.size() && j < b.size()) {
        if (a[i] < b[j]) res.push_back(a[i++]);
        else res.push_back(b[j++]);
    }
    while (i < a.size()) res.push_back(a[i++]);
    while (j < b.size()) res.push_back(b[j++]);
    return res;
}

int query_subset(const vector<int>& verts) {
    vector<int> sorted = verts;
    sort(sorted.begin(), sorted.end());
    auto it = memo.find(sorted);
    if (it != memo.end()) return it->second;
    cout << "? " << sorted.size() << "\n";
    for (size_t i = 0; i < sorted.size(); ++i) {
        if (i) cout << " ";
        cout << sorted[i];
    }
    cout << "\n";
    cout.flush();
    int m;
    cin >> m;
    if (m == -1) exit(0);
    memo[sorted] = m;
    return m;
}

int find_candidate(const vector<int>& S, const vector<int>& R, int e_S) {
    vector<int> T = R;
    while (T.size() > 1) {
        int half = T.size() / 2;
        vector<int> T1(T.begin(), T.begin() + half);
        vector<int> T2(T.begin() + half, T.end());
        int e_T1 = query_subset(T1);
        vector<int> S_union_T1 = merge_sorted(S, T1);
        int e_S_union_T1 = query_subset(S_union_T1);
        int cross = e_S_union_T1 - e_S - e_T1;
        if (cross > 0) T = T1;
        else T = T2;
    }
    return T[0];
}

int find_neighbor(int v, const vector<int>& S) {
    vector<int> U = S;
    while (U.size() > 1) {
        int half = U.size() / 2;
        vector<int> U1(U.begin(), U.begin() + half);
        vector<int> U2(U.begin() + half, U.end());
        int e_U1 = query_subset(U1);
        vector<int> v_U1 = U1;
        v_U1.push_back(v);
        sort(v_U1.begin(), v_U1.end());
        int e_v_U1 = query_subset(v_U1);
        int edges_v_to_U1 = e_v_U1 - e_U1;
        if (edges_v_to_U1 > 0) U = U1;
        else U = U2;
    }
    return U[0];
}

pair<int,int> find_edge_within(const vector<int>& C) {
    vector<int> cur = C;
    while (cur.size() > 2) {
        int half = cur.size() / 2;
        vector<int> C1(cur.begin(), cur.begin() + half);
        vector<int> C2(cur.begin() + half, cur.end());
        int e_C1 = query_subset(C1);
        if (e_C1 > 0) cur = C1;
        else cur = C2;
    }
    return {cur[0], cur[1]};
}

vector<int> get_cycle(int x, int y, const vector<int>& parent, const vector<int>& depth) {
    int a = x, b = y;
    while (depth[a] > depth[b]) a = parent[a];
    while (depth[b] > depth[a]) b = parent[b];
    while (a != b) {
        a = parent[a];
        b = parent[b];
    }
    int lca = a;
    vector<int> path;
    a = x;
    while (a != lca) {
        path.push_back(a);
        a = parent[a];
    }
    path.push_back(lca);
    vector<int> temp;
    b = y;
    while (b != lca) {
        temp.push_back(b);
        b = parent[b];
    }
    reverse(temp.begin(), temp.end());
    for (int v : temp) path.push_back(v);
    return path;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> n;
    if (n == 1) {
        // Trivially bipartite
        cout << "Y 1\n1\n";
        cout.flush();
        return 0;
    }
    vector<int> S = {1};
    int e_S = 0; // e({1}) = 0
    vector<int> R;
    for (int i = 2; i <= n; ++i) R.push_back(i);
    vector<int> parent(n+1, 0), depth(n+1, 0), color(n+1, 0);
    parent[1] = 0; depth[1] = 0; color[1] = 0;
    while (!R.empty()) {
        int v = find_candidate(S, R, e_S);
        int u = find_neighbor(v, S);
        // Verify edge
        vector<int> edge_check = {v, u};
        sort(edge_check.begin(), edge_check.end());
        int e_edge = query_subset(edge_check);
        if (e_edge != 1) {
            // Should not happen; but continue anyway
        }
        parent[v] = u;
        depth[v] = depth[u] + 1;
        color[v] = color[u] ^ 1;
        vector<int> newS = merge_sorted(S, vector<int>(1, v));
        e_S = query_subset(newS);
        S = newS;
        R.erase(remove(R.begin(), R.end(), v), R.end());
    }
    vector<int> A, B;
    for (int i = 1; i <= n; ++i) {
        if (color[i] == 0) A.push_back(i);
        else B.push_back(i);
    }
    int e_A = query_subset(A);
    int e_B = query_subset(B);
    if (e_A == 0 && e_B == 0) {
        cout << "Y " << A.size() << "\n";
        for (size_t i = 0; i < A.size(); ++i) {
            if (i) cout << " ";
            cout << A[i];
        }
        cout << "\n";
    } else {
        vector<int> C = (e_A > 0) ? A : B;
        auto edge = find_edge_within(C);
        int x = edge.first, y = edge.second;
        vector<int> cycle = get_cycle(x, y, parent, depth);
        cout << "N " << cycle.size() << "\n";
        for (size_t i = 0; i < cycle.size(); ++i) {
            if (i) cout << " ";
            cout << cycle[i];
        }
        cout << "\n";
    }
    cout.flush();
    return 0;
}