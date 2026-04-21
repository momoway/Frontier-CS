#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    if (!(cin >> n)) return 0;
    vector<int> p(n);
    for (int i = 0; i < n; ++i) cin >> p[i];
    vector<int> a, b, c, d;
    bool active_a = true, active_c = true, active_b = true, active_d = true;
    for (int i = 0; i < n; ++i) {
        int x = p[i];
        bool placed = false;
        // Try increasing subsequence a
        if (active_a && (a.empty() || a.back() < x)) {
            if (active_c && !c.empty() && c.back() < x) {
                // Both a and c can take x, choose the one with larger last
                if (a.empty() || (!c.empty() && c.back() > a.back())) {
                    c.push_back(x);
                } else {
                    a.push_back(x);
                }
                placed = true;
            } else {
                a.push_back(x);
                placed = true;
            }
        }
        // Try increasing subsequence c
        if (!placed && active_c && (c.empty() || c.back() < x)) {
            c.push_back(x);
            placed = true;
        }
        // Try decreasing subsequence b
        if (!placed && active_b && (b.empty() || b.back() > x)) {
            if (active_d && !d.empty() && d.back() > x) {
                // Both b and d can take x, choose the one with smaller last
                if (b.empty() || (!d.empty() && d.back() < b.back())) {
                    d.push_back(x);
                } else {
                    b.push_back(x);
                }
                placed = true;
            } else {
                b.push_back(x);
                placed = true;
            }
        }
        // Try decreasing subsequence d
        if (!placed && active_d && (d.empty() || d.back() > x)) {
            d.push_back(x);
            placed = true;
        }
        // If still not placed, dump into a and inactivate a
        if (!placed) {
            a.push_back(x);
            active_a = false;
        }
    }
    cout << a.size() << " " << b.size() << " " << c.size() << " " << d.size() << "\n";
    for (int x : a) cout << x << " ";
    cout << "\n";
    for (int x : b) cout << x << " ";
    cout << "\n";
    for (int x : c) cout << x << " ";
    cout << "\n";
    for (int x : d) cout << x << " ";
    cout << "\n";
    return 0;
}