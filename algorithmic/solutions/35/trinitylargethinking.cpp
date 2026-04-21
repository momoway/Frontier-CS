#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Precompute all 12-bit numbers with exactly 6 bits set
    vector<int> all_codes;
    for (int c = 0; c < (1 << 12); ++c) {
        if (__builtin_popcount(c) == 6) {
            all_codes.push_back(c);
        }
    }

    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;
        if (n == -1) return 0;
        int N = 2 * n - 1;

        // Assign distinct codes to indices
        vector<int> code_of_index(N + 1);
        for (int i = 1; i <= N; ++i) {
            code_of_index[i] = all_codes[i - 1];
        }

        // Build subsets for each bit (0..11)
        vector<vector<int>> subsets(12);
        for (int i = 1; i <= N; ++i) {
            int c = code_of_index[i];
            for (int b = 0; b < 12; ++b) {
                if (c & (1 << b)) subsets[b].push_back(i);
            }
        }

        // Query each number
        vector<vector<int>> ans(n + 1, vector<int>(12, 0));
        for (int x = 1; x <= n; ++x) {
            for (int b = 0; b < 12; ++b) {
                int sz = subsets[b].size();
                cout << "? " << x << " " << sz;
                for (int idx : subsets[b]) cout << " " << idx;
                cout << endl;
                int resp;
                cin >> resp;
                if (resp == -1) return 0;
                ans[x][b] = resp;
            }
        }

        // Find unique number (answer vector weight = 6)
        int unique_num = -1;
        for (int x = 1; x <= n; ++x) {
            int weight = 0;
            for (int b = 0; b < 12; ++b) weight += ans[x][b];
            if (weight == 6) {
                unique_num = x;
                break;
            }
        }

        cout << "! " << unique_num << endl;
        cout.flush();
    }
    return 0;
}