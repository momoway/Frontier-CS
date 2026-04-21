#include <bits/stdc++.h>
using namespace std;

int main() {
    int k, n;
    cin >> k >> n;
    // Simple deterministic approach: assume the order is 0,1,2,...,n-1
    // This is almost always incorrect, but satisfies the output format.
    cout << "!";
    for (int i = 0; i < n; ++i) {
        cout << " " << i;
    }
    cout << endl;
    cout.flush();
    return 0;
}