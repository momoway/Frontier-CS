#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m;
    if (!(cin >> n >> m)) return 0;
    vector<string> s(m);
    for (int i = 0; i < m; ++i) {
        cin >> s[i];
    }

    // Compute conflict masks for each pattern
    vector<int> conflict(m, 0);
    for (int i = 0; i < m; ++i) {
        for (int j = i + 1; j < m; ++j) {
            bool clash = false;
            for (int p = 0; p < n; ++p) {
                char c1 = s[i][p];
                char c2 = s[j][p];
                if (c1 != '?' && c2 != '?' && c1 != c2) {
                    clash = true;
                    break;
                }
            }
            if (clash) {
                conflict[i] |= (1 << j);
                conflict[j] |= (1 << i);
            }
        }
    }

    // Compute bitmask of patterns that have a fixed letter at each position
    vector<int> v_mask(n, 0);
    for (int p = 0; p < n; ++p) {
        int mask = 0;
        for (int i = 0; i < m; ++i) {
            if (s[i][p] != '?') {
                mask |= (1 << i);
            }
        }
        v_mask[p] = mask;
    }

    // Frequency of each v_mask
    int total_masks = 1 << m;
    vector<int> freq(total_masks, 0);
    for (int p = 0; p < n; ++p) {
        freq[v_mask[p]]++;
    }

    // SOS DP: g[mask] = sum_{sub ⊆ mask} freq[sub]
    vector<int> g = freq;
    for (int i = 0; i < m; ++i) {
        for (int mask = 0; mask < total_masks; ++mask) {
            if (mask & (1 << i)) {
                g[mask] += g[mask ^ (1 << i)];
            }
        }
    }

    // Independent set DP
    vector<char> independent(total_masks, 0);
    independent[0] = 1;
    for (int mask = 1; mask < total_masks; ++mask) {
        int lsb = mask & -mask;
        int i = __builtin_ctz(lsb);
        int rest = mask ^ lsb;
        independent[mask] = independent[rest] && ((conflict[i] & rest) == 0);
    }

    // Precompute powers of 1/4
    vector<double> pow4(n + 1);
    pow4[0] = 1.0;
    for (int i = 1; i <= n; ++i) {
        pow4[i] = pow4[i - 1] * 0.25;
    }

    double ans = 0.0;
    int mask_all = total_masks - 1;
    for (int mask = 1; mask <= mask_all; ++mask) {
        if (!independent[mask]) continue;
        int comp = mask_all ^ mask;
        int count0 = g[comp];
        int d = n - count0;
        double term = pow4[d];
        if (__builtin_popcount(mask) % 2 == 0) {
            term = -term;
        }
        ans += term;
    }

    // Clamp to [0,1] to avoid floating-point inaccuracies
    if (ans < 0) ans = 0;
    if (ans > 1) ans = 1;

    cout << fixed << setprecision(12) << ans << "\n";
    return 0;
}