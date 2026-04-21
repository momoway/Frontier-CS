#include <bits/stdc++.h>
using namespace std;

using State = uint64_t;
struct HashState {
    size_t operator()(State x) const {
        x += 0x9e3779b97f4a7c15ULL;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049163f774ed9ULL;
        return x ^ (x >> 31);
    }
};

int n;
int orient_global[10];
int len_global[10];

uint64_t pack_state(int n, const array<pair<int,int>,10>& pos) {
    uint64_t mask = 0;
    for (int i = 0; i < n; i++) {
        uint64_t bits;
        if (orient_global[i] == 0) { // horizontal
            bits = ((uint64_t)(pos[i].first) << 3) | (uint64_t)(pos[i].second);
        } else { // vertical
            bits = ((uint64_t)(pos[i].second) << 3) | (uint64_t)(pos[i].first);
        }
        mask |= bits << (6*i);
    }
    return mask;
}

void unpack_state(uint64_t mask, int n, array<pair<int,int>,10>& pos) {
    for (int i = 0; i < n; i++) {
        uint64_t bits = (mask >> (6*i)) & 0x3F;
        if (orient_global[i] == 0) {
            pos[i].first = (bits >> 3) & 0x7;
            pos[i].second = bits & 0x7;
        } else {
            pos[i].second = (bits >> 3) & 0x7;
            pos[i].first = bits & 0x7;
        }
    }
}

void compute_board(const array<pair<int,int>,10>& pos, int board[6][6]) {
    for (int i = 0; i < 6; i++) for (int j = 0; j < 6; j++) board[i][j] = -1;
    for (int i = 0; i < n; i++) {
        int r = pos[i].first;
        int c = pos[i].second;
        for (int k = 0; k < len_global[i]; k++) {
            int rr = r + (orient_global[i] ? k : 0);
            int cc = c + (orient_global[i] ? 0 : k);
            if (rr >= 0 && rr < 6 && cc >= 0 && cc < 6) {
                board[rr][cc] = i;
            }
        }
    }
}

bool can_move(const array<pair<int,int>,10>& pos, int i, int dir) {
    int r = pos[i].first;
    int c = pos[i].second;
    int L = len_global[i];
    int new_r = r, new_c = c;
    if (dir == 0) new_r = r - 1; // up
    else if (dir == 1) new_r = r + 1; // down
    else if (dir == 2) new_c = c - 1; // left
    else if (dir == 3) new_c = c + 1; // right

    // Check at least one cell on board
    bool at_least_one = false;
    if (orient_global[i] == 0) { // horizontal
        for (int k = 0; k < L; k++) {
            int col = new_c + k;
            if (col >= 0 && col < 6) {
                at_least_one = true;
                break;
            }
        }
    } else { // vertical
        for (int k = 0; k < L; k++) {
            int row = new_r + k;
            if (row >= 0 && row < 6) {
                at_least_one = true;
                break;
            }
        }
    }
    if (!at_least_one) return false;

    // For non-red vehicles, require fully on board
    if (i != 0) {
        if (orient_global[i] == 0) {
            if (new_c < 0 || new_c + L - 1 >= 6) return false;
        } else {
            if (new_r < 0 || new_r + L - 1 >= 6) return false;
        }
    } else {
        // red car special
        if (dir == 2) { // left
            if (new_c < 0 || new_c + L - 1 >= 6) return false;
        } else if (dir == 3) { // right
            // allow partial off to the right, already checked at_least_one
            // nothing more
        }
    }

    // Check collisions
    int board[6][6];
    compute_board(pos, board);
    if (orient_global[i] == 0) {
        for (int k = 0; k < L; k++) {
            int col = new_c + k;
            if (col >= 0 && col < 6) {
                int row = new_r;
                if (board[row][col] != -1 && board[row][col] != i) {
                    return false;
                }
            }
        }
    } else {
        for (int k = 0; k < L; k++) {
            int row = new_r + k;
            if (row >= 0 && row < 6) {
                int col = new_c;
                if (board[row][col] != -1 && board[row][col] != i) {
                    return false;
                }
            }
        }
    }
    return true;
}

uint64_t apply_move_mask(uint64_t mask, int i, int dir) {
    array<pair<int,int>,10> pos;
    unpack_state(mask, n, pos);
    if (dir == 0) pos[i].first--;
    else if (dir == 1) pos[i].first++;
    else if (dir == 2) pos[i].second--;
    else if (dir == 3) pos[i].second++;
    return pack_state(n, pos);
}

int opposite_dir(int d) {
    if (d == 0) return 1;
    if (d == 1) return 0;
    if (d == 2) return 3;
    if (d == 3) return 2;
    return -1;
}

int heuristic_func(const array<pair<int,int>,10>& pos) {
    int red_left = pos[0].second; // red car horizontal, row=2
    int h1 = 5 - red_left;
    int blockers = 0;
    for (int i = 1; i < n; i++) {
        if (orient_global[i] == 0 && pos[i].first == 2) { // horizontal on same row
            int v_left = pos[i].second;
            int v_right = v_left + len_global[i] - 1;
            int block_start = max(red_left + 2, v_left);
            int block_end = min(5, v_right);
            if (block_start <= block_end) blockers++;
        } else if (orient_global[i] == 1) { // vertical
            int col = pos[i].second;
            if (col >= red_left + 2 && col <= 5) {
                int top = pos[i].first;
                if (top <= 2 && top + len_global[i] - 1 >= 2) blockers++;
            }
        }
    }
    return h1 + blockers;
}

bool dfs(int depth, int bound, int last_vid, int last_dir,
         vector<pair<int,int>>& path_moves,
         uint64_t& exit_state, vector<pair<int,int>>& exit_moves,
         array<pair<int,int>,10>& cur_pos,
         vector<uint64_t>& path_states) {
    int h = heuristic_func(cur_pos);
    if (depth + h > bound) return false;
    if (cur_pos[0].second == 5) {
        exit_state = pack_state(n, cur_pos);
        exit_moves = path_moves;
        return true;
    }
    for (int i = 0; i < n; i++) {
        int dirs[2];
        int dir_cnt;
        if (orient_global[i] == 0) { dirs[0] = 2; dirs[1] = 3; dir_cnt = 2; }
        else { dirs[0] = 0; dirs[1] = 1; dir_cnt = 2; }
        for (int k = 0; k < dir_cnt; k++) {
            int d = dirs[k];
            if (i == last_vid && d == opposite_dir(last_dir)) continue;
            if (!can_move(cur_pos, i, d)) continue;
            pair<int,int> old = cur_pos[i];
            if (d == 0) cur_pos[i].first--;
            else if (d == 1) cur_pos[i].first++;
            else if (d == 2) cur_pos[i].second--;
            else if (d == 3) cur_pos[i].second++;
            uint64_t new_mask = pack_state(n, cur_pos);
            if (find(path_states.begin(), path_states.end(), new_mask) != path_states.end()) {
                cur_pos[i] = old;
                continue;
            }
            path_states.push_back(new_mask);
            path_moves.push_back({i, d});
            if (dfs(depth+1, bound, i, d, path_moves, exit_state, exit_moves)) return true;
            path_moves.pop_back();
            path_states.pop_back();
            cur_pos[i] = old;
        }
    }
    return false;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    // Read input
    array<array<int,6>,6> grid_inp;
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            if (!(cin >> grid_inp[i][j])) return 0;
        }
    }

    // Build vehicles
    vector<vector<pair<int,int>>> veh_cells(11); // ids 1..10
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            int id = grid_inp[i][j];
            if (id > 0) {
                veh_cells[id].push_back({i, j});
            }
        }
    }

    int num_veh = 0;
    for (int id = 1; id <= 10; id++) {
        if (!veh_cells[id].empty()) num_veh++;
    }

    // Initialize global arrays
    n = num_veh;
    // We'll store vehicles in order of id: index = id-1
    // So we need arrays of size 10
    static int orient_global[10];
    static int len_global[10];
    array<pair<int,int>,10> initial_pos; // for indices 0..n-1

    for (int id = 1; id <= 10; id++) {
        if (veh_cells[id].empty()) continue;
        int idx = id - 1;
        vector<pair<int,int>>& cells = veh_cells[id];
        // Determine orientation
        bool same_row = true;
        bool same_col = true;
        int first_row = cells[0].first, first_col = cells[0].second;
        for (auto& p : cells) {
            if (p.first != first_row) same_row = false;
            if (p.second != first_col) same_col = false;
        }
        if (same_row && same_col) {
            // single cell? but vehicles length 2 or 3, so not possible
            // treat as horizontal? but shouldn't happen
        }
        if (same_row) {
            orient_global[idx] = 0; // horizontal
            len_global[idx] = cells.size();
            // leftmost column
            int min_col = first_col;
            for (auto& p : cells) min_col = min(min_col, p.second);
            initial_pos[idx] = {first_row, min_col};
        } else if (same_col) {
            orient_global[idx] = 1; // vertical
            len_global[idx] = cells.size();
            // topmost row
            int min_row = first_row;
            for (auto& p : cells) min_row = min(min_row, p.first);
            initial_pos[idx] = {min_row, first_col};
        } else {
            // invalid, but problem guarantees valid
            orient_global[idx] = 0;
            len_global[idx] = 2;
            initial_pos[idx] = {cells[0].first, cells[0].second};
        }
    }

    // IDA* to find exit state (red car leftmost=5) and path to it
    array<pair<int,int>,10> cur_pos = initial_pos;
    int bound = heuristic_func(cur_pos);
    uint64_t exit_state_mask = 0;
    vector<pair<int,int>> exit_moves_vec;
    bool found = false;
    while (true) {
        vector<uint64_t> path_states;
        path_states.push_back(pack_state(n, cur_pos));
        vector<pair<int,int>> path_moves;
        if (dfs(0, bound, -1, -1, path_moves, exit_state_mask, exit_moves_vec)) {
            found = true;
            break;
        }
        bound++;
    }

    // BFS from exit_state_mask to compute distances to goal (completely off)
    struct Node {
        uint16_t dist;
        uint8_t move; // (dir<<4 | id)
    };
    unordered_map<uint64_t, int, HashState> idx_map;
    vector<Node> nodes;
    queue<uint64_t> q;

    idx_map[exit_state_mask] = 0;
    nodes.push_back({1, 0}); // dist=1 (one move to goal), move=0 sentinel
    q.push(exit_state_mask);

    uint64_t best_state = exit_state_mask;
    int max_dist = 1;

    while (!q.empty()) {
        uint64_t cur_mask = q.front(); q.pop();
        int idx = idx_map[cur_mask];
        uint16_t cur_dist = nodes[idx].dist;
        if (cur_dist > max_dist) {
            max_dist = cur_dist;
            best_state = cur_mask;
        }
        // generate moves
        array<pair<int,int>,10> pos;
        unpack_state(cur_mask, n, pos);
        for (int i = 0; i < n; i++) {
            int dirs[2];
            int dir_cnt;
            if (orient_global[i] == 0) { dirs[0] = 2; dirs[1] = 3; dir_cnt = 2; }
            else { dirs[0] = 0; dirs[1] = 1; dir_cnt = 2; }
            for (int k = 0; k < dir_cnt; k++) {
                int d = dirs[k];
                if (!can_move(pos, i, d)) continue;
                uint64_t new_mask = apply_move_mask(cur_mask, i, d);
                if (idx_map.find(new_mask) == idx_map.end()) {
                    idx_map[new_mask] = nodes.size();
                    nodes.push_back({ (uint16_t)(cur_dist + 1), (uint8_t)((d << 4) | i) });
                    q.push(new_mask);
                }
            }
        }
    }

    // Reconstruct S2 from best_state to exit_state
    vector<pair<int,int>> s2_moves;
    uint64_t cur = best_state;
    while (cur != exit_state_mask) {
        int idx = idx_map[cur];
        uint8_t move_code = nodes[idx].move;
        int d = move_code >> 4;
        int i = move_code & 0xF;
        int opp_d = opposite_dir(d);
        s2_moves.push_back({i, d});
        cur = apply_move_mask(cur, i, opp_d);
    }
    reverse(s2_moves.begin(), s2_moves.end());

    // Concatenate sequences
    vector<pair<int,int>> full_seq = exit_moves_vec;
    full_seq.insert(full_seq.end(), s2_moves.begin(), s2_moves.end());

    // Output
    cout << max_dist << " " << full_seq.size() << "\n";
    for (auto& m : full_seq) {
        char dc;
        if (m.second == 0) dc = 'U';
        else if (m.second == 1) dc = 'D';
        else if (m.second == 2) dc = 'L';
        else dc = 'R';
        cout << (m.first + 1) << " " << dc << "\n";
    }

    return 0;
}