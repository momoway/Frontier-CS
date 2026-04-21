#include <bits/stdc++.h>
using namespace std;

int main() {
    const int N = 100;
    bool adj[N][N] = {};

    // Reference set: vertices 1,2,3,4,5 (0-indexed 0..4)
    vector<int> ref = {0,1,2,3,4};

    // Map edge index to pair (i,j)
    vector<pair<int,int>> edges_ref;
    for (int i = 0; i < 5; ++i)
        for (int j = i+1; j < 5; ++j)
            edges_ref.push_back({i, j});
    // There should be 10 edges

    // Query all triples among reference vertices
    map<tuple<int,int,int>, int> ans_ref;
    for (int i = 0; i < 5; ++i) {
        for (int j = i+1; j < 5; ++j) {
            for (int k = j+1; k < 5; ++k) {
                int a = ref[i], b = ref[j], c = ref[k];
                cout << "? " << a+1 << " " << b+1 << " " << c+1 << endl;
                cout.flush();
                int res;
                cin >> res;
                // sort a,b,c for key
                if (a > b) swap(a,b);
                if (a > c) swap(a,c);
                if (b > c) swap(b,c);
                ans_ref[make_tuple(a,b,c)] = res;
            }
        }
    }

    // Brute-force to find edge assignment for reference set
    int ref_sol = -1;
    for (int mask = 0; mask < (1<<10); ++mask) {
        bool ok = true;
        for (auto &p : ans_ref) {
            auto [a,b,c] = p.first;
            int sum = 0;
            // find edge indices for pairs
            for (int idx = 0; idx < 10; ++idx) {
                int u = edges_ref[idx].first, v = edges_ref[idx].second;
                if ((u == a && v == b) || (u == b && v == a)) sum += ((mask>>idx)&1);
                if ((u == a && v == c) || (u == c && v == a)) sum += ((mask>>idx)&1);
                if ((u == b && v == c) || (u == c && v == b)) sum += ((mask>>idx)&1);
            }
            if (sum != p.second) { ok = false; break; }
        }
        if (ok) {
            if (ref_sol == -1) ref_sol = mask;
            else {
                // multiple solutions, should not happen; we'll keep first
                // but better to handle: we could use extra query, but we assume unique
            }
        }
    }

    if (ref_sol == -1) {
        // No solution found; this should not happen. We'll exit.
        cerr << "No reference solution found!" << endl;
        return 1;
    }

    // Set adjacency for reference set
    for (int idx = 0; idx < 10; ++idx) {
        int u = edges_ref[idx].first, v = edges_ref[idx].second;
        if ((ref_sol >> idx) & 1) {
            adj[u][v] = adj[v][u] = true;
        }
    }

    // For each new vertex (indices 5..99)
    for (int v = 5; v < N; ++v) {
        // Query 5-cycle: (v, ref[0], ref[1]), (v, ref[1], ref[2]), (v, ref[2], ref[3]), (v, ref[3], ref[4]), (v, ref[4], ref[0])
        int s01, s12, s23, s34, s40;
        cout << "? " << v+1 << " " << ref[0]+1 << " " << ref[1]+1 << endl; cout.flush(); cin >> s01;
        cout << "? " << v+1 << " " << ref[1]+1 << " " << ref[2]+1 << endl; cout.flush(); cin >> s12;
        cout << "? " << v+1 << " " << ref[2]+1 << " " << ref[3]+1 << endl; cout.flush(); cin >> s23;
        cout << "? " << v+1 << " " << ref[3]+1 << " " << ref[4]+1 << endl; cout.flush(); cin >> s34;
        cout << "? " << v+1 << " " << ref[4]+1 << " " << ref[0]+1 << endl; cout.flush(); cin >> s40;

        // Compute constants: c = answer - known edge between ref vertices
        int c01 = s01 - (adj[ref[0]][ref[1]] ? 1 : 0);
        int c12 = s12 - (adj[ref[1]][ref[2]] ? 1 : 0);
        int c23 = s23 - (adj[ref[2]][ref[3]] ? 1 : 0);
        int c34 = s34 - (adj[ref[3]][ref[4]] ? 1 : 0);
        int c40 = s40 - (adj[ref[4]][ref[0]] ? 1 : 0);

        // Brute-force y[0..4] (adjacency from v to ref[i])
        int sol_y = -1;
        for (int mask = 0; mask < (1<<5); ++mask) {
            bool ok = true;
            // y0+y1 == c01
            if ( ((mask>>0)&1) + ((mask>>1)&1) != c01 ) ok = false;
            // y1+y2 == c12
            if ( ((mask>>1)&1) + ((mask>>2)&1) != c12 ) ok = false;
            // y2+y3 == c23
            if ( ((mask>>2)&1) + ((mask>>3)&1) != c23 ) ok = false;
            // y3+y4 == c34
            if ( ((mask>>3)&1) + ((mask>>4)&1) != c34 ) ok = false;
            // y4+y0 == c40
            if ( ((mask>>4)&1) + ((mask>>0)&1) != c40 ) ok = false;
            if (ok) {
                if (sol_y == -1) sol_y = mask;
                else {
                    // multiple; keep first
                }
            }
        }

        if (sol_y == -1) {
            cerr << "No solution for vertex " << v+1 << endl;
            return 1;
        }

        // Set adjacency
        for (int i = 0; i < 5; ++i) {
            if ((sol_y >> i) & 1) {
                adj[v][ref[i]] = adj[ref[i]][v] = true;
            }
        }
    }

    // Determine edges among new vertices (indices 5..99)
    for (int u = 5; u < N; ++u) {
        for (int v = u+1; v < N; ++v) {
            // Query with ref[0] (vertex 1)
            cout << "? " << u+1 << " " << v+1 << " " << ref[0]+1 << endl;
            cout.flush();
            int s;
            cin >> s;
            int x = s - (adj[u][ref[0]] ? 1 : 0) - (adj[v][ref[0]] ? 1 : 0);
            if (x < 0) x = 0;
            if (x > 1) x = 1;
            adj[u][v] = adj[v][u] = (x == 1);
        }
    }

    // Output the graph
    cout << "!" << endl;
    for (int i = 0; i < N; ++i) {
        string line;
        for (int j = 0; j < N; ++j) {
            line.push_back(adj[i][j] ? '1' : '0');
        }
        cout << line << endl;
    }
    cout.flush();

    return 0;
}