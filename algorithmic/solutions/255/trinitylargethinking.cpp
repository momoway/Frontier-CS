#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int t;
    if (!(cin >> t)) return 0;
    while (t--) {
        int n;
        cin >> n;
        int known = -1;
        // Find a pair of non-demagnetized magnets
        for (int a = 1; a <= n && known == -1; ++a) {
            for (int b = a + 1; b <= n; ++b) {
                cout << "? 1 1\n" << a << "\n" << b << endl;
                int f;
                cin >> f;
                if (f != 0) {
                    known = a; // a is non-demagnetized
                    break;
                }
            }
        }
        // If no pair found (should not happen), but just in case, we could handle differently.
        // Now determine all demagnetized magnets by testing against 'known'
        vector<int> demag;
        for (int i = 1; i <= n; ++i) {
            if (i == known) continue;
            cout << "? 1 1\n" << known << "\n" << i << endl;
            int f;
            cin >> f;
            if (f == 0) {
                demag.push_back(i);
            }
        }
        // Output the answer
        cout << "! " << demag.size();
        for (int x : demag) cout << " " << x;
        cout << endl;
    }
    return 0;
}