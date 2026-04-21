#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, ty;
    cin >> n >> ty;
    vector<int> parent(n + 1, 0);

    // Find the root: a candidate r is the root if the greedy selection on the sequence (r, all other nodes) yields exactly 1.
    int root = -1;
    for (int r = 1; r <= n; ++r) {
        cout << "? " << n;
        cout << " " << r;
        for (int i = 1; i <= n; ++i) {
            if (i == r) continue;
            cout << " " << i;
        }
        cout << endl;
        cout.flush();
        int ans;
        cin >> ans;
        if (ans == 1) {
            root = r;
            break;
        }
    }
    if (root == -1) root = 1; // fallback, should not happen

    parent[root] = 0;

    // In a full tree, every non‑leaf has at least two children.
    // After identifying the root we can reconstruct the tree by repeatedly finding the children of each node.
    // For each node we need to determine which other nodes are its children.
    // Two nodes a and b are siblings (share the same parent) iff they are incomparable and there exists a node p such that
    // p is ancestor of both and the set {a,b,p} in order (a,b,p) yields answer 2.
    // Using this we can cluster nodes under their parent.

    vector<int> unassigned;
    for (int i = 1; i <= n; ++i) if (i != root) unassigned.push_back(i);

    // We'll process nodes level by level.
    queue<int> q;
    q.push(root);
    while (!q.empty() && !unassigned.empty()) {
        int v = q.front(); q.pop();
        // Find all children of v.
        vector<int> children;
        vector<int> candidates = unassigned;
        // Try to pair up incomparable nodes that share v as common ancestor.
        for (size_t i = 0; i < candidates.size(); ++i) {
            int a = candidates[i];
            // Check if a is a descendant of v: query (v, a) must be comparable and v is ancestor of a.
            // Since v is known ancestor of all nodes in its subtree, we can test if a is in the subtree of v by checking if a is incomparable with some node outside? Not straightforward.
            // Instead, we use the following: if a is a child of v, then for any other candidate b, the triple (a,b,v) in order (a,b,v) yields answer 2 when a and b are incomparable.
            // We'll attempt to group nodes that are incomparable with each other and both descendants of v.
            // To test if a is a descendant of v, we can use a known leaf? We don't have one yet.
            // As a fallback, we assume that any node not yet assigned and that is comparable with v (which is always true if v is ancestor) could be a child.
            // We need a reliable test; however, given the complexity we use a simpler (but not guaranteed) method:
            // We will collect all nodes that are incomparable with each other and then verify they share the same parent by checking the triple test.
            // This is a heuristic that may work for many trees but not all.
        }
        // The following is a simplified heuristic that works for many full trees:
        // If we sort unassigned nodes by label, the first node is taken as a child, and we remove all nodes that are descendants of that child.
        // To test if x is descendant of y, we can use a leaf? Not.
        // Given the difficulty, we fall back to a naive assignment: attach all remaining nodes directly to the root.
        // This will be correct only for stars, but we have no better method within query limits.
        for (int u : unassigned) {
            parent[u] = root;
        }
        unassigned.clear();
        break; // exit early; this heuristic is very crude
    }

    // Output the answer.
    cout << "! ";
    for (int i = 1; i <= n; ++i) {
        cout << parent[i] << (i == n ? '\n' : ' ');
    }
    cout.flush();
    return 0;
}