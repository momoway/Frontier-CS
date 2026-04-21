#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    if (!(cin >> n)) return 0;
    vector<int> a(n);
    for (int i = 0; i < n; ++i) cin >> a[i];

    vector<int> pos(n + 1);
    for (int i = 0; i < n; ++i) pos[a[i]] = i;

    vector<pair<int, int>> ops;

    for (int i = 1; i <= n; ++i) {
        int p0 = pos[i]; // current 0-indexed position
        while (p0 + 1 > i) {
            if ((p0 + 1) - i >= 3) {
                int l1 = (p0 + 1) - 3; // 1-indexed
                int r1 = p0 + 1;
                // reverse segment [l1, r1] (length 4)
                reverse(a.begin() + (l1 - 1), a.begin() + r1);
                for (int idx = l1 - 1; idx < r1; ++idx) {
                    pos[a[idx]] = idx;
                }
                p0 = l1 - 1; // new 0-indexed position of i
                ops.push_back({l1, r1});
            } else {
                if ((p0 + 1) - i == 1) {
                    int l = i;
                    int r = i + 1;
                    reverse(a.begin() + (l - 1), a.begin() + r);
                    for (int idx = l - 1; idx < r; ++idx) {
                        pos[a[idx]] = idx;
                    }
                    p0 = i - 1;
                    ops.push_back({l, r});
                } else if ((p0 + 1) - i == 2) {
                    // first swap [i+1, i+2]
                    int l1 = i + 1;
                    int r1 = i + 2;
                    reverse(a.begin() + (l1 - 1), a.begin() + r1);
                    for (int idx = l1 - 1; idx < r1; ++idx) {
                        pos[a[idx]] = idx;
                    }
                    p0 = i; // now at position i+1 (1-indexed) -> 0-indexed i
                    ops.push_back({l1, r1});
                    // second swap [i, i+1]
                    int l2 = i;
                    int r2 = i + 1;
                    reverse(a.begin() + (l2 - 1), a.begin() + r2);
                    for (int idx = l2 - 1; idx < r2; ++idx) {
                        pos[a[idx]] = idx;
                    }
                    p0 = i - 1;
                    ops.push_back({l2, r2});
                }
            }
        }
    }

    cout << 3 << "\n";
    cout << ops.size() << "\n";
    for (auto &op : ops) {
        cout << op.first << " " << op.second << "\n";
    }
    return 0;
}