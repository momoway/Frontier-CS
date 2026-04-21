#include <bits/stdc++.h>
using namespace std;

int n;
int m0;
vector<int> d; // d[1..n-1]
int p[1005];
bool used[1005];
int found_fixed;
int total_fixed;

bool dfs(int i) {
    if (i > n) {
        return found_fixed == total_fixed;
    }
    vector<int> candidates;
    if (i == 1) {
        for (int v = 1; v <= n; v++) {
            if (!used[v]) candidates.push_back(v);
        }
    } else {
        int prev = p[i-1];
        int known = 0;
        if (prev == i) known++;
        if (prev == i-1) known--;
        int rhs = d[i-1] - known;
        if (rhs == 1) {
            int v = i-1;
            if (v >= 1 && v <= n && !used[v]) candidates.push_back(v);
        } else if (rhs == -1) {
            int v = i;
            if (!used[v]) candidates.push_back(v);
        } else if (rhs == 0) {
            for (int v = 1; v <= n; v++) {
                if (!used[v] && v != i-1 && v != i) candidates.push_back(v);
            }
        } else {
            return false;
        }
    }
    if (candidates.empty()) return false;
    for (int v : candidates) {
        int add_fixed = (v == i) ? 1 : 0;
        if (found_fixed + add_fixed > total_fixed) continue;
        // Compute max possible additional fixed points from positions i+1..n
        int count = 0;
        for (int j = i+1; j <= n; j++) {
            if (!used[j]) {
                if (j == v) continue; // value j will be used by p[i]
                count++;
            }
        }
        if (found_fixed + add_fixed + count < total_fixed) continue;
        // Assign
        p[i] = v;
        used[v] = true;
        int old_found = found_fixed;
        if (v == i) found_fixed++;
        bool ok = dfs(i+1);
        if (ok) return true;
        // Backtrack
        used[v] = false;
        found_fixed = old_found;
        p[i] = 0;
    }
    return false;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> n;
    if (n == 1) {
        // No need to query, just guess
        cout << "1 1" << endl;
        cout.flush();
        return 0;
    }
    // Identity query
    cout << "0";
    for (int i = 1; i <= n; i++) {
        cout << " " << i;
    }
    cout << endl;
    cout.flush();
    cin >> m0;
    d.assign(n+1, 0); // d[1..n-1]
    for (int i = 1; i <= n-1; i++) {
        cout << "0";
        for (int j = 1; j <= n; j++) {
            if (j == i) cout << " " << (i+1);
            else if (j == i+1) cout << " " << i;
            else cout << " " << j;
        }
        cout << endl;
        cout.flush();
        int ans;
        cin >> ans;
        d[i] = ans - m0;
    }
    // Initialize
    memset(used, 0, sizeof(used));
    memset(p, 0, sizeof(p));
    found_fixed = 0;
    total_fixed = m0;
    bool ok = dfs(1);
    if (ok) {
        cout << "1";
        for (int i = 1; i <= n; i++) {
            cout << " " << p[i];
        }
        cout << endl;
        cout.flush();
    } else {
        // Fallback: output identity (should not happen)
        cout << "1";
        for (int i = 1; i <= n; i++) {
            cout << " " << i;
        }
        cout << endl;
        cout.flush();
    }
    return 0;
}