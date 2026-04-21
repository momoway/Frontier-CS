#include <bits/stdc++.h>
using namespace std;

const int MAXN = 50;
const int MAXM = 100;

int n, m;
int cur_color[MAXN][MAXN];
int grid[MAXN][MAXN];

vector<pair<int,int>> cells[MAXM+1];
int pos_idx[MAXN][MAXN];

int edge_count[MAXM+1][MAXM+1];
bool adj_orig[MAXM+1][MAXM+1];
bool inA0[MAXM+1];

bool artic_mark[MAXN][MAXN];

int disc[MAXN][MAXN];
int low[MAXN][MAXN];
int parent_i[MAXN][MAXN], parent_j[MAXN][MAXN];
int vis_time[MAXN][MAXN];
int cur_timestamp = 1;

const int di[4] = {-1,1,0,0};
const int dj[4] = {0,0,-1,1};

void dfs(int i, int j, int c, int &time) {
    vis_time[i][j] = cur_timestamp;
    disc[i][j] = low[i][j] = ++time;
    int children = 0;
    for (int dir = 0; dir < 4; ++dir) {
        int ni = i + di[dir];
        int nj = j + dj[dir];
        if (ni < 0 || ni >= n || nj < 0 || nj >= n) continue;
        if (cur_color[ni][nj] != c) continue;
        if (vis_time[ni][nj] != cur_timestamp) {
            parent_i[ni][nj] = i;
            parent_j[ni][nj] = j;
            children++;
            dfs(ni, nj, c, time);
            low[i][j] = min(low[i][j], low[ni][nj]);
            if (parent_i[i][j] != -1 && low[ni][nj] >= disc[i][j]) {
                artic_mark[i][j] = true;
            }
        } else {
            if (parent_i[i][j] != ni || parent_j[i][j] != nj) {
                low[i][j] = min(low[i][j], disc[ni][nj]);
            }
        }
    }
    if (parent_i[i][j] == -1 && children > 1) {
        artic_mark[i][j] = true;
    }
}

bool can_remove(int i, int j) {
    int c = cur_color[i][j];
    if (c == 0) return false;
    if (cells[c].size() <= 1) return false;
    if (artic_mark[i][j]) return false;

    bool on_boundary = (i == 0 || i == n-1 || j == 0 || j == n-1);
    bool adj_zero = false;
    if (!on_boundary) {
        for (int dir = 0; dir < 4; ++dir) {
            int ni = i + di[dir];
            int nj = j + dj[dir];
            if (ni < 0 || ni >= n || nj < 0 || nj >= n) continue;
            if (cur_color[ni][nj] == 0) {
                adj_zero = true;
                break;
            }
        }
    }
    if (!on_boundary && !adj_zero) return false;

    int num_zero_neighbors = 0;
    int num_c_neighbors = 0;
    for (int dir = 0; dir < 4; ++dir) {
        int ni = i + di[dir];
        int nj = j + dj[dir];
        if (ni < 0 || ni >= n || nj < 0 || nj >= n) continue;
        int d = cur_color[ni][nj];
        if (d == c) {
            num_c_neighbors++;
            continue;
        }
        if (d == 0) {
            num_zero_neighbors++;
            continue;
        }
        if (!adj_orig[0][d]) return false;
        if (edge_count[c][d] < 2) return false;
    }

    int old_edge_c0 = edge_count[0][c];
    int new_edge_c0 = old_edge_c0 - num_zero_neighbors + num_c_neighbors;
    if (adj_orig[0][c]) {
        if (new_edge_c0 < 1) return false;
    } else {
        if (num_c_neighbors > 0) return false;
        if (num_zero_neighbors > 0) return false;
    }
    return true;
}

void remove_cell(int i, int j) {
    int c = cur_color[i][j];
    int idx = pos_idx[i][j];
    auto last_cell = cells[c].back();
    cells[c][idx] = last_cell;
    pos_idx[last_cell.first][last_cell.second] = idx;
    cells[c].pop_back();
    pos_idx[i][j] = -1;

    for (int dir = 0; dir < 4; ++dir) {
        int ni = i + di[dir];
        int nj = j + dj[dir];
        if (ni < 0 || ni >= n || nj < 0 || nj >= n) continue;
        int d = cur_color[ni][nj];
        if (d == c) continue;
        int a = min(c, d), b = max(c, d);
        edge_count[a][b]--;
    }

    cur_color[i][j] = 0;

    for (int dir = 0; dir < 4; ++dir) {
        int ni = i + di[dir];
        int nj = j + dj[dir];
        if (ni < 0 || ni >= n || nj < 0 || nj >= n) continue;
        int d = cur_color[ni][nj];
        if (d == 0) continue;
        edge_count[0][d]++;
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> n >> m;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cin >> grid[i][j];
            cur_color[i][j] = grid[i][j];
        }
    }

    for (int i = 0; i <= m; ++i)
        for (int j = 0; j <= m; ++j)
            adj_orig[i][j] = false;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int c = cur_color[i][j];
            if (i == 0 || i == n-1 || j == 0 || j == n-1)
                adj_orig[0][c] = true;
            for (int dir = 0; dir < 4; ++dir) {
                int ni = i + di[dir];
                int nj = j + dj[dir];
                if (ni < 0 || ni >= n || nj < 0 || nj >= n) continue;
                int d = cur_color[ni][nj];
                if (d == c) continue;
                adj_orig[min(c,d)][max(c,d)] = true;
            }
        }
    }

    for (int i = 0; i <= m; ++i)
        for (int j = 0; j <= m; ++j)
            edge_count[i][j] = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int c = cur_color[i][j];
            for (int dir = 0; dir < 4; ++dir) {
                int ni = i + di[dir];
                int nj = j + dj[dir];
                if (ni < 0 || ni >= n || nj < 0 || nj >= n) continue;
                int d = cur_color[ni][nj];
                if (d == c) continue;
                if (c < d) edge_count[c][d]++;
            }
        }
    }

    for (int c = 1; c <= m; ++c)
        inA0[c] = adj_orig[0][c];

    for (int c = 0; c <= m; ++c)
        cells[c].clear();

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int c = cur_color[i][j];
            cells[c].push_back({i, j});
            pos_idx[i][j] = (int)cells[c].size() - 1;
        }
    }

    while (true) {
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                artic_mark[i][j] = false;

        for (int c = 1; c <= m; ++c) {
            if (cells[c].empty()) continue;
            cur_timestamp++;
            int root_i = cells[c][0].first;
            int root_j = cells[c][0].second;
            parent_i[root_i][root_j] = -1;
            parent_j[root_i][root_j] = -1;
            int time = 0;
            dfs(root_i, root_j, c, time);
        }

        bool progress = false;
        for (int i = 0; i < n && !progress; ++i) {
            for (int j = 0; j < n && !progress; ++j) {
                if (can_remove(i, j)) {
                    remove_cell(i, j);
                    progress = true;
                }
            }
        }
        if (!progress) break;
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (j) cout << ' ';
            cout << cur_color[i][j];
        }
        cout << '\n';
    }
    return 0;
}