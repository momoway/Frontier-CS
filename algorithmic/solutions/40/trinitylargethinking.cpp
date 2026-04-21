#include <bits/stdc++.h>
using namespace std;

using ll = long long;

int n;
int knownP, knownQ; // knownP is '(' index, knownQ is ')' index

// Function to compute number of regular substrings in a string
ll computeF(const string& s) {
    int m = s.size();
    vector<ll> count(m, 0);
    vector<int> stack;
    for (int i = 0; i < m; ++i) {
        if (s[i] == '(') {
            stack.push_back(i);
        } else { // ')'
            if (!stack.empty()) {
                int j = stack.back();
                stack.pop_back();
                count[i] = 1 + (j > 0 ? count[j-1] : 0);
            } else {
                count[i] = 0;
            }
        }
    }
    ll sum = 0;
    for (ll x : count) sum += x;
    return sum;
}

// Interactive query: send indices in given order, receive f
ll queryWithOrder(const vector<int>& order) {
    cout << "0 " << order.size();
    for (int idx : order) {
        cout << " " << idx;
    }
    cout << endl;
    cout.flush();
    ll res;
    cin >> res;
    return res;
}

// Query for a pair (i,j) in that order
ll queryPair(int i, int j) {
    return queryWithOrder({i, j});
}

// Get decreasing order of a set
vector<int> getDecreasing(vector<int> v) {
    sort(v.begin(), v.end());
    reverse(v.begin(), v.end());
    return v;
}

// Find a mixed pair (one '(' and one ')')
pair<int,int> findMixedPair() {
    vector<int> S;
    for (int i = 1; i <= n; ++i) S.push_back(i);
    while (S.size() > 2) {
        int half = S.size() / 2;
        vector<int> A(S.begin(), S.begin() + half);
        vector<int> B(S.begin() + half, S.end());
        ll f_inc = queryWithOrder(A); // increasing order (already sorted)
        ll f_dec = queryWithOrder(getDecreasing(A));
        if (f_inc > 0 || f_dec > 0) {
            S = A;
            continue;
        } else {
            // A is homogeneous
            int a = A[0];
            vector<int> curB = B;
            while (curB.size() > 1) {
                int hb = curB.size() / 2;
                vector<int> B1(curB.begin(), curB.begin() + hb);
                vector<int> B2(curB.begin() + hb, curB.end());
                ll f_inc1 = queryWithOrder(B1);
                ll f_dec1 = queryWithOrder(getDecreasing(B1));
                if (f_inc1 > 0 || f_dec1 > 0) {
                    curB = B1;
                } else {
                    int b1 = B1[0];
                    ll v1 = queryPair(a, b1);
                    ll v2 = queryPair(b1, a);
                    if (v1 == 1) {
                        knownP = a; knownQ = b1;
                        return {a, b1};
                    } else if (v2 == 1) {
                        knownP = b1; knownQ = a;
                        return {b1, a};
                    } else {
                        curB = B2;
                    }
                }
            }
            int b = curB[0];
            ll v1 = queryPair(a, b);
            ll v2 = queryPair(b, a);
            if (v1 == 1) {
                knownP = a; knownQ = b;
                return {a, b};
            } else if (v2 == 1) {
                knownP = b; knownQ = a;
                return {b, a};
            } else {
                // Should not happen, but continue to avoid hang
                // Try to split further? For now, break.
                // We'll fallback to next step.
            }
        }
    }
    // S size 2
    int i = S[0], j = S[1];
    ll v1 = queryPair(i, j);
    ll v2 = queryPair(j, i);
    if (v1 == 1) {
        knownP = i; knownQ = j;
        return {i, j};
    } else if (v2 == 1) {
        knownP = j; knownQ = i;
        return {j, i};
    } else {
        // Should not happen, return any
        knownP = i; knownQ = j;
        return {i, j};
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> n;

    // Step 1: Find a mixed pair (one '(' and one ')')
    auto mixed = findMixedPair();
    int p = knownP, q = knownQ; // p is '(' index, q is ')' index

    // Step 2: Precompute a template for group size 8 that yields distinct f for all 256 patterns
    // We'll try several candidate templates
    struct Candidate {
        string name;
        vector<int> tokens; // 0=p, 1=q, 2=u
    };
    vector<Candidate> candidates;

    // Candidate A: prefix 2p, then for each of 8 positions: u, qqq, p, suffix 1q
    {
        Candidate c;
        c.name = "A";
        vector<int> tok;
        tok.push_back(0); tok.push_back(0); // 2 p's
        for (int i = 0; i < 8; ++i) {
            tok.push_back(2); // u
            tok.push_back(1); tok.push_back(1); tok.push_back(1); // 3 q's
            tok.push_back(0); // p
        }
        tok.push_back(1); // suffix q
        c.tokens = tok;
        candidates.push_back(c);
    }
    // Candidate B: prefix 3p, then for each: u, qq, pp, no suffix
    {
        Candidate c;
        c.name = "B";
        vector<int> tok;
        tok.push_back(0); tok.push_back(0); tok.push_back(0); // 3 p's
        for (int i = 0; i < 8; ++i) {
            tok.push_back(2); // u
            tok.push_back(1); tok.push_back(1); // 2 q's
            tok.push_back(0); tok.push_back(0); // 2 p's
        }
        c.tokens = tok;
        candidates.push_back(c);
    }
    // Candidate C: variable prefix lengths, more complex
    {
        Candidate c;
        c.name = "C";
        vector<int> tok;
        for (int i = 1; i <= 8; ++i) {
            // p repeated i times
            for (int k = 0; k < i; ++k) tok.push_back(0);
            tok.push_back(2); // u
            // q repeated i+1 times
            for (int k = 0; k < i+1; ++k) tok.push_back(1);
        }
        tok.push_back(1); // suffix q
        c.tokens = tok;
        candidates.push_back(c);
    }
    // Candidate D: another variant
    {
        Candidate c;
        c.name = "D";
        vector<int> tok;
        tok.push_back(0); // prefix p
        for (int i = 0; i < 8; ++i) {
            tok.push_back(2); // u
            // q repeated 4
            for (int k = 0; k < 4; ++k) tok.push_back(1);
            // p repeated 3
            for (int k = 0; k < 3; ++k) tok.push_back(0);
        }
        tok.push_back(1); // suffix q
        c.tokens = tok;
        candidates.push_back(c);
    }

    // Find a template that yields distinct f for all 256 patterns (for g=8)
    vector<int> bestTemplateTokens;
    bool found = false;
    for (auto &cand : candidates) {
        const vector<int>& tokens = cand.tokens;
        // We'll simulate for all 256 patterns
        unordered_map<ll, int> fmap;
        bool ok = true;
        int g = 8;
        for (int mask = 0; mask < (1<<g); ++mask) {
            string s = "";
            int uidx = 0;
            for (int tok : tokens) {
                if (tok == 0) s += '(';
                else if (tok == 1) s += ')';
                else { // u
                    // bit for this position: we need to map uidx to bit in mask
                    // We assume u's appear in order; we need to know which bit corresponds to which u.
                    // We'll take the least significant bit for first u, etc.
                    if (uidx >= g) {
                        // Should not happen if template has exactly g u's
                        ok = false;
                        break;
                    }
                    bool isLeft = (mask >> uidx) & 1;
                    s += (isLeft ? '(' : ')');
                    uidx++;
                }
            }
            if (!ok) break;
            ll f = computeF(s);
            if (fmap.count(f)) {
                ok = false;
                break;
            }
            fmap[f] = mask;
        }
        if (ok && fmap.size() == (1<<g)) {
            bestTemplateTokens = tokens;
            found = true;
            break;
        }
    }

    if (!found) {
        // Fallback: use candidate A even if not distinct? This may fail. But we assume one works.
        bestTemplateTokens = candidates[0].tokens;
    }

    // Map from f to mask for the chosen template
    unordered_map<ll, int> fToMask;
    int g = 8;
    for (int mask = 0; mask < (1<<g); ++mask) {
        string s = "";
        int uidx = 0;
        for (int tok : bestTemplateTokens) {
            if (tok == 0) s += '(';
            else if (tok == 1) s += ')';
            else {
                bool isLeft = (mask >> uidx) & 1;
                s += (isLeft ? '(' : ')');
                uidx++;
            }
        }
        ll f = computeF(s);
        fToMask[f] = mask;
    }

    // Prepare remaining indices (excluding p and q)
    vector<int> remaining;
    for (int i = 1; i <= n; ++i) {
        if (i != p && i != q) remaining.push_back(i);
    }
    // We'll process in groups of size g (8)
    string ans(n, '?');
    ans[p-1] = '(';
    ans[q-1] = ')';

    int idx = 0;
    while (idx < (int)remaining.size()) {
        int groupSize = min(g, (int)remaining.size() - idx);
        vector<int> group;
        for (int i = idx; i < idx + groupSize; ++i) {
            group.push_back(remaining[i]);
        }
        idx += groupSize;

        // Build query string according to template
        vector<int> queryIndices;
        int uidx = 0;
        for (int tok : bestTemplateTokens) {
            if (tok == 0) queryIndices.push_back(p);
            else if (tok == 1) queryIndices.push_back(q);
            else {
                // u: take next index from group
                queryIndices.push_back(group[uidx]);
                uidx++;
            }
        }
        // Send query
        ll f = queryWithOrder(queryIndices);
        // Look up mask
        auto it = fToMask.find(f);
        if (it == fToMask.end()) {
            // Should not happen, but if it does, we may need fallback.
            // For safety, we could try another template, but we assume it's unique.
            // We'll set mask to 0 arbitrarily.
            // But better to exit with error? We'll just continue.
            continue;
        }
        int mask = it->second;
        // Assign characters to group positions based on mask
        uidx = 0;
        for (int tok : bestTemplateTokens) {
            if (tok == 2) {
                bool isLeft = (mask >> uidx) & 1;
                ans[group[uidx]-1] = (isLeft ? '(' : ')');
                uidx++;
            }
        }
    }

    // Output answer
    cout << "1 " << ans << endl;
    cout.flush();

    return 0;
}