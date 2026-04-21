#include <bits/stdc++.h>
using namespace std;

const int MAXN = 65;
bool occupied[MAXN][MAXN];
bool horiz_used[MAXN][MAXN-1];
bool vert_used[MAXN][MAXN-1];

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int N, M;
    if (!(cin >> N >> M)) return 0;
    memset(occupied, 0, sizeof(occupied));
    memset(horiz_used, 0, sizeof(horiz_used));
    memset(vert_used, 0, sizeof(vert_used));
    for (int i = 0; i < M; ++i) {
        int x, y;
        cin >> x >> y;
        occupied[x][y] = true;
    }
    int center = (N - 1) / 2;
    vector<array<int,8>> ops;

    while (true) {
        bool moved = false;

        // Try unit squares first
        long long best_weight = -1;
        int best_i = -1, best_j = -1, best_ex = -1, best_ey = -1;
        array<int,8> best_op;
        bool found_unit = false;
        for (int i = 0; i < N-1; ++i) {
            for (int j = 0; j < N-1; ++j) {
                bool a = occupied[i][j];
                bool b = occupied[i+1][j];
                bool c = occupied[i+1][j+1];
                bool d = occupied[i][j+1];
                int cnt = a + b + c + d;
                if (cnt != 3) continue;
                int ex, ey;
                if (!a) { ex = i; ey = j; }
                else if (!b) { ex = i+1; ey = j; }
                else if (!c) { ex = i+1; ey = j+1; }
                else { ex = i; ey = j+1; }
                if (horiz_used[j][i]) continue;
                if (vert_used[i+1][j]) continue;
                if (horiz_used[j+1][i]) continue;
                if (vert_used[i][j]) continue;
                long long w = (long long)(ex - center)*(ex - center) + (long long)(ey - center)*(ey - center) + 1;
                if (w > best_weight) {
                    best_weight = w;
                    best_i = i; best_j = j;
                    best_ex = ex; best_ey = ey;
                    if (!a) {
                        best_op = {i, j, i+1, j, i+1, j+1, i, j+1};
                    } else if (!b) {
                        best_op = {i+1, j, i+1, j+1, i, j+1, i, j};
                    } else if (!c) {
                        best_op = {i+1, j+1, i, j+1, i, j, i+1, j};
                    } else {
                        best_op = {i, j+1, i, j, i+1, j, i+1, j+1};
                    }
                    found_unit = true;
                }
            }
        }
        if (found_unit) {
            occupied[best_ex][best_ey] = true;
            horiz_used[best_j][best_i] = true;
            vert_used[best_i+1][best_j] = true;
            horiz_used[best_j+1][best_i] = true;
            vert_used[best_i][best_j] = true;
            ops.push_back(best_op);
            moved = true;
            continue;
        }

        // Fallback: general axis-aligned rectangles
        bool found_fallback = false;
        for (int w = 1; w < N && !found_fallback; ++w) {
            for (int h = 1; h < N && !found_fallback; ++h) {
                for (int x = 0; x <= N-1-w && !found_fallback; ++x) {
                    for (int y = 0; y <= N-1-h && !found_fallback; ++y) {
                        bool a = occupied[x][y];
                        bool b = occupied[x+w][y];
                        bool c = occupied[x+w][y+h];
                        bool d = occupied[x][y+h];
                        int cnt = a + b + c + d;
                        if (cnt != 3) continue;
                        int ex, ey;
                        if (!a) { ex = x; ey = y; }
                        else if (!b) { ex = x+w; ey = y; }
                        else if (!c) { ex = x+w; ey = y+h; }
                        else { ex = x; ey = y+h; }
                        bool ok = true;
                        // Bottom edge
                        for (int xi = x+1; xi < x+w; ++xi) if (occupied[xi][y]) { ok = false; break; }
                        if (!ok) continue;
                        for (int xi = x; xi < x+w; ++xi) if (horiz_used[y][xi]) { ok = false; break; }
                        if (!ok) continue;
                        // Top edge
                        for (int xi = x+1; xi < x+w; ++xi) if (occupied[xi][y+h]) { ok = false; break; }
                        if (!ok) continue;
                        for (int xi = x; xi < x+w; ++xi) if (horiz_used[y+h][xi]) { ok = false; break; }
                        if (!ok) continue;
                        // Left edge
                        for (int yi = y+1; yi < y+h; ++yi) if (occupied[x][yi]) { ok = false; break; }
                        if (!ok) continue;
                        for (int yi = y; yi < y+h; ++yi) if (vert_used[x][yi]) { ok = false; break; }
                        if (!ok) continue;
                        // Right edge
                        for (int yi = y+1; yi < y+h; ++yi) if (occupied[x+w][yi]) { ok = false; break; }
                        if (!ok) continue;
                        for (int yi = y; yi < y+h; ++yi) if (vert_used[x+w][yi]) { ok = false; break; }
                        if (!ok) continue;
                        // Valid rectangle found
                        occupied[ex][ey] = true;
                        for (int xi = x; xi < x+w; ++xi) horiz_used[y][xi] = true;
                        for (int xi = x; xi < x+w; ++xi) horiz_used[y+h][xi] = true;
                        for (int yi = y; yi < y+h; ++yi) vert_used[x][yi] = true;
                        for (int yi = y; yi < y+h; ++yi) vert_used[x+w][yi] = true;
                        array<int,8> op;
                        if (!a) {
                            op = {x, y, x+w, y, x+w, y+h, x, y+h};
                        } else if (!b) {
                            op = {x+w, y, x+w, y+h, x, y+h, x, y};
                        } else if (!c) {
                            op = {x+w, y+h, x, y+h, x, y, x+w, y};
                        } else {
                            op = {x, y+h, x, y, x+w, y, x+w, y+h};
                        }
                        ops.push_back(op);
                        found_fallback = true;
                        moved = true;
                    }
                }
            }
        }
        if (!moved) break;
    }

    cout << ops.size() << "\n";
    for (auto &op : ops) {
        for (int i = 0; i < 8; ++i) {
            if (i) cout << ' ';
            cout << op[i];
        }
        cout << "\n";
    }
    return 0;
}