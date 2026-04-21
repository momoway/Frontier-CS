#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int t;
    if (!(cin >> t)) return 0;
    while (t--) {
        int n, m;
        cin >> n >> m;
        vector<vector<int>> G(n, vector<int>(n));
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                cin >> G[i][j];
            }
        }
        int N = 2 * n - 1;
        vector<int> A(N + 1);
        // A[1] and A[2] are known from grid
        A[1] = G[0][0];
        A[2] = G[1][0];
        // Query the rest of the head values for the length-1 snake
        for (int T = 3; T <= N; ++T) {
            cout << "? 1 " << T << endl;
            cout.flush();
            cin >> A[T];
        }
        // First row values
        vector<int> R(n + 1);
        for (int j = 1; j <= n; ++j) R[j] = G[0][j - 1];
        // Prefix max for R
        vector<int> prefR(n + 1, 0);
        for (int i = 1; i <= n; ++i) prefR[i] = max(prefR[i - 1], R[i]);
        // Precompute maxInRange for A
        vector<vector<int>> maxInRange(N + 1, vector<int>(N + 1, 0));
        for (int i = 1; i <= N; ++i) maxInRange[i][i] = A[i];
        for (int len = 2; len <= N; ++len) {
            for (int i = 1; i <= N - len + 1; ++i) {
                int j = i + len - 1;
                maxInRange[i][j] = max(maxInRange[i][j - 1], A[j]);
            }
        }
        auto queryA = [&](int l, int r) {
            return maxInRange[l][r];
        };
        // Compute all f(l,T)
        vector<int> all;
        all.reserve(n * N);
        for (int l = 1; l <= n; ++l) {
            for (int T = 1; T <= N; ++T) {
                int fval;
                if (T == 1) {
                    fval = prefR[l];
                } else if (T == 2) {
                    if (l == 1) {
                        fval = A[2];
                    } else {
                        fval = max(A[2], prefR[l - 1]);
                    }
                } else { // T >= 3
                    if (T < l) {
                        int maxHead = queryA(2, T);
                        int initLen = l - (T - 1);
                        int maxInit = prefR[initLen];
                        fval = max(maxHead, maxInit);
                    } else { // T >= l
                        int L = T - l + 1;
                        int Rbound = T;
                        fval = queryA(L, Rbound);
                    }
                }
                all.push_back(fval);
            }
        }
        sort(all.begin(), all.end());
        cout << "! ";
        for (int i = 0; i < m; ++i) {
            if (i) cout << ' ';
            cout << all[i];
        }
        cout << endl;
        cout.flush();
    }
    return 0;
}