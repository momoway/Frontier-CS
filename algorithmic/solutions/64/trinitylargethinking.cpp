#include <bits/stdc++.h>
using namespace std;

int n;
long long T;
vector<long long> a;
vector<long long> suff;
string best_sel;
long long best_err = LLONG_MAX;
bool exact_found = false;
bool timeout = false;
chrono::high_resolution_clock::time_point start_time;
double TIME_LIMIT = 1.9; // seconds

void dfs(int idx, long long cur, string &sel) {
    if (timeout || exact_found) return;
    auto now = chrono::high_resolution_clock::now();
    double elapsed = chrono::duration<double>(now - start_time).count();
    if (elapsed > TIME_LIMIT) {
        timeout = true;
        return;
    }

    if (idx == n) {
        long long err = llabs(cur - T);
        if (err < best_err) {
            best_err = err;
            best_sel = sel;
            if (best_err == 0) exact_found = true;
        }
        return;
    }

    // Prune if even adding all remaining cannot bring within best_err of T
    if (cur + suff[idx] < T - best_err) return;
    // Prune if current sum already exceeds T by more than best_err
    if (cur > T + best_err) return;

    // Try include
    sel[idx] = '1';
    dfs(idx + 1, cur + a[idx], sel);
    if (exact_found || timeout) return;

    // Try exclude
    sel[idx] = '0';
    dfs(idx + 1, cur, sel);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    start_time = chrono::high_resolution_clock::now();

    if (!(cin >> n >> T)) return 0;
    a.resize(n);
    for (int i = 0; i < n; ++i) cin >> a[i];

    sort(a.begin(), a.end(), greater<long long>()); // descending

    suff.assign(n + 1, 0);
    for (int i = n - 1; i >= 0; --i) suff[i] = suff[i + 1] + a[i];

    string sel(n, '0');
    dfs(0, 0, sel);

    cout << best_sel << "\n";
    return 0;
}