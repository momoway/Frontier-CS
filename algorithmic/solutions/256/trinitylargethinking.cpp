#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    cin >> n;
    vector<vector<int>> val(n+1, vector<int>(n+1, -1));
    vector<vector<int>> base(n+1, vector<int>(n+1, -1));
    // Set top-left corner
    val[1][1] = 1;

    // Even BFS: propagate using forward moves (i+2,j), (i,j+2), (i+1,j+1)
    queue<pair<int,int>> qe;
    qe.push({1,1});
    while (!qe.empty()) {
        auto [i,j] = qe.front(); qe.pop();
        // forward moves
        const int dirs[3][2] = {{2,0},{0,2},{1,1}};
        for (auto &d : dirs) {
            int ni = i + d[0];
            int nj = j + d[1];
            if (ni <= n && nj <= n && val[ni][nj] == -1) {
                cout << "? " << i << " " << j << " " << ni << " " << nj << endl;
                cout.flush();
                int ans;
                cin >> ans;
                if (ans == -1) return 0;
                if (ans == 1) val[ni][nj] = val[i][j];
                else val[ni][nj] = 1 - val[i][j];
                qe.push({ni,nj});
            }
        }
    }

    // Find minimal odd cells: those with no strictly smaller odd cell
    vector<pair<int,int>> minimal_odds;
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= n; ++j) {
            if ((i+j) % 2 == 1) {
                bool minimal = true;
                for (int ii = 1; ii <= i; ++ii) {
                    for (int jj = 1; jj <= j; ++jj) {
                        if ((ii+jj) % 2 == 1 && !(ii == i && jj == j) && ii <= i && jj <= j && (ii < i || jj < j)) {
                            minimal = false;
                            break;
                        }
                    }
                    if (!minimal) break;
                }
                if (minimal) {
                    base[i][j] = 0;
                    minimal_odds.push_back({i,j});
                }
            }
        }
    }

    // Odd BFS: propagate using all distance-2 moves, but only assign when current cell is smaller than neighbor
    queue<pair<int,int>> qo;
    for (auto &p : minimal_odds) {
        qo.push(p);
    }
    while (!qo.empty()) {
        auto [i,j] = qo.front(); qo.pop();
        // all eight distance-2 neighbors
        const int dirs[8][2] = {{2,0},{-2,0},{0,2},{0,-2},{1,1},{-1,-1},{1,-1},{-1,1}};
        for (auto &d : dirs) {
            int ni = i + d[0];
            int nj = j + d[1];
            if (ni < 1 || ni > n || nj < 1 || nj > n) continue;
            if ((ni+nj) % 2 != 1) continue; // must be odd
            if (base[ni][nj] != -1) continue; // already known
            // Only assign if (i,j) is strictly smaller than (ni,nj)
            if (i <= ni && j <= nj && (i < ni || j < nj)) {
                cout << "? " << i << " " << j << " " << ni << " " << nj << endl;
                cout.flush();
                int ans;
                cin >> ans;
                if (ans == -1) return 0;
                if (ans == 1) base[ni][nj] = base[i][j];
                else base[ni][nj] = 1 - base[i][j];
                qo.push({ni,nj});
            }
        }
    }

    // Step 3: Find a distinguishing query among distance-3 pairs (even, odd)
    int flip = -1;
    bool found = false;
    for (int i = 1; i <= n && !found; ++i) {
        for (int j = 1; j <= n && !found; ++j) {
            if ((i+j) % 2 != 0) continue; // must be even
            if (val[i][j] == -1) continue; // should be known
            // Type A: horizontal right (i, j+3)
            if (j+3 <= n) {
                int ni = i, nj = j+3;
                int base_B = base[ni][nj];
                int mid_odd = base[i][j+1];
                int mid_even = val[i][j+2];
                int a = val[i][j];
                bool ans0 = (a == base_B) && (mid_odd == mid_even);
                bool ans1 = (a == (1 - base_B)) && ((mid_odd ^ 1) == mid_even);
                if (ans0 != ans1) {
                    cout << "? " << i << " " << j << " " << ni << " " << nj << endl;
                    cout.flush();
                    int ans; cin >> ans;
                    if (ans == -1) return 0;
                    if (ans == ans0) flip = 0;
                    else flip = 1;
                    found = true;
                    break;
                }
            }
            // Type B: vertical down (i+3, j)
            if (i+3 <= n) {
                int ni = i+3, nj = j;
                int base_B = base[ni][nj];
                int mid_odd = base[i+1][j];
                int mid_even = val[i+2][j];
                int a = val[i][j];
                bool ans0 = (a == base_B) && (mid_odd == mid_even);
                bool ans1 = (a == (1 - base_B)) && ((mid_odd ^ 1) == mid_even);
                if (ans0 != ans1) {
                    cout << "? " << i << " " << j << " " << ni << " " << nj << endl;
                    cout.flush();
                    int ans; cin >> ans;
                    if (ans == -1) return 0;
                    if (ans == ans0) flip = 0;
                    else flip = 1;
                    found = true;
                    break;
                }
            }
            // Type C: dx=1, dy=2 (i+1, j+2)
            if (i+1 <= n && j+2 <= n) {
                int ni = i+1, nj = j+2;
                int base_B = base[ni][nj];
                int base_mid1 = base[i+1][j]; // odd
                int me1 = val[i+1][j+1];
                int base_mid2 = base[i][j+1]; // odd (same for path2 and path3)
                int me2 = val[i+1][j+1];
                int me3 = val[i][j+2];
                int a = val[i][j];
                bool cond1 = (base_mid1 == me1);
                bool cond2 = (base_mid2 == me2);
                bool cond3 = (base_mid2 == me3);
                bool ans0 = (a == base_B) && (cond1 || cond2 || cond3);
                bool cond1_1 = ((base_mid1 ^ 1) == me1);
                bool cond2_1 = ((base_mid2 ^ 1) == me2);
                bool cond3_1 = ((base_mid2 ^ 1) == me3);
                bool ans1 = (a == (1 - base_B)) && (cond1_1 || cond2_1 || cond3_1);
                if (ans0 != ans1) {
                    cout << "? " << i << " " << j << " " << ni << " " << nj << endl;
                    cout.flush();
                    int ans; cin >> ans;
                    if (ans == -1) return 0;
                    if (ans == ans0) flip = 0;
                    else flip = 1;
                    found = true;
                    break;
                }
            }
            // Type D: dx=2, dy=1 (i+2, j+1)
            if (i+2 <= n && j+1 <= n) {
                int ni = i+2, nj = j+1;
                int base_B = base[ni][nj];
                int base_mid1 = base[i+1][j]; // odd
                int me1 = val[i+2][j];
                int base_mid2 = base[i+1][j]; // same odd
                int me2 = val[i+1][j+1];
                int base_mid3 = base[i][j+1]; // odd
                int me3 = val[i+1][j+1];
                int a = val[i][j];
                bool cond1_0 = (base_mid1 == me1);
                bool cond2_0 = (base_mid1 == me2);
                bool cond3_0 = (base_mid3 == me3);
                bool ans0 = (a == base_B) && (cond1_0 || cond2_0 || cond3_0);
                bool cond1_1 = ((base_mid1 ^ 1) == me1);
                bool cond2_1 = ((base_mid1 ^ 1) == me2);
                bool cond3_1 = ((base_mid3 ^ 1) == me3);
                bool ans1 = (a == (1 - base_B)) && (cond1_1 || cond2_1 || cond3_1);
                if (ans0 != ans1) {
                    cout << "? " << i << " " << j << " " << ni << " " << nj << endl;
                    cout.flush();
                    int ans; cin >> ans;
                    if (ans == -1) return 0;
                    if (ans == ans0) flip = 0;
                    else flip = 1;
                    found = true;
                    break;
                }
            }
        }
    }

    // If not found, something is wrong; but we assume found.
    if (flip == -1) {
        // fallback: set flip=0 arbitrarily
        flip = 0;
    }

    // Apply flip to odd cells
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= n; ++j) {
            if ((i+j) % 2 == 1) {
                if (base[i][j] != -1) {
                    val[i][j] = base[i][j] ^ flip;
                } else {
                    // Should not happen; assign 0
                    val[i][j] = 0;
                }
            }
        }
    }

    // Output result
    cout << "!" << endl;
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= n; ++j) {
            cout << val[i][j];
        }
        cout << endl;
    }
    cout.flush();
    return 0;
}