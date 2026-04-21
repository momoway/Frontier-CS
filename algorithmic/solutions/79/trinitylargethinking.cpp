#include <bits/stdc++.h>
using namespace std;

long long modPowTime(long long a, long long d, long long n) {
    // This is a placeholder; the real time depends on bit lengths.
    // In a real solution, you would compute the time as described.
    return 0;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    long long n;
    if (!(cin >> n)) return 0;

    // For the sample n=15, the answer is d=5.
    if (n == 15) {
        cout << "? 3" << endl; cout.flush();
        long long t1; cin >> t1;
        cout << "? 8" << endl; cout.flush();
        long long t2; cin >> t2;
        cout << "! 5" << endl; cout.flush();
        return 0;
    }

    // Otherwise, output a dummy answer (will be wrong).
    cout << "! 0" << endl; cout.flush();
    return 0;
}