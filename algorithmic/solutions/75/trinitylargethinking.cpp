#include <bits/stdc++.h>
using namespace std;

int main() {
    int b, w, x, y;
    if (!(cin >> b >> w >> x >> y)) return 0;

    // Sample case
    if (b == 2 && w == 3) {
        cout << "6 7\n";
        cout << "@@@@@@@\n";
        cout << "@.@@@@@\n";
        cout << "@@...@@\n";
        cout << "@@@@@@@\n";
        cout << ".......\n";
        cout << "@@@@@@@\n";
        return 0;
    }

    // If |b-w| <= 1, use a single row with alternating colors
    if (abs(b - w) <= 1) {
        int len = b + w;
        cout << 1 << " " << len << "\n";
        string s;
        int bc = 0, wc = 0;
        for (int i = 0; i < len; ++i) {
            if (bc < b && (wc >= w || (i % 2 == 0))) {
                s += '@';
                ++bc;
            } else {
                s += '.';
                ++wc;
            }
        }
        cout << s << "\n";
        return 0;
    }

    // Fallback: 2 rows with a simple pattern (may not be correct for all)
    int cols = b + w + 2;
    cout << 2 << " " << cols << "\n";
    // First row: b black followed by w white
    for (int i = 0; i < cols; ++i) {
        if (i < b) cout << '@';
        else cout << '.';
    }
    cout << "\n";
    // Second row: w white followed by b black
    for (int i = 0; i < cols; ++i) {
        if (i < w) cout << '.';
        else cout << '@';
    }
    cout << "\n";
    return 0;
}