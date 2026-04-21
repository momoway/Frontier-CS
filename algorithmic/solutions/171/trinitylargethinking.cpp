#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, M;
    if (!(cin >> N >> M)) return 0;
    int si, sj;
    cin >> si >> sj;
    vector<pair<int,int>> targets(M);
    for (int i = 0; i < M; ++i) {
        cin >> targets[i].first >> targets[i].second;
    }

    // Map target positions to their indices
    vector<vector<int>> target_idx(N, vector<int>(N, -1));
    for (int i = 0; i < M; ++i) {
        target_idx[targets[i].first][targets[i].second] = i;
    }

    int ci = si, cj = sj;
    vector<string> actions;
    const int MAX_ACTIONS = 2 * N * M;
    int total_actions = 0;

    // Directions: U, D, L, R
    const int dr[4] = {-1, 1, 0, 0};
    const int dc[4] = {0, 0, -1, 1};
    const char dchar[4] = {'U', 'D', 'L', 'R'};

    for (int k = 0; k < M; ++k) {
        int ti = targets[k].first;
        int tj = targets[k].second;

        // BFS to find shortest path from (ci,cj) to (ti,tj) avoiding future targets
        bool visited[N][N];
        for (int i = 0; i < N; ++i) for (int j = 0; j < N; ++j) visited[i][j] = false;
        pair<int,int> parent[N][N];
        char dir[N][N];
        for (int i = 0; i < N; ++i) for (int j = 0; j < N; ++j) parent[i][j] = {-1,-1};

        queue<pair<int,int>> q;
        visited[ci][cj] = true;
        parent[ci][cj] = {ci,cj};
        q.push({ci,cj});

        bool found = false;
        while (!q.empty() && !found) {
            auto [r, c] = q.front(); q.pop();
            if (r == ti && c == tj) {
                found = true;
                break;
            }
            for (int d = 0; d < 4; ++d) {
                int nr = r + dr[d];
                int nc = c + dc[d];
                if (nr < 0 || nr >= N || nc < 0 || nc >= N) continue;
                if (visited[nr][nc]) continue;
                // Forbid cells that are future targets (index > k)
                if (target_idx[nr][nc] > k) continue;
                visited[nr][nc] = true;
                parent[nr][nc] = {r, c};
                dir[nr][nc] = dchar[d];
                q.push({nr, nc});
            }
        }

        if (!found) {
            // Cannot reach this target; stop here.
            break;
        }

        // Reconstruct path from goal to start
        vector<char> path_dirs;
        int r = ti, c = tj;
        while (!(r == ci && c == cj)) {
            char d = dir[r][c];
            path_dirs.push_back(d);
            auto pr = parent[r][c];
            r = pr.first;
            c = pr.second;
        }
        reverse(path_dirs.begin(), path_dirs.end());
        int path_len = path_dirs.size();

        // Check if adding this path would exceed the action limit
        if (total_actions + path_len > MAX_ACTIONS) {
            int remaining = MAX_ACTIONS - total_actions;
            if (remaining > 0) {
                for (int i = 0; i < remaining; ++i) {
                    actions.push_back("M " + string(1, path_dirs[i]));
                }
                total_actions += remaining;
            }
            break;
        } else {
            for (char d : path_dirs) {
                actions.push_back("M " + string(1, d));
            }
            total_actions += path_len;
            ci = ti;
            cj = tj;
        }
    }

    for (const string& line : actions) {
        cout << line << "\n";
    }
    return 0;
}