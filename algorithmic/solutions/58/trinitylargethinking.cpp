#include <bits/stdc++.h>
using namespace std;

using ull = unsigned long long;

// Compute binomial coefficient C(n, k) with overflow protection
ull binom(int n, int k) {
    if (k < 0 || k > n) return 0;
    if (k > n - k) k = n - k;
    ull res = 1;
    for (int i = 1; i <= k; ++i) {
        // Check overflow: res * (n - k + i) may overflow
        if (res > (ull)std::numeric_limits<ull>::max() / (ull)(n - k + i)) {
            return ULL_MAX; // indicate overflow / too large
        }
        res = res * (n - k + i) / i;
    }
    return res;
}

int main() {
    ull x;
    if (!(cin >> x)) return 0;
    // Try n from 1 to 33 (since for n=33, C(64,32) ~ 1.83e18 > 1e18)
    for (int n = 1; n <= 33; ++n) {
        // Compute max paths F = C(2n-2, n-1)
        ull F = binom(2*n-2, n-1);
        if (F < x) continue; // need larger n
        // Initialize grid all 1
        vector<vector<int>> grid(n+1, vector<int>(n+1, 1));
        grid[1][1] = 1;
        grid[n][n] = 1;
        // Compute current path count
        auto compute_dp = [&](vector<vector<int>>& g) {
            vector<vector<ull>> ways(n+1, vector<ull>(n+1, 0));
            ways[1][1] = g[1][1] ? 1 : 0;
            for (int i = 1; i <= n; ++i) {
                for (int j = 1; j <= n; ++j) {
                    if (i == 1 && j == 1) continue;
                    if (g[i][j]) {
                        ways[i][j] = 0;
                        if (i > 1) ways[i][j] += ways[i-1][j];
                        if (j > 1) ways[i][j] += ways[i][j-1];
                    }
                }
            }
            return ways[n][n];
        };
        ull cur = compute_dp(grid);
        // Greedy reduction
        bool ok = false;
        while (cur > x) {
            // Compute ways and ways2 for current grid
            vector<vector<ull>> ways(n+1, vector<ull>(n+1, 0));
            vector<vector<ull>> ways2(n+1, vector<ull>(n+1, 0));
            // forward
            ways[1][1] = grid[1][1] ? 1 : 0;
            for (int i = 1; i <= n; ++i) {
                for (int j = 1; j <= n; ++j) {
                    if (i == 1 && j == 1) continue;
                    if (grid[i][j]) {
                        ways[i][j] = 0;
                        if (i > 1) ways[i][j] += ways[i-1][j];
                        if (j > 1) ways[i][j] += ways[i][j-1];
                    }
                }
            }
            // backward
            ways2[n][n] = grid[n][n] ? 1 : 0;
            for (int i = n; i >= 1; --i) {
                for (int j = n; j >= 1; --j) {
                    if (i == n && j == n) continue;
                    if (grid[i][j]) {
                        ways2[i][j] = 0;
                        if (i < n) ways2[i][j] += ways2[i+1][j];
                        if (j < n) ways2[i][j] += ways2[i][j+1];
                    }
                }
            }
            ull remaining = cur - x;
            int best_i = -1, best_j = -1;
            ull best_red = 0;
            for (int i = 1; i <= n; ++i) {
                for (int j = 1; j <= n; ++j) {
                    if ((i == 1 && j == 1) || (i == n && j == n)) continue;
                    if (grid[i][j] == 0) continue;
                    ull w = ways[i][j];
                    ull w2 = ways2[i][j];
                    if (w == 0 || w2 == 0) continue;
                    // Check if w * w2 > remaining (avoid overflow)
                    if (w > remaining / w2) continue; // reduction > remaining
                    ull red = w * w2;
                    if (red > remaining) continue;
                    if (red > best_red) {
                        best_red = red;
                        best_i = i;
                        best_j = j;
                    }
                }
            }
            if (best_red == 0) {
                // Cannot reduce further without overshooting
                break;
            }
            // Flip the cell
            grid[best_i][best_j] = 0;
            // Recompute cur (could subtract best_red, but recompute to be safe)
            cur = compute_dp(grid);
        }
        if (cur == x) {
            // Output grid
            cout << n << "\n";
            for (int i = 1; i <= n; ++i) {
                for (int j = 1; j <= n; ++j) {
                    cout << grid[i][j];
                    if (j < n) cout << " ";
                }
                cout << "\n";
            }
            return 0;
        }
    }
    // If no n worked, output a fallback (should not happen for given constraints)
    // As a fallback, output a 1x1 grid with 1 (only works for x=1)
    cout << "1\n1 1\n"; // This is a placeholder; may be incorrect for many x.
    return 0;
}