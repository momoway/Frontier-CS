#include <bits/stdc++.h>
using namespace std;

const int MAX_COORD = 10000;
const int OFFSET = 10000;
const int FORBIDDEN_SIZE = 20001;

struct Point {
    int x, y;
};

int N, K;
int a[11]; // a[1..10]
vector<Point> points;
vector<int> x_order, y_order;
bool forb_x[FORBIDDEN_SIZE];
bool forb_y[FORBIDDEN_SIZE];
bool used_x[FORBIDDEN_SIZE];
bool used_y[FORBIDDEN_SIZE];

double evaluate(const vector<int>& vert, const vector<int>& horiz) {
    int nv = vert.size();
    int nh = horiz.size();
    vector<int> iv(N);
    int v_ptr = 0;
    for (int idx = 0; idx < N; idx++) {
        int p_idx = x_order[idx];
        int x = points[p_idx].x;
        while (v_ptr < nv && vert[v_ptr] <= x) v_ptr++;
        iv[p_idx] = v_ptr;
    }
    vector<int> jh(N);
    int h_ptr = 0;
    for (int idx = 0; idx < N; idx++) {
        int p_idx = y_order[idx];
        int y = points[p_idx].y;
        while (h_ptr < nh && horiz[h_ptr] <= y) h_ptr++;
        jh[p_idx] = h_ptr;
    }
    int nvi = nv + 1;
    int nhi = nh + 1;
    int total_cells = nvi * nhi;
    vector<int> cell_counts(total_cells, 0);
    for (int i = 0; i < N; i++) {
        int idx = iv[i] * nhi + jh[i];
        cell_counts[idx]++;
    }
    vector<int> b(11, 0);
    for (int cnt : cell_counts) {
        if (cnt > 0 && cnt <= 10) b[cnt]++;
    }
    long long obj = 0;
    for (int d = 1; d <= 10; d++) {
        obj += min(a[d], b[d]);
    }
    return static_cast<double>(obj);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> N >> K;
    for (int d = 1; d <= 10; d++) {
        cin >> a[d];
    }
    points.resize(N);
    for (int i = 0; i < N; i++) {
        cin >> points[i].x >> points[i].y;
    }

    memset(forb_x, 0, sizeof(forb_x));
    memset(forb_y, 0, sizeof(forb_y));
    memset(used_x, 0, sizeof(used_x));
    memset(used_y, 0, sizeof(used_y));
    for (int i = 0; i < N; i++) {
        int idx = points[i].x + OFFSET;
        if (idx >= 0 && idx < FORBIDDEN_SIZE) forb_x[idx] = true;
        idx = points[i].y + OFFSET;
        if (idx >= 0 && idx < FORBIDDEN_SIZE) forb_y[idx] = true;
    }

    x_order.resize(N);
    iota(x_order.begin(), x_order.end(), 0);
    sort(x_order.begin(), x_order.end(), [&](int i, int j) {
        return points[i].x < points[j].x;
    });
    y_order.resize(N);
    iota(y_order.begin(), y_order.end(), 0);
    sort(y_order.begin(), y_order.end(), [&](int i, int j) {
        return points[i].y < points[j].y;
    });

    vector<int> vert, horiz;
    double cur_obj = evaluate(vert, horiz);
    double best_obj = cur_obj;
    vector<int> best_vert = vert;
    vector<int> best_horiz = horiz;

    mt19937 rng(random_device{}());
    uniform_int_distribution<int> dist_coord(-MAX_COORD, MAX_COORD);
    uniform_real_distribution<double> dist_real(0.0, 1.0);

    const int N_ITER = 50000;
    double temp = 1000.0;
    const double cooling = 0.99;

    for (int iter = 0; iter < N_ITER; iter++) {
        int total_lines = vert.size() + horiz.size();
        int move_type;
        if (total_lines == 0) {
            move_type = 0;
        } else if (total_lines == K) {
            move_type = (rng() % 2) + 1;
        } else {
            move_type = rng() % 3;
        }

        if (move_type == 0) { // Add
            if (total_lines >= K) continue;
            bool is_vertical = (rng() % 2 == 0);
            int coord = 0;
            bool found = false;
            for (int try_cnt = 0; try_cnt < 100; try_cnt++) {
                int c = dist_coord(rng);
                int idx = c + OFFSET;
                if (idx < 0 || idx >= FORBIDDEN_SIZE) continue;
                if (is_vertical) {
                    if (forb_x[idx] || used_x[idx]) continue;
                } else {
                    if (forb_y[idx] || used_y[idx]) continue;
                }
                coord = c;
                found = true;
                break;
            }
            if (!found) continue;

            if (is_vertical) {
                auto it = lower_bound(vert.begin(), vert.end(), coord);
                vert.insert(it, coord);
                used_x[coord + OFFSET] = true;
            } else {
                auto it = lower_bound(horiz.begin(), horiz.end(), coord);
                horiz.insert(it, coord);
                used_y[coord + OFFSET] = true;
            }

            double new_obj = evaluate(vert, horiz);
            double delta = new_obj - cur_obj;
            bool accept = false;
            if (delta > 0) accept = true;
            else {
                double prob = exp(delta / temp);
                if (dist_real(rng) < prob) accept = true;
            }
            if (accept) {
                cur_obj = new_obj;
                if (cur_obj > best_obj) {
                    best_obj = cur_obj;
                    best_vert = vert;
                    best_horiz = horiz;
                }
            } else {
                if (is_vertical) {
                    auto it = lower_bound(vert.begin(), vert.end(), coord);
                    if (it != vert.end() && *it == coord) {
                        vert.erase(it);
                        used_x[coord + OFFSET] = false;
                    }
                } else {
                    auto it = lower_bound(horiz.begin(), horiz.end(), coord);
                    if (it != horiz.end() && *it == coord) {
                        horiz.erase(it);
                        used_y[coord + OFFSET] = false;
                    }
                }
            }
        } else if (move_type == 1) { // Remove
            if (total_lines == 0) continue;
            bool is_vertical;
            int idx_line;
            if (!vert.empty() && !horiz.empty()) {
                if (rng() % 2 == 0) {
                    is_vertical = true;
                    idx_line = rng() % vert.size();
                } else {
                    is_vertical = false;
                    idx_line = rng() % horiz.size();
                }
            } else if (!vert.empty()) {
                is_vertical = true;
                idx_line = rng() % vert.size();
            } else {
                is_vertical = false;
                idx_line = rng() % horiz.size();
            }

            int old_coord;
            if (is_vertical) {
                old_coord = vert[idx_line];
                vert.erase(vert.begin() + idx_line);
                used_x[old_coord + OFFSET] = false;
            } else {
                old_coord = horiz[idx_line];
                horiz.erase(horiz.begin() + idx_line);
                used_y[old_coord + OFFSET] = false;
            }

            double new_obj = evaluate(vert, horiz);
            double delta = new_obj - cur_obj;
            bool accept = false;
            if (delta > 0) accept = true;
            else {
                double prob = exp(delta / temp);
                if (dist_real(rng) < prob) accept = true;
            }
            if (accept) {
                cur_obj = new_obj;
                if (cur_obj > best_obj) {
                    best_obj = cur_obj;
                    best_vert = vert;
                    best_horiz = horiz;
                }
            } else {
                if (is_vertical) {
                    auto it = lower_bound(vert.begin(), vert.end(), old_coord);
                    vert.insert(it, old_coord);
                    used_x[old_coord + OFFSET] = true;
                } else {
                    auto it = lower_bound(horiz.begin(), horiz.end(), old_coord);
                    horiz.insert(it, old_coord);
                    used_y[old_coord + OFFSET] = true;
                }
            }
        } else { // Move
            if (total_lines == 0) continue;
            bool is_vertical;
            int idx_line;
            if (!vert.empty() && !horiz.empty()) {
                if (rng() % 2 == 0) {
                    is_vertical = true;
                    idx_line = rng() % vert.size();
                } else {
                    is_vertical = false;
                    idx_line = rng() % horiz.size();
                }
            } else if (!vert.empty()) {
                is_vertical = true;
                idx_line = rng() % vert.size();
            } else {
                is_vertical = false;
                idx_line = rng() % horiz.size();
            }

            int old_coord;
            if (is_vertical) old_coord = vert[idx_line];
            else old_coord = horiz[idx_line];

            int new_coord = 0;
            bool found = false;
            for (int try_cnt = 0; try_cnt < 100; try_cnt++) {
                int c = dist_coord(rng);
                int idx = c + OFFSET;
                if (idx < 0 || idx >= FORBIDDEN_SIZE) continue;
                if (is_vertical) {
                    if (forb_x[idx] || used_x[idx]) continue;
                } else {
                    if (forb_y[idx] || used_y[idx]) continue;
                }
                new_coord = c;
                found = true;
                break;
            }
            if (!found) continue;
            if (new_coord == old_coord) continue;

            if (is_vertical) {
                vert.erase(vert.begin() + idx_line);
                used_x[old_coord + OFFSET] = false;
                auto it = lower_bound(vert.begin(), vert.end(), new_coord);
                vert.insert(it, new_coord);
                used_x[new_coord + OFFSET] = true;
            } else {
                horiz.erase(horiz.begin() + idx_line);
                used_y[old_coord + OFFSET] = false;
                auto it = lower_bound(horiz.begin(), horiz.end(), new_coord);
                horiz.insert(it, new_coord);
                used_y[new_coord + OFFSET] = true;
            }

            double new_obj = evaluate(vert, horiz);
            double delta = new_obj - cur_obj;
            bool accept = false;
            if (delta > 0) accept = true;
            else {
                double prob = exp(delta / temp);
                if (dist_real(rng) < prob) accept = true;
            }
            if (accept) {
                cur_obj = new_obj;
                if (cur_obj > best_obj) {
                    best_obj = cur_obj;
                    best_vert = vert;
                    best_horiz = horiz;
                }
            } else {
                if (is_vertical) {
                    auto it = lower_bound(vert.begin(), vert.end(), new_coord);
                    if (it != vert.end() && *it == new_coord) {
                        vert.erase(it);
                        used_x[new_coord + OFFSET] = false;
                    }
                    it = lower_bound(vert.begin(), vert.end(), old_coord);
                    vert.insert(it, old_coord);
                    used_x[old_coord + OFFSET] = true;
                } else {
                    auto it = lower_bound(horiz.begin(), horiz.end(), new_coord);
                    if (it != horiz.end() && *it == new_coord) {
                        horiz.erase(it);
                        used_y[new_coord + OFFSET] = false;
                    }
                    it = lower_bound(horiz.begin(), horiz.end(), old_coord);
                    horiz.insert(it, old_coord);
                    used_y[old_coord + OFFSET] = true;
                }
            }
        }

        temp *= cooling;
    }

    cout << best_vert.size() + best_horiz.size() << "\n";
    for (int x : best_vert) {
        cout << x << " " << -MAX_COORD << " " << x << " " << MAX_COORD << "\n";
    }
    for (int y : best_horiz) {
        cout << -MAX_COORD << " " << y << " " << MAX_COORD << " " << y << "\n";
    }

    return 0;
}