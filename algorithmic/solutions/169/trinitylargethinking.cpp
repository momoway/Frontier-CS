#include <bits/stdc++.h>
using namespace std;

struct Segment {
    int cost;
    int dir; // 0: up, 1: down, 2: left, 3: right
    int param; // row for L/R, column for U/D
    int k; // number of shifts in each direction
    unsigned long long mask;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int N;
    if (!(cin >> N)) return 0;
    vector<string> board(N);
    for (int i = 0; i < N; ++i) {
        cin >> board[i];
    }

    vector<pair<int,int>> oni_pos;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (board[i][j] == 'x') {
                oni_pos.push_back({i, j});
            }
        }
    }
    int M = oni_pos.size();

    vector<Segment> segs;
    for (int a = 0; a < M; ++a) {
        int i = oni_pos[a].first;
        int j = oni_pos[a].second;

        // Up: no Fuku above
        bool valid_up = true;
        for (int r = 0; r < i; ++r) {
            if (board[r][j] == 'o') { valid_up = false; break; }
        }
        if (valid_up) {
            int k = i + 1;
            unsigned long long mask = 0;
            for (int b = 0; b < M; ++b) {
                int rb = oni_pos[b].first;
                int cb = oni_pos[b].second;
                if (cb == j && rb <= i) mask |= (1ULL << b);
            }
            segs.push_back({2*k, 0, j, k, mask});
        }

        // Down: no Fuku below
        bool valid_down = true;
        for (int r = i+1; r < N; ++r) {
            if (board[r][j] == 'o') { valid_down = false; break; }
        }
        if (valid_down) {
            int k = N - i;
            unsigned long long mask = 0;
            for (int b = 0; b < M; ++b) {
                int rb = oni_pos[b].first;
                int cb = oni_pos[b].second;
                if (cb == j && rb >= i) mask |= (1ULL << b);
            }
            segs.push_back({2*k, 1, j, k, mask});
        }

        // Left: no Fuku to the left
        bool valid_left = true;
        for (int c = 0; c < j; ++c) {
            if (board[i][c] == 'o') { valid_left = false; break; }
        }
        if (valid_left) {
            int k = j + 1;
            unsigned long long mask = 0;
            for (int b = 0; b < M; ++b) {
                int rb = oni_pos[b].first;
                int cb = oni_pos[b].second;
                if (rb == i && cb <= j) mask |= (1ULL << b);
            }
            segs.push_back({2*k, 2, i, k, mask});
        }

        // Right: no Fuku to the right
        bool valid_right = true;
        for (int c = j+1; c < N; ++c) {
            if (board[i][c] == 'o') { valid_right = false; break; }
        }
        if (valid_right) {
            int k = N - j;
            unsigned long long mask = 0;
            for (int b = 0; b < M; ++b) {
                int rb = oni_pos[b].first;
                int cb = oni_pos[b].second;
                if (rb == i && cb >= j) mask |= (1ULL << b);
            }
            segs.push_back({2*k, 3, i, k, mask});
        }
    }

    // Greedy set cover
    unsigned long long uncovered = (M == 64 ? ~0ULL : ((1ULL << M) - 1));
    vector<Segment> selected;
    while (uncovered) {
        int best_idx = -1;
        long long best_cost = 0;
        int best_cnt = 0;
        for (int s = 0; s < (int)segs.size(); ++s) {
            unsigned long long cov = segs[s].mask & uncovered;
            if (cov == 0) continue;
            int cnt = __builtin_popcountll(cov);
            if (best_idx == -1) {
                best_idx = s;
                best_cost = segs[s].cost;
                best_cnt = cnt;
            } else {
                long long cost = segs[s].cost;
                long long left = cost * best_cnt;
                long long right = best_cost * cnt;
                if (left < right || (left == right && cnt > best_cnt)) {
                    best_idx = s;
                    best_cost = cost;
                    best_cnt = cnt;
                }
            }
        }
        if (best_idx == -1) break; // Should not happen
        selected.push_back(segs[best_idx]);
        uncovered &= ~segs[best_idx].mask;
    }

    // Output moves
    for (const auto& seg : selected) {
        char d, opp;
        if (seg.dir == 0) { d = 'U'; opp = 'D'; }
        else if (seg.dir == 1) { d = 'D'; opp = 'U'; }
        else if (seg.dir == 2) { d = 'L'; opp = 'R'; }
        else { d = 'R'; opp = 'L'; }
        for (int t = 0; t < seg.k; ++t) {
            cout << d << ' ' << seg.param << '\n';
        }
        for (int t = 0; t < seg.k; ++t) {
            cout << opp << ' ' << seg.param << '\n';
        }
    }

    return 0;
}