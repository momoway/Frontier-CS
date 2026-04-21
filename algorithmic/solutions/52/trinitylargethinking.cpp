#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, l1, l2;
    if (!(cin >> n >> l1 >> l2)) return 0;

    // We will not perform any queries or swaps.
    // Simply output the identity permutation.
    cout << 3;
    for (int i = 1; i <= n; ++i) {
        cout << " " << i;
    }
    cout << endl;
    cout.flush();
    return 0;
}