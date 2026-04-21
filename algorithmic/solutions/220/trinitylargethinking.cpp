#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    if (!(cin >> n)) return 0;
    vector<vector<int>> cnt(n, vector<int>(n, 0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int x;
            cin >> x;
            x--; // to 0-index
            cnt[i][x]++;
        }
    }

    vector<vector<int>> ops; // each op: vector of n ints (0-index numbers)
    const int MAX_OPS = n * (n - 1);
    for (int step = 0; step < MAX_OPS; step++) {
        vector<int> pass(n);
        // decide pass for each player
        for (int i = 0; i < n; i++) {
            int target = (i + 1) % n; // neighbor's number
            if (cnt[i][target] > 0) {
                pass[i] = target;
            } else {
                int v = -1;
                for (int num = 0; num < n; num++) {
                    if (num != i && cnt[i][num] > 0) {
                        if (v == -1 || num < v) v = num;
                    }
                }
                if (v != -1) pass[i] = v;
                else pass[i] = i; // only own numbers left
            }
        }
        ops.push_back(pass);
        // perform the operation
        vector<int> receive(n);
        for (int i = 0; i < n; i++) {
            int left = (i - 1 + n) % n;
            receive[i] = pass[left];
        }
        for (int i = 0; i < n; i++) {
            cnt[i][pass[i]]--;
            cnt[i][receive[i]]++;
        }
        // check if all solid
        bool solid = true;
        for (int i = 0; i < n; i++) {
            if (cnt[i][i] != n) {
                solid = false;
                break;
            }
        }
        if (solid) break;
    }

    // Output
    cout << ops.size() << "\n";
    for (auto &op : ops) {
        for (int i = 0; i < n; i++) {
            if (i) cout << ' ';
            cout << op[i] + 1;
        }
        cout << "\n";
    }
    return 0;
}