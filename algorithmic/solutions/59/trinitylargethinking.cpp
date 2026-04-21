#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    if(!(cin >> n)) return 0;
    vector<int> ans(n+1);
    set<int> freePos;
    for (int i = 1; i <= n; ++i) freePos.insert(i);
    map<pair<int,int>, bool> cmp;
    auto query = [&](int i, int j) -> bool {
        if (i == j) return false;
        if (cmp.count({i,j})) return cmp[{i,j}];
        if (cmp.count({j,i})) return !cmp[{j,i}];
        cout << "? " << i << " " << j << endl;
        char r; cin >> r;
        bool gt = (r == '>');
        cmp[{i,j}] = gt;
        return gt;
    };
    for (int v = n; v >= 1; --v) {
        vector<int> cand;
        int lo = max(1, v-2);
        for (int i = lo; i <= n; ++i) if (freePos.count(i)) cand.push_back(i);
        int pos = cand[0];
        for (int i = 1; i < (int)cand.size(); ++i) {
            if (!query(pos, cand[i])) pos = cand[i];
        }
        ans[pos] = v;
        freePos.erase(pos);
    }
    cout << "! ";
    for (int i = 1; i <= n; ++i) cout << ans[i] << " \n"[i==n];
    cout.flush();
    return 0;
}