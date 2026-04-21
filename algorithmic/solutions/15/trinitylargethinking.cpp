#include <bits/stdc++.h>
using namespace std;

vector<int> apply(const vector<int>& p, int x, int y) {
    int n = p.size();
    // Prefix: indices [0, x)
    // Middle: indices [x, n-y)
    // Suffix: indices [n-y, n)
    vector<int> res;
    res.reserve(n);
    // Suffix
    res.insert(res.end(), p.begin() + (n - y), p.end());
    // Middle
    res.insert(res.end(), p.begin() + x, p.begin() + (n - y));
    // Prefix
    res.insert(res.end(), p.begin(), p.begin() + x);
    return res;
}

bool is_sorted(const vector<int>& p) {
    int n = p.size();
    for (int i = 0; i < n; i++) {
        if (p[i] != i + 1) return false;
    }
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    if (!(cin >> n)) return 0;
    vector<int> p(n);
    for (int i = 0; i < n; i++) cin >> p[i];

    if (n == 3) {
        vector<int> rev = {p[2], p[1], p[0]};
        if (p <= rev) {
            cout << 0 << "\n";
        } else {
            cout << 1 << "\n";
            cout << 1 << " " << 1 << "\n";
        }
        return 0;
    }

    vector<pair<int,int>> ops;
    // For n >= 4, use greedy with operations (1,1), (1,2), (2,1)
    for (int step = 0; step < 4 * n; step++) {
        if (is_sorted(p)) break;
        vector<pair<int,int>> candidates = {{1,1}, {1,2}, {2,1}};
        vector<vector<int>> results;
        results.reserve(3);
        for (auto &op : candidates) {
            results.push_back(apply(p, op.first, op.second));
        }
        int best_idx = 0;
        for (int i = 1; i < 3; i++) {
            if (results[i] < results[best_idx]) best_idx = i;
        }
        auto chosen = candidates[best_idx];
        p = results[best_idx];
        ops.push_back(chosen);
    }

    cout << ops.size() << "\n";
    for (auto &op : ops) {
        cout << op.first << " " << op.second << "\n";
    }
    return 0;
}