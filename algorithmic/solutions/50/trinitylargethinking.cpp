#include <bits/stdc++.h>
using namespace std;

const int MAXN = 400;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m;
    if (!(cin >> n >> m)) return 0;
    vector<int> cost(m);
    for (int i = 0; i < m; ++i) {
        cin >> cost[i];
    }
    vector<bitset<MAXN>> set_mask(m);
    vector<vector<int>> set_elements(m);
    for (int i = 0; i < n; ++i) {
        int k;
        cin >> k;
        for (int t = 0; t < k; ++t) {
            int a;
            cin >> a;
            --a;
            set_mask[a].set(i);
            set_elements[a].push_back(i);
        }
    }

    const int NUM_TRIALS = 5;
    std::mt19937 rng(std::random_device{}());

    vector<char> best_selected(m, 0);
    long long best_cost = LLONG_MAX;

    for (int trial = 0; trial < NUM_TRIALS; ++trial) {
        vector<char> selected(m, 0);
        vector<int> cover_count(n, 0);
        bitset<MAXN> uncovered;
        uncovered.reset();
        for (int i = 0; i < n; ++i) uncovered.set(i);

        while (uncovered.any()) {
            long long best_numer = -1;
            int best_denom = 1;
            vector<int> candidates;
            for (int j = 0; j < m; ++j) {
                if (selected[j]) continue;
                bitset<MAXN> new_bits = set_mask[j] & uncovered;
                int new_count = new_bits.count();
                if (new_count == 0) continue;
                if (candidates.empty()) {
                    best_numer = cost[j];
                    best_denom = new_count;
                    candidates.clear();
                    candidates.push_back(j);
                } else {
                    long long lhs = (long long)cost[j] * best_denom;
                    long long rhs = best_numer * (long long)new_count;
                    if (lhs < rhs) {
                        best_numer = cost[j];
                        best_denom = new_count;
                        candidates.clear();
                        candidates.push_back(j);
                    } else if (lhs == rhs) {
                        candidates.push_back(j);
                    }
                }
            }
            if (candidates.empty()) break;
            int chosen = candidates[rng() % candidates.size()];
            selected[chosen] = 1;
            uncovered &= ~set_mask[chosen];
            for (int e : set_elements[chosen]) cover_count[e]++;
        }

        long long total_cost = 0;
        for (int j = 0; j < m; ++j) if (selected[j]) total_cost += cost[j];

        bool removed = true;
        while (removed) {
            removed = false;
            for (int i = 0; i < m; ++i) {
                if (!selected[i]) continue;
                bool redundant = true;
                for (int e : set_elements[i]) {
                    if (cover_count[e] < 2) {
                        redundant = false;
                        break;
                    }
                }
                if (redundant) {
                    selected[i] = 0;
                    for (int e : set_elements[i]) cover_count[e]--;
                    removed = true;
                    break;
                }
            }
        }

        set<pair<int, int>> non_selected;
        for (int j = 0; j < m; ++j) {
            if (!selected[j]) non_selected.insert({cost[j], j});
        }

        const int MAX_SWAP_ITER = 1000;
        int swap_iter = 0;
        bool improved = true;
        while (improved && swap_iter < MAX_SWAP_ITER) {
            improved = false;
            ++swap_iter;

            int max_cost_redundant = -1;
            int best_redundant_i = -1;
            for (int i = 0; i < m; ++i) {
                if (selected[i]) {
                    bool redundant = true;
                    for (int e : set_elements[i]) {
                        if (cover_count[e] < 2) {
                            redundant = false;
                            break;
                        }
                    }
                    if (redundant) {
                        if (cost[i] > max_cost_redundant) {
                            max_cost_redundant = cost[i];
                            best_redundant_i = i;
                        }
                    }
                }
            }
            if (best_redundant_i != -1) {
                if (!non_selected.empty()) {
                    auto it = non_selected.begin();
                    int cheapest_cost = it->first;
                    int cheapest_j = it->second;
                    if (cheapest_cost < max_cost_redundant) {
                        int i = best_redundant_i;
                        selected[i] = 0;
                        for (int e : set_elements[i]) cover_count[e]--;
                        selected[cheapest_j] = 1;
                        for (int e : set_elements[cheapest_j]) cover_count[e]++;
                        non_selected.erase(it);
                        non_selected.insert({cost[i], i});
                        improved = true;
                        continue;
                    }
                }
            }

            vector<int> sel_indices;
            for (int i = 0; i < m; ++i) if (selected[i]) sel_indices.push_back(i);
            sort(sel_indices.begin(), sel_indices.end(), [&](int a, int b) {
                return cost[a] > cost[b];
            });
            int try_limit = min(10, (int)sel_indices.size());
            bool swapped = false;
            for (int idx = 0; idx < try_limit; ++idx) {
                int i = sel_indices[idx];
                bitset<MAXN> unique_mask;
                unique_mask.reset();
                for (int e : set_elements[i]) {
                    if (cover_count[e] == 1) unique_mask.set(e);
                }
                if (unique_mask.none()) continue;
                int found_j = -1;
                int cnt = 0;
                for (auto it = non_selected.begin(); it != non_selected.end() && cnt < 1000 && it->first < cost[i]; ++it, ++cnt) {
                    int j = it->second;
                    if ((set_mask[j] & unique_mask) == unique_mask) {
                        found_j = j;
                        break;
                    }
                }
                if (found_j != -1) {
                    selected[i] = 0;
                    for (int e : set_elements[i]) cover_count[e]--;
                    selected[found_j] = 1;
                    for (int e : set_elements[found_j]) cover_count[e]++;
                    non_selected.erase({cost[found_j], found_j});
                    non_selected.insert({cost[i], i});
                    improved = true;
                    swapped = true;
                    break;
                }
            }
            if (swapped) continue;
        }

        total_cost = 0;
        for (int j = 0; j < m; ++j) if (selected[j]) total_cost += cost[j];
        if (total_cost < best_cost) {
            best_cost = total_cost;
            best_selected = selected;
        }
    }

    int cnt = 0;
    for (int i = 0; i < m; ++i) if (best_selected[i]) cnt++;
    cout << cnt << "\n";
    bool first = true;
    for (int i = 0; i < m; ++i) {
        if (best_selected[i]) {
            if (!first) cout << " ";
            first = false;
            cout << (i + 1);
        }
    }
    cout << "\n";

    return 0;
}