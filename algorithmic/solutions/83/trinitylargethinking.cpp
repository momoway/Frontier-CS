#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;

    vector<int> spf(n + 1, 0);
    vector<int> primes;
    for (int i = 2; i <= n; ++i) {
        if (spf[i] == 0) {
            spf[i] = i;
            primes.push_back(i);
        }
        for (int j = 0; j < (int)primes.size() && primes[j] <= spf[i]; ++j) {
            long long x = 1LL * i * primes[j];
            if (x > n) break;
            spf[x] = primes[j];
        }
    }

    vector<int> f(n + 1);
    f[1] = 1;
    for (int i = 2; i <= n; ++i) {
        int p = spf[i];
        f[i] = -f[i / p];
    }

    for (int i = 1; i <= n; ++i) {
        cout << f[i];
        if (i < n) cout << ' ';
    }
    cout << '\n';

    return 0;
}