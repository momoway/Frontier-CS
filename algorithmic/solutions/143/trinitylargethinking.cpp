#include <iostream>
#include <vector>
#include <string>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const int MAX_BUDGET = 3000000;
    int total_samples = 0;

    int G;
    cin >> G;
    if (G == -1) return 0;

    for (int hand = 1; hand <= G; ++hand) {
        while (true) {
            int h, r, a, b, P, k;
            cin >> h >> r >> a >> b >> P >> k;
            if (h == -1 || r == -1 || a == -1 || b == -1 || P == -1 || k == -1) return 0;

            int c1, v1, c2, v2;
            cin >> c1 >> v1 >> c2 >> v2;
            if (c1 == -1 || v1 == -1 || c2 == -1 || v2 == -1) return 0;

            string board_word;
            cin >> board_word;
            if (board_word == "-1") return 0;

            vector<pair<int,int>> board_cards;
            for (int i = 0; i < k; ++i) {
                int cs, vs;
                cin >> cs >> vs;
                if (cs == -1 || vs == -1) return 0;
                board_cards.push_back({cs, vs});
            }

            bool raise = false;
            if (total_samples < MAX_BUDGET) {
                int t0 = 30;
                int remaining = MAX_BUDGET - total_samples;
                int t_use = min(t0, remaining);
                cout << "RATE " << t_use << endl;
                double w, d;
                cin >> w >> d;
                if (w == -1.0 || d == -1.0) return 0;
                total_samples += t_use;
                double equity = 2.0 * w + d;

                if (equity > 1.02) {
                    raise = (a > 0);
                } else if (equity < 0.98) {
                    raise = false;
                } else {
                    if (remaining - t_use <= 0) {
                        if (equity >= 1.0 && a > 0) raise = true; else raise = false;
                    } else {
                        int t_add = min(100, remaining - t_use);
                        cout << "RATE " << t_add << endl;
                        double w2, d2;
                        cin >> w2 >> d2;
                        if (w2 == -1.0 || d2 == -1.0) return 0;
                        total_samples += t_add;
                        double w_comb = (t_use * w + t_add * w2) / (t_use + t_add);
                        double d_comb = (t_use * d + t_add * d2) / (t_use + t_add);
                        double equity_comb = 2.0 * w_comb + d_comb;
                        if (equity_comb >= 1.0 && a > 0) raise = true; else raise = false;
                    }
                }
            } else {
                raise = false;
            }

            if (raise) {
                cout << "RAISE " << a << endl;
            } else {
                cout << "CHECK" << endl;
            }

            string response;
            cin >> response;
            if (response == "-1") return 0;

            if (response == "OPP") {
                string sub;
                cin >> sub;
                if (sub == "-1") return 0;
                if (sub == "CHECK") {
                    if (r == 4) {
                        int delta;
                        cin >> delta;
                        if (delta == -1) return 0;
                        break;
                    }
                } else if (sub == "FOLD") {
                    int delta;
                    cin >> delta;
                    if (delta == -1) return 0;
                    break;
                } else if (sub == "CALL") {
                    int call_amount;
                    cin >> call_amount;
                    if (call_amount == -1) return 0;
                    if (r == 4) {
                        int delta;
                        cin >> delta;
                        if (delta == -1) return 0;
                        break;
                    }
                }
            } else if (response == "RESULT") {
                int delta;
                cin >> delta;
                if (delta == -1) return 0;
                break;
            }
        }
    }

    string score_word;
    cin >> score_word;
    if (score_word == "-1") return 0;
    double W;
    cin >> W;
    if (W == -1.0) return 0;

    return 0;
}