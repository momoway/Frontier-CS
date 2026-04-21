#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    long long n;
    if (!(cin >> n)) return 0;

    long long la = 1, ra = n; // search range for a
    long long lb = 1, rb = n; // search range for b

    // First, find a using binary search with y = 1
    while (la < ra) {
        long long ma = (la + ra) / 2;
        cout << ma << " " << 1 << endl;
        cout.flush();
        int res;
        cin >> res;
        if (res == 0) {
            // Found both numbers
            return 0;
        } else if (res == 1) {
            // x < a  => a > ma
            la = ma + 1;
        } else {
            // res == 2 or 3 => a <= ma
            ra = ma;
        }
    }
    long long a = la; // a is now known

    // Now find b using binary search with x = a (since we know a)
    // We can use x = a to get unambiguous responses for b.
    // For (a, y):
    //   if y < b -> response 2 (only true? Actually check: if y < b, then 2 true.
    //        Also 1 false (a not < a), 3 false (a > a false, y > b false) -> only 2 true -> response 2.
    //   if y == b -> response 0.
    //   if y > b -> response 3 (since 2 false, 1 false, 3 true because y > b).
    // So we can binary search using these.
    while (lb < rb) {
        long long mb = (lb + rb) / 2;
        cout << a << " " << mb << endl;
        cout.flush();
        int res;
        cin >> res;
        if (res == 0) {
            return 0;
        } else if (res == 2) {
            // y < b => b > mb
            lb = mb + 1;
        } else {
            // res == 3 => y > b => b < mb
            rb = mb;
        }
    }
    // At this point, b = lb, but we should verify with a final query?
    // Actually the loop ends when lb == rb, which should be b.
    // We can output the answer to be safe, but the interactor will respond 0 if correct.
    // However, we must still query to confirm. The loop condition ensures we have narrowed to one value.
    // We can query (a, lb) to confirm and get response 0.
    cout << a << " " << lb << endl;
    cout.flush();
    int res;
    cin >> res;
    if (res == 0) return 0;
    // If not zero, something went wrong, but we can exit.
    return 0;
}