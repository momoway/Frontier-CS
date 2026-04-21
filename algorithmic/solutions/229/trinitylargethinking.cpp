#include <bits/stdc++.h>
using namespace std;

const int MAXK = 11;

struct SegNode {
    array<int, MAXK> val;
    array<int, MAXK> idx;
    SegNode() {
        val.fill(0);
        idx.fill(-1);
    }
};

int n, x;
vector<long long> t;
vector<long long> all_vals;
int m;
vector<SegNode> seg;
vector<array<Pred, MAXK>> pre; // Pred defined later

struct Pred {
    int prev_j, prev_k, c;
};

vector<SegNode> seg_tree; // segment tree

void seg_update(int idx, int l, int r, int pos, const array<int, MAXK>& new_val, int new_idx) {
    if (l == r) {
        for (int k = 0; k < MAXK; k++) {
            if (new_val[k] > seg_tree[idx].val[k]) {
                seg_tree[idx].val[k] = new_val[k];
                seg_tree[idx].idx[k] = new_idx;
            }
        }
        return;
    }
    int mid = (l + r) >> 1;
    if (pos <= mid) seg_update(idx << 1, l, mid, pos, new_val, new_idx);
    else seg_update(idx << 1 | 1, mid + 1, r, pos, new_val, new_idx);
    for (int k = 0; k < MAXK; k++) {
        if (seg_tree[idx << 1].val[k] >= seg_tree[idx << 1 | 1].val[k]) {
            seg_tree[idx].val[k] = seg_tree[idx << 1].val[k];
            seg_tree[idx].idx[k] = seg_tree[idx << 1].idx[k];
        } else {
            seg_tree[idx].val[k] = seg_tree[idx << 1 | 1].val[k];
            seg_tree[idx].idx[k] = seg_tree[idx << 1 | 1].idx[k];
        }
    }
}

SegNode seg_query(int idx, int l, int r, int ql, int qr) {
    if (ql > r || qr < l) return SegNode();
    if (ql <= l && r <= qr) return seg_tree[idx];
    int mid = (l + r) >> 1;
    if (qr <= mid) return seg_query(idx << 1, l, mid, ql, qr);
    if (ql > mid) return seg_query(idx << 1 | 1, mid + 1, r, ql, qr);
    SegNode left = seg_query(idx << 1, l, mid, ql, qr);
    SegNode right = seg_query(idx << 1 | 1, mid + 1, r, ql, qr);
    SegNode res;
    for (int k = 0; k < MAXK; k++) {
        if (left.val[k] >= right.val[k]) {
            res.val[k] = left.val[k];
            res.idx[k] = left.idx[k];
        } else {
            res.val[k] = right.val[k];
            res.idx[k] = right.idx[k];
        }
    }
    return res;
}

int compute_LIS() {
    vector<long long> tails;
    for (int i = 0; i < n; i++) {
        auto it = lower_bound(tails.begin(), tails.end(), t[i]);
        if (it == tails.end()) tails.push_back(t[i]);
        else *it = t[i];
    }
    return (int)tails.size();
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> n >> x;
    t.resize(n);
    for (int i = 0; i < n; i++) cin >> t[i];
    if (x == 0) {
        int lis = compute_LIS();
        cout << lis << "\n";
        for (int i = 0; i < 10; i++) cout << "1 1 0\n";
        return 0;
    }
    // compress
    all_vals = t;
    sort(all_vals.begin(), all_vals.end());
    all_vals.erase(unique(all_vals.begin(), all_vals.end()), all_vals.end());
    m = all_vals.size();
    seg_tree.assign(4 * m, SegNode());
    pre.resize(n);
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < MAXK; k++) pre[i][k] = {-1, -1, -1};
    }
    vector<array<int, MAXK>> dp_all(n);
    for (int i = 0; i < n; i++) {
        int idx_i = lower_bound(all_vals.begin(), all_vals.end(), t[i]) - all_vals.begin() + 1;
        array<int, MAXK> cur_dp;
        cur_dp.fill(1);
        for (int c = 0; c < MAXK; c++) {
            long long low, high;
            int l_idx, r_idx;
            if (c == 0) {
                if (idx_i == 1) continue;
                l_idx = 1;
                r_idx = idx_i - 1;
            } else {
                low = t[i] + 1LL * (c - 1) * x;
                high = t[i] + 1LL * c * x - 1;
                auto it_low = lower_bound(all_vals.begin(), all_vals.end(), low);
                if (it_low == all_vals.end()) continue;
                l_idx = it_low - all_vals.begin() + 1;
                auto it_high = upper_bound(all_vals.begin(), all_vals.end(), high);
                if (it_high == all_vals.begin()) continue;
                it_high--;
                r_idx = it_high - all_vals.begin() + 1;
                if (l_idx > r_idx) continue;
            }
            SegNode res = seg_query(1, 1, m, l_idx, r_idx);
            for (int k = c; k < MAXK; k++) {
                int prev_val = res.val[k - c];
                if (prev_val == 0) continue;
                int candidate = prev_val + 1;
                if (candidate > cur_dp[k]) {
                    cur_dp[k] = candidate;
                    pre[i][k] = {res.idx[k - c], k - c, c};
                }
            }
        }
        dp_all[i] = cur_dp;
        seg_update(1, 1, m, idx_i, cur_dp, i);
    }
    int best_len = 0, best_i = -1, best_k = -1;
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < MAXK; k++) {
            if (dp_all[i][k] > best_len) {
                best_len = dp_all[i][k];
                best_i = i;
                best_k = k;
            }
        }
    }
    vector<int> seq;
    int cur_i = best_i, cur_k = best_k;
    while (cur_i != -1) {
        seq.push_back(cur_i);
        Pred p = pre[cur_i][cur_k];
        if (p.prev_j == -1) break;
        cur_i = p.prev_j;
        cur_k = p.prev_k;
    }
    reverse(seq.begin(), seq.end());
    vector<tuple<int, int, int>> ops;
    for (int idx = 1; idx < (int)seq.size(); idx++) {
        int i_prev = seq[idx - 1];
        int i_cur = seq[idx];
        long long deficit = t[i_prev] - t[i_cur] + 1;
        if (deficit < 0) deficit = 0;
        if (deficit == 0) continue;
        long long ops_needed = (deficit + x - 1) / x;
        long long full = deficit / x;
        long long rem = deficit % x;
        for (int cnt = 0; cnt < full; cnt++) {
            ops.push_back({i_cur + 1, n, (int)x});
        }
        if (rem > 0) {
            ops.push_back({i_cur + 1, n, (int)rem});
        }
    }
    while (ops.size() < 10) {
        ops.push_back({1, 1, 0});
    }
    cout << best_len << "\n";
    for (auto &op : ops) {
        cout << get<0>(op) << " " << get<1>(op) << " " << get<2>(op) << "\n";
    }
    return 0;
}