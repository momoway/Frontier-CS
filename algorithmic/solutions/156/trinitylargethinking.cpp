#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const int N = 30;
    int base[N][N];
    for (int i = 0; i < N; ++i) {
        string s;
        if (!(cin >> s)) return 0;
        for (int j = 0; j < N; ++j) {
            base[i][j] = s[j] - '0';
        }
    }

    // to[t][d]: exit direction when entering from direction d, or -1 if cannot
    int to[8][4] = {
        {1, 0, -1, -1},
        {3, -1, -1, 0},
        {-1, -1, 3, 2},
        {-1, 2, 1, -1},
        {1, 0, 3, 2},
        {3, 2, 1, 0},
        {2, -1, 0, -1},
        {-1, 3, -1, 1}
    };

    // Precompute effective type after r rotations (0=0°, 1=90°, 2=180°, 3=270° CCW)
    int eff[8][4];
    for (int t = 0; t < 8; ++t) {
        for (int r = 0; r < 4; ++r) {
            if (t < 4) {
                eff[t][r] = (t + r) % 4;
            } else if (t == 4) {
                eff[t][r] = (r % 2 == 0) ? 4 : 5;
            } else if (t == 5) {
                eff[t][r] = (r % 2 == 0) ? 5 : 4;
            } else if (t == 6) {
                eff[t][r] = (r % 2 == 0) ? 6 : 7;
            } else { // t == 7
                eff[t][r] = (r % 2 == 0) ? 7 : 6;
            }
        }
    }

    int rot[N][N] = {0};

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int best_r = 0;
            int best_score = -1;
            for (int r = 0; r < 4; ++r) {
                int t_eff = eff[base[i][j]][r];
                int score = 0;
                // Check connection with left neighbor (if exists)
                if (j > 0) {
                    int t_left = eff[base[i][j-1]][rot[i][j-1]];
                    if (to[t_eff][0] != -1 && to[t_left][2] != -1) score++;
                }
                // Check connection with upper neighbor (if exists)
                if (i > 0) {
                    int t_up = eff[base[i-1][j]][rot[i-1][j]];
                    if (to[t_eff][1] != -1 && to[t_up][3] != -1) score++;
                }
                if (score > best_score || (score == best_score && r < best_r)) {
                    best_score = score;
                    best_r = r;
                }
            }
            rot[i][j] = best_r;
        }
    }

    string out;
    out.reserve(N * N);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            out.push_back('0' + rot[i][j]);
        }
    }
    cout << out << "\n";
    return 0;
}