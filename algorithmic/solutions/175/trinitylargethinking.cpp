#include <bits/stdc++.h>
using namespace std;

struct ClauseInfo {
    bool conflict;
    int var_count;
    struct Var {
        int id;
        int required;
    } vars[3];
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    if (!(cin >> n >> m)) return 0;
    if (m == 0) {
        for (int i = 1; i <= n; ++i) {
            if (i > 1) cout << ' ';
            cout << 0;
        }
        cout << '\n';
        return 0;
    }

    vector<ClauseInfo> clause_info(m);
    vector<vector<int>> var_clauses(n + 1);

    const double pow_half[] = {1.0, 0.5, 0.25, 0.125};

    for (int i = 0; i < m; ++i) {
        int a, b, c;
        cin >> a >> b >> c;
        struct TempVar {
            int id;
            bool has_pos;
            bool has_neg;
        };
        TempVar temps[3];
        int temp_count = 0;

        auto process_lit = [&](int lit) {
            int v = abs(lit);
            bool pos = lit > 0;
            int idx = -1;
            for (int j = 0; j < temp_count; ++j) {
                if (temps[j].id == v) { idx = j; break; }
            }
            if (idx == -1) {
                idx = temp_count++;
                temps[idx].id = v;
                temps[idx].has_pos = false;
                temps[idx].has_neg = false;
            }
            if (pos) temps[idx].has_pos = true;
            else temps[idx].has_neg = true;
        };

        process_lit(a);
        process_lit(b);
        process_lit(c);

        ClauseInfo ci;
        ci.conflict = false;
        ci.var_count = 0;
        for (int j = 0; j < temp_count; ++j) {
            if (temps[j].has_pos && temps[j].has_neg) {
                ci.conflict = true;
            } else {
                int required = temps[j].has_pos ? 0 : 1;
                ci.vars[ci.var_count].id = temps[j].id;
                ci.vars[ci.var_count].required = required;
                ci.var_count++;
            }
        }
        clause_info[i] = ci;

        for (int j = 0; j < temp_count; ++j) {
            int v = temps[j].id;
            var_clauses[v].push_back(i);
        }
    }

    vector<int> assign(n + 1, -1);

    for (int x = 1; x <= n; ++x) {
        double delta0 = 0.0, delta1 = 0.0;
        for (int cid : var_clauses[x]) {
            const ClauseInfo& ci = clause_info[cid];
            double old_exp, new_exp0, new_exp1;
            if (ci.conflict) {
                old_exp = new_exp0 = new_exp1 = 1.0;
            } else {
                int unassigned = 0;
                bool mismatch = false;
                for (int j = 0; j < ci.var_count; ++j) {
                    int v = ci.vars[j].id;
                    int req = ci.vars[j].required;
                    if (assign[v] != -1) {
                        if (assign[v] != req) {
                            mismatch = true;
                            break;
                        }
                    } else {
                        unassigned++;
                    }
                }
                if (mismatch) {
                    old_exp = new_exp0 = new_exp1 = 1.0;
                } else {
                    old_exp = 1.0 - pow_half[unassigned];
                    int required_x = -1;
                    for (int j = 0; j < ci.var_count; ++j) {
                        if (ci.vars[j].id == x) {
                            required_x = ci.vars[j].required;
                            break;
                        }
                    }
                    if (0 == required_x) {
                        new_exp0 = 1.0 - pow_half[unassigned - 1];
                    } else {
                        new_exp0 = 1.0;
                    }
                    if (1 == required_x) {
                        new_exp1 = 1.0 - pow_half[unassigned - 1];
                    } else {
                        new_exp1 = 1.0;
                    }
                }
            }
            delta0 += new_exp0 - old_exp;
            delta1 += new_exp1 - old_exp;
        }
        assign[x] = (delta0 >= delta1) ? 0 : 1;
    }

    for (int i = 1; i <= n; ++i) {
        if (i > 1) cout << ' ';
        cout << assign[i];
    }
    cout << '\n';
    return 0;
}