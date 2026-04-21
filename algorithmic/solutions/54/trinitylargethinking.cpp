#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    if (!(cin >> n)) return 0;

    vector<int> distA(n+1), distB(n+1), distC(n+1);
    auto query = [&](int u, int v) {
        cout << "? " << u << " " << v << endl;
        cout.flush();
        int d;
        cin >> d;
        return d;
    };

    // Step 1: find diameter endpoints A and B
    // Query from node 1 to all others
    vector<int> dist1(n+1);
    for (int i = 1; i <= n; ++i) {
        if (i == 1) continue;
        dist1[i] = query(1, i);
    }
    int A = 1;
    for (int i = 2; i <= n; ++i) {
        if (dist1[i] > dist1[A]) A = i;
    }
    // Query from A to all others
    for (int i = 1; i <= n; ++i) {
        if (i == A) continue;
        distA[i] = query(A, i);
    }
    distA[A] = 0;
    int B = A;
    for (int i = 1; i <= n; ++i) {
        if (i != A && distA[i] > distA[B]) B = i;
    }
    // Query from B to all others
    for (int i = 1; i <= n; ++i) {
        if (i == B) continue;
        distB[i] = query(B, i);
    }
    distB[B] = 0;
    int D = distA[B]; // distance A-B

    // Step 2: choose third reference C (off-diameter, farthest from A and B)
    int C = -1;
    int bestSum = -1;
    for (int i = 1; i <= n; ++i) {
        if (i == A || i == B) continue;
        int sum = distA[i] + distB[i];
        if (sum > bestSum) {
            bestSum = sum;
            C = i;
        }
    }
    // Query from C to all others
    for (int i = 1; i <= n; ++i) {
        if (i == C) continue;
        distC[i] = query(C, i);
    }
    distC[C] = 0;

    // Step 3: reconstruct tree using three references
    int dAB = D, dAC = distA[C], dBC = distB[C];
    long long aO = (1LL * dAB + dAC - dBC) / 2;
    long long bO = dAB - aO;
    long long cO = dAC - aO;
    // Ensure integer
    assert(aO*2 == dAB + dAC - dBC);
    assert(bO*2 == dAB - (dAB + dAC - dBC) + 2*dAB? Actually bO = dAB - aO, so ok.
    assert(cO*2 == dAC - (dAB + dAC - dBC) + 2*dAC? Not needed.

    int O = -1;
    for (int i = 1; i <= n; ++i) {
        if (distA[i] == aO && distB[i] == bO && distC[i] == cO) {
            O = i;
            break;
        }
    }
    assert(O != -1);

    // Precompute t values and classify nodes
    vector<int> tAB(n+1), tAC(n+1), tBC(n+1);
    for (int i = 1; i <= n; ++i) {
        tAB[i] = (distA[i] + distB[i] - dAB) / 2;
        tAC[i] = (distA[i] + distC[i] - dAC) / 2;
        tBC[i] = (distB[i] + distC[i] - dBC) / 2;
    }

    // Build maps for paths
    vector<int> abNodes, acNodes, bcNodes;
    vector<int> abDistMap(dAB+1, -1), acDistMap(dAC+1, -1), bcDistMap(dBC+1, -1);
    for (int i = 1; i <= n; ++i) {
        if (tAB[i] == 0) {
            abNodes.push_back(i);
            if (distA[i] <= dAB) abDistMap[distA[i]] = i;
        }
        if (tAC[i] == 0) {
            acNodes.push_back(i);
            if (distA[i] <= dAC) acDistMap[distA[i]] = i;
        }
        if (tBC[i] == 0) {
            bcNodes.push_back(i);
            if (distB[i] <= dBC) bcDistMap[distB[i]] = i;
        }
    }
    // Sort by distances (they may not be sorted by insertion order)
    sort(abNodes.begin(), abNodes.end(), [&](int i, int j) { return distA[i] < distA[j]; });
    sort(acNodes.begin(), acNodes.end(), [&](int i, int j) { return distA[i] < distA[j]; });
    sort(bcNodes.begin(), bcNodes.end(), [&](int i, int j) { return distB[i] < distB[j]; });

    // Build adjacency list
    vector<vector<pair<int,int>>> adj(n+1);
    auto addEdge = [&](int u, int v, int w) {
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    };
    set<pair<int,int>> edgeSet;

    // Add branch edges
    for (int x = 1; x <= n; ++x) {
        // Skip nodes that are on any path (they will be connected via path edges)
        if (tAB[x] == 0 || tAC[x] == 0 || tBC[x] == 0) continue;
        vector<tuple<int,int,int>> candidates; // (t, pathIndex, nodeY)
        int t = tAB[x];
        if (t > 0) {
            int da = distA[x] - t;
            int db = distB[x] - t;
            if (da >= 0 && da <= dAB && abDistMap[da] != -1) {
                int y = abDistMap[da];
                if (distB[y] == db) {
                    candidates.emplace_back(t, 0, y);
                }
            }
        }
        t = tAC[x];
        if (t > 0) {
            int da = distA[x] - t;
            int dc = distC[x] - t;
            if (da >= 0 && da <= dAC && acDistMap[da] != -1) {
                int y = acDistMap[da];
                if (distC[y] == dc) {
                    candidates.emplace_back(t, 1, y);
                }
            }
        }
        t = tBC[x];
        if (t > 0) {
            int db = distB[x] - t;
            int dc = distC[x] - t;
            if (db >= 0 && db <= dBC && bcDistMap[db] != -1) {
                int y = bcDistMap[db];
                if (distC[y] == dc) {
                    candidates.emplace_back(t, 2, y);
                }
            }
        }
        if (!candidates.empty()) {
            // Choose candidate with smallest t
            sort(candidates.begin(), candidates.end());
            auto [tMin, pathIdx, Y] = candidates[0];
            int weight = tMin;
            int u = x;
            if (edgeSet.count({min(u,Y), max(u,Y)}) == 0) {
                addEdge(u, Y, weight);
                edgeSet.insert({min(u,Y), max(u,Y)});
            }
        }
    }

    // Add path edges
    auto addPathEdges = [&](const vector<int>& nodes, bool isABorAC) {
        for (size_t i = 0; i + 1 < nodes.size(); ++i) {
            int u = nodes[i], v = nodes[i+1];
            int w = isABorAC ? (distA[v] - distA[u]) : (distB[v] - distB[u]);
            if (edgeSet.count({min(u,v), max(u,v)}) == 0) {
                addEdge(u, v, w);
                edgeSet.insert({min(u,v), max(u,v)});
            }
        }
    };
    addPathEdges(abNodes, true);
    addPathEdges(acNodes, true);
    addPathEdges(bcNodes, false);

    // Now we have the tree. Compute centroid.
    vector<int> parent(n+1, -1), depth(n+1), subtreeSize(n+1);
    function<void(int)> dfs = [&](int u) {
        subtreeSize[u] = 1;
        for (auto [v, w] : adj[u]) {
            if (v == parent[u]) continue;
            parent[v] = u;
            depth[v] = depth[u] + 1;
            dfs(v);
            subtreeSize[u] += subtreeSize[v];
        }
    };
    parent[1] = 0;
    depth[1] = 0;
    dfs(1);

    int centroid = -1;
    for (int u = 1; u <= n; ++u) {
        int maxComp = 0;
        for (auto [v, w] : adj[u]) {
            int compSize;
            if (v == parent[u]) {
                compSize = n - subtreeSize[u];
            } else {
                compSize = subtreeSize[v];
            }
            if (compSize > maxComp) maxComp = compSize;
        }
        if (maxComp <= n/2) {
            centroid = u;
            break; // unique centroid guaranteed
        }
    }
    cout << "! " << centroid << endl;
    cout.flush();
    return 0;
}