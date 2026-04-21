#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    if (!(cin >> n)) return 0;
    long long target = n / 2;
    int req = (int)floor(sqrt((double)target));
    while ((long long)(req + 1) * (req + 1) <= target) ++req;
    while ((long long)req * req > target) --req;
    if (req == 0) req = 1; // ensure at least one element

    int limit = 1;
    while (limit <= n) limit <<= 1;
    vector<char> seen(limit, 0);
    vector<int> S;
    S.reserve(10000);

    for (int x = 1; x <= n && (int)S.size() < req; ++x) {
        bool ok = true;
        for (int y : S) {
            int xr = x ^ y;
            if (seen[xr]) {
                ok = false;
                break;
            }
        }
        if (ok) {
            for (int y : S) {
                int xr = x ^ y;
                seen[xr] = 1;
            }
            S.push_back(x);
        }
    }

    cout << S.size() << "\n";
    for (size_t i = 0; i < S.size(); ++i) {
        if (i) cout << ' ';
        cout << S[i];
    }
    cout << "\n";
    return 0;
}