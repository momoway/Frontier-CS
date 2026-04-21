#include <bits/stdc++.h>
using namespace std;

struct Swap {
    int x1, y1, x2, y2;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const int N = 30;
    const int TOTAL = N * (N + 1) / 2; // 465

    // Read input
    vector<vector<int>> val(N, vector<int>(N));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j <= i; ++j) {
            if (!(cin >> val[i][j])) return 0;
        }
    }

    // Precompute index to coordinate mapping (level order)
    vector<pair<int,int>> idxToCoord(TOTAL);
    for (int x = 0; x < N; ++x) {
        for (int y = 0; y <= x; ++y) {
            int idx = x * (x + 1) / 2 + y;
            idxToCoord[idx] = {x, y};
        }
    }

    vector<Swap> swaps;
    const int MAX_SWAPS = 10000;

    // Process nodes in reverse level order (from last index to 0)
    for (int i = TOTAL - 1; i >= 0; --i) {
        if (swaps.size() >= MAX_SWAPS) break;
        int x = idxToCoord[i].first;
        int y = idxToCoord[i].second;
        // Skip last row (no children)
        if (x == N - 1) continue;

        // Sift-down starting at (x, y)
        int curX = x, curY = y;
        while (true) {
            // Find the smallest among current node and its two children
            int curVal = val[curX][curY];
            int minVal = curVal;
            int minX = curX, minY = curY;

            // Child 1: (curX+1, curY)
            if (curX + 1 < N) {
                int v = val[curX+1][curY];
                if (v < minVal) {
                    minVal = v;
                    minX = curX + 1;
                    minY = curY;
                }
            }
            // Child 2: (curX+1, curY+1)
            if (curX + 1 < N) {
                int v = val[curX+1][curY+1];
                if (v < minVal) {
                    minVal = v;
                    minX = curX + 1;
                    minY = curY + 1;
                }
            }

            // If current node is the smallest, heap property satisfied
            if (minX == curX && minY == curY) break;

            // Perform swap
            if (swaps.size() + 1 > MAX_SWAPS) {
                // Cannot exceed swap limit
                goto output;
            }
            swap(val[curX][curY], val[minX][minY]);
            swaps.push_back({curX, curY, minX, minY});

            // Move current position to the child where swap occurred
            curX = minX;
            curY = minY;
        }
    }

output:
    cout << swaps.size() << "\n";
    for (const auto& s : swaps) {
        cout << s.x1 << " "s.y1 << " " << s.x2 << " " << s.y2 << "\n";
    }
    return 0;
}