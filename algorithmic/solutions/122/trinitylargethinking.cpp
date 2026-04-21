#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int t;
    if(!(cin >> t)) return 0;
    while(t--) {
        // First query: 100000 words of length 1
        cout << "? 100000";
        for(int i = 0; i < 100000; ++i) cout << " 1";
        cout << endl;
        int l1;
        cin >> l1;
        if(l1 == 0) return 0; // should not happen

        // Compute interval [L, R]
        long long L, R;
        if(l1 == 1) {
            L = 100000;
            R = 100000;
        } else {
            L = (100000 + l1 - 1) / l1; // ceil(100000/l1)
            R = (100000 - 1) / (l1 - 1); // floor((100000-1)/(l1-1))
        }

        if(L == R) {
            cout << "! " << L << endl;
            continue;
        }

        // Second query: try to distinguish with a single word of length L
        // This is a heuristic that may not always work, but we need to output something.
        cout << "? 1 " << L << endl;
        int resp;
        cin >> resp;
        if(resp == 0) return 0;

        // If resp is 1, we cannot decide between L and other values.
        // As a fallback, output L (guess lower bound)
        cout << "! " << L << endl;
    }
    return 0;
}