#include <bits/stdc++.h>
using namespace std;

static uint64_t rng_seed = 123456789;

uint64_t rng_next() {
    rng_seed ^= rng_seed << 13;
    rng_seed ^= rng_seed >> 7;
    rng_seed ^= rng_seed << 17;
    return rng_seed;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int subtask, n;
    cin >> subtask >> n;

    vector<bool> inI(n + 1, false), inC(n + 1, false);
    vector<int> I, C;

    // Build independent set I
    for (int v = 1; v <= n; ++v) {
        cout << 1 << "\n" << v << "\n";
        cout.flush();
        int a;
        cin >> a;
        if (a == 0) {
            inI[v] = true;
            I.push_back(v);
        } else {
            cout << 1 << "\n" << v << "\n";
            cout.flush();
            cin >> a; // should be 0
        }
    }

    if (n == 1) {
        cout << -1 << " " << 1 << "\n";
        cout.flush();
        return 0;
    }

    for (int v = 1; v <= n; ++v) {
        if (!inI[v]) {
            inC[v] = true;
            C.push_back(v);
        }
    }

    int k = I.size();
    int c_sz = C.size();

    vector<int> I_vertex(k);
    for (int i = 0; i < k; ++i) I_vertex[i] = I[i];

    vector<uint64_t> mask1(k), mask2(k), mask3(k);
    for (int i = 0; i < k; ++i) {
        mask1[i] = rng_next();
        mask2[i] = rng_next();
        mask3[i] = rng_next();
    }

    vector<int> C_idx(n + 1, -1);
    for (int i = 0; i < c_sz; ++i) C_idx[C[i]] = i;

    vector<vector<char>> ans1(64, vector<char>(c_sz, 0));
    vector<vector<char>> ans2(64, vector<char>(c_sz, 0));
    vector<vector<char>> ans3(64, vector<char>(c_sz, 0));

    // Round 1
    {
        vector<int> sizeA(64, 0);
        for (int idx = 0; idx < k; ++idx) {
            uint64_t m = mask1[idx];
            for (int b = 0; b < 64; ++b) {
                if (((m >> b) & 1ULL) == 0) sizeA[b]++;
            }
        }
        long long totalL = k; // clearing I
        for (int b = 0; b < 64; ++b) {
            totalL += 2LL * sizeA[b] + 2LL * c_sz;
        }
        vector<int> ops;
        vector<pair<int, int>> add_c_map;

        // Clearing I
        for (int idx = 0; idx < k; ++idx) {
            ops.push_back(I_vertex[idx]);
        }
        // Bits
        for (int b = 0; b < 64; ++b) {
            // Add A
            for (int idx = 0; idx < k; ++idx) {
                if (((mask1[idx] >> b) & 1ULL) == 0) {
                    ops.push_back(I_vertex[idx]);
                }
            }
            // For each c
            for (int i = 0; i < c_sz; ++i) {
                int c = C[i];
                ops.push_back(c);
                add_c_map.push_back({b, i});
                ops.push_back(c);
            }
            // Remove A
            for (int idx = 0; idx < k; ++idx) {
                if (((mask1[idx] >> b) & 1ULL) == 0) {
                    ops.push_back(I_vertex[idx]);
                }
            }
        }
        cout << totalL << "\n";
        for (int x : ops) cout << x << " ";
        cout << "\n";
        cout.flush();

        vector<int> answers(totalL);
        for (int i = 0; i < totalL; ++i) cin >> answers[i];

        int idx_ans = k; // skip clearing answers
        for (auto &p : add_c_map) {
            int b = p.first;
            int i = p.second;
            ans1[b][i] = answers[idx_ans];
            idx_ans++;
        }
    }

    // Round 2
    {
        vector<int> sizeA(64, 0);
        for (int idx = 0; idx < k; ++idx) {
            uint64_t m = mask2[idx];
            for (int b = 0; b < 64; ++b) {
                if (((m >> b) & 1ULL) == 0) sizeA[b]++;
            }
        }
        long long totalL = 0;
        for (int b = 0; b < 64; ++b) {
            totalL += 2LL * sizeA[b] + 2LL * c_sz;
        }
        vector<int> ops;
        vector<pair<int, int>> add_c_map;

        for (int b = 0; b < 64; ++b) {
            for (int idx = 0; idx < k; ++idx) {
                if (((mask2[idx] >> b) & 1ULL) == 0) {
                    ops.push_back(I_vertex[idx]);
                }
            }
            for (int i = 0; i < c_sz; ++i) {
                int c = C[i];
                ops.push_back(c);
                add_c_map.push_back({b, i});
                ops.push_back(c);
            }
            for (int idx = 0; idx < k; ++idx) {
                if (((mask2[idx] >> b) & 1ULL) == 0) {
                    ops.push_back(I_vertex[idx]);
                }
            }
        }
        cout << totalL << "\n";
        for (int x : ops) cout << x << " ";
        cout << "\n";
        cout.flush();

        vector<int> answers(totalL);
        for (int i = 0; i < totalL; ++i) cin >> answers[i];

        int idx_ans = 0;
        for (auto &p : add_c_map) {
            int b = p.first;
            int i = p.second;
            ans2[b][i] = answers[idx_ans];
            idx_ans++;
        }
    }

    // Round 3
    {
        vector<int> sizeA(64, 0);
        for (int idx = 0; idx < k; ++idx) {
            uint64_t m = mask3[idx];
            for (int b = 0; b < 64; ++b) {
                if (((m >> b) & 1ULL) == 0) sizeA[b]++;
            }
        }
        long long totalL = 0;
        for (int b = 0; b < 64; ++b) {
            totalL += 2LL * sizeA[b] + 2LL * c_sz;
        }
        vector<int> ops;
        vector<pair<int, int>> add_c_map;

        for (int b = 0; b < 64; ++b) {
            for (int idx = 0; idx < k; ++idx) {
                if (((mask3[idx] >> b) & 1ULL) == 0) {
                    ops.push_back(I_vertex[idx]);
                }
            }
            for (int i = 0; i < c_sz; ++i) {
                int c = C[i];
                ops.push_back(c);
                add_c_map.push_back({b, i});
                ops.push_back(c);
            }
            for (int idx = 0; idx < k; ++idx) {
                if (((mask3[idx] >> b) & 1ULL) == 0) {
                    ops.push_back(I_vertex[idx]);
                }
            }
        }
        cout << totalL << "\n";
        for (int x : ops) cout << x << " ";
        cout << "\n";
        cout.flush();

        vector<int> answers(totalL);
        for (int i = 0; i < totalL; ++i) cin >> answers[i];

        int idx_ans = 0;
        for (auto &p : add_c_map) {
            int b = p.first;
            int i = p.second;
            ans3[b][i] = answers[idx_ans];
            idx_ans++;
        }
    }

    // Build bit lists for 192 bits
    vector<vector<int>> bitList(192);
    for (int idx = 0; idx < k; ++idx) {
        for (int b = 0; b < 64; ++b) {
            if ((mask1[idx] >> b) & 1ULL) bitList[b].push_back(idx);
        }
        for (int b = 64; b < 128; ++b) {
            if ((mask2[idx] >> (b - 64)) & 1ULL) bitList[b].push_back(idx);
        }
        for (int b = 128; b < 192; ++b) {
            if ((mask3[idx] >> (b - 128)) & 1ULL) bitList[b].push_back(idx);
        }
    }

    // Prepare for verification
    vector<vector<int>> tests_for_c(c_sz);
    vector<vector<int>> neighbors_in_I(c_sz);

    // For each c, find candidates
    for (int ci = 0; ci < c_sz; ++ci) {
        uint64_t v1 = 0, v2 = 0, v3 = 0;
        for (int b = 0; b < 64; ++b) {
            if (ans1[b][ci]) v1 |= (1ULL << b);
            if (ans2[b][ci]) v2 |= (1ULL << b);
            if (ans3[b][ci]) v3 |= (1ULL << b);
        }
        uint64_t M1 = ~v1;
        uint64_t M2 = ~v2;
        uint64_t M3 = ~v3;

        vector<int> candidates;
        int best_bit = -1;
        int best_size = INT_MAX;

        for (int b = 0; b < 64; ++b) {
            if ((M1 >> b) & 1ULL) {
                if ((int)bitList[b].size() < best_size) {
                    best_size = bitList[b].size();
                    best_bit = b;
                }
            }
        }
        for (int b = 64; b < 128; ++b) {
            if ((M2 >> (b - 64)) & 1ULL) {
                if ((int)bitList[b].size() < best_size) {
                    best_size = bitList[b].size();
                    best_bit = b;
                }
            }
        }
        for (int b = 128; b < 192; ++b) {
            if ((M3 >> (b - 128)) & 1ULL) {
                if ((int)bitList[b].size() < best_size) {
                    best_size = bitList[b].size();
                    best_bit = b;
                }
            }
        }

        if (best_bit == -1) {
            for (int idx = 0; idx < k; ++idx) {
                if (((mask1[idx] & M1) == M1) && ((mask2[idx] & M2) == M2) && ((mask3[idx] & M3) == M3)) {
                    candidates.push_back(idx);
                }
            }
        } else {
            const vector<int> &lst = bitList[best_bit];
            for (int idx : lst) {
                if (((mask1[idx] & M1) == M1) && ((mask2[idx] & M2) == M2) && ((mask3[idx] & M3) == M3)) {
                    candidates.push_back(idx);
                }
            }
        }

        tests_for_c[ci] = candidates;
    }

    // Verification query
    vector<int> ops_verify;
    vector<pair<int, int>> add_c_map_verify; // (c_idx, candidate idx)
    for (int ci = 0; ci < c_sz; ++ci) {
        for (int cand_idx : tests_for_c[ci]) {
            int u = I_vertex[cand_idx];
            int c = C[ci];
            ops_verify.push_back(u);
            ops_verify.push_back(c);
            ops_verify.push_back(u);
            ops_verify.push_back(c);
            add_c_map_verify.push_back({ci, cand_idx});
        }
    }
    long long totalL_verify = ops_verify.size();
    cout << totalL_verify << "\n";
    for (int x : ops_verify) cout << x << " ";
    cout << "\n";
    cout.flush();

    vector<int> answers_verify(totalL_verify);
    for (int i = 0; i < totalL_verify; ++i) cin >> answers_verify[i];

    // Process verification answers
    int idx_ans = 0;
    for (auto &p : add_c_map_verify) {
        int ci = p.first;
        int cand_idx = p.second;
        if (answers_verify[idx_ans] == 1) {
            neighbors_in_I[ci].push_back(I_vertex[cand_idx]);
        }
        idx_ans += 4; // skip removal answers
    }

    // For odd n, find special C vertices and add edge between them
    if (n % 2 == 1) {
        vector<int> special;
        for (int i = 0; i < c_sz; ++i) {
            if (neighbors_in_I[i].size() == 1) {
                special.push_back(C[i]);
            }
        }
        if (special.size() == 2) {
            int u = special[0];
            int v = special[1];
            // Test adjacency
            cout << 4 << "\n" << u << " " << v << " " << u << " " << v << "\n";
            cout.flush();
            int a1, a2, a3, a4;
            cin >> a1 >> a2 >> a3 >> a4;
            if (a2 == 1) {
                // add edge both ways
                // we will add to adjacency later
            } else {
                // Should not happen, but ignore
            }
            // Add to adjacency (will be added in building adjacency)
            // We'll handle after building adjacency list
        }
    }

    // Build adjacency list
    vector<vector<int>> adj(n + 1);
    // Add edges I-C
    for (int ci = 0; ci < c_sz; ++ci) {
        for (int u : neighbors_in_I[ci]) {
            int v = C[ci];
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
    }
    // For odd n, add edge between the two special C vertices if not already added
    if (n % 2 == 1) {
        vector<int> special;
        for (int i = 0; i < c_sz; ++i) {
            if (neighbors_in_I[i].size() == 1) {
                special.push_back(C[i]);
            }
        }
        if (special.size() == 2) {
            int u = special[0], v = special[1];
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
    }

    // Traverse cycle
    vector<int> order;
    vector<bool> visited(n + 1, false);
    int start = 1;
    int prev = -1;
    int cur = start;
    do {
        order.push_back(cur);
        visited[cur] = true;
        int next = -1;
        for (int nb : adj[cur]) {
            if (nb != prev) {
                next = nb;
                break;
            }
        }
        prev = cur;
        cur = next;
    } while (cur != start);

    // Output guess
    cout << -1;
    for (int v : order) cout << " " << v;
    cout << "\n";
    cout.flush();

    return 0;
}