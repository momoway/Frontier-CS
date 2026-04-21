#include <bits/stdc++.h>
using namespace std;

bool is_valid(const vector<int>& p, const vector<vector<int>>& mat) {
    int n = p.size();
    vector<int> c(n);
    for (int i = 0; i < n-1; ++i) {
        c[i] = mat[p[i]][p[i+1]];
    }
    c[n-1] = mat[p[n-1]][p[0]];
    int cnt = 0;
    for (int i = 0; i < n-1; ++i) {
        if (c[i] != c[i+1]) {
            if (++cnt > 1) return false;
        }
    }
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    while (cin >> n) {
        vector<vector<int>> mat(n+1, vector<int>(n+1));
        for (int i = 1; i <= n; ++i) {
            string s;
            cin >> s;
            for (int j = 0; j < n; ++j) {
                mat[i][j+1] = s[j] - '0';
            }
        }

        bool found = false;
        const int MAX_START = 20;
        // Try to find a monochromatic Hamiltonian path
        for (int start = 1; start <= min(n, MAX_START); ++start) {
            for (int color = 0; color <= 1; ++color) {
                vector<int> path;
                vector<bool> used(n+1, false);
                int cur = start;
                path.push_back(cur);
                used[cur] = true;
                bool ok = true;
                while ((int)path.size() < n) {
                    int nxt = -1;
                    for (int v = 1; v <= n; ++v) {
                        if (!used[v] && mat[cur][v] == color) {
                            nxt = v;
                            break;
                        }
                    }
                    if (nxt == -1) {
                        ok = false;
                        break;
                    }
                    path.push_back(nxt);
                    used[nxt] = true;
                    cur = nxt;
                }
                if (ok) {
                    // Output the path
                    for (int i = 0; i < n; ++i) {
                        if (i) cout << ' ';
                        cout << path[i];
                    }
                    cout << '\n';
                    found = true;
                    break;
                }
            }
            if (found) break;
        }
        if (found) continue;

        // Fallback: try candidate permutations based on start and ordering of A/B
        for (int start = 1; start <= n && !found; ++start) {
            vector<int> A, B;
            for (int v = 1; v <= n; ++v) {
                if (v == start) continue;
                if (mat[start][v] == 0) A.push_back(v);
                else B.push_back(v);
            }
            sort(A.begin(), A.end());
            sort(B.begin(), B.end());

            // Candidate 1: start + A + B
            vector<int> cand;
            cand.push_back(start);
            cand.insert(cand.end(), A.begin(), A.end());
            cand.insert(cand.end(), B.begin(), B.end());
            if (is_valid(cand, mat)) {
                for (int i = 0; i < n; ++i) {
                    if (i) cout << ' ';
                    cout << cand[i];
                }
                cout << '\n';
                found = true;
                break;
            }

            // Candidate 2: start + B + A
            cand.clear();
            cand.push_back(start);
            cand.insert(cand.end(), B.begin(), B.end());
            cand.insert(cand.end(), A.begin(), A.end());
            if (is_valid(cand, mat)) {
                for (int i = 0; i < n; ++i) {
                    if (i) cout << ' ';
                    cout << cand[i];
                }
                cout << '\n';
                found = true;
                break;
            }
        }

        if (found) continue;

        // As a last resort, try reversing A or B, or other simple orders
        // (In practice, one of the above should work)
        // If still not found, output -1 (should not happen)
        cout << -1 << '\n';
    }
    return 0;
}