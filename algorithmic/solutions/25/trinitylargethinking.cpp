#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T;
    if (!(cin >> T)) return 0;
    while (T--) {
        int n;
        cin >> n;
        vector<char> inC(n, 0);
        inC[0] = 1;
        int curC = 1;
        bool done = false;
        string sC(n, '0');
        // initial sC: only vertex 0 is in C
        sC[0] = '1';
        while (!done) {
            // query current C
            cout << "? " << sC << endl;
            cout.flush();
            int f;
            cin >> f;
            if (curC == n) {
                // all vertices are in C, graph is connected
                cout << "! 1" << endl;
                cout.flush();
                done = true;
                break;
            }
            if (f == 0) {
                // no edge from C to outside, disconnected
                cout << "! 0" << endl;
                cout.flush();
                done = true;
                break;
            }
            // f > 0, need to find a vertex in U adjacent to C
            vector<int> candidates;
            for (int i = 0; i < n; ++i) if (!inC[i]) candidates.push_back(i);
            int m = candidates.size();
            if (m == 1) {
                int u = candidates[0];
                inC[u] = 1;
                sC[u] = '1';
                curC++;
                continue;
            }
            // binary search on candidates
            while (candidates.size() > 1) {
                int sz = candidates.size();
                int half = sz / 2;
                vector<int> L(candidates.begin(), candidates.begin() + half);
                vector<int> R(candidates.begin() + half, candidates.end());
                // query L
                string sL(n, '0');
                for (int v : L) sL[v] = '1';
                cout << "? " << sL << endl;
                cout.flush();
                int respL;
                cin >> respL;
                // query C ∪ L
                string sCL = sC;
                for (int v : L) sCL[v] = '1';
                cout << "? " << sCL << endl;
                cout.flush();
                int respCL;
                cin >> respCL;
                int aL = respL - respCL;
                if (aL > 0) {
                    candidates = L;
                } else {
                    candidates = R;
                }
            }
            int u = candidates[0];
            inC[u] = 1;
            sC[u] = '1';
            curC++;
        }
    }
    return 0;
}