#include <bits/stdc++.h>
using namespace std;

bool is_subsequence(const vector<int>& order, const vector<int>& q) {
    int i = 0;
    for (int x : order) {
        if (i < (int)q.size() && x == q[i]) i++;
    }
    return i == (int)q.size();
}

vector<pair<int,int>> construct_path(const vector<int>& order, int n, int m, int L, int R) {
    int w = R - L + 1;
    vector<pair<int,int>> path;
    if (order.empty()) return path;

    int cur_row = order[0];
    int cur_col = L;
    path.push_back({cur_row, cur_col});
    // Traverse first row from L to R (excluding the first cell which is already added)
    for (int col = L+1; col <= R; ++col) {
        path.push_back({cur_row, col});
    }
    // After first row, we are at (cur_row, R)
    int pos = 1; // 1-indexed position of current row in the order

    for (size_t idx = 1; idx < order.size(); ++idx) {
        int next_row = order[idx];
        // Determine exit column of current row based on its position pos
        int exit_col = (pos % 2 == 1) ? R : L; // odd -> R, even -> L
        int entry_col = exit_col; // same as exit column

        if (abs(cur_row - next_row) == 1) {
            // Adjacent move: vertical step to next_row at exit_col (which is entry_col)
            path.push_back({next_row, entry_col}); // this is the entry cell
            // Traverse next row from entry to exit
            int next_pos = pos + 1;
            int next_exit_col = (next_pos % 2 == 1) ? R : L;
            if (entry_col == L) {
                for (int col = L+1; col <= R; ++col) {
                    path.push_back({next_row, col});
                }
            } else {
                for (int col = R-1; col >= L; --col) {
                    path.push_back({next_row, col});
                }
            }
            cur_row = next_row;
            cur_col = next_exit_col;
        } else {
            // Need to use a highway column outside [L,R]
            int highway_col;
            if (exit_col > 1) {
                highway_col = exit_col - 1;
            } else if (exit_col < m) {
                highway_col = exit_col + 1;
            } else {
                // Should not happen if feasibility checked, but set to avoid compile error
                highway_col = exit_col;
            }
            // Move to highway column in current row
            path.push_back({cur_row, highway_col});
            // Move vertically along highway column to next_row
            if (cur_row < next_row) {
                for (int r = cur_row+1; r <= next_row; ++r) {
                    path.push_back({r, highway_col});
                }
            } else {
                for (int r = cur_row-1; r >= next_row; --r) {
                    path.push_back({r, highway_col});
                }
            }
            // Enter next row at entry_col
            path.push_back({next_row, entry_col});
            // Traverse next row from entry to exit
            int next_pos = pos + 1;
            int next_exit_col = (next_pos % 2 == 1) ? R : L;
            if (entry_col == L) {
                for (int col = L+1; col <= R; ++col) {
                    path.push_back({next_row, col});
                }
            } else {
                for (int col = R-1; col >= L; --col) {
                    path.push_back({next_row, col});
                }
            }
            cur_row = next_row;
            cur_col = next_exit_col;
        }
        pos++; // move to next position
    }
    return path;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m, L, R, Sx, Sy, Lq, s;
    if (!(cin >> n >> m >> L >> R >> Sx >> Sy >> Lq >> s)) return 0;
    vector<int> q(Lq);
    for (int i = 0; i < Lq; ++i) cin >> q[i];

    // Ensure Sy = L (as guaranteed)
    // Sy is not used further.

    int w = R - L + 1;

    // Case: required rectangle covers whole grid
    if (L == 1 && R == m) {
        if (Sx != 1 && Sx != n) {
            cout << "NO\n";
            return 0;
        }
        vector<int> order;
        if (Sx == 1) {
            for (int i = 1; i <= n; ++i) order.push_back(i);
        } else {
            for (int i = n; i >= 1; --i) order.push_back(i);
        }
        if (!is_subsequence(order, q)) {
            cout << "NO\n";
            return 0;
        }
        vector<pair<int,int>> path = construct_path(order, n, m, L, R);
        cout << "YES\n" << path.size() << "\n";
        for (auto &p : path) cout << p.first << " " << p.second << "\n";
        return 0;
    }

    // General case: there is at least one non-required column
    vector<int> order_up, order_down;
    bool up_ok = false, down_ok = false;
    int extra_up = 0, extra_down = 0;

    // Build up order: Sx, Sx-1, ..., 1, Sx+1, ..., n
    if (Sx == 1) {
        for (int i = 1; i <= n; ++i) order_up.push_back(i);
        up_ok = true;
        extra_up = 0;
    } else if (Sx == n) {
        for (int i = n; i >= 1; --i) order_up.push_back(i);
        up_ok = true;
        extra_up = 0;
    } else {
        for (int i = Sx; i >= 1; --i) order_up.push_back(i);
        for (int i = Sx+1; i <= n; ++i) order_up.push_back(i);
        // Check highway for jump from row1 to row Sx+1
        int pos_row1 = Sx; // position of row1 in this order
        int exit_col_row1 = (pos_row1 % 2 == 1) ? R : L;
        if (exit_col_row1 == L) {
            up_ok = (L > 1);
        } else { // R
            up_ok = (R < m);
        }
        extra_up = Sx + 1;
    }

    // Build down order: Sx, Sx+1, ..., n, Sx-1, ..., 1
    if (Sx == 1) {
        for (int i = 1; i <= n; ++i) order_down.push_back(i);
        down_ok = true;
        extra_down = 0;
    } else if (Sx == n) {
        for (int i = n; i >= 1; --i) order_down.push_back(i);
        down_ok = true;
        extra_down = 0;
    } else {
        for (int i = Sx; i <= n; ++i) order_down.push_back(i);
        for (int i = Sx-1; i >= 1; --i) order_down.push_back(i);
        int pos_rown = n - Sx + 1; // position of row n
        int exit_col_rown = (pos_rown % 2 == 1) ? R : L;
        if (exit_col_rown == L) {
            down_ok = (L > 1);
        } else {
            down_ok = (R < m);
        }
        extra_down = n - Sx + 2;
    }

    bool q_in_up = false, q_in_down = false;
    if (up_ok) q_in_up = is_subsequence(order_up, q);
    if (down_ok) q_in_down = is_subsequence(order_down, q);

    if (!q_in_up && !q_in_down) {
        cout << "NO\n";
        return 0;
    }

    const vector<int> *chosen_order = nullptr;
    if (q_in_up && q_in_down) {
        if (extra_up <= extra_down) chosen_order = &order_up;
        else chosen_order = &order_down;
    } else if (q_in_up) {
        chosen_order = &order_up;
    } else {
        chosen_order = &order_down;
    }

    vector<pair<int,int>> path = construct_path(*chosen_order, n, m, L, R);
    cout << "YES\n" << path.size() << "\n";
    for (auto &p : path) {
        cout << p.first << " " << p.second << "\n";
    }

    return 0;
}