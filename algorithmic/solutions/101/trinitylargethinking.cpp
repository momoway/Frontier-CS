#include <bits/stdc++.h>
using namespace std;

const int AND = 0;
const int OR = 1;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int N, R;
    if (!(cin >> N >> R)) return 0;
    vector<int> U(N), V(N);
    for (int i = 0; i < N; ++i) {
        cin >> U[i] >> V[i];
    }

    // Build parent and side arrays
    vector<int> parent_arr(N, -1);
    vector<int> side_arr(N, -1); // 0 for left, 1 for right
    for (int i = 0; i < N; ++i) {
        if (U[i] < N) {
            parent_arr[U[i]] = i;
            side_arr[U[i]] = 0;
        }
        if (V[i] < N) {
            parent_arr[V[i]] = i;
            side_arr[V[i]] = 1;
        }
    }

    // Precompute leaf lists for each slot (subtree leaves)
    vector<vector<int>> leaf_lists(N);
    for (int i = N - 1; i >= 0; --i) {
        // left child U[i]
        if (U[i] >= N) {
            leaf_lists[i].push_back(U[i]);
        } else {
            const vector<int> &l = leaf_lists[U[i]];
            leaf_lists[i].insert(leaf_lists[i].end(), l.begin(), l.end());
        }
        // right child V[i]
        if (V[i] >= N) {
            leaf_lists[i].push_back(V[i]);
        } else {
            const vector<int> &r = leaf_lists[V[i]];
            leaf_lists[i].insert(leaf_lists[i].end(), r.begin(), r.end());
        }
    }

    // BFS to determine slot types
    vector<int> ans(N, -1); // -1 unknown, 0 AND, 1 OR
    vector<bool> visited(N, false);
    queue<int> q;
    q.push(0);
    while (!q.empty()) {
        int i = q.front(); q.pop();
        if (visited[i]) continue;
        // Build query for slot i
        string query(2 * N + 1, '0');
        vector<int> leaves_to_set_one;

        // Compute path from i to root
        vector<int> path_nodes;
        int cur = i;
        while (cur != -1) {
            path_nodes.push_back(cur);
            if (cur == 0) break;
            cur = parent_arr[cur];
        }
        // path_nodes[0] = i, path_nodes[1] = parent(i), ..., path_nodes.back() = 0

        // Set right subtree of i to 1, left to 0 (already 0)
        if (V[i] >= N) {
            leaves_to_set_one.push_back(V[i]);
        } else {
            const vector<int> &r = leaf_lists[V[i]];
            leaves_to_set_one.insert(leaves_to_set_one.end(), r.begin(), r.end());
        }

        // For each ancestor a (excluding i), set its other child's subtree to C = (ans[a]==AND?1:0)
        for (int idx = 1; idx < (int)path_nodes.size(); ++idx) {
            int a = path_nodes[idx];
            int child_on_path = path_nodes[idx - 1];
            int side_child = side_arr[child_on_path]; // 0 if left, 1 if right
            int other_child = (side_child == 0) ? V[a] : U[a];
            int C = (ans[a] == AND) ? 1 : 0;
            if (C == 1) {
                if (other_child >= N) {
                    leaves_to_set_one.push_back(other_child);
                } else {
                    const vector<int> &vec = leaf_lists[other_child];
                    leaves_to_set_one.insert(leaves_to_set_one.end(), vec.begin(), vec.end());
                }
            }
        }

        // Apply the settings to the query string
        for (int leaf : leaves_to_set_one) {
            query[leaf] = '1';
        }

        // Send query and receive response
        cout << "? " << query << endl;
        cout.flush();
        int resp;
        cin >> resp;
        if (resp == 0) {
            ans[i] = AND;
        } else {
            ans[i] = OR;
        }
        visited[i] = true;

        // Enqueue children that are slots
        if (U[i] < N) q.push(U[i]);
        if (V[i] < N) q.push(V[i]);
    }

    // Output final answer
    cout << "! ";
    for (int i = 0; i < N; ++i) {
        if (ans[i] == AND) cout << '&';
        else cout << '|';
    }
    cout << endl;
    return 0;
}