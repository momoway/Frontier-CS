#include <bits/stdc++.h>
using namespace std;

int computeProb4(int idx, int var, int valVar, const vector<int>& val, const vector<int>& A, const vector<int>& B) {
    int a = A[idx];
    int b = B[idx];
    // Both literals involve the same variable and that variable is var
    if (abs(a) == abs(b) && abs(a) == var) {
        bool litA_true = (a > 0) ? (valVar == 1) : (valVar == 0);
        bool litB_true = (b > 0) ? (valVar == 1) : (valVar == 0);
        if (litA_true || litB_true) return 4;
        else return 0;
    }
    // Exactly one literal involves var
    bool isA = (abs(a) == var);
    int otherVar = isA ? abs(b) : abs(a);
    bool otherSign = isA ? (b > 0) : (a > 0);
    bool litSign = isA ? (a > 0) : (b > 0);
    bool litTrue = (litSign ? (valVar == 1) : (valVar == 0));
    bool otherKnown = (val[otherVar] != -1);
    if (litTrue) {
        return 4;
    } else {
        if (otherKnown) {
            bool otherTrue = (otherSign ? (val[otherVar] == 1) : (val[otherVar] == 0));
            if (otherTrue) return 4;
            else return 0;
        } else {
            return 2;
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    if (!(cin >> n >> m)) return 0;
    if (m == 0) {
        for (int i = 0; i < n; ++i) {
            if (i) cout << ' ';
            cout << 0;
        }
        cout << '\n';
        return 0;
    }

    vector<int> A(m), B(m);
    vector<vector<int>> occ(n+1);
    int totalNodes = 2 * n;
    vector<vector<int>> adj(totalNodes), adjRev(totalNodes);

    for (int i = 0; i < m; ++i) {
        cin >> A[i] >> B[i];
        occ[abs(A[i])].push_back(i);
        occ[abs(B[i])].push_back(i);
        int na = (A[i] > 0) ? 2*(A[i]-1) : 2*(-A[i]-1)+1;
        int nb = (B[i] > 0) ? 2*(B[i]-1) : 2*(-B[i]-1)+1;
        adj[na^1].push_back(nb);
        adj[nb^1].push_back(na);
        adjRev[nb].push_back(na^1);
        adjRev[na].push_back(nb^1);
    }

    // Kosaraju's algorithm for 2-SAT
    vector<int> order;
    vector<bool> visited(totalNodes, false);
    function<void(int)> dfs1 = [&](int u) {
        visited[u] = true;
        for (int v : adj[u]) {
            if (!visited[v]) dfs1(v);
        }
        order.push_back(u);
    };
    for (int i = 0; i < totalNodes; ++i) {
        if (!visited[i]) dfs1(i);
    }

    vector<int> comp(totalNodes, -1);
    int cid = 0;
    function<void(int)> dfs2 = [&](int u) {
        comp[u] = cid;
        for (int v : adjRev[u]) {
            if (comp[v] == -1) dfs2(v);
        }
    };
    for (int i = totalNodes-1; i >= 0; --i) {
        int u = order[i];
        if (comp[u] == -1) {
            dfs2(u);
            cid++;
        }
    }

    bool satisfiable = true;
    for (int i = 0; i < n; ++i) {
        if (comp[2*i] == comp[2*i+1]) {
            satisfiable = false;
            break;
        }
    }

    if (satisfiable) {
        vector<int> ans(n);
        for (int i = 0; i < n; ++i) {
            ans[i] = (comp[2*i] > comp[2*i+1]) ? 1 : 0;
        }
        for (int i = 0; i < n; ++i) {
            if (i) cout << ' ';
            cout << ans[i];
        }
        cout << '\n';
        return 0;
    }

    // Approximation algorithm for Max-2-SAT
    vector<int> clauseProb4(m);
    for (int i = 0; i < m; ++i) {
        int a = A[i], b = B[i];
        if (abs(a) == abs(b)) {
            if (a == b) clauseProb4[i] = 2; // 0.5
            else clauseProb4[i] = 4; // always true
        } else {
            clauseProb4[i] = 3; // 0.75
        }
    }

    vector<int> val(n+1, -1); // -1 = unassigned, 0/1 = assigned

    for (int i = 1; i <= n; ++i) {
        long long delta0 = 0, delta1 = 0;
        for (int idx : occ[i]) {
            int cur = clauseProb4[idx];
            int p0 = computeProb4(idx, i, 0, val, A, B);
            int p1 = computeProb4(idx, i, 1, val, A, B);
            delta0 += p0 - cur;
            delta1 += p1 - cur;
        }
        if (delta0 >= delta1) {
            val[i] = 0;
            for (int idx : occ[i]) {
                clauseProb4[idx] = computeProb4(idx, i, 0, val, A, B);
            }
        } else {
            val[i] = 1;
            for (int idx : occ[i]) {
                clauseProb4[idx] = computeProb4(idx, i, 1, val, A, B);
            }
        }
    }

    for (int i = 1; i <= n; ++i) {
        if (i > 1) cout << ' ';
        cout << val[i];
    }
    cout << '\n';
    return 0;
}