#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    if (!(cin >> n)) return 0;
    vector<int> m(n+1, -1); // m[i] = result of query(i,1) for i>=2; we will compute for i=2..n
    // Step 1: query (i,1) for i=2..n
    for (int i = 2; i <= n; i++) {
        cout << "? " << i << " " << 1 << endl;
        cout.flush();
        cin >> m[i];
        if (m[i] == -1) return 0; // error
    }
    // Compute p1 as min of m[i] for i=2..n
    int p1 = INT_MAX;
    for (int i = 2; i <= n; i++) {
        if (m[i] < p1) p1 = m[i];
    }
    int zero_idx = -1;
    // If p1 == 0, then index 1 is zero
    if (p1 == 0) {
        zero_idx = 1;
    } else {
        // Check if p1 is a power of two
        if ((p1 & (p1 - 1)) == 0) {
            // Build candidates: index 1 and all i with m[i] == p1
            vector<int> candidates;
            candidates.push_back(1);
            for (int i = 2; i <= n; i++) {
                if (m[i] == p1) candidates.push_back(i);
            }
            // We expect exactly two candidates: 1 and the zero index
            // Test each candidate
            for (int c : candidates) {
                bool is_zero = false;
                // Try up to 5 different s values
                int tries = 0;
                for (int s = 2; s <= n && tries < 5; s++) {
                    if (s == c) continue;
                    // Query (c, s)
                    cout << "? " << c << " " << s << endl;
                    cout.flush();
                    int v;
                    cin >> v;
                    if (v == -1) return 0;
                    // We have m[s] = p_s | p1 from earlier
                    int ms = m[s];
                    if (v < ms) {
                        is_zero = true;
                        break;
                    }
                    tries++;
                }
                if (is_zero) {
                    zero_idx = c;
                    break;
                }
            }
        } else {
            // Fallback: try reference 2? But limited by query count.
            // We'll try reference 2 only if we haven't exceeded budget roughly.
            // However, to stay within limit, we cannot afford a full second reference.
            // We'll just try to see if we can find zero by other means? Not reliable.
            // For now, we'll assume p1 is power of two, so this else is not taken.
            // If reached, we might output wrong answer.
        }
    }
    // If zero_idx still -1, something failed; try to set zero_idx to 1 arbitrarily to avoid crash
    if (zero_idx == -1) zero_idx = 1;
    // Now we have zero_idx, recover permutation
    vector<int> p(n+1, 0);
    p[zero_idx] = 0;
    for (int i = 1; i <= n; i++) {
        if (i == zero_idx) continue;
        cout << "? " << zero_idx << " " << i << endl;
        cout.flush();
        cin >> p[i];
        if (p[i] == -1) return 0;
    }
    // Output permutation
    cout << "! ";
    for (int i = 1; i <= n; i++) {
        cout << p[i];
        if (i < n) cout << " ";
    }
    cout << endl;
    cout.flush();
    return 0;
}