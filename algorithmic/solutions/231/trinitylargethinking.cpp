#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m, T;
    if (!(cin >> n >> m >> T)) return 0;

    vector<vector<int>> graph(n+1);
    vector<int> indegree(n+1, 0);
    vector<vector<char>> adjMat(n+1, vector<char>(n+1, 0));

    for (int i = 0; i < m; ++i) {
        int a, b;
        cin >> a >> b;
        graph[a].push_back(b);
        indegree[b]++;
        adjMat[a][b] = 1;
    }

    // Topological order (Kahn)
    vector<int> order;
    order.reserve(n);
    queue<int> q;
    for (int i = 1; i <= n; ++i) {
        if (indegree[i] == 0) q.push(i);
    }
    while (!q.empty()) {
        int v = q.front(); q.pop();
        order.push_back(v);
        for (int w : graph[v]) {
            if (--indegree[w] == 0) q.push(w);
        }
    }

    // Compute Grundy numbers for initial graph
    vector<int> g_initial(n+1, 0);
    vector<int> seen(n+2, 0);
    bool distinct = true;
    // Process in reverse topological order
    for (int i = n-1; i >= 0; --i) {
        int v = order[i];
        // mark successor Grundy numbers
        for (int w : graph[v]) {
            seen[g_initial[w]] = 1;
        }
        int mex = 0;
        while (seen[mex]) mex++;
        g_initial[v] = mex;
        // clear seen for next iteration
        for (int w : graph[v]) {
            seen[g_initial[w]] = 0;
        }
        // check distinctness later
    }
    // Check distinctness
    vector<char> seenDistinct(n+1, 0);
    for (int v = 1; v <= n; ++v) {
        if (seenDistinct[g_initial[v]]) {
            distinct = false;
            break;
        }
        seenDistinct[g_initial[v]] = 1;
    }

    // Decide modifications
    int K = 0;
    vector<pair<int,int>> addedEdges;
    if (distinct) {
        K = 0;
    } else {
        // Add missing edges to make complete DAG (edges from higher label to lower label)
        // Labels: label[order[i]] = n-1-i
        for (int i = 0; i < n; ++i) {
            for (int j = i+1; j < n; ++j) {
                int u = order[i];
                int v = order[j];
                if (!adjMat[u][v]) {
                    adjMat[u][v] = 1;
                    addedEdges.push_back({u, v});
                    K++;
                }
            }
        }
    }

    // Output modifications
    cout << K << "\n";
    for (auto &e : addedEdges) {
        cout << "+ " << e.first << " " << e.second << "\n";
    }
    cout.flush();

    // Precompute pos (index in order) for label calculation
    vector<int> pos(n+1);
    for (int i = 0; i < n; ++i) {
        pos[order[i]] = i;
    }

    // Process T rounds
    for (int round = 0; round < T; ++round) {
        // Determine hidden vertex v
        int found = -1;
        // We'll use linear search: ask "? 1 x" for each x until we get Lose
        for (int x = 1; x <= n; ++x) {
            cout << "? 1 " << x << endl;
            string ans;
            cin >> ans;
            if (ans == "Lose") {
                found = x;
                break;
            } else if (ans == "Wrong") {
                return 0;
            }
        }
        if (found == -1) {
            // Should not happen, but for safety
            // Try again? Just exit
            return 0;
        }
        cout << "! " << found << endl;
        string verdict;
        cin >> verdict;
        if (verdict == "Wrong") {
            return 0;
        }
    }
    return 0;
}