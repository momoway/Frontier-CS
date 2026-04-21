#include <bits/stdc++.h>
using namespace std;

using Grid = vector<vector<int>>;

pair<int,int> getCell(int p, const Grid& grid) {
    int cnt = 0;
    for (int r = 0; r < 10; ++r) {
        for (int c = 0; c < 10; ++c) {
            if (grid[r][c] == 0) {
                ++cnt;
                if (cnt == p) return {r, c};
            }
        }
    }
    return {-1, -1};
}

Grid tilt(const Grid& grid, char dir) {
    Grid res(10, vector<int>(10, 0));
    if (dir == 'F') {
        for (int c = 0; c < 10; ++c) {
            vector<int> candies;
            for (int r = 0; r < 10; ++r)
                if (grid[r][c] != 0) candies.push_back(grid[r][c]);
            int k = candies.size();
            for (int i = 0; i < k; ++i)
                res[i][c] = candies[i];
        }
    } else if (dir == 'B') {
        for (int c = 0; c < 10; ++c) {
            vector<int> candies;
            for (int r = 0; r < 10; ++r)
                if (grid[r][c] != 0) candies.push_back(grid[r][c]);
            int k = candies.size();
            for (int i = 0; i < k; ++i) {
                int newRow = 9 - (k - 1 - i);
                res[newRow][c] = candies[i];
            }
        }
    } else if (dir == 'L') {
        for (int r = 0; r < 10; ++r) {
            vector<int> candies;
            for (int c = 0; c < 10; ++c)
                if (grid[r][c] != 0) candies.push_back(grid[r][c]);
            int k = candies.size();
            for (int i = 0; i < k; ++i)
                res[r][i] = candies[i];
        }
    } else if (dir == 'R') {
        for (int r = 0; r < 10; ++r) {
            vector<int> candies;
            for (int c = 0; c < 10; ++c)
                if (grid[r][c] != 0) candies.push_back(grid[r][c]);
            int k = candies.size();
            for (int i = 0; i < k; ++i) {
                int newCol = 9 - (k - 1 - i);
                res[r][newCol] = candies[i];
            }
        }
    }
    return res;
}

long long computeScore(const Grid& grid) {
    bool visited[10][10] = {false};
    long long total = 0;
    const int dr[4] = {-1, 1, 0, 0};
    const int dc[4] = {0, 0, -1, 1};
    for (int r = 0; r < 10; ++r) {
        for (int c = 0; c < 10; ++c) {
            if (grid[r][c] != 0 && !visited[r][c]) {
                int flavor = grid[r][c];
                queue<pair<int,int>> q;
                q.push({r, c});
                visited[r][c] = true;
                int size = 0;
                while (!q.empty()) {
                    auto [cr, cc] = q.front(); q.pop();
                    ++size;
                    for (int d = 0; d < 4; ++d) {
                        int nr = cr + dr[d];
                        int nc = cc + dc[d];
                        if (nr >= 0 && nr < 10 && nc >= 0 && nc < 10 && !visited[nr][nc] && grid[nr][nc] == flavor) {
                            visited[nr][nc] = true;
                            q.push({nr, nc});
                        }
                    }
                }
                total += 1LL * size * size;
            }
        }
    }
    return total;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    vector<int> flavors(100);
    for (int i = 0; i < 100; ++i) cin >> flavors[i];
    Grid grid(10, vector<int>(10, 0));
    for (int t = 0; t < 100; ++t) {
        int p; cin >> p;
        auto [r, c] = getCell(p, grid);
        grid[r][c] = flavors[t];
        char bestDir = 'F';
        long long bestScore = -1;
        vector<char> dirs = {'F', 'B', 'L', 'R'};
        for (char d : dirs) {
            Grid cand = tilt(grid, d);
            long long score = computeScore(cand);
            if (score > bestScore) {
                bestScore = score;
                bestDir = d;
            }
        }
        cout << bestDir << endl;
        grid = tilt(grid, bestDir);
    }
    return 0;
}