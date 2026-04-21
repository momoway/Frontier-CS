#include <bits/stdc++.h>
using namespace std;

const int N = 20;

struct Placement {
    int str_id;
    int dir; // 0: horizontal, 1: vertical
    int row; // fixed row for horizontal, start row for vertical
    int col; // start col for horizontal, fixed col for vertical
    int len;
};

vector<int> cell_placements[N][N];
vector<Placement> placements;
vector<int> mismatch_count;
vector<int> str_match_count;
char grid[N][N];
char best_grid[N][N];
int c, d;
int M;
vector<string> S;
int total_len_sum;
int per_cell_count;

void apply_change(int i, int j, char new_val) {
    char old_val = grid[i][j];
    if (old_val == new_val) return;

    // Update d
    if (old_val == '.') d--;
    if (new_val == '.') d++;

    // Update grid
    grid[i][j] = new_val;

    // Update placements that include (i,j)
    for (int pid : cell_placements[i][j]) {
        const Placement& p = placements[pid];
        int offset;
        if (p.dir == 0) {
            offset = (j - p.col + N) % N;
        } else {
            offset = (i - p.row + N) % N;
        }
        char expected = S[p.str_id][offset];
        int old_contrib = (old_val == expected) ? 0 : 1;
        int new_contrib = (new_val == expected) ? 0 : 1;
        int& mc = mismatch_count[pid];
        int old_mc = mc;
        mc = mc - old_contrib + new_contrib;
        if (old_mc == 0 && mc > 0) {
            int sid = p.str_id;
            str_match_count[sid]--;
            if (str_match_count[sid] == 0) c--;
        } else if (old_mc > 0 && mc == 0) {
            int sid = p.str_id;
            str_match_count[sid]++;
            if (str_match_count[sid] == 1) c++;
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> M;
    S.resize(M);
    total_len_sum = 0;
    for (int i = 0; i < M; i++) {
        cin >> S[i];
        total_len_sum += S[i].size();
    }
    per_cell_count = 2 * total_len_sum;

    // Reserve cell_placements capacity
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cell_placements[i][j].reserve(per_cell_count);
        }
    }

    // Generate placements
    placements.reserve(M * 2 * N * N);
    for (int idx = 0; idx < M; idx++) {
        int len = S[idx].size();
        // Horizontal
        for (int i = 0; i < N; i++) {
            for (int start = 0; start < N; start++) {
                Placement p{idx, 0, i, start, len};
                placements.push_back(p);
                int pid = placements.size() - 1;
                for (int offset = 0; offset < len; offset++) {
                    int col = (start + offset) % N;
                    cell_placements[i][col].push_back(pid);
                }
            }
        }
        // Vertical
        for (int j = 0; j < N; j++) {
            for (int start = 0; start < N; start++) {
                Placement p{idx, 1, start, j, len};
                placements.push_back(p);
                int pid = placements.size() - 1;
                for (int offset = 0; offset < len; offset++) {
                    int row = (start + offset) % N;
                    cell_placements[row][j].push_back(pid);
                }
            }
        }
    }

    int P = placements.size();
    mismatch_count.assign(P, 0);
    // Initialize grid with all '.'
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            grid[i][j] = '.';
        }
    }
    // Compute initial mismatches
    for (int pid = 0; pid < P; pid++) {
        const Placement& p = placements[pid];
        int cnt = 0;
        for (int offset = 0; offset < p.len; offset++) {
            int r, c;
            if (p.dir == 0) {
                r = p.row;
                c = (p.col + offset) % N;
            } else {
                r = (p.row + offset) % N;
                c = p.col;
            }
            if (grid[r][c] != S[p.str_id][offset]) cnt++;
        }
        mismatch_count[pid] = cnt;
    }

    str_match_count.assign(M, 0);
    for (int pid = 0; pid < P; pid++) {
        if (mismatch_count[pid] == 0) {
            str_match_count[placements[pid].str_id]++;
        }
    }
    c = 0;
    for (int i = 0; i < M; i++) if (str_match_count[i] > 0) c++;
    d = N * N; // all cells are '.'

    // Initialize best solution
    int best_c = c, best_d = d;
    long long best_score = (long long)best_c * 1000 + best_d;
    memcpy(best_grid, grid, sizeof(grid));

    // Simulated annealing
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> real_dist(0.0, 1.0);
    double T = 10000.0;
    const double cooling = 0.999;
    const int max_iter = 80000;

    for (int iter = 0; iter < max_iter; iter++) {
        int old_c = c;
        int old_d = d;
        long long old_score = (long long)old_c * 1000 + old_d;

        int i = rng() % N;
        int j = rng() % N;
        char old_char = grid[i][j];
        int r = rng() % 9;
        char new_char = (r < 8) ? ('A' + r) : '.';

        if (old_char != new_char) {
            apply_change(i, j, new_char);
        } else {
            // No change, just cool down
            T *= cooling;
            continue;
        }

        int new_c = c;
        int new_d = d;
        long long new_score = (long long)new_c * 1000 + new_d;

        bool accept = false;
        if (new_score >= old_score) {
            accept = true;
        } else {
            double prob = exp((new_score - old_score) / T);
            if (real_dist(rng) < prob) accept = true;
        }

        if (!accept) {
            // Revert
            apply_change(i, j, old_char);
        } else {
            if (new_score > best_score) {
                best_score = new_score;
                memcpy(best_grid, grid, sizeof(grid));
            }
        }

        T *= cooling;
    }

    // Output best grid
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            std::cout << best_grid[i][j];
        }
        std::cout << '\n';
    }

    return 0;
}