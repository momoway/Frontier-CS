#include <bits/stdc++.h>
using namespace std;

int largestComponent(const vector<int>& board, int N) {
    static int parent[100];
    static int compSize[100];
    int total = N * N;
    for (int i = 0; i < total; ++i) {
        if (board[i] == 0) {
            parent[i] = -1;
            compSize[i] = 0;
        } else {
            parent[i] = i;
            compSize[i] = 1;
        }
    }
    function<int(int)> find = [&](int x) {
        if (parent[x] != x) parent[x] = find(parent[x]);
        return parent[x];
    };
    auto unite = [&](int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return;
        if (compSize[a] < compSize[b]) swap(a, b);
        parent[b] = a;
        compSize[a] += compSize[b];
    };
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int idx = i * N + j;
            int v = board[idx];
            if (v == 0) continue;
            // up
            if (i > 0) {
                int nidx = (i - 1) * N + j;
                if (board[nidx] != 0) {
                    int nv = board[nidx];
                    if ((v & 2) && (nv & 8)) unite(idx, nidx);
                }
            }
            // down
            if (i < N - 1) {
                int nidx = (i + 1) * N + j;
                if (board[nidx] != 0) {
                    int nv = board[nidx];
                    if ((v & 8) && (nv & 2)) unite(idx, nidx);
                }
            }
            // left
            if (j > 0) {
                int nidx = i * N + (j - 1);
                if (board[nidx] != 0) {
                    int nv = board[nidx];
                    if ((v & 1) && (nv & 4)) unite(idx, nidx);
                }
            }
            // right
            if (j < N - 1) {
                int nidx = i * N + (j + 1);
                if (board[nidx] != 0) {
                    int nv = board[nidx];
                    if ((v & 4) && (nv & 1)) unite(idx, nidx);
                }
            }
        }
    }
    int maxSize = 0;
    for (int i = 0; i < total; ++i) {
        if (parent[i] == i && compSize[i] > maxSize) maxSize = compSize[i];
    }
    return maxSize;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int N, T;
    if (!(cin >> N >> T)) return 0;
    vector<int> board(N * N);
    for (int i = 0; i < N; ++i) {
        string s;
        cin >> s;
        for (int j = 0; j < N; ++j) {
            char c = s[j];
            if (c >= '0' && c <= '9') board[i * N + j] = c - '0';
            else board[i * N + j] = c - 'a' + 10;
        }
    }
    int empty_idx = -1;
    for (int i = 0; i < N * N; ++i) if (board[i] == 0) { empty_idx = i; break; }
    int empty_i = empty_idx / N, empty_j = empty_idx % N;
    const int target = N * N - 1;

    // Directions: up(0), right(1), down(2), left(3)
    const int dx[4] = { -1, 0, 1, 0 };
    const int dy[4] = { 0, 1, 0, -1 };
    const char dirChar[4] = { 'D', 'L', 'U', 'R' }; // mapping: up->D, right->L, down->U, left->R

    string moves;
    int cur_size = largestComponent(board, N);
    if (cur_size == target) {
        cout << moves << "\n";
        return 0;
    }

    for (int step = 0; step < T && cur_size < target; ++step) {
        struct Candidate {
            int dir;
            int size1;
            int new_idx;
            int best_next;
        };
        vector<Candidate> cand;
        bool found_full = false;
        int full_dir = -1;

        // Evaluate each possible move
        for (int d = 0; d < 4; ++d) {
            int ni = empty_i + dx[d];
            int nj = empty_j + dy[d];
            if (ni < 0 || ni >= N || nj < 0 || nj >= N) continue;
            int nidx = ni * N + nj;
            // simulate move
            swap(board[empty_idx], board[nidx]);
            int sz = largestComponent(board, N);
            if (sz == target) {
                found_full = true;
                full_dir = d;
                swap(board[empty_idx], board[nidx]); // undo
                break;
            }
            cand.push_back({d, sz, nidx, -1});
            swap(board[empty_idx], board[nidx]); // undo
        }
        if (found_full) {
            // perform the full move
            int ni = empty_i + dx[full_dir];
            int nj = empty_j + dy[full_dir];
            int nidx = ni * N + nj;
            swap(board[empty_idx], board[nidx]);
            empty_i = ni; empty_j = nj; empty_idx = nidx;
            moves.push_back(dirChar[full_dir]);
            cur_size = target;
            break;
        }

        if (cand.empty()) {
            // no moves? should not happen, but break
            break;
        }

        // Find max size1
        int max_size1 = -1;
        for (auto &c : cand) if (c.size1 > max_size1) max_size1 = c.size1;

        // For candidates with size1 == max_size1, compute best_next (best size after one more move)
        for (auto &c : cand) {
            if (c.size1 != max_size1) continue;
            // simulate first move
            swap(board[empty_idx], board[c.new_idx]);
            int new_empty_i = c.new_idx / N, new_empty_j = c.new_idx % N;
            int best_next = -1;
            // try each second move (exclude reverse)
            for (int d2 = 0; d2 < 4; ++d2) {
                if (d2 == (c.dir ^ 2)) continue; // reverse direction
                int ni2 = new_empty_i + dx[d2];
                int nj2 = new_empty_j + dy[d2];
                if (ni2 < 0 || ni2 >= N || nj2 < 0 || nj2 >= N) continue;
                int nidx2 = ni2 * N + nj2;
                swap(board[c.new_idx], board[nidx2]);
                int sz2 = largestComponent(board, N);
                if (sz2 > best_next) best_next = sz2;
                swap(board[c.new_idx], board[nidx2]); // undo second move
            }
            // undo first move
            swap(board[empty_idx], board[c.new_idx]);
            c.best_next = best_next;
        }

        // Choose candidate with highest best_next (among those with max_size1)
        int best_idx = -1;
        int best_best_next = -1;
        for (int i = 0; i < (int)cand.size(); ++i) {
            if (cand[i].size1 != max_size1) continue;
            if (cand[i].best_next > best_best_next) {
                best_best_next = cand[i].best_next;
                best_idx = i;
            }
        }
        if (best_idx == -1) {
            // all best_next are -1? pick any with max_size1
            for (int i = 0; i < (int)cand.size(); ++i) {
                if (cand[i].size1 == max_size1) {
                    best_idx = i;
                    break;
                }
            }
        }
        // Perform the chosen move
        auto &best = cand[best_idx];
        swap(board[empty_idx], board[best.new_idx]);
        empty_i = best.new_idx / N;
        empty_j = best.new_idx % N;
        empty_idx = best.new_idx;
        moves.push_back(dirChar[best.dir]);
        cur_size = best.size1;
    }

    cout << moves << "\n";
    return 0;
}