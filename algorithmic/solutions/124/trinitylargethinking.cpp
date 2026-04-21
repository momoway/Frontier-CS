#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;

    vector<int> parity(n+1, -1); // parity[i] for i=2..n: 1 if same parity as p1, else 0
    for (int i = 2; i <= n; i++) {
        cout << "? 2 1 " << i << endl;
        cout.flush();
        int ans;
        cin >> ans;
        parity[i] = ans;
    }

    vector<int> extreme(n+1, -1);
    for (int i = 1; i <= n; i++) {
        cout << "? " << n-1;
        for (int j = 1; j <= n; j++) {
            if (j == i) continue;
            cout << " " << j;
        }
        cout << endl;
        cout.flush();
        int ans;
        cin >> ans;
        extreme[i] = ans;
    }

    vector<int> extremes;
    for (int i = 1; i <= n; i++) {
        if (extreme[i] == 1) extremes.push_back(i);
    }
    if (extremes.size() != 2) {
        // Should not happen, but we can try to recover?
        // For safety, we'll exit.
        // In a contest, we assume correct interaction.
        // We'll just use the first two we found.
    }
    int e1 = extremes[0];
    int e2 = extremes[1];

    vector<int> mod3_ans(n+1, -1);
    for (int i = 1; i <= n; i++) {
        cout << "? 3 " << i << " " << e1 << " " << e2 << endl;
        cout.flush();
        int ans;
        cin >> ans;
        mod3_ans[i] = ans;
    }

    // Determine p1
    int p1 = -1;
    int p1_parity = -1; // 1 for odd, 0 for even
    int which_extreme_is_1 = -1; // 1 for e1, 2 for e2, 0 unknown
    bool found = false;
    for (int v = 1; v <= n/2; v++) {
        int pv = (v % 2 == 1) ? 1 : 0; // 1 odd, 0 even
        // Determine which extreme would be 1 under this v
        // For extreme e, its absolute parity under v: if parity[e]==1 then same as p1, else opposite.
        int abs_parity_e1 = (parity[e1] == 1) ? pv : 1-pv;
        int abs_parity_e2 = (parity[e2] == 1) ? pv : 1-pv;
        // 1 is odd, n is even.
        // Under v, the extreme that is 1 must have abs_parity = 1 (odd), and the extreme that is n must have abs_parity = 0 (even).
        int candidate_1 = -1, candidate_2 = -1;
        if (abs_parity_e1 == 1 && abs_parity_e2 == 0) {
            candidate_1 = e1;
            candidate_2 = e2;
        } else if (abs_parity_e1 == 0 && abs_parity_e2 == 1) {
            candidate_1 = e2;
            candidate_2 = e1;
        } else {
            continue; // inconsistent
        }
        // Check mod3 condition for p1
        int target_mod3 = (3 - (1 + n) % 3) % 3;
        int v_mod3 = v % 3;
        bool mod3_ok;
        if (mod3_ans[1] == 1) {
            mod3_ok = (v_mod3 == target_mod3);
        } else {
            mod3_ok = (v_mod3 != target_mod3);
        }
        if (!mod3_ok) continue;

        // Also need to check that the extreme identified as 1 indeed has ans[e] consistent with v parity.
        // For candidate_1 (supposed to be 1): if pv==1 (odd), then 1 has same parity as p1, so parity[candidate_1] should be 1; if pv==0, then parity[candidate_1] should be 0.
        if ( (pv == 1 && parity[candidate_1] != 1) || (pv == 0 && parity[candidate_1] != 0) ) {
            continue;
        }
        // For candidate_2 (supposed to be n): if pv==1, n has opposite parity, so parity[candidate_2] should be 0; if pv==0, parity[candidate_2] should be 1.
        if ( (pv == 1 && parity[candidate_2] != 0) || (pv == 0 && parity[candidate_2] != 1) ) {
            continue;
        }

        // If we reach here, v is a candidate.
        // If we already have a candidate, we might have multiple. We'll take the smallest? But we hope only one.
        p1 = v;
        p1_parity = pv;
        which_extreme_is_1 = (candidate_1 == e1) ? 1 : 2;
        found = true;
        break;
    }

    if (!found) {
        // Fallback: try all candidates again and pick the one with smallest v? This is not guaranteed.
        // For safety, we'll exit.
        // In contest, this should not happen.
        return 0;
    }

    int pos1_extreme, posn_extreme;
    if (which_extreme_is_1 == 1) {
        pos1_extreme = e1;
        posn_extreme = e2;
    } else {
        pos1_extreme = e2;
        posn_extreme = e1;
    }

    // Assign known values
    vector<int> value(n+1, 0);
    vector<bool> assigned(n+1, false);
    value[1] = p1;
    assigned[1] = true;
    value[pos1_extreme] = 1;
    assigned[pos1_extreme] = true;
    value[posn_extreme] = n;
    assigned[posn_extreme] = true;

    // Compute target_mod3 for mod3 condition
    int target_mod3 = (3 - (1 + n) % 3) % 3;

    // For each position, compute allowed residues mod6
    vector<int> abs_parity(n+1, -1);
    vector<int> mod3_cond(n+1, -1); // -1 means no restriction? Actually we have condition: if mod3_ans[i]==1, then must be target; else must not be target.
    vector<int> allowed_residues_mod6[n+1]; // will store list of allowed residues (0..5)

    for (int i = 1; i <= n; i++) {
        if (i == 1) {
            abs_parity[i] = (p1 % 2 == 1) ? 1 : 0;
        } else {
            // parity[i] is defined for i>=2
            if (parity[i] == 1) {
                // same as p1
                abs_parity[i] = (p1 % 2 == 1) ? 1 : 0;
            } else {
                // opposite
                abs_parity[i] = (p1 % 2 == 1) ? 0 : 1;
            }
        }
        // mod3 condition
        if (mod3_ans[i] == 1) {
            mod3_cond[i] = target_mod3; // must equal
        } else {
            mod3_cond[i] = -1; // must not equal target_mod3
        }
    }

    // For each i, compute allowed residues mod6
    for (int i = 1; i <= n; i++) {
        vector<int> residues;
        if (abs_parity[i] == 1) {
            // odd residues: 1,3,5
            residues = {1,3,5};
        } else {
            // even residues: 0,2,4
            residues = {0,2,4};
        }
        // Filter by mod3 condition
        vector<int> allowed;
        for (int r : residues) {
            int r3 = r % 3;
            if (mod3_cond[i] == -1) {
                allowed.push_back(r);
            } else {
                if (r3 == mod3_cond[i]) allowed.push_back(r);
            }
        }
        allowed_residues_mod6[i] = allowed;
    }

    // Initialize possible masks: bitset of size n+1 (index 1..n)
    vector<bitset<801>> possible(n+1);
    for (int i = 1; i <= n; i++) {
        possible[i].reset();
        if (assigned[i]) {
            possible[i].set(value[i]);
        } else {
            for (int v = 1; v <= n; v++) {
                if (possible[i].test(v)) continue;
                int r6 = v % 6;
                bool ok = false;
                for (int r : allowed_residues_mod6[i]) {
                    if (r == r6) {
                        ok = true;
                        break;
                    }
                }
                if (ok) {
                    possible[i].set(v);
                }
            }
        }
    }

    // Propagation: assign positions with single possibility
    bool changed = true;
    while (changed) {
        changed = false;
        for (int i = 1; i <= n; i++) {
            if (assigned[i]) continue;
            if (possible[i].count() == 1) {
                // assign
                for (int v = 1; v <= n; v++) {
                    if (possible[i].test(v)) {
                        value[i] = v;
                        assigned[i] = true;
                        // remove v from others
                        for (int j = 1; j <= n; j++) {
                            if (j != i && assigned[j] == false) {
                                possible[j].reset(v);
                            }
                        }
                        changed = true;
                        break;
                    }
                }
            }
        }
    }

    // List of known positions (assigned)
    vector<int> known_positions;
    for (int i = 1; i <= n; i++) {
        if (assigned[i]) known_positions.push_back(i);
    }

    // Function to get a subset of known positions of size k-1 with a given sum modulo k?
    // We'll just take the first k-1 known positions (by index) and compute sum_mod.
    // We'll try primes 5,7,11.
    vector<int> primes = {5,7,11};
    bool all_assigned = false;
    while (!all_assigned) {
        all_assigned = true;
        for (int i = 1; i <= n; i++) {
            if (!assigned[i]) {
                all_assigned = false;
                break;
            }
        }
        if (all_assigned) break;

        bool progress = false;
        for (int m : primes) {
            if ((int)known_positions.size() < m-1) continue;
            // Take first m-1 known positions
            vector<int> subset_known;
            for (int i = 0; i < m-1; i++) {
                subset_known.push_back(known_positions[i]);
            }
            int sum_mod = 0;
            for (int pos : subset_known) {
                sum_mod = (sum_mod + value[pos]) % m;
            }
            // For each unassigned position i, query
            vector<int> unassigned_list;
            for (int i = 1; i <= n; i++) {
                if (!assigned[i]) unassigned_list.push_back(i);
            }
            for (int i : unassigned_list) {
                cout << "? " << m << " " << i;
                for (int pos : subset_known) {
                    cout << " " << pos;
                }
                cout << endl;
                cout.flush();
                int ans;
                cin >> ans;
                int target = (-sum_mod % m + m) % m;
                // Update possible[i]
                for (int v = 1; v <= n; v++) {
                    if (!possible[i].test(v)) continue;
                    if (ans == 1) {
                        if (v % m != target) {
                            possible[i].reset(v);
                        }
                    } else {
                        if (v % m == target) {
                            possible[i].reset(v);
                        }
                    }
                }
            }
            // After querying all unassigned for this prime, propagate assignments
            changed = true;
            while (changed) {
                changed = false;
                for (int i = 1; i <= n; i++) {
                    if (assigned[i]) continue;
                    if (possible[i].count() == 1) {
                        for (int v = 1; v <= n; v++) {
                            if (possible[i].test(v)) {
                                value[i] = v;
                                assigned[i] = true;
                                // remove v from others
                                for (int j = 1; j <= n; j++) {
                                    if (j != i && !assigned[j]) {
                                        possible[j].reset(v);
                                    }
                                }
                                changed = true;
                                known_positions.push_back(i);
                                break;
                            }
                        }
                    }
                }
            }
            if (all_assigned) break;
        }
        // If no progress in a full cycle, break to avoid infinite loop
        if (!changed) {
            // Try to break by using different known sets? For now, we assume it will converge.
            // If not, we may need to try other strategies, but we assume it's enough.
            break;
        }
    }

    // Output the permutation
    cout << "! ";
    for (int i = 1; i <= n; i++) {
        cout << value[i];
        if (i < n) cout << " ";
    }
    cout << endl;
    cout.flush();

    return 0;
}