#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T;
    cin >> T;
    while (T--) {
        int n;
        cin >> n;
        if (n == 1) {
            cout << "!\n";
            cout.flush();
            continue;
        }
        vector<long long> distA(n + 1), distB(n + 1);
        // Step 1: find endpoint A
        int A = 1;
        {
            for (int v = 2; v <= n; ++v) {
                cout << "? 1 " << v << endl;
                cout.flush();
                cin >> distA[v];
            }
            distA[1] = 0;
            for (int v = 2; v <= n; ++v) {
                if (distA[v] > distA[A]) A = v;
            }
        }
        // Query distances from A
        for (int v = 1; v <= n; ++v) {
            if (v == A) {
                distA[v] = 0;
                continue;
            }
            cout << "? " << A << " " << v << endl;
            cout.flush();
            cin >> distA[v];
        }
        // Find endpoint B
        int B = A;
        for (int v = 1; v <= n; ++v) {
            if (v == A) continue;
            if (distA[v] > distA[B]) B = v;
        }
        long long D = distA[B];
        // Query distances from B
        for (int v = 1; v <= n; ++v) {
            if (v == B) {
                distB[v] = 0;
                continue;
            }
            cout << "? " << B << " " << v << endl;
            cout.flush();
            cin >> distB[v];
        }
        // Step 2: identify diameter nodes
        vector<int> diam;
        for (int v = 1; v <= n; ++v) {
            if (distA[v] + distB[v] == D) {
                diam.push_back(v);
            }
        }
        sort(diam.begin(), diam.end(), [&](int a, int b) {
            return distA[a] < distA[b];
        });
        // Map from distance from A to node on diameter
        unordered_map<long long, int> distToNode;
        for (int v : diam) {
            distToNode[distA[v]] = v;
        }
        // Step 3: compute projection and depth for non-diameter nodes
        vector<int> proj(n, -1);
        vector<long long> depth(n, 0);
        vector<char> isDiam(n + 1, 0);
        for (int v : diam) isDiam[v] = 1;
        for (int v = 1; v <= n; ++v) {
            if (isDiam[v]) {
                proj[v] = v;
                depth[v] = 0;
            } else {
                long long x = (distA[v] + D - distB[v]) / 2;
                auto it = distToNode.find(x);
                if (it == distToNode.end()) {
                    // Should not happen, but fallback
                    proj[v] = -1;
                } else {
                    proj[v] = it->second;
                    depth[v] = distA[v] - x;
                }
            }
        }
        // Step 4: group nodes by projection (excluding diameter nodes themselves)
        unordered_map<int, vector<int>> groups;
        for (int v = 1; v <= n; ++v) {
            if (!isDiam[v]) {
                groups[proj[v]].push_back(v);
            }
        }
        // Sort each group by depth ascending
        for (auto &kv : groups) {
            auto &vec = kv.second;
            sort(vec.begin(), vec.end(), [&](int a, int b) {
                return depth[a] < depth[b];
            });
        }
        // Step 5: build tree
        vector<bool> inTree(n + 1, false);
        for (int v : diam) inTree[v] = true;
        vector<tuple<int, int, long long>> edges;
        // Add diameter edges
        for (size_t i = 0; i + 1 < diam.size(); ++i) {
            int u = diam[i], v = diam[i + 1];
            long long w = distA[v] - distA[u];
            edges.emplace_back(u, v, w);
        }
        // Process each group
        for (auto &kv : groups) {
            int p = kv.first;
            auto &vec = kv.second;
            for (int v : vec) {
                int parent = -1;
                long long w = -1;
                // Scan candidates in descending depth order (from largest depth less than depth[v] downwards)
                for (int i = (int)vec.size() - 1; i >= 0; --i) {
                    int u = vec[i];
                    if (u == v) continue;
                    if (depth[u] >= depth[v]) continue; // should not happen due to sorting
                    // Query distance d(u, v)
                    long long d_uv;
                    cout << "? " << u << " " << v << endl;
                    cout.flush();
                    cin >> d_uv;
                    if (d_uv == depth[v] - depth[u]) {
                        parent = u;
                        w = depth[v] - depth[u]; // equals d_uv
                        break;
                    }
                }
                if (parent == -1) {
                    // Fallback: attach to projection p (should not happen)
                    parent = p;
                    w = depth[v];
                }
                edges.emplace_back(v, parent, w);
                inTree[v] = true;
            }
        }
        // Output answer
        cout << "!";
        for (auto &e : edges) {
            int u, v;
            long long w;
            tie(u, v, w) = e;
            cout << " " << u << " " << v << " " << w;
        }
        cout << endl;
        cout.flush();
    }
    return 0;
}