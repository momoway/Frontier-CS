#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout << fixed << setprecision(10);

    int n;
    if (!(cin >> n)) return 0;

    if (n == 2) {
        long double sqrt3 = sqrtl(3.0);
        long double r = sqrt3 / (2.0 + 2.0 * sqrt3);
        cout << (double)r << " " << (double)r << " " << (double)r << "\n";
        cout << (double)(1.0 - r) << " " << (double)(1.0 - r) << " " << (double)(1.0 - r) << "\n";
    } else if (n == 3) {
        long double sqrt2 = sqrtl(2.0);
        long double r = 1.0L / (2.0L + sqrt2);
        long double a = 2.0L * sqrt2 * r;
        long double offset = r;
        vector<array<long double,3>> pts;
        pts.push_back({offset, offset, offset});
        pts.push_back({offset + 0.5L*a, offset + 0.5L*a, offset});
        pts.push_back({offset + 0.5L*a, offset, offset + 0.5L*a});
        pts.push_back({offset, offset + 0.5L*a, offset + 0.5L*a});
        for (int i = 0; i < 3; ++i) {
            cout << (double)pts[i][0] << " " << (double)pts[i][1] << " " << (double)pts[i][2] << "\n";
        }
    } else if (n == 4) {
        long double sqrt2 = sqrtl(2.0);
        long double r = 1.0L / (2.0L + sqrt2);
        long double a = 2.0L * sqrt2 * r;
        long double offset = r;
        cout << (double)offset << " " << (double)offset << " " << (double)offset << "\n";
        cout << (double)(offset + 0.5L*a) << " " << (double)(offset + 0.5L*a) << " " << (double)offset << "\n";
        cout << (double)(offset + 0.5L*a) << " " << (double)offset << " " << (double)(offset + 0.5L*a) << "\n";
        cout << (double)offset << " " << (double)(offset + 0.5L*a) << " " << (double)(offset + 0.5L*a) << "\n";
    } else if (n >= 5 && n <= 8) {
        int d = 1;
        while (1LL*d*d*d < n) ++d;
        int cnt = 0;
        for (int i = 0; i < d && cnt < n; ++i) {
            for (int j = 0; j < d && cnt < n; ++j) {
                for (int k = 0; k < d && cnt < n; ++k) {
                    double x = (i + 0.5) / d;
                    double y = (j + 0.5) / d;
                    double z = (k + 0.5) / d;
                    cout << x << " " << y << " " << z << "\n";
                    ++cnt;
                }
            }
        }
    } else if (n == 9) {
        long double sqrt3 = sqrtl(3.0);
        long double r = sqrt3 / (2.0L * (2.0L + sqrt3));
        long double corners[2] = {r, 1.0L - r};
        int cnt = 0;
        for (int i = 0; i < 2 && cnt < 8; ++i) {
            for (int j = 0; j < 2 && cnt < 8; ++j) {
                for (int k = 0; k < 2 && cnt < 8; ++k) {
                    cout << (double)corners[i] << " " << (double)corners[j] << " " << (double)corners[k] << "\n";
                    ++cnt;
                }
            }
        }
        cout << 0.5 << " " << 0.5 << " " << 0.5 << "\n";
    } else if (n >= 10 && n <= 14) {
        long double sqrt2 = sqrtl(2.0);
        long double r = 0.5L / (1.0L + sqrt2);
        long double corners[2] = {r, 1.0L - r};
        int cnt = 0;
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                for (int k = 0; k < 2; ++k) {
                    cout << (double)corners[i] << " " << (double)corners[j] << " " << (double)corners[k] << "\n";
                    ++cnt;
                }
            }
        }
        vector<array<long double,3>> faces = {
            {0.5L, 0.5L, r},
            {0.5L, 0.5L, 1.0L - r},
            {0.5L, r, 0.5L},
            {0.5L, 1.0L - r, 0.5L},
            {r, 0.5L, 0.5L},
            {1.0L - r, 0.5L, 0.5L}
        };
        for (int i = 0; i < n - 8; ++i) {
            cout << (double)faces[i][0] << " " << (double)faces[i][1] << " " << (double)faces[i][2] << "\n";
        }
    } else if (n >= 15 && n <= 27) {
        int d = 1;
        while (1LL*d*d*d < n) ++d;
        int cnt = 0;
        for (int i = 0; i < d && cnt < n; ++i) {
            for (int j = 0; j < d && cnt < n; ++j) {
                for (int k = 0; k < d && cnt < n; ++k) {
                    double x = (i + 0.5) / d;
                    double y = (j + 0.5) / d;
                    double z = (k + 0.5) / d;
                    cout << x << " " << y << " " << z << "\n";
                    ++cnt;
                }
            }
        }
    } else { // n >= 28
        long long target = (n + 3) / 4; // ceil(n/4)
        int m = 1;
        while (1LL*m*m*m < target) ++m;
        long double sqrt2 = sqrtl(2.0);
        long double a = 1.0L / (m - 0.5L + 1.0L / sqrt2);
        long double offset = a / (2.0L * sqrt2);
        int cnt = 0;
        for (int i = 0; i < m && cnt < n; ++i) {
            for (int j = 0; j < m && cnt < n; ++j) {
                for (int k = 0; k < m && cnt < n; ++k) {
                    if (cnt < n) {
                        cout << (double)(offset + i*a) << " " << (double)(offset + j*a) << " " << (double)(offset + k*a) << "\n";
                        ++cnt;
                    }
                    if (cnt < n) {
                        cout << (double)(offset + (i+0.5L)*a) << " " << (double)(offset + (j+0.5L)*a) << " " << (double)(offset + k*a) << "\n";
                        ++cnt;
                    }
                    if (cnt < n) {
                        cout << (double)(offset + (i+0.5L)*a) << " " << (double)(offset + j*a) << " " << (double)(offset + (k+0.5L)*a) << "\n";
                        ++cnt;
                    }
                    if (cnt < n) {
                        cout << (double)(offset + i*a) << " " << (double)(offset + (j+0.5L)*a) << " " << (double)(offset + (k+0.5L)*a) << "\n";
                        ++cnt;
                    }
                }
            }
        }
    }
    return 0;
}