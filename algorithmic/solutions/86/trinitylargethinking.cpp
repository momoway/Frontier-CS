#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    if (!(cin >> n)) return 0;

    vector<int> proj12(n+1), proj13(n+1), proj23(n+1);

    // Query median of 1,2,3
    int m123;
    cout << "0 1 2 3" << endl;
    cout.flush();
    cin >> m123;

    // For nodes 4..n, query projections onto the three paths
    for (int i = 4; i <= n; ++i) {
        cout << "0 1 2 " << i << endl;
        cout.flush();
        cin >> proj12[i];
        cout << "0 1 3 " << i << endl;
        cout.flush();
        cin >> proj13[i];
        cout << "0 2 3 " << i << endl;
        cout.flush();
        cin >> proj23[i];
    }

    // Set projections for nodes 1,2,3
    proj12[1] = 1; proj12[2] = 2; proj12[3] = m123;
    proj13[1] = 1; proj13[3] = 3; proj13[2] = m123; // (1,3,2) gives same median
    proj23[1] = m123; proj23[2] = 2; proj23[3] = 3;

    // Count nodes on each path
    int count12 = 0, count13 = 0, count23 = 0;
    for (int i = 1; i <= n; ++i) {
        if (proj12[i] == i) count12++;
        if (proj13[i] == i) count13++;
        if (proj23[i] == i) count23++;
    }

    int A, B;
    vector<int> *proj_ptr;
    if (count12 >= count13 && count12 >= count23) {
        A = 1; B = 2; proj_ptr = &proj12;
    } else if (count13 >= count12 && count13 >= count23) {
        A = 1; B = 3; proj_ptr = &proj13;
    } else {
        A = 2; B = 3; proj_ptr = &proj23;
    }
    vector<int> proj = *proj_ptr;

    // Collect nodes on the chosen diameter
    vector<int> diam;
    for (int i = 1; i <= n; ++i) {
        if (proj[i] == i) diam.push_back(i);
    }

    // Sort diameter nodes by distance from A
    sort(diam.begin(), diam.end(), [&](int x, int y) {
        if (x == A) return true;
        if (y == A) return false;
        cout << "0 " << A << " " << x << " " << y << endl;
        cout.flush();
        int z;
        cin >> z;
        if (z == x) return true;   // x closer to A
        if (z == y) return false;  // y closer to A
        return true; // fallback (should not happen)
    });

    // Build edges
    vector<pair<int,int>> edges;
    // Edges along the diameter
    for (int i = 0; i + 1 < (int)diam.size(); ++i) {
        edges.push_back({diam[i], diam[i+1]});
    }
    // Edges for other nodes: attach to their projection on the diameter
    for (int i = 1; i <= n; ++i) {
        bool on_diam = false;
        for (int v : diam) if (v == i) { on_diam = true; break; }
        if (!on_diam) {
            int p = proj[i];
            edges.push_back({p, i});
        }
    }

    // Output the tree
    cout << "1";
    for (auto &e : edges) {
        cout << " " << e.first << " " << e.second;
    }
    cout << endl;
    cout.flush();

    return 0;
}