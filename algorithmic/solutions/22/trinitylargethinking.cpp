#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int N;
    if (!(cin >> N)) return 0;
    vector<int> parent(N + 1);
    vector<int> deg(N + 1, 0);
    for (int i = 2; i <= N; ++i) {
        cin >> parent[i];
        deg[parent[i]]++;
        deg[i]++;
    }

    vector<int> leaves;
    for (int i = 1; i <= N; ++i) {
        if (deg[i] == 1) leaves.push_back(i);
    }
    int k = leaves.size();

    vector<int> nextLeaf(N + 1, 0), prevLeaf(N + 1, 0);
    for (int idx = 0; idx < k; ++idx) {
        int l = leaves[idx];
        nextLeaf[l] = leaves[(idx + 1) % k];
        prevLeaf[l] = leaves[(idx - 1 + k) % k];
    }

    int K = N + 2 * k;
    cout << K << "\n";

    // B bags: vertices 1..N
    for (int i = 1; i <= N; ++i) {
        if (i == 1) {
            cout << "1 1\n";
        } else {
            cout << "2 " << parent[i] << " " << i << "\n";
        }
    }

    // L and R bags for leaves
    vector<int> L_idx(N + 1, 0), R_idx(N + 1, 0);
    int curIdx = N + 1;
    for (int l : leaves) {
        int nxt = nextLeaf[l];
        int prv = prevLeaf[l];
        L_idx[l] = curIdx;
        cout << "2 " << l << " " << nxt << "\n";
        curIdx++;
        R_idx[l] = curIdx;
        cout << "2 " << l << " " << prv << "\n";
        curIdx++;
    }

    // Edges among B bags
    for (int i = 2; i <= N; ++i) {
        cout << i << " " << parent[i] << "\n";
    }
    // Edges from leaf bags to L and R
    for (int l : leaves) {
        cout << l << " " << L_idx[l] << "\n";
        cout << l << " " << R_idx[l] << "\n";
    }

    return 0;
}