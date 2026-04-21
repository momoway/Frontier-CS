#include <bits/stdc++.h>
using namespace std;

const int MAXN = 2005;
int n;
int bit[MAXN][MAXN];
char M[MAXN][MAXN]; // M[i][j] for i<j

void bit_update(int x, int y, int val) {
    for (int i = x; i <= n; i += i & -i)
        for (int j = y; j <= n; j += j & -j)
            bit[i][j] ^= val;
}

int bit_query(int x, int y) {
    int res = 0;
    for (int i = x; i > 0; i -= i & -i)
        for (int j = y; j > 0; j -= j & -j)
            res ^= bit[i][j];
    return res;
}

int rect_query(int x1, int y1, int x2, int y2) {
    int res = bit_query(x2, y2);
    res ^= bit_query(x1-1, y2);
    res ^= bit_query(x2, y1-1);
    res ^= bit_query(x1-1, y1-1);
    return res;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> n;

    // Process all pairs (i,j) with i<j
    for (int d = 1; d <= n-1; ++d) {
        for (int i = 1; i <= n-d; ++i) {
            int j = i + d;
            cout << "0 " << i << " " << j << endl;
            cout.flush();
            int P;
            cin >> P;
            int T = rect_query(i, i, j, j);
            int Mval = P ^ T;
            M[i][j] = (char)Mval;
            bit_update(i, j, Mval);
        }
    }

    // Reconstruct permutation from comparison matrix M
    vector<int> idx(n);
    iota(idx.begin(), idx.end(), 1);
    sort(idx.begin(), idx.end(), [&](int a, int b) {
        if (a == b) return false;
        if (a > b) {
            bool less = (M[b][a] == 0);
            return !less;
        } else {
            return M[a][b] == 0;
        }
    });

    vector<int> ans(n+1);
    for (int r = 0; r < n; ++r) {
        ans[idx[r]] = r+1;
    }

    cout << "1 ";
    for (int i = 1; i <= n; ++i) {
        cout << ans[i] << (i == n ? '\n' : ' ');
    }
    cout.flush();
    return 0;
}