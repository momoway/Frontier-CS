#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int type;
    if (!(cin >> type)) return 0;
    vector<string> small = {
        "012301230123",
        "123012301230",
        "230123012301",
        "301230123012",
        "012301230123",
        "123012301230",
        "230123012301",
        "301230123012",
        "012301230123",
        "123012301230",
        "230123012301",
        "301230123012"
    };
    vector<string> large = {
        "111111111111",
        "222222222222",
        "333333333333",
        "111111111111",
        "222222222222",
        "333333333333",
        "111111111111",
        "222222222222",
        "333333333333",
        "111111111111",
        "222222222222",
        "333333333333"
    };
    const vector<string>& out = (type == 0) ? small : large;
    for (const string& row : out) {
        cout << row << "\n";
    }
    return 0;
}