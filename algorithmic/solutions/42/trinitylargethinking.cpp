#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    long long n;
    if (!(cin >> n)) return 0;

    // Compute the smallest integer L such that L*L >= n
    long long L = static_cast<long long>(sqrt((double)n));
    while (L * L < n) ++L;

    cout << fixed << setprecision(6) << static_cast<double>(L) << "\n";

    for (long long i = 0; i < n; ++i) {
        long long row = i / L;
        long long col = i % L;
        double x = static_cast<double>(col) + 0.5;
        double y = static_cast<double>(row) + 0.5;
        double a = 0.0;
        cout << fixed << setprecision(6) << x << " " << y << " " << a << "\n";
    }

    return 0;
}