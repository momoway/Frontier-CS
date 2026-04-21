#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const int NUM_ORDERS = 1000;
    const int SELECT_COUNT = 50;
    const int DEPOT_X = 400, DEPOT_Y = 400;
    const int depot_start_idx = 0;
    const int depot_end_idx = 2001; // after adding starts and ends

    vector<int> xs(2002), ys(2002);
    xs[depot_start_idx] = DEPOT_X;
    ys[depot_start_idx] = DEPOT_Y;

    struct Order { int a, b, c, d; };
    vector<Order> orders(NUM_ORDERS);
    for (int i = 0; i < NUM_ORDERS; ++i) {
        cin >> orders[i].a >> orders[i].b >> orders[i].c >> orders[i].d;
    }

    // Assign coordinates: starts at 1..1000, ends at 1001..2000
    for (int i = 0; i < NUM_ORDERS; ++i) {
        xs[i+1] = orders[i].a;
        ys[i+1] = orders[i].b;
        xs[1001+i] = orders[i].c;
        ys[1001+i] = orders[i].d;
    }
    xs[depot_end_idx] = DEPOT_X;
    ys[depot_end_idx] = DEPOT_Y;

    // Precompute Manhattan distances
    vector<vector<int>> dist(2002, vector<int>(2002));
    for (int i = 0; i < 2002; ++i) {
        for (int j = 0; j < 2002; ++j) {
            dist[i][j] = abs(xs[i] - xs[j]) + abs(ys[i] - ys[j]);
        }
    }

    // Compute scores for each order: 2*(depot-start + end-depot) + start-end
    vector<pair<long long, int>> scores;
    scores.reserve(NUM_ORDERS);
    for (int i = 0; i < NUM_ORDERS; ++i) {
        int start_idx = i+1;
        int end_idx = 1001 + i;
        long long d_depot_start = dist[depot_start_idx][start_idx];
        long long d_end_depot = dist[end_idx][depot_end_idx];
        long long d_start_end = dist[start_idx][end_idx];
        long long score = 2*(d_depot_start + d_end_depot) + d_start_end;
        scores.emplace_back(score, i);
    }
    sort(scores.begin(), scores.end(),
        [](const pair<long long,int>& a, const pair<long long,int>& b) {
            return a.first < b.first;
        });

    vector<int> selected(SELECT_COUNT);
    vector<int> pos_in_selected(NUM_ORDERS, -1);
    for (int i = 0; i < SELECT_COUNT; ++i) {
        selected[i] = scores[i].second;
        pos_in_selected[selected[i]] = i;
    }

    // Prepare data for selected orders
    vector<int> start_node(SELECT_COUNT);
    vector<int> end_node(SELECT_COUNT);
    vector<int> node_order(101); // 1..100
    vector<char> node_is_start(101);
    for (int i = 0; i < SELECT_COUNT; ++i) {
        start_node[i] = i+1;
        end_node[i] = 51 + i;
        node_order[i+1] = i;
        node_is_start[i+1] = true;
        node_order[51+i] = i;
        node_is_start[51+i] = false;
    }

    // Nearest insertion to build initial route
    vector<int> route;
    route.push_back(depot_start_idx);
    long long total_cur = 0;
    vector<int> pos(2002, -1);
    pos[depot_start_idx] = 0;

    vector<int> available;
    for (int id = 1; id <= 50; ++id) available.push_back(id);
    vector<char> is_inserted(101, false);

    while (!available.empty()) {
        long long best_delta = LLONG_MAX;
        int best_v = -1;
        int best_k = -1; // insert between route[k] and route[k+1]; -1 means append

        for (int v : available) {
            // Insertion between existing nodes
            for (int k = 0; k+1 < (int)route.size(); ++k) {
                int u = route[k];
                int w = route[k+1];
                long long delta = (long long)dist[u][v] + dist[v][w] - dist[u][w];
                if (delta < best_delta) {
                    best_delta = delta;
                    best_v = v;
                    best_k = k;
                }
            }
            // Append at end
            int last = route.back();
            long long delta_append = dist[last][v];
            if (delta_append < best_delta) {
                best_delta = delta_append;
                best_v = v;
                best_k = -1;
            }
        }

        if (best_k >= 0) {
            route.insert(route.begin() + best_k + 1, best_v);
            total_cur += best_delta;
        } else {
            route.push_back(best_v);
            total_cur += best_delta;
        }

        is_inserted[best_v] = true;
        // Remove from available
        for (auto it = available.begin(); it != available.end(); ++it) {
            if (*it == best_v) {
                available.erase(it);
                break;
            }
        }
        // If start node, add its end to available
        if (node_is_start[best_v]) {
            int order_idx = node_order[best_v];
            int end_id = end_node[order_idx];
            if (!is_inserted[end_id]) {
                available.push_back(end_id);
            }
        }
        // Update pos
        for (int i = 0; i < (int)route.size(); ++i) {
            pos[route[i]] = i;
        }
    }

    // Append depot_end
    route.push_back(depot_end_idx);
    long long total_final = total_cur + dist[route[100]][depot_end_idx];
    int n = route.size(); // 102

    // Update pos for all nodes in route
    for (int i = 0; i < n; ++i) pos[route[i]] = i;

    // Local search
    bool improved = true;
    int iter = 0;
    while (improved && iter < 1000) {
        improved = false;
        ++iter;

        // 2-opt first-improvement
        for (int i = 0; i <= n-3 && !improved; ++i) {
            for (int j = i+1; j <= n-3 && !improved; ++j) {
                int u1 = route[i], v1 = route[i+1];
                int u2 = route[j], v2 = route[j+1];
                long long delta = (long long)dist[u1][u2] + dist[v1][v2] - dist[u1][v1] - dist[u2][v2];
                if (delta >= 0) continue;
                // Check feasibility: no order has both nodes in segment [i+1, j]
                bool feasible = true;
                for (int o = 0; o < SELECT_COUNT; ++o) {
                    int s = start_node[o], e = end_node[o];
                    int ps = pos[s], pe = pos[e];
                    if (ps >= i+1 && ps <= j && pe >= i+1 && pe <= j) {
                        feasible = false;
                        break;
                    }
                }
                if (!feasible) continue;
                // Apply move
                reverse(route.begin() + i+1, route.begin() + j+1);
                total_final += delta;
                for (int k = 0; k < n; ++k) pos[route[k]] = k;
                improved = true;
            }
        }

        if (improved) continue;

        // Relocation first-improvement
        for (int idx = 0; idx < n && !improved; ++idx) {
            int v = route[idx];
            if (v == depot_start_idx || v == depot_end_idx) continue;
            for (int new_pos = 1; new_pos <= n-2 && !improved; ++new_pos) {
                if (new_pos == idx) continue;
                // Compute delta
                int old_prev = route[idx-1];
                int old_next = route[idx+1];
                long long delta_remove = (long long)dist[old_prev][old_next] - dist[old_prev][v] - dist[v][old_next];
                long long delta_insert;
                if (new_pos < idx) {
                    int new_prev = route[new_pos-1];
                    int new_next = route[new_pos];
                    delta_insert = (long long)dist[new_prev][v] + dist[v][new_next] - dist[new_prev][new_next];
                } else { // new_pos > idx
                    int new_prev = (new_pos-1 < idx) ? route[new_pos-1] : route[new_pos];
                    int new_next = route[new_pos+1];
                    delta_insert = (long long)dist[new_prev][v] + dist[v][new_next] - dist[new_prev][new_next];
                }
                long long delta = delta_remove + delta_insert;
                if (delta >= 0) continue;
                // Feasibility: check order of v
                int order_idx = node_order[v];
                int s, e;
                if (node_is_start[v]) {
                    s = v;
                    e = end_node[order_idx];
                } else {
                    s = start_node[order_idx];
                    e = v;
                }
                auto compute_new_pos = [&](int node, int old_idx_node) -> int {
                    if (node == v) return new_pos;
                    int pos_after = (old_idx_node > idx) ? old_idx_node - 1 : old_idx_node;
                    if (pos_after >= new_pos) return pos_after + 1;
                    else return pos_after;
                };
                int old_idx_s = pos[s];
                int old_idx_e = pos[e];
                int new_idx_s = compute_new_pos(s, old_idx_s);
                int new_idx_e = compute_new_pos(e, old_idx_e);
                if (new_idx_s < new_idx_e) {
                    // Apply move
                    route.erase(route.begin() + idx);
                    route.insert(route.begin() + new_pos, v);
                    total_final += delta;
                    for (int k = 0; k < n; ++k) pos[route[k]] = k;
                    improved = true;
                }
            }
        }
    }

    // Output
    cout << SELECT_COUNT;
    for (int i = 0; i < SELECT_COUNT; ++i) {
        cout << " " << selected[i] + 1;
    }
    cout << "\n";
    cout << n;
    for (int i = 0; i < n; ++i) {
        cout << " " << xs[route[i]] << " " << ys[route[i]];
    }
    cout << endl;

    return 0;
}