#include <bits/stdc++.h>
using namespace std;

int n, q;
vector<int> a;
vector<int> value_to_id; // value -> initial set ID
int cnt; // current number of sets
vector<int> min_val, max_val; // min and max for each set ID
vector<pair<int,int>> ops; // merge operations

struct Node {
    int l, r;
    vector<pair<int,int>> intervals; // contiguous value ranges [start, end]
    Node *left, *right;
    Node(int l, int r) : l(l), r(r), left(nullptr), right(nullptr) {}
};

Node* build_tree(int l, int r) {
    Node* node = new Node(l, r);
    if (l == r) {
        node->intervals = {{a[l], a[l]}};
    } else {
        int mid = (l + r) / 2;
        Node* left = build_tree(l, mid);
        Node* right = build_tree(mid+1, r);
        node->left = left;
        node->right = right;
        // merge two sorted interval lists
        node->intervals.reserve(left->intervals.size() + right->intervals.size());
        merge(left->intervals.begin(), left->intervals.end(),
              right->intervals.begin(), right->intervals.end(),
              back_inserter(node->intervals),
              [](const pair<int,int>& p1, const pair<int,int>& p2) {
                  return p1.first < p2.first;
              });
    }
    return node;
}

// map from interval (start,end) to set ID
map<pair<int,int>, int> interval_set_id;

// get set ID for interval [x,y], creating it if necessary
int get_interval_set(int x, int y) {
    if (x > y) return -1;
    auto key = make_pair(x, y);
    auto it = interval_set_id.find(key);
    if (it != interval_set_id.end()) return it->second;
    // create by merging values from x to y in order
    int setId = value_to_id[x];
    for (int v = x+1; v <= y; ++v) {
        int nextId = value_to_id[v];
        ++cnt;
        ops.push_back({setId, nextId});
        int newId = cnt;
        min_val.push_back(min(min_val[setId], min_val[nextId]));
        max_val.push_back(max(max_val[setId], max_val[nextId]));
        setId = newId;
    }
    interval_set_id[key] = setId;
    return setId;
}

// query segment tree, return sorted list of set IDs (by min) covering the intersection
vector<int> query_tree(Node* node, int l, int r) {
    if (node->r < l || node->l > r) return {};
    if (l <= node->l && node->r <= r) {
        vector<int> res;
        res.reserve(node->intervals.size());
        for (auto &p : node->intervals) {
            int setId = get_interval_set(p.first, p.second);
            res.push_back(setId);
        }
        return res;
    }
    vector<int> left_res = query_tree(node->left, l, r);
    vector<int> right_res = query_tree(node->right, l, r);
    // merge two sorted lists by min value
    vector<int> merged;
    merged.reserve(left_res.size() + right_res.size());
    int i = 0, j = 0;
    while (i < left_res.size() && j < right_res.size()) {
        int id1 = left_res[i], id2 = right_res[j];
        if (min_val[id1] < min_val[id2]) {
            merged.push_back(id1);
            ++i;
        } else {
            merged.push_back(id2);
            ++j;
        }
    }
    while (i < left_res.size()) merged.push_back(left_res[i++]);
    while (j < right_res.size()) merged.push_back(right_res[j++]);
    return merged;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> n >> q;
    a.resize(n+1);
    for (int i = 1; i <= n; ++i) {
        cin >> a[i];
    }
    // mapping from value to its initial set ID (index in a)
    value_to_id.resize(n+1);
    for (int i = 1; i <= n; ++i) {
        value_to_id[a[i]] = i;
    }
    // initialize min_val and max_val for initial sets (1..n)
    min_val.push_back(0); // dummy for index 0
    max_val.push_back(0);
    for (int i = 1; i <= n; ++i) {
        min_val.push_back(a[i]);
        max_val.push_back(a[i]);
    }
    cnt = n;
    // build segment tree
    Node* root = build_tree(1, n);
    // process queries
    vector<int> query_answers(q);
    for (int i = 0; i < q; ++i) {
        int l, r;
        cin >> l >> r;
        vector<int> sets = query_tree(root, l, r);
        if (sets.empty()) {
            // should not happen
            query_answers[i] = -1;
        } else {
            int cur = sets[0];
            for (int j = 1; j < sets.size(); ++j) {
                int nxt = sets[j];
                ++cnt;
                ops.push_back({cur, nxt});
                int newId = cnt;
                min_val.push_back(min(min_val[cur], min_val[nxt]));
                max_val.push_back(max(max_val[cur], max_val[nxt]));
                cur = newId;
            }
            query_answers[i] = cur;
        }
    }
    // output
    cout << cnt << "\n";
    for (auto &p : ops) {
        cout << p.first << " " << p.second << "\n";
    }
    for (int i = 0; i < q; ++i) {
        if (i) cout << " ";
        cout << query_answers[i];
    }
    cout << "\n";
    return 0;
}