#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T;
    cin >> T;
    while (T--) {
        long long n;
        cin >> n;
        bool found = false;
        for (int i = 1; i <= 500 && i <= n; ++i) {
            int j = i + 2;
            if (j > n) j -= n;
            cout << "? " << i << " " << j << endl;
            cout.flush();
            long long dist;
            cin >> dist;
            if (dist == 1) {
                cout << "! " << i << " " << j << endl;
                cout.flush();
                int res;
                cin >> res;
                if (res == 1) {
                    found = true;
                    break;
                } else {
                    return 0;
                }
            }
        }
        if (!found) {
            int u = 1;
            int v = n/2 + 1;
            if (v > n) v -= n;
            cout << "! " << u << " " << v << endl;
            cout.flush();
            int res;
            cin >> res;
            if (res == -1) return 0;
        }
    }
    return 0;
}