#include <bits/stdc++.h>
using namespace std;

const int N = 400;
const int M = 1995;

int parent[N];
int sz[N];

int find(int a) {
    return parent[a] == a ? a : parent[a] = find(parent[a]);
}

void unionSets(int a, int b) {
    a = find(a);
    b = find(b);
    if (a != b) {
        if (sz[a] < sz[b]) swap(a, b);
        parent[b] = a;
        sz[a] += sz[b];
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    double x[N], y[N];
    for (int i = 0; i < N; ++i) {
        cin >> x[i] >> y[i];
    }

    int u[M], v[M];
    for (int i = 0; i < M; ++i) {
        cin >> u[i] >> v[i];
    }

    int d[M];
    for (int i = 0; i < M; ++i) {
        double dx = x[u[i]] - x[v[i]];
        double dy = y[u[i]] - y[v[i]];
        double dist = sqrt(dx*dx + dy*dy);
        d[i] = (int)(dist + 0.5);
    }

    for (int i = 0; i < N; ++i) {
        parent[i] = i;
        sz[i] = 1;
    }

    const double alpha = 0.5;

    for (int i = 0; i < M; ++i) {
        int w;
        cin >> w;
        int ru = find(u[i]);
        int rv = find(v[i]);
        if (ru == rv) {
            cout << 0 << '\n' << flush;
            continue;
        }

        bool isRoot[N];
        memset(isRoot, 0, sizeof(isRoot));
        vector<int> rootsList;
        for (int vtx = 0; vtx < N; ++vtx) {
            int r = find(vtx);
            if (!isRoot[r]) {
                isRoot[r] = true;
                rootsList.push_back(r);
            }
        }
        int C = rootsList.size();

        vector<int> rootIdx(N, -1);
        for (int idx = 0; idx < C; ++idx) {
            rootIdx[rootsList[idx]] = idx;
        }

        vector<int> tempParent(C);
        vector<int> tempSize(C, 1);
        for (int idx = 0; idx < C; ++idx) tempParent[idx] = idx;

        function<int(int)> tempFind = [&](int a) {
            return tempParent[a] == a ? a : tempParent[a] = tempFind(tempParent[a]);
        };
        auto tempUnion = [&](int a, int b) {
            a = tempFind(a);
            b = tempFind(b);
            if (a != b) {
                if (tempSize[a] < tempSize[b]) swap(a, b);
                tempParent[b] = a;
                tempSize[a] += tempSize[b];
            }
        };

        for (int j = i+1; j < M; ++j) {
            int ru_j = find(u[j]);
            int rv_j = find(v[j]);
            if (ru_j != rv_j) {
                int a = rootIdx[ru_j];
                int b = rootIdx[rv_j];
                tempUnion(a, b);
            }
        }

        bool connected = true;
        int root0 = tempFind(0);
        for (int idx = 1; idx < C; ++idx) {
            if (tempFind(idx) != root0) {
                connected = false;
                break;
            }
        }

        if (!connected) {
            cout << 1 << '\n' << flush;
            unionSets(ru, rv);
            continue;
        }

        double T0 = 2.0 * d[i];
        double factor = 1.0 + alpha * (C - 1) / (double)(M - i);
        double T = T0 * factor;
        if ((double)w < T - 1e-9) {
            cout << 1 << '\n' << flush;
            unionSets(ru, rv);
        } else {
            cout << 0 << '\n' << flush;
        }
    }

    return 0;
}