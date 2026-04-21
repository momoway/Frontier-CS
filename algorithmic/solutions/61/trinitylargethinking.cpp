#include <bits/stdc++.h>
using namespace std;

int n, m;
long long c;
vector<long long> A;
vector<long long> B;
vector<long long> dp;
vector<int> prevIdx;

long long f(long long d) {
    int idx = upper_bound(B.begin(), B.end(), d) - B.begin() - 1;
    return idx;
}

void solve(int l, int r, int optL, int optR) {
    if (l > r) return;
    int mid = (l + r) >> 1;
    solve(l, mid - 1, optL, optR);
    long long best = LLONG_MIN / 4;
    int bestK = -1;
    int start = optL;
    int end = min(mid - 1, optR);
    for (int k = start; k <= end; ++k) {
        long long val = dp[k] + f(A[mid] - A[k]) - c;
        if (val > best) {
            best = val;
            bestK = k;
        }
    }
    dp[mid] = best;
    prevIdx[mid] = bestK;
    solve(mid + 1, r, bestK, optR);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T;
    if (!(cin >> T)) return 0;
    while (T--) {
        cin >> n >> m >> c;
        A.assign(n + 1, 0);
        for (int i = 1; i <= n; ++i) {
            cin >> A[i];
            A[i] += A[i - 1];
        }
        B.assign(m + 1, 0);
        for (int i = 1; i <= m; ++i) {
            long long b;
            cin >> b;
            B[i] = B[i - 1] + b;
        }
        dp.assign(n + 1, LLONG_MIN / 4);
        dp[0] = 0;
        prevIdx.assign(n + 1, -1);
        if (n >= 1) {
            solve(1, n, 0, n - 1);
        }
        vector<pair<int, int>> segs;
        int i = n;
        while (i > 0) {
            int j = prevIdx[i];
            segs.push_back({j + 1, i});
            i = j;
        }
        reverse(segs.begin(), segs.end());
        cout << segs.size() << "\n";
        for (auto &p : segs) {
            cout << p.first << " " << p.second << "\n";
        }
    }
    return 0;
}