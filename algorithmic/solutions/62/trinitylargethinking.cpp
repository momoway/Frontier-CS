#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m;
    if (!(cin >> n >> m)) return 0;
    vector<vector<int>> inputStacks(n + 1);
    vector<int> allColors;
    for (int i = 1; i <= n; i++) {
        inputStacks[i].resize(m);
        for (int j = 0; j < m; j++) {
            cin >> inputStacks[i][j];
            allColors.push_back(inputStacks[i][j]);
        }
    }

    sort(allColors.begin(), allColors.end());
    allColors.erase(unique(allColors.begin(), allColors.end()), allColors.end());
    // There should be exactly n distinct colors
    unordered_map<int, int> colorToIdx;
    for (int i = 0; i < (int)allColors.size(); i++) {
        colorToIdx[allColors[i]] = i + 1;
    }

    // Build stacks with color indices
    vector<vector<int>> stacks(n + 2); // 1..n, n+1 is buffer
    for (int i = 1; i <= n; i++) {
        stacks[i] = inputStacks[i];
        for (int &x : stacks[i]) {
            x = colorToIdx[x];
        }
    }
    // Buffer empty
    stacks[n + 1] = vector<int>();

    // Data structures
    vector<int> cnt(n + 2, 0);
    vector<unordered_map<int, int>> colorCount(n + 2);
    vector<int> topColor(n + 2, -1);
    vector<unordered_set<int>> containsColor(n + 1); // for colors 1..n
    vector<unordered_set<int>> topColorPillars(n + 1); // for colors 1..n
    unordered_set<int> nonFullSet;

    // Initialize
    for (int i = 1; i <= n; i++) {
        cnt[i] = m;
        for (int c : stacks[i]) {
            colorCount[i][c]++;
            containsColor[c].insert(i);
        }
        topColor[i] = stacks[i].back();
        topColorPillars[topColor[i]].insert(i);
    }
    // Buffer
    cnt[n + 1] = 0;
    topColor[n + 1] = -1;
    nonFullSet.insert(n + 1);

    // Target pillar for each color (color index -> pillar index)
    vector<int> target(n + 1);
    for (int i = 1; i <= n; i++) target[i] = i;

    vector<pair<int, int>> moves;

    // Helper function to move top ball from x to y
    auto moveBall = [&](int x, int y) {
        int ball = stacks[x].back();
        stacks[x].pop_back();

        // Update x
        auto &cx = colorCount[x];
        int oldCntX = cx[ball];
        oldCntX--;
        if (oldCntX == 0) {
            cx.erase(ball);
            containsColor[ball].erase(x);
        } else {
            cx[ball] = oldCntX;
        }
        int oldTopX = stacks[x].empty() ? -1 : stacks[x].back();
        topColorPillars[ball].erase(x);
        if (oldTopX != -1) {
            topColorPillars[oldTopX].insert(x);
        }
        topColor[x] = oldTopX;
        cnt[x]--;
        if (cnt[x] == m - 1) nonFullSet.insert(x);

        // Record old top of y
        int oldTopY = stacks[y].empty() ? -1 : stacks[y].back();

        // Push to y
        stacks[y].push_back(ball);

        // Update y
        auto &cy = colorCount[y];
        int oldCntY = cy[ball];
        oldCntY++;
        cy[ball] = oldCntY;
        if (oldCntY == 1) {
            containsColor[ball].insert(y);
        }
        if (oldTopY != -1) {
            topColorPillars[oldTopY].erase(y);
        }
        topColorPillars[ball].insert(y);
        topColor[y] = ball;
        cnt[y]++;
        if (cnt[y] == m) nonFullSet.erase(y);
    };

    // Process colors i = 1..n
    for (int i = 1; i <= n; i++) {
        // Step 1: Empty pillar i
        while (cnt[i] > 0) {
            int ball = stacks[i].back();
            int dest = -1;
            if (target[ball] != i && cnt[target[ball]] < m) {
                dest = target[ball];
            } else {
                for (int k : nonFullSet) {
                    if (k != i) {
                        dest = k;
                        break;
                    }
                }
                // If still -1, fallback (should not happen)
                if (dest == -1 && cnt[i] < m) {
                    dest = i; // move to itself? not allowed, but avoid crash
                }
            }
            moveBall(i, dest);
            moves.push_back({i, dest});
        }

        // Step 2: Collect all balls of color i onto pillar i
        int collected = 0;
        while (collected < m) {
            if (!topColorPillars[i].empty()) {
                int j = -1;
                for (int cand : topColorPillars[i]) {
                    if (cand != i) {
                        j = cand;
                        break;
                    }
                }
                if (j != -1) {
                    moveBall(j, i);
                    moves.push_back({j, i});
                    collected++;
                    continue;
                }
            }
            // Expose an i ball
            int j = -1;
            for (int cand : containsColor[i]) {
                if (cand != i) {
                    j = cand;
                    break;
                }
            }
            if (j == -1) {
                // Only i itself contains i balls
                j = i;
            }
            int ball = topColor[j]; // color of top ball (not i, because else would have been moved)
            int dest = -1;
            if (target[ball] != i && cnt[target[ball]] < m) {
                dest = target[ball];
            } else {
                for (int k : nonFullSet) {
                    if (k != i) {
                        dest = k;
                        break;
                    }
                }
                if (dest == -1 && cnt[i] < m) {
                    dest = i;
                }
            }
            moveBall(j, dest);
            moves.push_back({j, dest});
        }
    }

    // Output moves
    cout << moves.size() << "\n";
    for (auto &p : moves) {
        cout << p.first << " " << p.second << "\n";
    }
    return 0;
}