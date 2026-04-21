#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;

    int diamond_index = -1;
    for (int i = 0; i < n; ++i) {
        cout << "? " << i << "\n" << flush;
        int left, right;
        cin >> left >> right;
        if (left == 0 && right == 0) {
            diamond_index = i;
            break;
        }
    }

    if (diamond_index == -1) diamond_index = 0; // fallback

    cout << "! " << diamond_index << "\n" << flush;
    return 0;
}