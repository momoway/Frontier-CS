#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m;
    if (!(cin >> n >> m)) return 0;

    // Trivial cases
    if (n == 1) {
        cout << m << "\n";
        for (int j = 1; j <= m; ++j) cout << 1 << " " << j << "\n";
        return 0;
    }
    if (m == 1) {
        cout << n << "\n";
        for (int i = 1; i <= n; ++i) cout << i << " " << 1 << "\n";
        return 0;
    }
    if (n == 2) {
        cout << m + 1 << "\n";
        cout << 1 << " " << 1 << "\n";
        for (int j = 1; j <= m; ++j) cout << 2 << " " << j << "\n";
        return 0;
    }
    if (m == 2) {
        cout << n + 1 << "\n";
        cout << 1 << " " << 1 << "\n";
        cout << 1 << " " << 2 << "\n";
        for (int i = 2; i <= n; ++i) cout << i << " " << 1 << "\n";
        return 0;
    }
    if (m == 3) {
        // Optimal construction for m=3
        if (n == 1) {
            cout << 3 << "\n";
            cout << 1 << " " << 1 << "\n";
            cout << 1 << " " << 2 << "\n";
            cout << 1 << " " << 3 << "\n";
        } else if (n == 2) {
            cout << 4 << "\n";
            cout << 1 << " " << 1 << "\n";
            cout << 1 << " " << 2 << "\n";
            cout << 2 << " " << 1 << "\n";
            cout << 2 << " " << 3 << "\n";
        } else { // n >= 3
            int k = n + 3;
            cout << k << "\n";
            // First three rows: all pairs
            cout << 1 << " " << 1 << "\n";
            cout << 1 << " " << 2 << "\n";
            cout << 2 << " " << 1 << "\n";
            cout << 2 << " " << 3 << "\n";
            cout << 3 << " " << 2 << "\n";
            cout << 3 << " " << 3 << "\n";
            // Remaining rows: single point, e.g., column 1
            for (int i = 4; i <= n; ++i) {
                cout << i << " " << 1 << "\n";
            }
        }
        return 0;
    }

    // General case: greedy construction with priority queue
    vector<int> deg(m + 1, 0);
    vector<vector<int>> col_rows(m + 1);
    // Pair count: key = ((long long)min_row << 32) | max_row
    unordered_map<long long, int> pairCount;
    pairCount.reserve(1000000);

    // Min-heap: (degree, column)
    using P = pair<int, int>;
    priority_queue<P, vector<P>, greater<P>> pq;
    for (int c = 1; c <= m; ++c) {
        pq.push({0, c});
    }

    vector<pair<int, int>> points;
    points.reserve(n * m);

    for (int i = 1; i <= n; ++i) {
        vector<int> tried(m + 1, 0); // 0 = not tried, 1 = tried
        vector<int> skipped; // columns that were skipped (not added) and need to be pushed back later
        vector<int> added;   // columns that were added (with updated degree) to be pushed back later

        while (true) {
            // Get valid column with smallest degree
            int c = -1;
            int cur_deg = -1;
            while (!pq.empty()) {
                auto [d, col] = pq.top();
                if (d != deg[col]) {
                    pq.pop();
                    continue;
                }
                c = col;
                cur_deg = d;
                pq.pop();
                break;
            }
            if (c == -1) break; // heap empty

            if (tried[c]) {
                // This should not happen if we set tried before pushing back, but safe
                continue;
            }
            tried[c] = 1;

            // Check if adding (i,c) would create a rectangle
            bool ok = true;
            for (int r : col_rows[c]) {
                int a = min(i, r), b = max(i, r);
                long long key = ((long long)a << 32) | b;
                if (pairCount[key] >= 1) {
                    ok = false;
                    break;
                }
            }

            if (!ok) {
                skipped.push_back(c);
                continue;
            }

            // Add point (i,c)
            points.emplace_back(i, c);
            deg[c]++; // increase degree
            // Update pair counts with existing rows in this column
            for (int r : col_rows[c]) {
                int a = min(i, r), b = max(i, r);
                long long key = ((long long)a << 32) | b;
                pairCount[key]++;
            }
            col_rows[c].push_back(i);
            added.push_back(c);
            // Do not push back now; we'll push after finishing the row
        }

        // Restore heap: push back skipped columns (with original degree) and added columns (with updated degree)
        for (int c : skipped) {
            pq.push({deg[c], c});
        }
        for (int c : added) {
            pq.push({deg[c], c});
        }
        // tried array will be destroyed; new one for next row
    }

    cout << points.size() << "\n";
    for (auto [r, c] : points) {
        cout << r << " " << c << "\n";
    }
    return 0;
}