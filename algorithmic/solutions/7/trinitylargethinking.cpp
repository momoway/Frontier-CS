#include <bits/stdc++.h>
using namespace std;

struct Node {
    int child[2];
    bool is_leaf;
    Node() {
        child[0] = child[1] = -1;
        is_leaf = false;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    long long L, R;
    if (!(cin >> L >> R)) return 0;

    vector<Node> nodes;
    nodes.reserve(25000000); // reserve for up to 20 million nodes
    nodes.push_back(Node()); // root index 0

    // Build trie
    for (long long x = L; x <= R; ++x) {
        // Convert x to binary string without leading zeros
        string s;
        long long t = x;
        while (t) {
            s.push_back('0' + (t & 1));
            t >>= 1;
        }
        reverse(s.begin(), s.end());
        int cur = 0;
        for (char c : s) {
            int b = c - '0';
            if (nodes[cur].child[b] == -1) {
                nodes[cur].child[b] = nodes.size();
                nodes.push_back(Node());
            }
            cur = nodes[cur].child[b];
            // Check for prefix conflict: if current node is already a leaf, then a shorter number is a prefix of x
            if (nodes[cur].is_leaf) {
                // This should not happen for valid input (prefix-free)
                // But if it does, we cannot represent the set with a single sink DAG.
                // According to problem constraints, we assume prefix-free.
                cerr << "Prefix conflict detected at number " << x << endl;
                return 1;
            }
        }
        // Mark leaf
        if (nodes[cur].is_leaf) {
            // duplicate number, ignore (should not happen)
        } else {
            nodes[cur].is_leaf = true;
        }
    }

    // Add sink node
    int sink_index = nodes.size();
    nodes.push_back(Node()); // sink, no children, not a leaf

    // Transform leaf children to point to sink
    for (int i = 0; i < sink_index; ++i) {
        for (int b = 0; b < 2; ++b) {
            int child = nodes[i].child[b];
            if (child != -1 && nodes[child].is_leaf) {
                nodes[i].child[b] = sink_index;
            }
        }
    }

    // Determine which nodes to keep (internal nodes and sink)
    vector<bool> keep(nodes.size(), false);
    for (int i = 0; i < sink_index; ++i) {
        if (!nodes[i].is_leaf) {
            keep[i] = true;
        }
    }
    keep[sink_index] = true;

    // DFS to get post-order of kept nodes
    vector<int> post_order;
    vector<bool> visited(nodes.size(), false);
    function<void(int)> dfs = [&](int u) {
        visited[u] = true;
        for (int b = 0; b < 2; ++b) {
            int v = nodes[u].child[b];
            if (v != -1 && keep[v] && !visited[v]) {
                dfs(v);
            }
        }
        post_order.push_back(u);
    };
    dfs(0); // start from root; all kept nodes should be reachable

    // Compute signatures and assign new IDs
    map<pair<int,int>, int> sig_to_id;
    vector<pair<int,int>> sig_of_id; // index = new ID, value = signature (c0,c1)
    // Assign ID 0 to sink
    sig_of_id.push_back({-1, -1});
    sig_to_id[{ -1, -1 }] = 0;

    vector<int> new_id_for_original(nodes.size(), -1);
    new_id_for_original[sink_index] = 0;

    for (int u : post_order) {
        if (u == sink_index) continue; // already done
        int c0, c1;
        int child0 = nodes[u].child[0];
        if (child0 == -1) c0 = -1;
        else c0 = new_id_for_original[child0];
        int child1 = nodes[u].child[1];
        if (child1 == -1) c1 = -1;
        else c1 = new_id_for_original[child1];
        pair<int,int> sig = {c0, c1};
        if (sig_to_id.count(sig)) {
            new_id_for_original[u] = sig_to_id[sig];
        } else {
            int new_id = sig_to_id.size();
            sig_to_id[sig] = new_id;
            new_id_for_original[u] = new_id;
            sig_of_id.push_back(sig);
        }
    }

    int n = sig_to_id.size(); // total nodes in minimized DAG

    // Output
    cout << n << "\n";
    for (int id = 0; id < n; ++id) {
        int c0 = sig_of_id[id].first;
        int c1 = sig_of_id[id].second;
        int k = 0;
        if (c0 != -1) k++;
        if (c1 != -1) k++;
        cout << k;
        if (c0 != -1) {
            cout << " " << c0 << " 0";
        }
        if (c1 != -1) {
            cout << " " << c1 << " 1";
        }
        cout << "\n";
    }

    return 0;
}