#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    if (!(cin >> n >> m)) return 0;

    vector<double> weight(n, 1.0);
    const double eta = 0.2;

    // Random number generator with a fixed seed
    mt19937_64 rng(123456789);
    uniform_real_distribution<double> dist(0.0, 1.0);

    for (int round = 0; round < m; ++round) {
        string s;
        cin >> s;

        double W0 = 0.0, W1 = 0.0;
        for (int i = 0; i < n; ++i) {
            if (s[i] == '0') W0 += weight[i];
            else W1 += weight[i];
        }

        double total = W0 + W1;
        int prediction;
        if (total == 0) {
            prediction = 0; // arbitrary when all weights are zero
        } else {
            double p0 = W0 / total;
            double r = dist(rng);
            prediction = (r < p0) ? 0 : 1;
        }

        cout << prediction << '\n' << flush;

        char outcome_char;
        cin >> outcome_char;
        int outcome = outcome_char - '0';

        for (int i = 0; i < n; ++i) {
            int pred = s[i] - '0';
            if (pred != outcome) {
                weight[i] *= (1.0 - eta);
                if (weight[i] < 1e-100) weight[i] = 0.0;
            }
        }
    }

    return 0;
}