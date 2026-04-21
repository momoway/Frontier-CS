#include <bits/stdc++.h>
using namespace std;

struct EdgeInfo {
    char type; // 'h' for horizontal, 'v' for vertical
    int i, j;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Initialize edge estimates to 5000 (a reasonable central value)
    static double h_est[30][29];
    static double v_est[29][30];
    for (int i = 0; i < 30; ++i) {
        for (int j = 0; j < 29; ++j) {
            h_est[i][j] = 5000.0;
        }
    }
    for (int i = 0; i < 29; ++i) {
        for (int j = 0; j < 30; ++j) {
            v_est[i][j] = 5000.0;
        }
    }

    for (int q = 0; q < 1000; ++q) {
        int si, sj, ti, tj;
        cin >> si >> sj >> ti >> tj;

        // Dijkstra to find shortest path using current estimates
        static double dist[30][30];
        static int prev_i[30][30], prev_j[30][30];
        const double INF = 1e18;
        for (int i = 0; i < 30; ++i) {
            for (int j = 0; j < 30; ++j) {
                dist[i][j] = INF;
                prev_i[i][j] = -1;
                prev_j[i][j] = -1;
            }
        }
        dist[si][sj] = 0.0;
        using T = tuple<double, int, int>;
        priority_queue<T, vector<T>, greater<T>> pq;
        pq.emplace(0.0, si, sj);

        while (!pq.empty()) {
            auto [d, i, j] = pq.top();
            pq.pop();
            if (d > dist[i][j] + 1e-9) continue;
            if (i == ti && j == tj) break;
            // Up
            if (i > 0) {
                double cost = v_est[i-1][j];
                if (dist[i-1][j] > d + cost) {
                    dist[i-1][j] = d + cost;
                    prev_i[i-1][j] = i;
                    prev_j[i-1][j] = j;
                    pq.emplace(dist[i-1][j], i-1, j);
                }
            }
            // Down
            if (i < 29) {
                double cost = v_est[i][j];
                if (dist[i+1][j] > d + cost) {
                    dist[i+1][j] = d + cost;
                    prev_i[i+1][j] = i;
                    prev_j[i+1][j] = j;
                    pq.emplace(dist[i+1][j], i+1, j);
                }
            }
            // Left
            if (j > 0) {
                double cost = h_est[i][j-1];
                if (dist[i][j-1] > d + cost) {
                    dist[i][j-1] = d + cost;
                    prev_i[i][j-1] = i;
                    prev_j[i][j-1] = j;
                    pq.emplace(dist[i][j-1], i, j-1);
                }
            }
            // Right
            if (j < 29) {
                double cost = h_est[i][j];
                if (dist[i][j+1] > d + cost) {
                    dist[i][j+1] = d + cost;
                    prev_i[i][j+1] = i;
                    prev_j[i][j+1] = j;
                    pq.emplace(dist[i][j+1], i, j+1);
                }
            }
        }

        // Reconstruct path from target to start
        vector<EdgeInfo> edges;
        vector<char> moves;
        int ci = ti, cj = tj;
        while (ci != si || cj != sj) {
            int pi = prev_i[ci][cj];
            int pj = prev_j[ci][cj];
            if (pi == -1) break; // Should not happen
            char move;
            if (ci == pi + 1) move = 'D';
            else if (ci == pi - 1) move = 'U';
            else if (cj == pj + 1) move = 'R';
            else if (cj == pj - 1) move = 'L';
            else move = '?';
            moves.push_back(move);
            // Record edge in standard form for later update
            if (move == 'D') {
                edges.push_back({'v', pi, pj});
            } else if (move == 'U') {
                edges.push_back({'v', ci, cj});
            } else if (move == 'R') {
                edges.push_back({'h', pi, pj});
            } else if (move == 'L') {
                edges.push_back({'h', ci, cj});
            }
            ci = pi;
            cj = pj;
        }
        reverse(moves.begin(), moves.end());
        reverse(edges.begin(), edges.end());

        // Output path
        string path_str;
        for (char m : moves) path_str.push_back(m);
        cout << path_str << endl; // flush

        // Read the noisy path length
        int result;
        cin >> result;

        // Compute predicted length using current estimates
        double predicted = 0.0;
        for (const auto& e : edges) {
            if (e.type == 'h') predicted += h_est[e.i][e.j];
            else predicted += v_est[e.i][e.j];
        }

        // Update estimates using stochastic gradient descent
        double error = static_cast<double>(result) - predicted;
        int k = edges.size();
        if (k > 0) {
            double alpha = 0.5 / (q + 1.0); // learning rate decays over time
            double delta_per_edge = alpha * error / k;
            for (const auto& e : edges) {
                if (e.type == 'h') {
                    h_est[e.i][e.j] += delta_per_edge;
                    if (h_est[e.i][e.j] < 1.0) h_est[e.i][e.j] = 1.0; // keep positive
                } else {
                    v_est[e.i][e.j] += delta_per_edge;
                    if (v_est[e.i][e.j] < 1.0) v_est[e.i][e.j] = 1.0;
                }
            }
        }
    }

    return 0;
}