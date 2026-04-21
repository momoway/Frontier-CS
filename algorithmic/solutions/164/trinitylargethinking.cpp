#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m;
    if (!(cin >> n >> m)) return 0;
    vector<vector<int>> stacks(m);
    int per = n / m;
    for (int i = 0; i < m; i++) {
        stacks[i].resize(per);
        for (int j = 0; j < per; j++) {
            cin >> stacks[i][j];
        }
    }

    vector<pair<int, int>> ops;

    for (int v = 1; v <= n; v++) {
        int s = -1, pos = -1;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < (int)stacks[i].size(); j++) {
                if (stacks[i][j] == v) {
                    s = i;
                    pos = j;
                    break;
                }
            }
            if (s != -1) break;
        }
        if (s == -1) {
            cerr << "Box " << v << " not found!\n";
            return 0;
        }

        while (pos != (int)stacks[s].size() - 1) {
            int u = stacks[s].back();
            int d = -1;

            // Try empty stack
            for (int i = 0; i < m; i++) {
                if (i == s) continue;
                if (stacks[i].empty()) {
                    d = i;
                    break;
                }
            }
            if (d == -1) {
                int best_top = -1;
                for (int i = 0; i < m; i++) {
                    if (i == s) continue;
                    if (!stacks[i].empty() && stacks[i].back() > u) {
                        if (d == -1 || stacks[i].back() < best_top) {
                            best_top = stacks[i].back();
                            d = i;
                        }
                    }
                }
            }

            if (d == -1) {
                bool moved = false;
                for (int t = 0; t < m && !moved; t++) {
                    if (t == s || stacks[t].empty()) continue;
                    int u2 = stacks[t].back();
                    int d2 = -1;
                    for (int i = 0; i < m; i++) {
                        if (i == t || i == s) continue;
                        if (stacks[i].empty()) {
                            d2 = i;
                            break;
                        }
                    }
                    if (d2 == -1) {
                        for (int i = 0; i < m; i++) {
                            if (i == t || i == s) continue;
                            if (!stacks[i].empty() && stacks[i].back() > u2) {
                                if (d2 == -1 || stacks[i].back() < stacks[d2].back()) {
                                    d2 = i;
                                }
                            }
                        }
                    }
                    if (d2 != -1) {
                        ops.push_back({u2, d2 + 1});
                        stacks[t].pop_back();
                        stacks[d2].push_back(u2);
                        moved = true;
                        break;
                    }
                }
                if (moved) continue;
                else {
                    cerr << "Stuck at box " << v << " unable to move top " << u << endl;
                    return 0;
                }
            }

            ops.push_back({u, d + 1});
            stacks[s].pop_back();
            stacks[d].push_back(u);
            pos--;
        }

        ops.push_back({v, 0});
        stacks[s].pop_back();
    }

    for (auto &p : ops) {
        cout << p.first << " " << p.second << "\n";
    }
    return 0;
}