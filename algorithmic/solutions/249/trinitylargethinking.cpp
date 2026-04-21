#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;

    // We'll use 0-indexed internally, but queries are 1-indexed.
    int a = 0, b = 1; // references

    auto query = [&](int i, int j) {
        cout << "? " << i+1 << " " << j+1 << endl;
        cout.flush();
        int res;
        cin >> res;
        if (res == -1) exit(0);
        return res;
    };

    int Z = query(a, b);

    vector<int> x(n), y(n);
    // For i from 2 to n-1 (0-indexed)
    for (int i = 2; i < n; ++i) {
        x[i] = query(a, i);
        y[i] = query(b, i);
    }

    // Precompute totalZero for each bit up to 11 (since 2^11=2048)
    const int MAXB = 11;
    vector<int> totalZero(MAXB, 0);
    for (int bit = 0; bit < MAXB; ++bit) {
        int mask = 1 << bit;
        for (int v = 0; v < n; ++v) {
            if ((v & mask) == 0) totalZero[bit]++;
        }
    }

    vector<int> c0x(MAXB, 0), c0y(MAXB, 0);
    for (int i = 2; i < n; ++i) {
        for (int bit = 0; bit < MAXB; ++bit) {
            if ((x[i] & (1 << bit)) == 0) c0x[bit]++;
            if ((y[i] & (1 << bit)) == 0) c0y[bit]++;
        }
    }

    int A = 0, B = 0;
    for (int bit = 0; bit < MAXB; ++bit) {
        int zbit = (Z >> bit) & 1;
        int tot = totalZero[bit];
        int cx = c0x[bit];
        int cy = c0y[bit];
        bool found = false;
        // Case (0,0): Zbit=0, cx = tot-2, cy = tot-2
        if (zbit == 0) {
            if (cx == tot - 2 && cy == tot - 2) {
                // a_bit=0, b_bit=0
                // A_bit=0, B_bit=0 -> nothing to set
                found = true;
            }
        }
        if (!found && zbit == 1) {
            // Case (0,1): cx = tot-1, cy = 0
            if (cx == tot - 1 && cy == 0) {
                // a_bit=0, b_bit=1 -> set B_bit
                B |= (1 << bit);
                found = true;
            }
            // Case (1,0): cx = 0, cy = tot-1
            else if (cx == 0 && cy == tot - 1) {
                // a_bit=1, b_bit=0 -> set A_bit
                A |= (1 << bit);
                found = true;
            }
            // Case (1,1): cx = 0, cy = 0
            else if (cx == 0 && cy == 0) {
                // a_bit=1, b_bit=1 -> set both
                A |= (1 << bit);
                B |= (1 << bit);
                found = true;
            }
        }
        if (!found) {
            // If none match, we may have an issue; but we try to deduce from other info?
            // As fallback, we can set based on minima? Not implemented.
            // We'll set to 0 for safety, but this may be wrong.
        }
    }

    int D = A & B;
    vector<int> p(n, -1);
    p[a] = A;
    p[b] = B;

    if (D == 0) {
        // Use formula p_i = (x_i & ~A) | (y_i & ~B)
        for (int i = 2; i < n; ++i) {
            int part1 = x[i] & ~A;
            int part2 = y[i] & ~B;
            p[i] = part1 | part2;
        }
    } else {
        // Fallback: try to use third reference to resolve collisions.
        // Choose c not equal to a or b.
        int c = (a == 0 && b == 1) ? 2 : 0; // simplistic
        if (c == a || c == b) c = (a == 0 ? 2 : 0);
        if (c == a || c == b) c = 3;
        if (c >= n) c = n-1; // fallback

        // Query (c,a) and (c,b)
        int Xc = query(c, a);
        int Yc = query(c, b);

        // For each i in S, if (x[i],y[i]) == (A,B), we need to determine p[i].
        // We'll query (c,i) for those i.
        vector<int> need_query;
        for (int i = 2; i < n; ++i) {
            if (x[i] == A && y[i] == B) {
                need_query.push_back(i);
            }
        }
        // For i not in need_query, we can compute p[i] using formula? Might be ambiguous but we try.
        for (int i = 2; i < n; ++i) {
            if (x[i] != A || y[i] != B) {
                int part1 = x[i] & ~A;
                int part2 = y[i] & ~B;
                p[i] = part1 | part2;
            }
        }
        // For i in need_query, query (c,i)
        for (int idx : need_query) {
            int Z_i = query(c, idx);
            // Try to deduce p_i from X, Y, Z_i, A, B.
            // We'll attempt to compute p_i bit by bit.
            int pi = 0;
            for (int bit = 0; bit < MAXB; ++bit) {
                int abit = (A >> bit) & 1;
                int bbit = (B >> bit) & 1;
                int zbit = (Z_i >> bit) & 1;
                int xbit = (x[idx] >> bit) & 1;
                int ybit = (y[idx] >> bit) & 1;
                int pbit = 0;
                if (abit == 0 && bbit == 0) {
                    pbit = xbit; // or ybit, same
                } else if (abit == 0 && bbit == 1) {
                    pbit = xbit;
                } else if (abit == 1 && bbit == 0) {
                    pbit = ybit;
                } else { // both 1
                    // Need zbit; if zbit == 0 then pbit=0 else pbit=1? But if C has 1, zbit=1 regardless.
                    // We cannot know C. This may fail.
                    // As fallback, we can try to use Xc and Yc to infer C? Not.
                    // We'll set pbit = 0 as guess, but may be wrong.
                    pbit = 0;
                }
                if (pbit) pi |= (1 << bit);
            }
            p[idx] = pi;
        }
    }

    // Output the permutation
    cout << "! ";
    for (int i = 0; i < n; ++i) {
        cout << p[i];
        if (i < n-1) cout << ' ';
    }
    cout << endl;
    cout.flush();

    return 0;
}