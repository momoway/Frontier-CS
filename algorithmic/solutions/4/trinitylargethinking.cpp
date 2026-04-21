#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

int n;
int64 k;

int64 query(int x, int y) {
    cout << "QUERY " << x << " " << y << endl;
    cout.flush();
    int64 v;
    cin >> v;
    return v;
}

// Count number of elements <= value, but stop early if count >= k
int64 count_le(int64 value) {
    int64 cnt = 0;
    int r = 1, c = n;
    while (r <= n && c >= 1) {
        int64 v = query(r, c);
        if (v <= value) {
            cnt += c; // all columns 1..c in this row are <= value
            r++;
        } else {
            c--;
        }
        if (cnt >= k) break; // early exit
    }
    return cnt;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> n >> k;

    if (k == 1) {
        int64 ans = query(1, 1);
        cout << "DONE " << ans << endl;
        return 0;
    }
    if (k == (int64)n * n) {
        int64 ans = query(n, n);
        cout << "DONE " << ans << endl;
        return 0;
    }

    // Get initial bounds using diagonal
    int64 low = query(1, 1);
    int64 high;
    if (k == 1) {
        high = low;
    } else {
        int i = (int)ceil(sqrt((double)k));
        if (i > n) i = n;
        high = query(i, i);
    }

    // Binary search for the smallest value such that count_le >= k
    while (low < high) {
        int64 mid = low / 2 + high / 2; // avoid overflow
        if (count_le(mid) >= k) {
            high = mid;
        } else {
            low = mid + 1;
        }
    }

    cout << "DONE " << low << endl;
    return 0;
}