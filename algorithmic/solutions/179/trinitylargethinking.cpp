#include <bits/stdc++.h>
#include <boost/multiprecision/cpp_int.hpp>

using namespace std;
using namespace boost::multiprecision;

cpp_int string_to_cpp_int(const string& s) {
    cpp_int res = 0;
    for (char c : s) {
        res = res * 10 + (c - '0');
    }
    return res;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    string W_str;
    if (!(cin >> n >> W_str)) return 0;
    cpp_int W = string_to_cpp_int(W_str);

    vector<cpp_int> a(n);
    for (int i = 0; i < n; ++i) {
        string s;
        cin >> s;
        a[i] = string_to_cpp_int(s);
    }

    cpp_int M = 0;
    for (int i = 0; i < n; ++i) {
        if (a[i] > M) M = a[i];
    }

    if (W == 0) {
        for (int i = 0; i < n; ++i) {
            if (i) cout << ' ';
            cout << 0;
        }
        cout << "\n";
        return 0;
    }

    vector<int> small_idx;
    cpp_int min_big = -1;
    int best_big_idx = -1;
    for (int i = 0; i < n; ++i) {
        if (a[i] <= W) {
            small_idx.push_back(i);
        } else {
            if (best_big_idx == -1 || a[i] < min_big) {
                min_big = a[i];
                best_big_idx = i;
            }
        }
    }

    if (small_idx.empty()) {
        cpp_int diff_empty = W;
        cpp_int diff_big = min_big - W;
        if (diff_big < diff_empty) {
            vector<int> ans(n, 0);
            ans[best_big_idx] = 1;
            for (int i = 0; i < n; ++i) {
                if (i) cout << ' ';
                cout << ans[i];
            }
            cout << "\n";
        } else {
            for (int i = 0; i < n; ++i) {
                if (i) cout << ' ';
                cout << 0;
            }
            cout << "\n";
        }
        return 0;
    }

    cpp_int threshold = cpp_int(20) * n;
    if (W <= threshold) {
        long long cap = W.convert_to<long long>();
        long long max_sum = 2 * cap;
        int max_sum_int = static_cast<int>(max_sum);

        vector<char> dp(max_sum_int + 1, 0);
        dp[0] = 1;
        vector<int> prev_sum(max_sum_int + 1, -1);
        vector<int> item_idx(max_sum_int + 1, -1);

        for (int idx : small_idx) {
            long long w = a[idx].convert_to<long long>();
            for (int s = max_sum_int; s >= w; --s) {
                if (!dp[s] && dp[s - w]) {
                    dp[s] = 1;
                    prev_sum[s] = s - w;
                    item_idx[s] = idx;
                }
            }
        }

        long long best_diff = cap;
        int best_s = 0;
        for (int s = 0; s <= max_sum_int; ++s) {
            if (dp[s]) {
                long long diff = llabs(cap - s);
                if (diff < best_diff) {
                    best_diff = diff;
                    best_s = s;
                }
            }
        }

        vector<int> ans(n, 0);
        int s = best_s;
        while (s > 0) {
            int idx = item_idx[s];
            ans[idx] = 1;
            s = prev_sum[s];
        }

        cpp_int S = 0;
        for (int i = 0; i < n; ++i) {
            if (ans[i]) S += a[i];
        }
        cpp_int diff_dp = (W > S) ? (W - S) : (S - W);

        bool use_big = false;
        if (best_big_idx != -1) {
            cpp_int diff_big = min_big - W;
            if (diff_big < diff_dp) {
                use_big = true;
            }
        }

        if (use_big) {
            ans.assign(n, 0);
            ans[best_big_idx] = 1;
        }

        for (int i = 0; i < n; ++i) {
            if (i) cout << ' ';
            cout << ans[i];
        }
        cout << "\n";

    } else {
        cpp_int denom = cpp_int(20) * n;
        cpp_int delta = W / denom;
        cpp_int T_prime_cpp = W / delta;
        int T = T_prime_cpp.convert_to<long long>();

        vector<pair<int, int>> scaled_items;
        for (int idx : small_idx) {
            cpp_int a_prime_cpp = a[idx] / delta;
            int w = a_prime_cpp.convert_to<int>();
            if (w > 0) {
                scaled_items.push_back({w, idx});
            }
        }

        vector<char> dp(T + 1, 0);
        dp[0] = 1;
        vector<int> prev_sum(T + 1, -1);
        vector<int> item_idx(T + 1, -1);

        for (auto &p : scaled_items) {
            int w = p.first;
            int idx_orig = p.second;
            for (int s = T; s >= w; --s) {
                if (!dp[s] && dp[s - w]) {
                    dp[s] = 1;
                    prev_sum[s] = s - w;
                    item_idx[s] = idx_orig;
                }
            }
        }

        int best_scaled_sum = 0;
        for (int s = T; s >= 0; --s) {
            if (dp[s]) {
                best_scaled_sum = s;
                break;
            }
        }

        vector<int> ans(n, 0);
        int s = best_scaled_sum;
        while (s > 0) {
            int idx = item_idx[s];
            ans[idx] = 1;
            s = prev_sum[s];
        }

        cpp_int S = 0;
        for (int i = 0; i < n; ++i) {
            if (ans[i]) S += a[i];
        }
        cpp_int diff_dp = (W > S) ? (W - S) : (S - W);

        bool use_big = false;
        if (best_big_idx != -1) {
            cpp_int diff_big = min_big - W;
            if (diff_big < diff_dp) {
                use_big = true;
            }
        }

        if (use_big) {
            ans.assign(n, 0);
            ans[best_big_idx] = 1;
        }

        for (int i = 0; i < n; ++i) {
            if (i) cout << ' ';
            cout << ans[i];
        }
        cout << "\n";
    }

    return 0;
}