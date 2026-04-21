#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int R, H;
    cin >> R >> H;

    const int TARGET_POS = 1000;
    const int MAX_T = 64; // we stay within 64-bit integers
    int t = 45; // starting number of robots
    const int BASE_SEEDS = 5;
    bool found = false;
    vector<uint64_t> vecs;
    unordered_map<uint64_t, pair<int,int>> mp;
    mt19937_64 rng;

    for (; t <= MAX_T; ++t) {
        int seeds = BASE_SEEDS;
        if (t == MAX_T) seeds = 100; // more attempts for the last t
        bool ok_for_t = false;
        for (int seed = 1; seed <= seeds && !ok_for_t; ++seed) {
            rng.seed(seed);
            vecs.clear();
            unordered_set<uint64_t> seen;
            seen.reserve(TARGET_POS * 2);
            uint64_t mask = (t >= 64) ? ~0ULL : ((1ULL << t) - 1);
            while (vecs.size() < TARGET_POS) {
                uint64_t v = rng() & mask;
                if (seen.insert(v).second) {
                    vecs.push_back(v);
                }
            }
            mp.clear();
            mp.reserve(TARGET_POS * (TARGET_POS + 1) / 2);
            bool ok = true;
            for (int i = 0; i < TARGET_POS && ok; ++i) {
                for (int j = i; j < TARGET_POS; ++j) {
                    uint64_t or_val = vecs[i] | vecs[j];
                    if (mp.find(or_val) != mp.end()) {
                        ok = false;
                        break;
                    }
                    mp[or_val] = {i, j};
                }
            }
            if (ok) {
                ok_for_t = true;
                found = true;
                break;
            }
        }
        if (found) break;
    }

    if (!found) {
        // Fallback: should never happen, but output something
        cout << "! 1 1" << endl;
        return 0;
    }

    // Send queries: for each bit position b (0..t-1)
    for (int b = 0; b < t; ++b) {
        vector<int> positions;
        for (int i = 0; i < TARGET_POS; ++i) {
            if ((vecs[i] >> b) & 1ULL) {
                positions.push_back(i + 1);
            }
        }
        cout << "? " << positions.size();
        for (int p : positions) {
            cout << " " << p;
        }
        cout << endl;
        cout.flush();
    }

    // Get results
    cout << "@" << endl;
    cout.flush();

    int L;
    if (!(cin >> L)) return 0;
    vector<int> results(t);
    for (int i = 0; i < L; ++i) {
        cin >> results[i];
    }
    uint64_t y = 0;
    for (int b = 0; b < t; ++b) {
        if (results[b] == 1) {
            y |= (1ULL << b);
        }
    }

    auto it = mp.find(y);
    if (it != mp.end()) {
        int a = it->second.first + 1;
        int b = it->second.second + 1;
        cout << "! " << a << " " << b << endl;
    } else {
        // Should not happen, but output a fallback
        cout << "! 1 1" << endl;
    }
    cout.flush();

    return 0;
}