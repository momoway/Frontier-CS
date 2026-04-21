#include <bits/stdc++.h>
using namespace std;
int main() {
    int n = 20, m = 20;
    vector<string> grid(n, string(m, '0'));
    int backbone = 9;
    for (int j = 0; j < m; ++j) {
        grid[backbone][j] = '1';
    }
    for (int j = 0; j < m; ++j) {
        if (j % 2 == 1) { // odd column
            grid[backbone-1][j] = '1';
            for (int i = backbone-2; i >= 0; --i) {
                grid[i][j] = '1';
            }
        } else { // even column
            grid[backbone+1][j] = '1';
            for (int i = backbone+2; i < n; ++i) {
                grid[i][j] = '1';
            }
        }
    }
    cout << n << " " << m << "\n";
    for (int i = 0; i < n; ++i) {
        cout << grid[i] << "\n";
    }
    return 0;
}