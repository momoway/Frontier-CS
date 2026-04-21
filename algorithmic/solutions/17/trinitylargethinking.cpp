#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    if (!(cin >> T)) return 0;
    while (T--) {
        int n;
        cin >> n;
        int L = 1, R = n;
        while (L < R) {
            cout << "? " << L << " " << R << endl;
            cout.flush();
            int s;
            cin >> s;
            if (s == L) {
                L = L + 1;
            } else if (s == R) {
                R = R - 1;
            } else {
                int left_len = s - L;
                int right_len = R - s;
                if (left_len <= right_len) {
                    cout << "? " << L << " " << s << endl;
                    cout.flush();
                    int ans;
                    cin >> ans;
                    if (ans == s) {
                        R = s - 1;
                    } else {
                        L = s + 1;
                    }
                } else {
                    cout << "? " << s << " " << R << endl;
                    cout.flush();
                    int ans;
                    cin >> ans;
                    if (ans == s) {
                        L = s + 1;
                    } else {
                        R = s - 1;
                    }
                }
            }
        }
        cout << "! " << L << endl;
        cout.flush();
    }
    return 0;
}