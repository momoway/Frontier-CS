#include <bits/stdc++.h>
using namespace std;

struct VectorHash {
    size_t operator()(const vector<uint16_t>& v) const {
        size_t h = v.size();
        for (uint16_t x : v) {
            h = h * 31 + x;
        }
        return h;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m;
    cin >> n >> m;
    int N = n * m;
    vector<int> rot(n, 0);
    vector<uint16_t> state_vec(n);
    unordered_set<vector<uint16_t>, VectorHash> visited;

    // Initial rotation to get a starting unblocked count
    cout << "? 0 1" << endl;
    cout.flush();
    int U_old;
    cin >> U_old;
    rot[0] = 1; // rotate ring 0 clockwise once

    int cur = 0;
    int dir = 1;
    bool improved_last = false; // after initial move, no previous to compare

    while (U_old < N - m) {
        // Perform rotation on current ring with current direction
        cout << "? " << cur << " " << dir << endl;
        cout.flush();
        int U_new;
        cin >> U_new;

        // Update rotation count for this ring
        rot[cur] = (rot[cur] + dir) % N;
        if (rot[cur] < 0) rot[cur] += N;

        // Build state vector (modulo N)
        for (int i = 0; i < n; ++i) {
            int val = rot[i] % N;
            if (val < 0) val += N;
            state_vec[i] = (uint16_t)val;
        }

        // Check if this state has been visited
        if (visited.find(state_vec) != visited.end()) {
            // Already seen this state, move to next ring to escape
            cur = (cur + 1) % n;
            dir = 1;
            U_old = U_new;
            continue;
        }
        visited.insert(state_vec);

        // Decide next steps based on improvement
        if (U_new > U_old) {
            // Improvement, keep current ring and direction
            improved_last = true;
        } else {
            // No improvement
            if (improved_last) {
                // First non-improving after an improvement: flip direction
                dir = -dir;
                improved_last = false;
            } else {
                // Second consecutive non-improving: move to next ring
                cur = (cur + 1) % n;
                dir = 1;
                improved_last = false;
            }
        }
        U_old = U_new;
    }

    // All rings aligned: output zeros for relative positions
    cout << "!";
    for (int i = 1; i < n; ++i) {
        cout << " 0";
    }
    cout << endl;
    return 0;
}