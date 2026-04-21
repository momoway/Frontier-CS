#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;

    int l = 1, r = n;
    int questions = 0;
    const int MAX_QUEST = 53;
    const int MAX_GUESS = 2;

    auto ask = [&](const vector<int>& s) {
        cout << "? " << s.size();
        for (int x : s) cout << " " << x;
        cout << endl;
        cout.flush();
        string ans;
        cin >> ans;
        return ans;
    };

    auto guess = [&](int x) {
        cout << "! " << x << endl;
        cout.flush();
        string res;
        cin >> res;
        if (res == ":)") {
            exit(0);
        }
        return false;
    };

    while (l < r && questions < MAX_QUEST) {
        int mid = (l + r) / 2;
        vector<int> set1(mid);
        iota(set1.begin(), set1.end(), 1);
        string a1 = ask(set1);
        questions++;
        string a2 = ask(set1);
        questions++;

        if (a1 == a2) {
            if (a1 == "YES") {
                r = mid;
            } else {
                l = mid + 1;
            }
        } else {
            if (mid > l) {
                vector<int> set2(mid - 1);
                iota(set2.begin(), set2.end(), 1);
                string a3 = ask(set2);
                questions++;
                if (a3 == a1) {
                    r = mid - 1;
                } else if (a3 == a2) {
                    l = mid;
                } else {
                    l = mid + 1;
                }
            } else {
                // mid == l, cannot ask about mid-1; break to guess
                break;
            }
        }
    }

    // After loop, interval [l, r] should be small; use guesses
    if (l == r) {
        guess(l);
    } else {
        // Try guessing l and r
        guess(l);
        guess(r);
        // If both wrong, we have no more guesses; but we must output something
        // As a fallback, guess l again? Not allowed. Just exit.
    }

    return 0;
}