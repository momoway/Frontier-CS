#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    if (!(cin >> N)) return 0;
    const int m = 1002;
    vector<int> s(N);

    for (int i = 0; i < N; ++i) {
        cout << "1\n";
        cout << m << "\n";
        // Output sequence a
        for (int x = 0; x < m; ++x) {
            int a_val, b_val;
            if (x < i) {
                a_val = x + 1;
                b_val = x + 1;
            } else if (x == i) {
                a_val = 1000;
                b_val = 1001;
            } else if (x == 1000) {
                a_val = 1000;
                b_val = 1000;
            } else if (x == 1001) {
                a_val = 1001;
                b_val = 1001;
            } else {
                a_val = 0;
                b_val = 0;
            }
            cout << a_val << (x == m-1 ? '\n' : ' ');
        }
        // Output sequence b
        for (int x = 0; x < m; ++x) {
            int a_val, b_val;
            if (x < i) {
                a_val = x + 1;
                b_val = x + 1;
            } else if (x == i) {
                a_val = 1000;
                b_val = 1001;
            } else if (x == 1000) {
                a_val = 1000;
                b_val = 1000;
            } else if (x == 1001) {
                a_val = 1001;
                b_val = 1001;
            } else {
                a_val = 0;
                b_val = 0;
            }
            cout << a_val << (x == m-1 ? '\n' : ' ');
        }
        cout.flush();

        int res;
        cin >> res;
        if (res == 1000) s[i] = 0;
        else if (res == 1001) s[i] = 1;
        else s[i] = 0; // fallback, should not happen
    }

    cout << "0 ";
    for (int i = 0; i < N; ++i) {
        cout << char('0' + s[i]);
    }
    cout << endl;
    return 0;
}