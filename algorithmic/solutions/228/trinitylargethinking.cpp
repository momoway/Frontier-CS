#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string s;
    if (!(cin >> s)) return 0;
    int n = s.size();
    vector<int> pref(n + 1);
    pref[0] = 0;
    for (int i = 0; i < n; ++i) {
        pref[i + 1] = pref[i] + (s[i] == '1');
    }
    long long ans = 0;
    int total_ones = pref[n];
    for (int k = 1; ; ++k) {
        int L = k * k + k;
        if (L > n) break;
        if (k > total_ones) continue;
        int limit = n - L;
        const int* p = pref.data();
        for (int i = 0; i <= limit; ++i) {
            if (p[i + L] - p[i] == k) ++ans;
        }
    }
    cout << ans << '\n';
    return 0;
}