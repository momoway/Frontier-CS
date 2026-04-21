#include <bits/stdc++.h>
using namespace std;

struct Operation {
    int op, x, y;
};

int n, m, k;
vector<string> cur, target;
vector<pair<int, int>> presetSize;
vector<vector<string>> presetMat;

int charToIndex(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'Z') return 10 + (c - 'A');
    if (c >= 'a' && c <= 'z') return 36 + (c - 'a');
    return -1;
}

void applyPreset(int idx, int x, int y) { // x,y 1-indexed
    int np = presetSize[idx].first;
    int mp = presetSize[idx].second;
    for (int dr = 0; dr < np; dr++) {
        for (int dc = 0; dc < mp; dc++) {
            int r = x - 1 + dr;
            int c = y - 1 + dc;
            cur[r][c] = presetMat[idx][dr][dc];
        }
    }
}

void applyRotation(int x, int y) { // x,y 1-indexed, rotate clockwise the 2x2 block at (x,y) to (x+1,y+1)
    int r = x - 1, c = y - 1;
    // cells: (r,c), (r,c+1), (r+1,c+1), (r+1,c)
    char a = cur[r][c];
    char b = cur[r][c+1];
    char d = cur[r+1][c+1];
    char e = cur[r+1][c];
    // clockwise: (r,c) gets e, (r,c+1) gets a, (r+1,c+1) gets b, (r+1,c) gets d
    cur[r][c] = e;
    cur[r][c+1] = a;
    cur[r+1][c+1] = b;
    cur[r+1][c] = d;
}

void applySwap(int op, int x, int y) { // op: -1, -2, -3, -4
    if (op == -1) { // swap (x,y) and (x,y+1)
        int r = x - 1, c = y - 1;
        swap(cur[r][c], cur[r][c+1]);
    } else if (op == -2) { // swap (x,y) and (x,y-1)
        int r = x - 1, c = y - 1;
        swap(cur[r][c], cur[r][c-1]);
    } else if (op == -3) { // swap (x,y) and (x-1,y)
        int r = x - 1, c = y - 1;
        swap(cur[r][c], cur[r-1][c]);
    } else if (op == -4) { // swap (x,y) and (x+1,y)
        int r = x - 1, c = y - 1;
        swap(cur[r][c], cur[r+1][c]);
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> n >> m >> k;
    cur.resize(n);
    for (int i = 0; i < n; i++) {
        cin >> cur[i];
    }
    // skip empty line if any
    string line;
    getline(cin, line); // consume newline after last row
    getline(cin, line); // empty line?
    target.resize(n);
    for (int i = 0; i < n; i++) {
        cin >> target[i];
    }
    presetSize.resize(k);
    presetMat.resize(k);
    for (int i = 0; i < k; i++) {
        // each preset starts with empty line? We'll just read integers
        cin >> presetSize[i].first >> presetSize[i].second;
        presetMat[i].resize(presetSize[i].first);
        for (int r = 0; r < presetSize[i].first; r++) {
            cin >> presetMat[i][r];
        }
    }

    // Compute character counts
    vector<int> cntCur(62, 0), cntTarget(62, 0);
    auto upd = [&](const vector<string>& mat, vector<int>& cnt) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                int idx = charToIndex(mat[i][j]);
                if (idx >= 0) cnt[idx]++;
            }
        }
    };
    upd(cur, cntCur);
    upd(target, cntTarget);

    // Check availability of characters in presets
    vector<int> cntPreset(62, 0);
    for (int p = 0; p < k; p++) {
        for (int i = 0; i < presetSize[p].first; i++) {
            for (int j = 0; j < presetSize[p].second; j++) {
                int idx = charToIndex(presetMat[p][i][j]);
                if (idx >= 0) cntPreset[idx]++;
            }
        }
    }

    for (int c = 0; c < 62; c++) {
        if (cntTarget[c] > 0 && cntCur[c] == 0 && cntPreset[c] == 0) {
            cout << "-1\n";
            return 0;
        }
    }

    vector<Operation> ops;
    int presetUsedCount = 0;

    // Pre-introduction of missing characters (if any)
    for (int c = 0; c < 62; c++) {
        if (cntTarget[c] > 0 && cntCur[c] == 0) {
            // find a preset containing this character
            int foundPreset = -1;
            for (int p = 0; p < k; p++) {
                bool has = false;
                for (int i = 0; i < presetSize[p].first; i++) {
                    for (int j = 0; j < presetSize[p].second; j++) {
                        if (charToIndex(presetMat[p][i][j]) == c) {
                            has = true;
                            break;
                        }
                    }
                    if (has) break;
                }
                if (has) {
                    foundPreset = p;
                    break;
                }
            }
            if (foundPreset == -1) {
                // should not happen due to earlier check
                cout << "-1\n";
                return 0;
            }
            // apply preset at a valid position, e.g., top-left (1,1) if fits
            int np = presetSize[foundPreset].first;
            int mp = presetSize[foundPreset].second;
            int x = 1, y = 1;
            if (x > n - np + 1) x = n - np + 1;
            if (y > m - mp + 1) y = m - mp + 1;
            if (x < 1) x = 1;
            if (y < 1) y = 1;
            applyPreset(foundPreset, x, y);
            ops.push_back({foundPreset + 1, x, y}); // preset indices are 1-based
            presetUsedCount++;
            // update counts (optional, but we may not need exact counts for algorithm)
            // We'll just continue; counts may be off but algorithm will adjust.
        }
    }

    // Main loop
    const int MAX_OPS = 400000;
    const int MAX_PRESETS = 400;

    while (true) {
        // Find first incorrect cell
        int i0 = -1, j0 = -1;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                if (cur[i][j] != target[i][j]) {
                    i0 = i; j0 = j;
                    break;
                }
            }
            if (i0 != -1) break;
        }
        if (i0 == -1) break; // solved

        bool progress = false;

        // Try presets
        for (int p = 0; p < k && !progress; p++) {
            int np = presetSize[p].first;
            int mp = presetSize[p].second;
            // possible top-left positions that cover (i0,j0)
            int minx = max(1, i0 + 1 - np + 1);
            int maxx = min(n - np + 1, i0 + 1);
            int miny = max(1, j0 + 1 - mp + 1);
            int maxy = min(m - mp + 1, j0 + 1);
            for (int x = minx; x <= maxx && !progress; x++) {
                for (int y = miny; y <= maxy && !progress; y++) {
                    int rel_i = (i0 + 1) - x;
                    int rel_j = (j0 + 1) - y;
                    if (presetMat[p][rel_i - 1][rel_j - 1] != target[i0][j0]) continue;
                    // Evaluate impact: count how many cells in the block will become correct/incorrect
                    int newCorrect = 0;
                    int newIncorrect = 0;
                    for (int dr = 0; dr < np; dr++) {
                        for (int dc = 0; dc < mp; dc++) {
                            int r = x - 1 + dr;
                            int c = y - 1 + dc;
                            char oldVal = cur[r][c];
                            char newVal = presetMat[p][dr][dc];
                            bool wasCorrect = (oldVal == target[r][c]);
                            bool willBeCorrect = (newVal == target[r][c]);
                            if (wasCorrect && !willBeCorrect) newIncorrect++;
                            if (!wasCorrect && willBeCorrect) newCorrect++;
                        }
                    }
                    // If the current cell becomes correct, it's already counted in newCorrect (since it was incorrect)
                    // Accept if net change >= 0
                    if (newCorrect - newIncorrect >= 0) {
                        applyPreset(p, x, y);
                        ops.push_back({p + 1, x, y});
                        presetUsedCount++;
                        progress = true;
                        if (presetUsedCount > MAX_PRESETS) {
                            cout << "-1\n";
                            return 0;
                        }
                    }
                }
            }
        }
        if (progress) continue;

        // Try rotations
        // Possible top-left positions for 2x2 block that include (i0,j0)
        for (int dx = -1; dx <= 0; dx++) {
            for (int dy = -1; dy <= 0; dy++) {
                int x0 = i0 + dx; // 0-indexed row of top-left
                int y0 = j0 + dy; // 0-indexed col of top-left
                if (x0 < 0 || y0 < 0 || x0 + 1 >= n || y0 + 1 >= m) continue;
                int x = x0 + 1; // 1-indexed
                int y = y0 + 1;
                // Simulate rotation
                // Save original values
                char a = cur[x0][y0];
                char b = cur[x0][y0+1];
                char d = cur[x0+1][y0+1];
                char e = cur[x0+1][y0];
                // After clockwise rotation:
                char na = e;
                char nb = a;
                char nd = b;
                char ne = d;
                // Compute impact
                int newCorrect = 0;
                int newIncorrect = 0;
                // cell (x0,y0)
                if (cur[x0][y0] == target[x0][y0] && na != target[x0][y0]) newIncorrect++;
                else if (cur[x0][y0] != target[x0][y0] && na == target[x0][y0]) newCorrect++;
                // cell (x0,y0+1)
                if (cur[x0][y0+1] == target[x0][y0+1] && nb != target[x0][y0+1]) newIncorrect++;
                else if (cur[x0][y0+1] != target[x0][y0+1] && nb == target[x0][y0+1]) newCorrect++;
                // cell (x0+1,y0+1)
                if (cur[x0+1][y0+1] == target[x0+1][y0+1] && nd != target[x0+1][y0+1]) newIncorrect++;
                else if (cur[x0+1][y0+1] != target[x0+1][y0+1] && nd == target[x0+1][y0+1]) newCorrect++;
                // cell (x0+1,y0)
                if (cur[x0+1][y0] == target[x0+1][y0] && ne != target[x0+1][y0]) newIncorrect++;
                else if (cur[x0+1][y0] != target[x0+1][y0] && ne == target[x0+1][y0]) newCorrect++;
                if (newCorrect - newIncorrect >= 0) {
                    applyRotation(x, y);
                    ops.push_back({0, x, y});
                    progress = true;
                    break;
                }
            }
            if (progress) break;
        }
        if (progress) continue;

        // Try swaps: find a token of target[i0][j0] and bring it here
        // We'll search for a cell (p,q) with cur[p][q] == target[i0][j0] and (p,q) != (i0,j0)
        int pi = -1, qi = -1;
        for (int i = 0; i < n && pi == -1; i++) {
            for (int j = 0; j < m; j++) {
                if ((i != i0 || j != j0) && cur[i][j] == target[i0][j0]) {
                    pi = i; qi = j;
                    break;
                }
            }
        }
        if (pi == -1) {
            // no token found, impossible
            cout << "-1\n";
            return 0;
        }

        // Move token from (pi,qi) to (i0,j0) using swaps, avoiding correct cells if possible
        // We'll use BFS to find a path that minimizes stepping on correct cells, but for simplicity we use a fixed strategy:
        int cr = pi, cc = qi;
        while (cr != i0 || cc != j0) {
            // Determine next move
            if (cr == i0) {
                // same row, move left if cc > j0, else move right if cc < j0
                if (cc > j0) {
                    // move left
                    applySwap(-2, cr+1, cc+1); // swap (cr,cc) with (cr,cc-1)
                    cc--;
                    ops.push_back({-2, cr+1, cc+1});
                } else { // cc < j0
                    applySwap(-1, cr+1, cc+1); // swap (cr,cc) with (cr,cc+1)
                    cc++;
                    ops.push_back({-1, cr+1, cc+1});
                }
            } else if (cr > i0) {
                // below target row
                if (cc < j0) {
                    // move right first
                    applySwap(-1, cr+1, cc+1);
                    cc++;
                    ops.push_back({-1, cr+1, cc+1});
                } else {
                    // move up
                    applySwap(-3, cr+1, cc+1);
                    cr--;
                    ops.push_back({-3, cr+1, cc+1});
                }
            } else {
                // cr < i0, should not happen because token should be in unprocessed region (rows >= i0) or same row with col >= j0? Actually if token is above target row, it's in processed region (since rows < i0 are fully processed). So we shouldn't pick such token. So this case shouldn't occur.
                // Fallback: move down
                applySwap(-4, cr+1, cc+1);
                cr++;
                ops.push_back({-4, cr+1, cc+1});
            }
            if ((int)ops.size() > MAX_OPS) {
                cout << "-1\n";
                return 0;
            }
        }
        progress = true;
        if (!progress) {
            cout << "-1\n";
            return 0;
        }
    }

    // Output result
    cout << ops.size() << "\n";
    for (auto &op : ops) {
        cout << op.op << " " << op.x << " " << op.y << "\n";
    }
    return 0;
}