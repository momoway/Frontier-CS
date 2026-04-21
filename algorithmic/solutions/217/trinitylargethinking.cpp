#include <bits/stdc++.h>
using namespace std;

int N, M, L;
vector<bool> assigned;
vector<vector<int>> clusters;
vector<int> activeReps;
int queryCount = 0;
int limit;

int query(const vector<int>& v) {
    cout << "? " << v.size();
    for (int x : v) cout << " " << x;
    cout << endl;
    cout.flush();
    int ans;
    cin >> ans;
    return ans;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> N >> M;
    L = N * M;
    assigned.assign(L + 1, false);
    if (M == 1) {
        for (int i = 1; i <= L; ++i) {
            cout << "! " << i << endl;
            cout.flush();
        }
        return 0;
    }
    limit = 5 * N * M;
    for (int i = 1; i <= L; ++i) {
        if (assigned[i]) continue;
        bool found = false;
        for (int ridx : activeReps) {
            int rep = clusters[ridx][0];
            vector<int> comp;
            comp.reserve(L - 2);
            for (int idx = 1; idx <= L; ++idx) {
                if (idx == i || idx == rep) continue;
                comp.push_back(idx);
            }
            int ans = query(comp);
            queryCount++;
            if (ans == M - 2) {
                clusters[ridx].push_back(i);
                assigned[i] = true;
                if ((int)clusters[ridx].size() == M) {
                    auto it = find(activeReps.begin(), activeReps.end(), ridx);
                    if (it != activeReps.end()) {
                        *it = activeReps.back();
                        activeReps.pop_back();
                    }
                }
                found = true;
                break;
            }
        }
        if (found) continue;
        clusters.push_back({i});
        assigned[i] = true;
        if (M > 1) activeReps.push_back((int)clusters.size() - 1);
        if (queryCount >= limit) break;
    }
    for (auto& cl : clusters) sort(cl.begin(), cl.end());
    for (int j = 0; j < M; ++j) {
        cout << "! ";
        for (int i = 0; i < N; ++i) {
            if (j < (int)clusters[i].size()) cout << clusters[i][j];
            else cout << clusters[i][0];
            if (i == N - 1) cout << endl;
            else cout << " ";
        }
        cout.flush();
    }
    return 0;
}