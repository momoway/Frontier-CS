#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int t;
    if (!(cin >> t)) return 0;
    while (t--) {
        int n, m;
        cin >> n >> m;
        vector<int> A(m), B(m);
        for (int i = 0; i < m; i++) {
            cin >> A[i] >> B[i];
        }

        // Step 0: Block all edges
        for (int i = 0; i < m; i++) {
            cout << "- " << (i+1) << endl;
            cout.flush();
        }

        // Step 1: Find s using binary search on vertices (all edges blocked)
        vector<int> cand;
        for (int i = 1; i <= n; i++) cand.push_back(i);
        while (cand.size() > 1) {
            int mid = cand.size() / 2;
            vector<int> Y(cand.begin(), cand.begin() + mid);
            cout << "? " << Y.size();
            for (int v : Y) cout << " " << v;
            cout << endl;
            cout.flush();
            int res;
            cin >> res;
            if (res == -1) return 0;
            if (res == 1) {
                cand.erase(cand.begin() + mid, cand.end());
            } else {
                cand.erase(cand.begin(), cand.begin() + mid);
            }
        }
        int s = cand[0];

        // Data structures
        vector<bool> known(m, false);
        vector<bool> is_rep(m, false);
        vector<bool> inK(m, false); // edge is in known set and unblocked
        vector<bool> inR(n+1, false);
        inR[s] = true;
        vector<int> parent_edge(n+1, -1); // edge index (0-based) connecting to parent
        vector<int> depth(n+1, 0);

        // Step 2: Process edges incident to s
        for (int i = 0; i < m; i++) {
            if (A[i] == s || B[i] == s) {
                // Unblock edge i
                cout << "+ " << (i+1) << endl;
                cout.flush();
                int u = A[i], v = B[i];
                int other = (u == s) ? v : u;
                // Query other endpoint
                cout << "? 1 " << other << endl;
                cout.flush();
                int ans;
                cin >> ans;
                if (ans == -1) return 0;
                if (ans == 1) {
                    is_rep[i] = true;
                    known[i] = true;
                    inK[i] = true;
                    inR[other] = true;
                    parent_edge[other] = i;
                    depth[other] = 1;
                } else {
                    is_rep[i] = false;
                    known[i] = true;
                    // Block it again
                    cout << "- " << (i+1) << endl;
                    cout.flush();
                }
            }
        }

        // Step 3: Expand reachable set R using BFS-like approach
        while (true) {
            bool all_in = true;
            for (int v = 1; v <= n; v++) {
                if (!inR[v]) { all_in = false; break; }
            }
            if (all_in) break;

            int found_edge = -1;
            int u_in = -1, v_out = -1;
            for (int i = 0; i < m; i++) {
                if (known[i]) continue;
                int u = A[i], v = B[i];
                if (inR[u] && !inR[v]) {
                    found_edge = i;
                    u_in = u;
                    v_out = v;
                    break;
                }
                if (inR[v] && !inR[u]) {
                    found_edge = i;
                    u_in = v;
                    v_out = u;
                    break;
                }
            }
            if (found_edge == -1) {
                // Should not happen for a connected graph, but break to avoid infinite loop
                break;
            }

            // Unblock found_edge
            cout << "+ " << (found_edge+1) << endl;
            cout.flush();
            // Query the outside endpoint
            cout << "? 1 " << v_out << endl;
            cout.flush();
            int ans;
            cin >> ans;
            if (ans == -1) return 0;
            if (ans == 1) {
                is_rep[found_edge] = true;
                known[found_edge] = true;
                inK[found_edge] = true;
                inR[v_out] = true;
                parent_edge[v_out] = found_edge;
                depth[v_out] = depth[u_in] + 1;
            } else {
                is_rep[found_edge] = false;
                known[found_edge] = true;
                // Block it again
                cout << "- " << (found_edge+1) << endl;
                cout.flush();
            }
        }

        // Step 4: Process remaining edges (non-tree edges)
        for (int i = 0; i < m; i++) {
            if (known[i]) continue;
            int u = A[i], v = B[i];
            // Determine deeper endpoint
            int d = u, o = v;
            if (depth[u] < depth[v]) {
                d = v;
                o = u;
            }
            // d is deeper, its parent edge is in K
            int pedge = parent_edge[d];
            if (pedge == -1) {
                // Should not happen (d should not be s), but skip to avoid error
                continue;
            }
            // Block parent edge
            cout << "- " << (pedge+1) << endl;
            cout.flush();
            // Unblock edge i
            cout << "+ " << (i+1) << endl;
            cout.flush();
            // Query deeper endpoint
            cout << "? 1 " << d << endl;
            cout.flush();
            int ans;
            cin >> ans;
            if (ans == -1) return 0;
            if (ans == 1) {
                is_rep[i] = true;
            } else {
                is_rep[i] = false;
            }
            // Restore parent edge
            cout << "+ " << (pedge+1) << endl;
            cout.flush();
            // Block edge i again (not needed, but keep state clean)
            cout << "- " << (i+1) << endl;
            cout.flush();
        }

        // Output answer
        cout << "! ";
        for (int i = 0; i < m; i++) {
            cout << (is_rep[i] ? '1' : '0') << " ";
        }
        cout << endl;
        cout.flush();

        int resp;
        cin >> resp;
        if (resp == 0) {
            // Wrong answer, terminate
            return 0;
        }
    }
    return 0;
}