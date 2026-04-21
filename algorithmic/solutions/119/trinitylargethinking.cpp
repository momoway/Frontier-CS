#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    if (!(cin >> n)) return 0;
    vector<int> op(n + 1, 0); // 1-indexed: op[1..n], 0 for '+', 1 for '*'

    for (int i = 1; i <= n; ++i) {
        if (i == n) {
            // Query for op_n: a0=0, a1..a_{n-1}=0, a_n=1
            cout << "? ";
            for (int j = 0; j < n; ++j) cout << 0 << " ";
            cout << 1 << endl;
            int res;
            cin >> res;
            op[i] = (res == 1) ? 0 : 1;
        } else if (i == n - 1) {
            // Query for op_{n-1}: a0=0, a1..a_{n-2}=0, a_{n-1}=2, a_n=1
            cout << "? ";
            cout << 0 << " ";
            for (int j = 1; j <= n - 2; ++j) cout << 0 << " ";
            cout << 2 << " " << 1 << endl;
            int res;
            cin >> res;
            op[i] = (res >= 2) ? 0 : 1;
        } else {
            // i <= n-2
            int m = n - i - 1; // number of operators after i+1
            cout << "? ";
            cout << 0 << " "; // a0
            for (int j = 1; j <= i - 1; ++j) cout << 0 << " ";
            int X = n - i; // a_i
            cout << X << " ";
            cout << 2 << " "; // a_{i+1}
            for (int j = i + 2; j <= n; ++j) cout << 1 << " ";
            cout << endl;
            int res;
            cin >> res;
            int threshold = n - i + 2;
            op[i] = (res >= threshold) ? 0 : 1;
        }
    }

    cout << "! ";
    for (int i = 1; i <= n; ++i) {
        cout << op[i] << (i == n ? "\n" : " ");
    }
    cout << flush;
    return 0;
}