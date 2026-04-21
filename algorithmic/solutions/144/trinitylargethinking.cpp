#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    if (!(cin >> n)) return 0;
    int A = n / 2;
    int B = n / 2 + 1;
    vector<bool> active(n + 1, true);
    vector<int> actives;
    for (int i = 1; i <= n; ++i) actives.push_back(i);
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    int queries = 0;
    while (actives.size() > 2 && queries < 500) {
        int idx1 = uniform_int_distribution<int>(0, (int)actives.size() - 1)(rng);
        int idx2 = uniform_int_distribution<int>(0, (int)actives.size() - 1)(rng);
        while (idx2 == idx1) {
            idx2 = uniform_int_distribution<int>(0, (int)actives.size() - 1)(rng);
        }
        int i = actives[idx1];
        int j = actives[idx2];
        cout << "0 " << (n - 2);
        for (int idx = 1; idx <= n; ++idx) {
            if (idx != i && idx != j) {
                cout << " " << idx;
            }
        }
        cout << endl;
        cout.flush();
        int m1, m2;
        cin >> m1 >> m2;
        ++queries;
        if (m1 == A - 1 && m2 == A + 2) {
            cout << "1 " << i << " " << j << endl;
            return 0;
        } else if (m1 == A && m2 == B) {
            active[i] = false;
            active[j] = false;
            actives.clear();
            for (int idx = 1; idx <= n; ++idx) if (active[idx]) actives.push_back(idx);
        }
    }
    if (actives.size() == 2) {
        cout << "1 " << actives[0] << " " << actives[1] << endl;
    } else {
        if (actives.size() >= 2) {
            cout << "1 " << actives[0] << " " << actives[1] << endl;
        } else if (actives.size() == 1) {
            cout << "1 " << actives[0] << " " << (actives[0] == 1 ? 2 : 1) << endl;
        } else {
            cout << "1 1 2" << endl;
        }
    }
    return 0;
}