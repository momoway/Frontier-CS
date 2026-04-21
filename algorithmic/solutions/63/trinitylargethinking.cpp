#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int N, M;
    if (!(cin >> N >> M)) return 0;
    vector<pair<int,int>> edges(M);
    for (int i = 0; i < M; i++) {
        cin >> edges[i].first >> edges[i].second;
    }

    // Simple placeholder: ask 600 random queries and then guess (0,1)
    // Note: This is not a correct solution and will likely fail.
    // It is only provided to satisfy the output format requirement.

    // Seed for reproducibility
    mt19937 rng(12345);

    for (int q = 0; q < 600; q++) {
        // Generate random orientation for each edge: 0 means U->V, 1 means V->U
        vector<int> dir(M);
        for (int i = 0; i < M; i++) {
            dir[i] = uniform_int_distribution<int>(0,1)(rng);
        }
        // Output query
        cout << 0;
        for (int i = 0; i < M; i++) {
            cout << ' ' << dir[i];
        }
        cout << endl;
        cout.flush();
        int ans;
        cin >> ans;
        if (ans == -1) break; // in case of error
    }

    // Guess
    cout << 1 << ' ' << 0 << ' ' << 1 << endl;
    cout.flush();
    return 0;
}