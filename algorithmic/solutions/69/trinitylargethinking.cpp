#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    if (!(cin >> n)) return 0;
    vector<string> words(n+1);
    for (int i = 1; i <= n; ++i) {
        string s;
        s.append(i, 'X');
        s.push_back('O');
        s.append(n, 'X');
        words[i] = s;
    }
    for (int i = 1; i <= n; ++i) {
        cout << words[i] << "\n";
    }
    cout.flush();

    long long C = 1LL * n * n + 4LL * n + 2;
    unordered_map<long long, vector<pair<int,int>>> mp;
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= n; ++j) {
            long long D = 1LL * i * j + 1LL * n * (i + j) + i + 3LL * j + C;
            mp[D].push_back({i, j});
        }
    }

    int q;
    cin >> q;
    while (q--) {
        long long p;
        cin >> p;
        if (mp.find(p) != mp.end()) {
            auto pr = mp[p][0];
            cout << pr.first << " " << pr.second << "\n";
        } else {
            cout << "1 1\n";
        }
        cout.flush();
    }
    return 0;
}