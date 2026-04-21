#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int N, M;
    if (!(cin >> N >> M)) return 0;
    int si, sj;
    cin >> si >> sj;
    vector<string> grid(N);
    for (int i = 0; i < N; ++i) {
        cin >> grid[i];
    }
    vector<string> targets(M);
    for (int k = 0; k < M; ++k) {
        cin >> targets[k];
    }

    // Concatenate all target strings to form S
    string S;
    S.reserve(M * 5);
    for (const string &t : targets) {
        S += t;
    }
    int L = S.size();

    int cells = N * N;
    // Precompute positions for each letter
    vector<vector<int>> posIndices(26);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            char c = grid[i][j];
            posIndices[c - 'A'].push_back(i * N + j);
        }
    }

    // Precompute Manhattan distance between all pairs of cells
    vector<vector<int>> distMat(cells, vector<int>(cells));
    for (int idx1 = 0; idx1 < cells; ++idx1) {
        int i1 = idx1 / N, j1 = idx1 % N;
        for (int idx2 = 0; idx2 < cells; ++idx2) {
            int i2 = idx2 / N, j2 = idx2 % N;
            distMat[idx1][idx2] = abs(i1 - i2) + abs(j1 - j2);
        }
    }

    const int INF = 1e9;
    vector<int> dp_prev(cells, INF), dp_curr(cells, INF);
    // prev_step[step][cell] = previous cell index
    vector<vector<int>> prev_step(L + 1, vector<int>(cells, -1));

    // First character
    char firstChar = S[0];
    for (int idx : posIndices[firstChar - 'A']) {
        int i = idx / N, j = idx % N;
        dp_prev[idx] = abs(i - si) + abs(j - sj);
        prev_step[1][idx] = -1;
    }

    // Subsequent characters
    for (int step = 2; step <= L; ++step) {
        char c_prev = S[step - 2];
        char c_curr = S[step - 1];
        for (int idx_curr : posIndices[c_curr - 'A']) {
            int best = INF;
            int best_prev = -1;
            for (int idx_prev : posIndices[c_prev - 'A']) {
                if (dp_prev[idx_prev] == INF) continue;
                int d = dp_prev[idx_prev] + distMat[idx_prev][idx_curr];
                if (d < best) {
                    best = d;
                    best_prev = idx_prev;
                }
            }
            dp_curr[idx_curr] = best;
            prev_step[step][idx_curr] = best_prev;
        }
        dp_prev.swap(dp_curr);
        fill(dp_curr.begin(), dp_curr.end(), INF);
    }

    // Find the best final cell
    int best_idx = -1;
    int minDist = INF;
    char lastChar = S[L - 1];
    for (int idx : posIndices[lastChar - 'A']) {
        if (dp_prev[idx] < minDist) {
            minDist = dp_prev[idx];
            best_idx = idx;
        }
    }

    // Reconstruct the path
    vector<int> pathIndices(L);
    int cur = best_idx;
    for (int step = L; step >= 1; --step) {
        pathIndices[step - 1] = cur;
        cur = prev_step[step][cur];
    }

    // Output the sequence of positions
    for (int idx : pathIndices) {
        int i = idx / N;
        int j = idx % N;
        cout << i << " " << j << "\n";
    }

    return 0;
}