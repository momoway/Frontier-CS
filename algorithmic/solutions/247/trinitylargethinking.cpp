#include <bits/stdc++.h>
using namespace std;

struct VectorHash {
    size_t operator()(const vector<int>& v) const {
        size_t seed = v.size();
        for (int i : v) {
            seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int N;
    if (!(cin >> N)) return 0;
    vector<int> A(N), B(N);
    for (int i = 0; i < N; ++i) cin >> A[i];
    for (int i = 0; i < N; ++i) cin >> B[i];

    long long sumA = 0, sumB = 0;
    for (int x : A) sumA += x;
    for (int x : B) sumB += x;
    if (sumA != sumB) {
        cout << "No\n";
        return 0;
    }

    // Special handling for N=2
    if (N == 2) {
        if (A[0] == B[0] && A[1] == B[1]) {
            cout << "Yes\n0\n";
        } else if (B[0] == A[1] - 1 && B[1] == A[0] + 1) {
            cout << "Yes\n1\n1 2\n";
        } else {
            cout << "No\n";
        }
        return 0;
    }

    vector<int> cur = A;
    vector<pair<int,int>> ops;
    unordered_set<vector<int>, VectorHash> visited;
    visited.insert(cur);
    const int STEP_LIMIT = 100000;
    bool possible = true;

    while (true) {
        long long sum_abs = 0;
        for (int i = 0; i < N; ++i) sum_abs += llabs(cur[i] - B[i]);
        if (sum_abs == 0) break;

        if (ops.size() > STEP_LIMIT) {
            possible = false;
            break;
        }

        long long best_delta = -4e18;
        int best_i = -1, best_j = -1;
        vector<int> best_new_state;
        long long best_new_sum = (long long)4e18;

        for (int i = 0; i < N; ++i) {
            for (int j = i+1; j < N; ++j) {
                vector<int> nxt = cur;
                nxt[i] = cur[j] - 1;
                nxt[j] = cur[i] + 1;
                // Skip if operation does nothing (difference 1)
                if (cur[j] - cur[i] == 1) continue; // no change
                long long new_sum = sum_abs - llabs(cur[i] - B[i]) - llabs(cur[j] - B[j])
                                          + llabs(nxt[i] - B[i]) + llabs(nxt[j] - B[j]);
                long long delta = sum_abs - new_sum; // reduction
                if (delta > best_delta || (delta == best_delta && (i < best_i || (i == best_i && j < best_j)))) {
                    best_delta = delta;
                    best_i = i;
                    best_j = j;
                    best_new_state = nxt;
                    best_new_sum = new_sum;
                }
            }
        }

        // If no improving move found, consider all moves (including those that increase) but avoid no-ops
        if (best_delta <= 0) {
            best_delta = -4e18;
            best_i = -1; best_j = -1;
            for (int i = 0; i < N; ++i) {
                for (int j = i+1; j < N; ++j) {
                    if (cur[j] - cur[i] == 1) continue; // no change
                    vector<int> nxt = cur;
                    nxt[i] = cur[j] - 1;
                    nxt[j] = cur[i] + 1;
                    long long new_sum = sum_abs - llabs(cur[i] - B[i]) - llabs(cur[j] - B[j])
                                              + llabs(nxt[i] - B[i]) + llabs(nxt[j] - B[j]);
                    if (new_sum < best_new_sum || (new_sum == best_new_sum && (i < best_i || (i == best_i && j < best_j)))) {
                        best_new_sum = new_sum;
                        best_i = i;
                        best_j = j;
                        best_new_state = nxt;
                    }
                }
            }
            if (best_i == -1) {
                // No valid move (all differences 1) -> stuck
                possible = false;
                break;
            }
        }

        // Apply the chosen operation
        cur = best_new_state;
        ops.emplace_back(best_i + 1, best_j + 1);
        if (visited.count(cur)) {
            // Cycle detected
            possible = false;
            break;
        }
        visited.insert(cur);
    }

    if (possible && ops.size() <= STEP_LIMIT) {
        cout << "Yes\n" << ops.size() << "\n";
        for (auto [i, j] : ops) {
            cout << i << " " << j << "\n";
        }
    } else {
        cout << "No\n";
    }
    return 0;
}