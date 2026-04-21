#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, k;
    if (!(cin >> n >> k)) return 0;

    int B = min(k, 64); // block size, at most 64 to keep operations bounded

    // Partition into groups of size B
    vector<vector<int>> groups;
    for (int i = 1; i <= n; i += B) {
        int end = min(i + B - 1, n);
        vector<int> grp;
        for (int j = i; j <= end; ++j) grp.push_back(j);
        groups.push_back(grp);
    }

    vector<vector<int>> allReps;
    // Process each group
    for (const auto& grp : groups) {
        cout << "R" << endl;
        cout.flush();
        vector<int> reps;
        for (int idx : grp) {
            cout << "? " << idx << endl;
            cout.flush();
            char ans;
            cin >> ans;
            if (ans == 'N') {
                reps.push_back(idx);
            }
        }
        allReps.push_back(reps);
    }

    // Merge the representative sets
    vector<vector<int>> sets = allReps;
    while (sets.size() > 1) {
        auto A = sets.back(); sets.pop_back();
        auto Bvec = sets.back(); sets.pop_back();
        vector<int> C;
        if (A.empty() && Bvec.empty()) {
            // C remains empty
        } else if (A.empty()) {
            C = Bvec;
        } else if (Bvec.empty()) {
            C = A;
        } else {
            // Determine smaller and larger
            const vector<int> *smaller = &A, *larger = &Bvec;
            if (A.size() > Bvec.size()) {
                smaller = &Bvec;
                larger = &A;
            }
            int szS = smaller->size();
            // Reset before loading
            cout << "R" << endl;
            cout.flush();
            // Load smaller into memory
            for (int s : *smaller) {
                cout << "? " << s << endl;
                cout.flush();
                char ans; cin >> ans; // ignore
            }
            if (szS == k) {
                // Cannot batch, test each element of larger individually
                for (int t : *larger) {
                    // Reset and reload smaller for each test
                    cout << "R" << endl;
                    cout.flush();
                    for (int s : *smaller) {
                        cout << "? " << s << endl;
                        cout.flush();
                        char ans; cin >> ans; // ignore
                    }
                    cout << "? " << t << endl;
                    cout.flush();
                    char ans; cin >> ans;
                    if (ans == 'N') {
                        C.push_back(t);
                    }
                }
            } else {
                int g = k - szS; // batch size
                for (size_t i = 0; i < larger->size(); i += g) {
                    size_t chunk_end = min(i + g, larger->size());
                    // Reset, load smaller, then query this batch
                    cout << "R" << endl;
                    cout.flush();
                    for (int s : *smaller) {
                        cout << "? " << s << endl;
                        cout.flush();
                        char ans; cin >> ans; // ignore
                    }
                    for (size_t j = i; j < chunk_end; ++j) {
                        int t = (*larger)[j];
                        cout << "? " << t << endl;
                        cout.flush();
                        char ans; cin >> ans;
                        if (ans == 'N') {
                            C.push_back(t);
                        }
                    }
                }
            }
            // Add all elements of smaller (they are distinct)
            C.insert(C.end(), smaller->begin(), smaller->end());
        }
        sets.push_back(C);
    }

    int d = sets.empty() ? 0 : (int)sets[0].size();
    cout << "! " << d << endl;
    cout.flush();
    return 0;
}