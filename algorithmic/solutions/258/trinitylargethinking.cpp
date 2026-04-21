#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int t;
    if (!(cin >> t)) return 0;
    while (t--) {
        int n;
        cin >> n;
        vector<vector<int>> adj(n + 1);
        for (int i = 0; i < n - 1; i++) {
            int u, v;
            cin >> u >> v;
            adj[u].push_back(v);
            adj[v].push_back(u);
        }

        // First query: all nodes
        cout << "? " << n;
        for (int i = 1; i <= n; i++) cout << " " << i;
        cout << endl;
        cout.flush();

        int x0, d0;
        cin >> x0 >> d0;
        if (x0 == -1 && d0 == -1) return 0;

        int ref = x0;
        int d = d0;

        // BFS to compute distances and parents from ref
        vector<int> dist(n + 1, -1);
        vector<int> parent(n + 1, 0);
        queue<int> q;
        q.push(ref);
        dist[ref] = 0;
        parent[ref] = 0;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (int v : adj[u]) {
                if (dist[v] == -1) {
                    dist[v] = dist[u] + 1;
                    parent[v] = u;
                    q.push(v);
                }
            }
        }

        int maxDist = 0;
        for (int i = 1; i <= n; i++) {
            if (dist[i] > maxDist) maxDist = dist[i];
        }

        // Binary search for L: maximum distance from ref with a node on the path
        int low = 0, high = maxDist;
        while (low < high) {
            int mid = (low + high + 1) / 2;
            vector<int> nodes;
            for (int v = 1; v <= n; v++) {
                if (dist[v] == mid) nodes.push_back(v);
            }
            if (nodes.empty()) {
                high = mid - 1;
                continue;
            }
            cout << "? " << nodes.size();
            for (int v : nodes) cout << " " << v;
            cout << endl;
            cout.flush();
            int xq, dq;
            cin >> xq >> dq;
            if (xq == -1 && dq == -1) return 0;
            if (dq == d) low = mid;
            else high = mid - 1;
        }
        int L = low;

        if (L == d) {
            // ref is one endpoint
            vector<int> endpoints;
            endpoints.push_back(ref);
            vector<int> S_d;
            for (int v = 1; v <= n; v++) if (dist[v] == d) S_d.push_back(v);
            cout << "? " << S_d.size();
            for (int v : S_d) cout << " " << v;
            cout << endl;
            cout.flush();
            int xb, db;
            cin >> xb >> db;
            if (xb == -1 && db == -1) return 0;
            endpoints.push_back(xb);
            cout << "! " << endpoints[0] << " " << endpoints[1] << endl;
            cout.flush();
        } else if (2 * L == d) {
            // both endpoints at distance L
            vector<int> S_L;
            for (int v = 1; v <= n; v++) if (dist[v] == L) S_L.push_back(v);
            cout << "? " << S_L.size();
            for (int v : S_L) cout << " " << v;
            cout << endl;
            cout.flush();
            int a1, da1;
            cin >> a1 >> da1;
            if (a1 == -1 && da1 == -1) return 0;
            vector<int> S_L2;
            for (int v : S_L) if (v != a1) S_L2.push_back(v);
            cout << "? " << S_L2.size();
            for (int v : S_L2) cout << " " << v;
            cout << endl;
            cout.flush();
            int a2, da2;
            cin >> a2 >> da2;
            if (a2 == -1 && da2 == -1) return 0;
            cout << "! " << a1 << " " << a2 << endl;
            cout.flush();
        } else {
            // L > d - L: farther endpoint at distance L, nearer at distance d-L
            vector<int> S_L;
            for (int v = 1; v <= n; v++) if (dist[v] == L) S_L.push_back(v);
            cout << "? " << S_L.size();
            for (int v : S_L) cout << " " << v;
            cout << endl;
            cout.flush();
            int a, da;
            cin >> a >> da;
            if (a == -1 && da == -1) return 0;

            // Find neighbor of ref on the path to a
            int cur = a;
            while (parent[cur] != ref) {
                cur = parent[cur];
            }
            int y_a = cur;

            // Mark subtree of y_a (excluding ref)
            vector<bool> inSubtree(n + 1, false);
            queue<int> qq;
            qq.push(y_a);
            inSubtree[y_a] = true;
            while (!qq.empty()) {
                int u = qq.front(); qq.pop();
                for (int v : adj[u]) {
                    if (v == ref) continue;
                    if (!inSubtree[v]) {
                        inSubtree[v] = true;
                        qq.push(v);
                    }
                }
            }

            // Nodes at distance d-L not in that subtree
            vector<int> S_opposite;
            for (int v = 1; v <= n; v++) {
                if (dist[v] == d - L && !inSubtree[v]) {
                    S_opposite.push_back(v);
                }
            }

            cout << "? " << S_opposite.size();
            for (int v : S_opposite) cout << " " << v;
            cout << endl;
            cout.flush();
            int b, db;
            cin >> b >> db;
            if (b == -1 && db == -1) return 0;
            cout << "! " << a << " " << b << endl;
            cout.flush();
        }

        string feedback;
        cin >> feedback;
        if (feedback != "Correct") return 0;
    }
    return 0;
}