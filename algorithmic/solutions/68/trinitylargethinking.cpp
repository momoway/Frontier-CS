#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T;
    if(!(cin >> T)) return 0;
    while (T--) {
        int n;
        cin >> n;
        int t = (n - 3) / 2; // floor
        vector<int> dead(n, 0);
        vector<int> alive;
        // Perform t rounds of universal queries
        for (int round = 0; round < t; ++round) {
            for (int i = 0; i < n; ++i) {
                if (dead[i]) continue; // already dead, but we still query? Actually we query all each round regardless.
                cout << "0 " << i << endl;
                cout.flush();
                int x;
                cin >> x;
                if (x == 0) {
                    dead[i] = 1;
                }
            }
        }
        // Collect pens that are still alive (never returned 0)
        alive.clear();
        for (int i = 0; i < n; ++i) {
            if (!dead[i]) alive.push_back(i);
        }
        if (n % 2 == 1) {
            // For odd n, any two alive pens work
            cout << "1 " << alive[0] << " " << alive[1] << endl;
            cout.flush();
        } else {
            // For even n, perform one more round to identify the pen with initial value = t+1
            int p1 = -1;
            for (int i : alive) {
                cout << "0 " << i << endl;
                cout.flush();
                int x;
                cin >> x;
                if (x == 0) {
                    p1 = i; // this pen becomes empty, it's the smallest among alive (value t+1)
                }
            }
            // Remove p1 from alive (if found)
            vector<int> alive2;
            for (int i : alive) {
                if (i != p1) alive2.push_back(i);
            }
            // Output any two from alive2
            cout << "1 " << alive2[0] << " " << alive2[1] << endl;
            cout.flush();
        }
    }
    return 0;
}