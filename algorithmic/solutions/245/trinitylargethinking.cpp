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
        // spare = player 1
        const int spare = 1;
        vector<int> pool;
        pool.reserve(n-1);
        for (int i = 2; i <= n; ++i) pool.push_back(i);
        bool foundAsymmetric = false;
        int cand1 = -1, cand2 = -1;
        int aij = -1, aji = -1;
        while (pool.size() >= 2) {
            int i = pool.front(); pool.erase(pool.begin());
            int j = pool.front(); pool.erase(pool.begin());
            cout << "? " << i << " " << j << endl;
            cout.flush();
            cin >> aij;
            cout << "? " << j << " " << i << endl;
            cout.flush();
            cin >> aji;
            if (aij == aji) {
                // symmetric, both non-Impostor, discard
            } else {
                cand1 = i;
                cand2 = j;
                foundAsymmetric = true;
                break;
            }
        }
        if (!foundAsymmetric) {
            if (!pool.empty()) {
                // the remaining player in pool is the Impostor
                cout << "! " << pool[0] << endl;
                cout.flush();
            } else {
                // pool empty, Impostor is spare
                cout << "! " << spare << endl;
                cout.flush();
            }
        } else {
            // use spare as reference
            int r = spare;
            int ar1, ar2;
            cout << "? " << r << " " << cand1 << endl;
            cout.flush();
            cin >> ar1;
            cout << "? " << r << " " << cand2 << endl;
            cout.flush();
            cin >> ar2;
            // determine Impostor based on aij, aji and ar1, ar2
            if (aij == 1 && aji == 0) {
                // (cand1=K, cand2=I) or (cand1=I, cand2=V)
                if (ar1 == ar2) {
                    // cand1=K, cand2=I -> cand2 is Impostor
                    cout << "! " << cand2 << endl;
                } else {
                    cout << "! " << cand1 << endl;
                }
            } else if (aij == 0 && aji == 1) {
                // (cand1=V, cand2=I) or (cand1=I, cand2=K)
                if (ar1 == ar2) {
                    // cand1=I, cand2=K -> cand1 is Impostor
                    cout << "! " << cand1 << endl;
                } else {
                    cout << "! " << cand2 << endl;
                }
            } else {
                // Should not happen, but output something
                cout << "! " << cand1 << endl;
            }
            cout.flush();
        }
    }
    return 0;
}