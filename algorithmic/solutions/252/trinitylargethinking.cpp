#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;

    vector<int> a(n + 1);
    for (int i = 1; i <= n; ++i) {
        vector<int> cand;
        cand.reserve(n);
        for (int v = 1; v <= n; ++v) cand.push_back(v);

        while (cand.size() > 1) {
            int mid = cand.size() / 2;
            vector<int> left(cand.begin(), cand.begin() + mid);
            vector<int> right(cand.begin() + mid, cand.end());

            cout << "? " << i << " " << 1 << " " << left.size();
            for (int x : left) cout << " " << x;
            cout << endl;

            int reply;
            cin >> reply;
            if (reply == 1) {
                cand = left;
            } else {
                cand = right;
            }
        }
        a[i] = cand[0];
    }

    vector<vector<int>> rev(n + 1);
    for (int i = 1; i <= n; ++i) {
        rev[a[i]].push_back(i);
    }

    vector<int> order;
    vector<bool> visited(n + 1, false);
    int cur = 1;
    while (!visited[cur]) {
        visited[cur] = true;
        order.push_back(cur);
        cur = a[cur];
    }
    auto it = find(order.begin(), order.end(), cur);
    int pos = it - order.begin();
    vector<bool> in_cycle(n + 1, false);
    for (int i = pos; i < (int)order.size(); ++i) in_cycle[order[i]] = true;

    vector<bool> in_component(n + 1, false);
    queue<int> q;
    for (int i = 1; i <= n; ++i) {
        if (in_cycle[i]) {
            in_component[i] = true;
            q.push(i);
        }
    }
    while (!q.empty()) {
        int v = q.front(); q.pop();
        for (int u : rev[v]) {
            if (!in_component[u]) {
                in_component[u] = true;
                q.push(u);
            }
        }
    }

    vector<int> A;
    for (int i = 1; i <= n; ++i) {
        if (in_component[i]) A.push_back(i);
    }

    cout << "! " << A.size();
    for (int x : A) cout << " " << x;
    cout << endl;

    return 0;
}