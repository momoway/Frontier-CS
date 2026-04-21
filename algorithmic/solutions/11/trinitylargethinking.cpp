#include <bits/stdc++.h>
using namespace std;

int n, m;
vector<string> grid;
int sr, sc, er, ec;
vector<vector<bool>> visited;
vector<char> A; // closed walk

int dx[4] = {0, 0, -1, 1}; // L,R,U,D
int dy[4] = {-1, 1, 0, 0};
char dirChar[4] = {'L', 'R', 'U', 'D'};
int oppositeDir[4] = {1, 0, 3, 2}; // L<->R, U<->D

void dfs(int r, int c) {
    visited[r][c] = true;
    for (int i = 0; i < 4; i++) {
        int nr = r + dx[i];
        int nc = c + dy[i];
        if (nr >= 0 && nr < n && nc >= 0 && nc < m && grid[nr][nc] == '1' && !visited[nr][nc]) {
            A.push_back(dirChar[i]);
            dfs(nr, nc);
            A.push_back(dirChar[oppositeDir[i]]);
        }
    }
}

pair<int,int> simulate(const vector<char>& seq, int r0, int c0) {
    int r = r0, c = c0;
    for (char ch : seq) {
        int d = -1;
        for (int i = 0; i < 4; i++) if (dirChar[i] == ch) { d = i; break; }
        if (d == -1) continue; // should not happen
        int nr = r + dx[d];
        int nc = c + dy[d];
        if (nr >= 0 && nr < n && nc >= 0 && nc < m && grid[nr][nc] == '1') {
            r = nr;
            c = nc;
        }
    }
    return {r, c};
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> n >> m;
    grid.resize(n);
    for (int i = 0; i < n; i++) {
        cin >> grid[i];
    }
    cin >> sr >> sc >> er >> ec;
    sr--; sc--; er--; ec--;

    // Check connectivity from start to exit
    vector<vector<bool>> conn(n, vector<bool>(m, false));
    queue<pair<int,int>> q;
    q.push({sr, sc});
    conn[sr][sc] = true;
    while (!q.empty()) {
        auto [r, c] = q.front(); q.pop();
        for (int i = 0; i < 4; i++) {
            int nr = r + dx[i];
            int nc = c + dy[i];
            if (nr >= 0 && nr < n && nc >= 0 && nc < m && grid[nr][nc] == '1' && !conn[nr][nc]) {
                conn[nr][nc] = true;
                q.push({nr, nc});
            }
        }
    }
    if (!conn[er][ec]) {
        cout << "-1\n";
        return 0;
    }

    // Build closed walk A via DFS
    visited.assign(n, vector<bool>(m, false));
    A.clear();
    dfs(sr, sc);
    // Ensure all blank cells visited? DFS should cover all reachable. Since we checked connectivity, it will.
    // However, DFS might not visit all if some cells are not reachable? But we already checked connectivity, so fine.

    // Compute reverse(A) and simulate to get Q
    vector<char> revA = A;
    reverse(revA.begin(), revA.end());
    pair<int,int> Q = simulate(revA, sr, sc);

    if (Q.first == er && Q.second == ec) {
        // Output A + reverse(A)
        string S;
        for (char c : A) S.push_back(c);
        for (char c : revA) S.push_back(c);
        cout << S << "\n";
        return 0;
    }

    // Determine invalid directions at start
    vector<int> invalidDirs;
    for (int i = 0; i < 4; i++) {
        int nr = sr + dx[i];
        int nc = sc + dy[i];
        if (nr < 0 || nr >= n || nc < 0 || nc >= m || grid[nr][nc] != '1') {
            invalidDirs.push_back(i);
        }
    }

    if (invalidDirs.empty()) {
        // No invalid moves at start, cannot construct X
        cout << "-1\n";
        return 0;
    }

    // BFS from exit to find Q using only invalid directions and valid moves
    vector<vector<bool>> seen(n, vector<bool>(m, false));
    vector<vector<char>> moveFrom(n, vector<char>(m, 0));
    vector<vector<pair<int,int>>> prev(n, vector<pair<int,int>>(m, make_pair(-1,-1)));
    queue<pair<int,int>> qq;
    qq.push({er, ec});
    seen[er][ec] = true;
    bool found = false;
    while (!qq.empty()) {
        auto [r, c] = qq.front(); qq.pop();
        if (r == Q.first && c == Q.second) {
            found = true;
            break;
        }
        for (int dIdx : invalidDirs) {
            int nr = r + dx[dIdx];
            int nc = c + dy[dIdx];
            if (nr >= 0 && nr < n && nc >= 0 && nc < m && grid[nr][nc] == '1' && !seen[nr][nc]) {
                seen[nr][nc] = true;
                moveFrom[nr][nc] = dirChar[dIdx];
                prev[nr][nc] = {r, c};
                qq.push({nr, nc});
            }
        }
    }

    if (!found) {
        cout << "-1\n";
        return 0;
    }

    // Reconstruct path from exit to Q (X)
    vector<char> Xtemp; // will hold moves from exit to Q (in reverse order)
    pair<int,int> cur = Q;
    while (cur != make_pair(er, ec)) {
        char d = moveFrom[cur.first][cur.second];
        Xtemp.push_back(d);
        cur = prev[cur.first][cur.second];
    }
    // Xtemp is moves from Q back to exit? Actually we collected moves from Q to exit (reverse direction). We need X = sequence from exit to Q.
    // So reverse Xtemp.
    vector<char> X = Xtemp;
    reverse(X.begin(), X.end());

    // Construct final string S = X + A + reverse(A) + reverse(X)
    string S;
    for (char c : X) S.push_back(c);
    for (char c : A) S.push_back(c);
    for (char c : revA) S.push_back(c);
    for (int i = (int)X.size() - 1; i >= 0; i--) S.push_back(X[i]); // reverse(X)

    cout << S << "\n";
    return 0;
}