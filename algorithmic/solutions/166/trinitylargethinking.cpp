#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int N;
    if (!(cin >> N)) return 0;
    vector<vector<long long>> h(N, vector<long long>(N));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            cin >> h[i][j];
        }
    }
    vector<vector<bool>> done(N, vector<bool>(N, false));
    long long load = 0;
    int r = 0, c = 0;
    vector<string> ops;
    int remaining = 0;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (h[i][j] != 0) remaining++;
        }
    }
    while (remaining > 0) {
        long long bestDist = LLONG_MAX;
        int bi = -1, bj = -1;
        bool bestIsNeg = false;
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                if (done[i][j]) continue;
                if (h[i][j] == 0) continue;
                bool isPos = h[i][j] > 0;
                bool isNeg = h[i][j] < 0;
                bool canDo = false;
                if (load == 0) {
                    if (isPos) canDo = true;
                } else {
                    if (isPos) canDo = true;
                    else if (isNeg && load >= -h[i][j]) canDo = true;
                }
                if (!canDo) continue;
                long long dist = llabs(r - i) + llabs(c - j);
                if (dist < bestDist) {
                    bestDist = dist;
                    bi = i; bj = j;
                    bestIsNeg = isNeg;
                } else if (dist == bestDist) {
                    if (isNeg && !bestIsNeg) {
                        bi = i; bj = j;
                        bestIsNeg = true;
                    }
                }
            }
        }
        if (bi == -1) {
            break;
        }
        while (r < bi) { ops.push_back("D"); r++; }
        while (r > bi) { ops.push_back("U"); r--; }
        while (c < bj) { ops.push_back("R"); c++; }
        while (c > bj) { ops.push_back("L"); c--; }
        if (h[bi][bj] > 0) {
            long long amt = h[bi][bj];
            ops.push_back("+" + to_string(amt));
            load += amt;
            done[bi][bj] = true;
            remaining--;
        } else if (h[bi][bj] < 0) {
            long long amt = -h[bi][bj];
            ops.push_back("-" + to_string(amt));
            load -= amt;
            done[bi][bj] = true;
            remaining--;
        }
    }
    for (const string& op : ops) {
        cout << op << "\n";
    }
    return 0;
}