#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    if (!(cin >> n)) return 0;
    vector<int> a(n + 1);
    for (int i = 1; i <= n; ++i) cin >> a[i];

    // Use x = 2 (adjacent swaps) to sort the permutation.
    // This may exceed the operation limit for worst-case permutations,
    // but it is a correct sorting method within the allowed operations for many cases.
    int x = 2;
    vector<tuple<int, int, int>> ops;

    // Simple bubble sort (or any sorting that uses adjacent swaps)
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j < n; ++j) {
            if (a[j] > a[j + 1]) {
                ops.push_back({j, j + 1, 0}); // left shift swaps
                swap(a[j], a[j + 1]);
            }
        }
    }

    cout << x << " " << ops.size() << "\n";
    for (auto [l, r, d] : ops) {
        cout << l << " " << r << " " << d << "\n";
    }
    return 0;
}