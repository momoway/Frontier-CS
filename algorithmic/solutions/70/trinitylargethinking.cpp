#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int t;
    cin >> t;
    while (t--) {
        int n, m, start, base_move_count;
        cin >> n >> m >> start >> base_move_count;
        for (int i = 0; i < m; ++i) {
            int u, v;
            cin >> u >> v;
        }
        while (true) {
            string token;
            if (!(cin >> token)) break;
            if (token == "AC" || token == "F") break;
            int d = stoi(token);
            vector<int> deg(d), flag(d);
            for (int i = 0; i < d; ++i) {
                cin >> deg[i] >> flag[i];
            }
            int move_idx = -1;
            int best_deg = INT_MAX;
            for (int i = 0; i < d; ++i) {
                if (flag[i] == 0) {
                    if (deg[i] < best_deg || (deg[i] == best_deg && (move_idx == -1 || i < move_idx))) {
                        best_deg = deg[i];
                        move_idx = i;
                    }
                }
            }
            if (move_idx == -1) {
                best_deg = -1;
                for (int i = 0; i < d; ++i) {
                    if (deg[i] > best_deg || (deg[i] == best_deg && (move_idx == -1 || i < move_idx))) {
                        best_deg = deg[i];
                        move_idx = i;
                    }
                }
            }
            cout << move_idx + 1 << "\n" << flush;
        }
    }
    return 0;
}