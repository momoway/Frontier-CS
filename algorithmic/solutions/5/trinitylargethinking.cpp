#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m;
    if (!(cin >> n >> m)) return 0;
    for (int i = 0; i < 10; ++i) {
        int a;
        cin >> a;
    }
    vector<vector<int>> adjOut(n + 1), adjIn(n + 1);
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        adjOut[u].push_back(v);
        adjIn[v].push_back(u);
    }
    for (int i = 1; i <= n; ++i) {
        sort(adjOut[i].begin(), adjOut[i].end());
        sort(adjIn[i].begin(), adjIn[i].end());
    }
    vector<int> outdeg(n + 1), indeg(n + 1);
    for (int i = 1; i <= n; ++i) {
        outdeg[i] = adjOut[i].size();
        indeg[i] = adjIn[i].size();
    }
    int start = -1;
    for (int i = 1; i <= n; ++i) {
        if (indeg[i] == 0) {
            if (start == -1 || outdeg[i] < outdeg[start] || (outdeg[i] == outdeg[start] && i < start))
                start = i;
        }
    }
    if (start == -1) {
        for (int i = 1; i <= n; ++i) {
            if (start == -1 || outdeg[i] < outdeg[start] || (outdeg[i] == outdeg[start] && i < start))
                start = i;
        }
    }
    vector<bool> inPath(n + 1, false);
    vector<int> nxt(n + 1, -1), prv(n + 1, -1);
    vector<bool> hasIn(n + 1, false), hasOut(n + 1, false);
    deque<int> activeQueue;
    vector<bool> inActive(n + 1, false);
    int head = start, tail = start;
    inPath[start] = true;
    int visitedCount = 1;
    auto updateConnections = [&](int v) {
        for (int u : adjOut[v]) {
            if (inPath[u]) continue;
            if (!hasIn[u]) {
                hasIn[u] = true;
                if (hasOut[u] && !inActive[u]) {
                    activeQueue.push_back(u);
                    inActive[u] = true;
                }
            }
        }
        for (int u : adjIn[v]) {
            if (inPath[u]) continue;
            if (!hasOut[u]) {
                hasOut[u] = true;
                if (hasIn[u] && !inActive[u]) {
                    activeQueue.push_back(u);
                    inActive[u] = true;
                }
            }
        }
    };
    updateConnections(start);
    while (visitedCount < n) {
        bool extended = false;
        vector<int> tailCandidates;
        for (int v : adjOut[tail]) {
            if (!inPath[v]) tailCandidates.push_back(v);
        }
        if (!tailCandidates.empty()) {
            int bestV = -1, bestOut = INT_MAX;
            for (int v : tailCandidates) {
                if (outdeg[v] < bestOut) {
                    bestOut = outdeg[v];
                    bestV = v;
                }
            }
            int v = bestV;
            nxt[tail] = v;
            prv[v] = tail;
            nxt[v] = -1;
            tail = v;
            inPath[v] = true;
            visitedCount++;
            extended = true;
            updateConnections(v);
            continue;
        }
        vector<int> headCandidates;
        for (int v : adjIn[head]) {
            if (!inPath[v]) headCandidates.push_back(v);
        }
        if (!headCandidates.empty()) {
            int bestV = -1, bestOut = INT_MAX;
            for (int v : headCandidates) {
                if (outdeg[v] < bestOut) {
                    bestOut = outdeg[v];
                    bestV = v;
                }
            }
            int v = bestV;
            prv[head] = v;
            nxt[v] = head;
            prv[v] = -1;
            head = v;
            inPath[v] = true;
            visitedCount++;
            extended = true;
            updateConnections(v);
            continue;
        }
        bool inserted = false;
        int qs = activeQueue.size();
        for (int it = 0; it < qs; ++it) {
            int v = activeQueue.front();
            activeQueue.pop_front();
            if (inPath[v]) continue;
            bool success = false;
            for (int x : adjIn[v]) {
                if (!inPath[x]) continue;
                int y = nxt[x];
                if (y == -1) continue;
                if (binary_search(adjOut[v].begin(), adjOut[v].end(), y)) {
                    nxt[x] = v;
                    prv[v] = x;
                    nxt[v] = y;
                    prv[y] = v;
                    inPath[v] = true;
                    visitedCount++;
                    updateConnections(v);
                    success = true;
                    inserted = true;
                    break;
                }
            }
            if (success) break;
            for (int y : adjOut[v]) {
                if (!inPath[y]) continue;
                int x = prv[y];
                if (x == -1) continue;
                if (binary_search(adjOut[x].begin(), adjOut[x].end(), v)) {
                    nxt[x] = v;
                    prv[v] = x;
                    nxt[v] = y;
                    prv[y] = v;
                    inPath[v] = true;
                    visitedCount++;
                    updateConnections(v);
                    success = true;
                    inserted = true;
                    break;
                }
            }
            if (success) break;
            activeQueue.push_back(v);
        }
        if (inserted) continue;
        break;
    }
    vector<int> pathVec;
    int cur = head;
    while (cur != -1) {
        pathVec.push_back(cur);
        cur = nxt[cur];
    }
    cout << pathVec.size() << "\n";
    for (int i = 0; i < (int)pathVec.size(); ++i) {
        if (i) cout << " ";
        cout << pathVec[i];
    }
    cout << "\n";
    return 0;
}