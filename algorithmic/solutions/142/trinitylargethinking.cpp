#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m;
    if (!(cin >> n >> m)) return 0;
    vector<vector<int>> stk(n + 2); // 1..n for poles, n+1 for buffer
    vector<vector<int>> colorCount(n + 2, vector<int>(n + 1, 0));
    vector<int> distinctCount(n + 2, 0);

    // read poles 1..n
    for (int i = 1; i <= n; i++) {
        for (int j = 0; j < m; j++) {
            int col; cin >> col;
            stk[i].push_back(col);
            if (colorCount[i][col] == 0) distinctCount[i]++;
            colorCount[i][col]++;
        }
    }
    // pole n+1 (buffer) initially empty

    vector<pair<int,int>> moves;
    int movesCnt = 0;
    const int LIM = 2000000;

    while (true) {
        bool allMono = true;
        for (int i = 1; i <= n; i++) {
            if (distinctCount[i] > 1) {
                allMono = false;
                break;
            }
        }
        if (allMono) break;

        bool moved = false;

        // Priority 1: move from pole i to its target pole (color = top) if target has space
        for (int i = 1; i <= n && !moved; i++) {
            if (stk[i].empty()) continue;
            int c = stk[i].back();
            if (i == c) continue; // already on target pole
            if (stk[c].size() < m) {
                // perform move i -> c
                int ball = stk[i].back(); stk[i].pop_back();
                // update pole i
                colorCount[i][ball]--;
                if (colorCount[i][ball] == 0) distinctCount[i]--;
                // update pole c
                stk[c].push_back(ball);
                if (colorCount[c][ball] == 0) distinctCount[c]++;
                colorCount[c][ball]++;
                moves.emplace_back(i, c);
                movesCnt++;
                moved = true;
            }
        }
        if (moved) continue;

        // Priority 2: if buffer not full, move from a source pole to buffer
        if ((int)stk[n+1].size() < m) {
            int src = -1;
            // prefer non-monochromatic pole
            for (int i = 1; i <= n; i++) {
                if (!stk[i].empty() && distinctCount[i] > 1) {
                    src = i;
                    break;
                }
            }
            if (src == -1) {
                for (int i = 1; i <= n; i++) {
                    if (!stk[i].empty()) {
                        src = i;
                        break;
                    }
                }
            }
            if (src != -1) {
                int ball = stk[src].back(); stk[src].pop_back();
                // update src
                colorCount[src][ball]--;
                if (colorCount[src][ball] == 0) distinctCount[src]--;
                // update buffer
                stk[n+1].push_back(ball);
                if (colorCount[n+1][ball] == 0) distinctCount[n+1]++;
                colorCount[n+1][ball]++;
                moves.emplace_back(src, n+1);
                movesCnt++;
                moved = true;
            }
        }
        if (moved) continue;

        // Priority 3: if buffer not empty, move from buffer to any pole with space
        if (!stk[n+1].empty()) {
            int dst = -1;
            for (int i = 1; i <= n; i++) {
                if ((int)stk[i].size() < m) {
                    dst = i;
                    break;
                }
            }
            if (dst != -1) {
                int ball = stk[n+1].back(); stk[n+1].pop_back();
                // update buffer
                colorCount[n+1][ball]--;
                if (colorCount[n+1][ball] == 0) distinctCount[n+1]--;
                // update destination
                stk[dst].push_back(ball);
                if (colorCount[dst][ball] == 0) distinctCount[dst]++;
                colorCount[dst][ball]++;
                moves.emplace_back(n+1, dst);
                movesCnt++;
                moved = true;
            }
        }

        if (!moved) {
            // No move possible? Should not happen, but break to avoid infinite loop.
            break;
        }

        if (movesCnt > LIM) {
            // Exceeded limit, but we must output something. This should not occur.
            break;
        }
    }

    cout << movesCnt << "\n";
    for (auto &p : moves) {
        cout << p.first << " " << p.second << "\n";
    }
    return 0;
}