#include <bits/stdc++.h>
using namespace std;

const int MAXR = 100;
const int MAXC = 100;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int r, c;
    if (!(cin >> r >> c)) return 0;
    vector<string> grid(r);
    for (int i = 0; i < r; i++) {
        cin >> grid[i];
    }

    int dx[4] = {-1, 0, 1, 0};
    int dy[4] = {0, 1, 0, -1};

    int dist[4][MAXR][MAXC];
    for (int d = 0; d < 4; d++) {
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                if (grid[i][j] != '.') {
                    dist[d][i][j] = -1;
                } else {
                    int steps = 0;
                    int ni = i + dx[d];
                    int nj = j + dy[d];
                    while (ni >= 0 && ni < r && nj >= 0 && nj < c && grid[ni][nj] == '.') {
                        steps++;
                        ni += dx[d];
                        nj += dy[d];
                    }
                    dist[d][i][j] = steps;
                }
            }
        }
    }

    bool possible[4][MAXR][MAXC] = {};
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            if (grid[i][j] == '.') {
                for (int d = 0; d < 4; d++) {
                    possible[d][i][j] = true;
                }
            }
        }
    }

    while (true) {
        int d_obs;
        cin >> d_obs;
        if (d_obs == -1) break;

        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                if (grid[i][j] != '.') continue;
                for (int d = 0; d < 4; d++) {
                    if (possible[d][i][j] && dist[d][i][j] != d_obs) {
                        possible[d][i][j] = false;
                    }
                }
            }
        }

        bool any = false;
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                if (grid[i][j] != '.') continue;
                for (int d = 0; d < 4; d++) {
                    if (possible[d][i][j]) {
                        any = true;
                        break;
                    }
                }
                if (any) break;
            }
            if (any) break;
        }
        if (!any) break;

        int fi = -1, fj = -1;
        bool same = true;
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                if (grid[i][j] != '.') continue;
                for (int d = 0; d < 4; d++) {
                    if (possible[d][i][j]) {
                        if (fi == -1) {
                            fi = i; fj = j;
                        } else if (i != fi || j != fj) {
                            same = false;
                        }
                    }
                }
            }
        }
        if (same) {
            cout << "yes " << fi + 1 << " " << fj + 1 << endl;
            return 0;
        }

        bool all_isolated = true;
        bool has_possible = false;
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                if (grid[i][j] != '.') continue;
                bool cell_possible = false;
                for (int d = 0; d < 4; d++) {
                    if (possible[d][i][j]) {
                        cell_possible = true;
                        break;
                    }
                }
                if (!cell_possible) continue;
                has_possible = true;
                bool isolated = true;
                for (int d = 0; d < 4; d++) {
                    if (dist[d][i][j] > 0) {
                        isolated = false;
                        break;
                    }
                }
                if (!isolated) {
                    all_isolated = false;
                    break;
                }
            }
        }
        if (has_possible && all_isolated) {
            cout << "no" << endl;
            return 0;
        }

        vector<pair<string, int>> candidates;

        int cnt_left[101] = {0};
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                if (grid[i][j] != '.') continue;
                for (int d = 0; d < 4; d++) {
                    if (possible[d][i][j]) {
                        int nd = (d + 3) % 4;
                        int val = dist[nd][i][j];
                        cnt_left[val]++;
                    }
                }
            }
        }
        int worst_left = 0;
        for (int v = 0; v <= 100; v++) if (cnt_left[v] > worst_left) worst_left = v;
        candidates.push_back({"left", worst_left});

        int cnt_right[101] = {0};
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                if (grid[i][j] != '.') continue;
                for (int d = 0; d < 4; d++) {
                    if (possible[d][i][j]) {
                        int nd = (d + 1) % 4;
                        int val = dist[nd][i][j];
                        cnt_right[val]++;
                    }
                }
            }
        }
        int worst_right = 0;
        for (int v = 0; v <= 100; v++) if (cnt_right[v] > worst_right) worst_right = v;
        candidates.push_back({"right", worst_right});

        bool safe_step = true;
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                if (grid[i][j] != '.') continue;
                for (int d = 0; d < 4; d++) {
                    if (possible[d][i][j]) {
                        int ni = i + dx[d];
                        int nj = j + dy[d];
                        if (ni < 0 || ni >= r || nj < 0 || nj >= c || grid[ni][nj] != '.') {
                            safe_step = false;
                            break;
                        }
                    }
                }
                if (!safe_step) break;
            }
            if (!safe_step) break;
        }

        if (safe_step) {
            bool step_possible[4][MAXR][MAXC] = {};
            for (int i = 0; i < r; i++) {
                for (int j = 0; j < c; j++) {
                    if (grid[i][j] != '.') continue;
                    for (int d = 0; d < 4; d++) {
                        if (possible[d][i][j]) {
                            int ni = i + dx[d];
                            int nj = j + dy[d];
                            step_possible[d][ni][nj] = true;
                        }
                    }
                }
            }
            int cnt_step[101] = {0};
            for (int i = 0; i < r; i++) {
                for (int j = 0; j < c; j++) {
                    if (grid[i][j] != '.') continue;
                    for (int d = 0; d < 4; d++) {
                        if (step_possible[d][i][j]) {
                            int val = dist[d][i][j];
                            cnt_step[val]++;
                        }
                    }
                }
            }
            int worst_step = 0;
            for (int v = 0; v <= 100; v++) if (cnt_step[v] > worst_step) worst_step = v;
            candidates.push_back({"step", worst_step});
        }

        pair<string, int> best = candidates[0];
        for (auto &p : candidates) {
            if (p.second < best.second) best = p;
        }
        string action = best.first;

        bool new_possible[4][MAXR][MAXC] = {};
        if (action == "left") {
            for (int i = 0; i < r; i++) {
                for (int j = 0; j < c; j++) {
                    if (grid[i][j] != '.') continue;
                    for (int d = 0; d < 4; d++) {
                        if (possible[d][i][j]) {
                            int nd = (d + 3) % 4;
                            new_possible[nd][i][j] = true;
                        }
                    }
                }
            }
        } else if (action == "right") {
            for (int i = 0; i < r; i++) {
                for (int j = 0; j < c; j++) {
                    if (grid[i][j] != '.') continue;
                    for (int d = 0; d < 4; d++) {
                        if (possible[d][i][j]) {
                            int nd = (d + 1) % 4;
                            new_possible[nd][i][j] = true;
                        }
                    }
                }
            }
        } else if (action == "step") {
            for (int i = 0; i < r; i++) {
                for (int j = 0; j < c; j++) {
                    if (grid[i][j] != '.') continue;
                    for (int d = 0; d < 4; d++) {
                        if (possible[d][i][j]) {
                            int ni = i + dx[d];
                            int nj = j + dy[d];
                            new_possible[d][ni][nj] = true;
                        }
                    }
                }
            }
        }

        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                if (grid[i][j] != '.') continue;
                for (int d = 0; d < 4; d++) {
                    possible[d][i][j] = new_possible[d][i][j];
                }
            }
        }

        cout << action << endl;
    }
    return 0;
}