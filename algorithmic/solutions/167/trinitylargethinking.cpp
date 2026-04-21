#include <bits/stdc++.h>
using namespace std;

const int MAXY = 10005;
const int INF = -1000000000;

struct Point {
    int x, y, w, y_idx;
};

struct Node {
    int sum;
    int pref, suff, best, best2;
    int pref_l, pref_r;
    int suff_l, suff_r;
    int best_l, best_r;
    int best2_l, best2_r;
};

Node seg[4 * MAXY];

void reset_node(Node &node, int val, int pos) {
    node.sum = val;
    node.pref = val;
    node.pref_l = node.pref_r = pos;
    node.suff = val;
    node.suff_l = node.suff_r = pos;
    node.best = val;
    node.best_l = node.best_r = pos;
    node.best2 = INF;
    node.best2_l = node.best2_r = -1;
}

void merge(Node &res, const Node &L, const Node &R) {
    res.sum = L.sum + R.sum;

    if (L.pref > L.sum + R.pref) {
        res.pref = L.pref;
        res.pref_l = L.pref_l;
        res.pref_r = L.pref_r;
    } else {
        res.pref = L.sum + R.pref;
        res.pref_l = L.pref_l;
        res.pref_r = R.pref_r;
    }

    if (R.suff > R.sum + L.suff) {
        res.suff = R.suff;
        res.suff_l = R.suff_l;
        res.suff_r = R.suff_r;
    } else {
        res.suff = R.sum + L.suff;
        res.suff_l = L.suff_l;
        res.suff_r = R.suff_r;
    }

    res.best = L.best;
    res.best_l = L.best_l;
    res.best_r = L.best_r;
    if (R.best > res.best) {
        res.best = R.best;
        res.best_l = R.best_l;
        res.best_r = R.best_r;
    }
    int cross = L.suff + R.pref;
    if (cross > res.best) {
        res.best = cross;
        res.best_l = L.suff_l;
        res.best_r = R.pref_r;
    }

    res.best2 = INF;
    if (L.best2 > res.best2) {
        res.best2 = L.best2;
        res.best2_l = L.best2_l;
        res.best2_r = L.best2_r;
    }
    if (R.best2 > res.best2) {
        res.best2 = R.best2;
        res.best2_l = R.best2_l;
        res.best2_r = R.best2_r;
    }
    cross = L.suff + R.pref;
    if (cross > res.best2) {
        res.best2 = cross;
        res.best2_l = L.suff_l;
        res.best2_r = R.pref_r;
    }
}

void build(int node, int l, int r) {
    if (l == r) {
        reset_node(seg[node], 0, l);
        return;
    }
    int mid = (l + r) >> 1;
    build(node << 1, l, mid);
    build(node << 1 | 1, mid + 1, r);
    merge(seg[node], seg[node << 1], seg[node << 1 | 1]);
}

void update(int node, int l, int r, int pos, int delta) {
    if (l == r) {
        seg[node].sum += delta;
        seg[node].pref = seg[node].sum;
        seg[node].pref_l = seg[node].pref_r = pos;
        seg[node].suff = seg[node].sum;
        seg[node].suff_l = seg[node].suff_r = pos;
        seg[node].best = seg[node].sum;
        seg[node].best_l = seg[node].best_r = pos;
        return;
    }
    int mid = (l + r) >> 1;
    if (pos <= mid) update(node << 1, l, mid, pos, delta);
    else update(node << 1 | 1, mid + 1, r, pos, delta);
    merge(seg[node], seg[node << 1], seg[node << 1 | 1]);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int N;
    if (!(cin >> N)) return 0;
    vector<Point> pts;
    vector<int> mackerel_x;
    pts.reserve(2 * N);
    for (int i = 0; i < N; ++i) {
        int x, y;
        cin >> x >> y;
        mackerel_x.push_back(x);
        pts.push_back({x, y, 1, -1});
    }
    for (int i = 0; i < N; ++i) {
        int x, y;
        cin >> x >> y;
        pts.push_back({x, y, -1, -1});
    }

    sort(pts.begin(), pts.end(), [](const Point& a, const Point& b) {
        return a.x < b.x;
    });

    vector<int> y_vals;
    y_vals.reserve(2 * N + 2);
    for (const auto& p : pts) y_vals.push_back(p.y);
    y_vals.push_back(0);
    y_vals.push_back(100000);
    sort(y_vals.begin(), y_vals.end());
    y_vals.erase(unique(y_vals.begin(), y_vals.end()), y_vals.end());
    int nY = y_vals.size();

    for (auto& p : pts) {
        p.y_idx = lower_bound(y_vals.begin(), y_vals.end(), p.y) - y_vals.begin();
    }

    set<int> cand_set;
    cand_set.insert(0);
    cand_set.insert(100000);
    for (int x : mackerel_x) cand_set.insert(x);
    vector<int> cand_x(cand_set.begin(), cand_set.end());
    sort(cand_x.begin(), cand_x.end());

    const int MAX_LEFT = 500;
    vector<int> selected;
    selected.push_back(0);
    int max_mac_x = -1;
    for (int x : mackerel_x) if (x > max_mac_x) max_mac_x = x;
    if (max_mac_x != 0 && max_mac_x != 100000) selected.push_back(max_mac_x);
    int min_mac_x = 200000;
    for (int x : mackerel_x) if (x < min_mac_x) min_mac_x = x;
    if (min_mac_x != 0 && min_mac_x != 100000 && min_mac_x != max_mac_x) {
        selected.push_back(min_mac_x);
    }

    vector<int> other_mac_x;
    for (int x : cand_x) {
        if (x == 0 || x == 100000) continue;
        if (x == max_mac_x) continue;
        if (x == min_mac_x) continue;
        other_mac_x.push_back(x);
    }

    mt19937 rng(123456);
    shuffle(other_mac_x.begin(), other_mac_x.end(), rng);
    int remaining = MAX_LEFT - (int)selected.size();
    if (remaining > 0 && !other_mac_x.empty()) {
        if ((int)other_mac_x.size() > remaining) other_mac_x.resize(remaining);
        for (int x : other_mac_x) selected.push_back(x);
    }
    sort(selected.begin(), selected.end());

    build(1, 0, nY - 1);

    int best_sum = 0;
    int best_x_left = 0, best_x_right = 100000, best_y_bottom = 0, best_y_top = 100000;

    for (int L : selected) {
        int start_idx = lower_bound(pts.begin(), pts.end(), L,
            [](const Point& a, int val) { return a.x < val; }) - pts.begin();
        if (start_idx == (int)pts.size()) continue;

        build(1, 0, nY - 1);

        for (int j = start_idx; j < (int)pts.size(); ++j) {
            update(1, 0, nY - 1, pts[j].y_idx, pts[j].w);
            if (pts[j].x > L) {
                Node &root = seg[1];
                if (root.best2 > best_sum) {
                    best_sum = root.best2;
                    best_x_left = L;
                    best_x_right = pts[j].x;
                    best_y_bottom = y_vals[root.best2_l];
                    best_y_top = y_vals[root.best2_r];
                }
            }
        }
    }

    cout << 4 << "\n";
    cout << best_x_left << " " << best_y_bottom << "\n";
    cout << best_x_right << " " << best_y_bottom << "\n";
    cout << best_x_right << " " << best_y_top << "\n";
    cout << best_x_left << " " << best_y_top << "\n";
    return 0;
}