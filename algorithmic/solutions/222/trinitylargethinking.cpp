#include <bits/stdc++.h>
using namespace std;

const int M = 1000;
const int MAXV = 1000000;
const int MAX_EXP = M * M; // 1000000

vector<int> get_divisors(int d) {
    vector<int> divs;
    for (int i = 1; (long long)i * i <= d; ++i) {
        if (d % i == 0) {
            divs.push_back(i);
            if (i != d / i) divs.push_back(d / i);
        }
    }
    sort(divs.begin(), divs.end());
    return divs;
}

int find_s(int c0, int d, const vector<int>& known_exponent, int max_known) {
    vector<int> divs = get_divisors(d);
    for (int t : divs) {
        if (t == 0) continue;
        if (t <= max_known && known_exponent[t] != -1) {
            if (known_exponent[t] == c0) {
                return t;
            }
        } else {
            cout << "? " << c0 << " " << t << endl;
            int res;
            cin >> res;
            if (res == c0) {
                return t;
            }
        }
    }
    return -1; // Should not happen
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    cin >> n;
    for (int graph = 0; graph < n; ++graph) {
        cout << "? 1 1" << endl;
        int c0;
        cin >> c0;

        vector<int> known_exponent(MAX_EXP + 1, -1);
        vector<int> baby_map(MAXV + 1, -1);
        known_exponent[0] = c0;
        baby_map[c0] = 0;
        int cur = c0;
        bool solved = false;
        int s_ans = -1;
        int max_known = 0;

        // Baby steps
        for (int j = 1; j < M; ++j) {
            cout << "? " << cur << " " << 1 << endl;
            int nxt;
            cin >> nxt;
            if (nxt == c0) {
                s_ans = j;
                solved = true;
                break;
            }
            if (baby_map[nxt] != -1) {
                int i = baby_map[nxt];
                int d = j - i;
                s_ans = find_s(c0, d, known_exponent, max_known);
                solved = true;
                break;
            }
            baby_map[nxt] = j;
            known_exponent[j] = nxt;
            if (j > max_known) max_known = j;
            cur = nxt;
        }

        if (!solved) {
            // Giant steps
            for (int i = 1; i <= M; ++i) {
                long long step = 1LL * i * M;
                cout << "? " << c0 << " " << step << endl;
                int g;
                cin >> g;
                if (step <= MAX_EXP) {
                    known_exponent[step] = g;
                    if (step > max_known) max_known = (int)step;
                }
                if (g == c0) {
                    int d = i * M;
                    s_ans = find_s(c0, d, known_exponent, max_known);
                    solved = true;
                    break;
                }
                if (baby_map[g] != -1) {
                    int j = baby_map[g];
                    int d = i * M - j;
                    s_ans = find_s(c0, d, known_exponent, max_known);
                    solved = true;
                    break;
                }
            }
        }

        cout << "! " << s_ans << endl;
        int result;
        cin >> result;
        if (result == -1) {
            return 0;
        }
    }
    return 0;
}