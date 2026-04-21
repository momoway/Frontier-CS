#include <bits/stdc++.h>
using namespace std;

struct Result {
    int size;
    string expr;
    Result(int s = 0, const string& e = "") : size(s), expr(e) {}
};

struct VectorHash {
    size_t operator()(const vector<uint32_t>& v) const {
        size_t seed = v.size();
        for (uint32_t i : v) {
            seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

Result buildAND(uint32_t mask, int n) {
    if (mask == 0) return Result(0, "T"); // constant true (should not happen for non-zero mask)
    vector<int> vars;
    for (int i = 0; i < n; ++i) {
        if (mask & (1u << i)) vars.push_back(i);
    }
    if (vars.empty()) return Result(0, "T");
    if (vars.size() == 1) {
        char c = 'a' + vars[0];
        return Result(0, string(1, c));
    }
    string cur(1, 'a' + vars[0]);
    int sz = 0;
    for (size_t i = 1; i < vars.size(); ++i) {
        cur = "(" + cur + "&" + char('a' + vars[i]) + ")";
        ++sz;
    }
    return Result(sz, cur);
}

Result buildOR(vector<uint32_t> terms, int n, unordered_map<vector<uint32_t>, Result, VectorHash>& memo) {
    if (terms.empty()) return Result(0, "F");
    // Remove duplicates and sort for memo key
    sort(terms.begin(), terms.end());
    terms.erase(unique(terms.begin(), terms.end()), terms.end());
    auto it = memo.find(terms);
    if (it != memo.end()) return it->second;

    // Check for constant true: any term that is subset of fixed1? Actually, if any term is 0 (empty) -> T
    for (uint32_t mask : terms) {
        if (mask == 0) {
            memo[terms] = Result(0, "T");
            return memo[terms];
        }
    }

    if (terms.size() == 1) {
        Result res = buildAND(terms[0], n);
        memo[terms] = res;
        return res;
    }

    // Choose variable that appears in most terms
    vector<int> cnt(n, 0);
    for (uint32_t mask : terms) {
        for (int i = 0; i < n; ++i) {
            if (mask & (1u << i)) cnt[i]++;
        }
    }
    int bestVar = 0, maxCnt = -1;
    for (int i = 0; i < n; ++i) {
        if (cnt[i] > maxCnt) {
            maxCnt = cnt[i];
            bestVar = i;
        }
    }

    vector<uint32_t> S, R;
    for (uint32_t mask : terms) {
        if (mask & (1u << bestVar)) S.push_back(mask);
        else R.push_back(mask);
    }

    // Process S: remove bestVar from each term
    bool hasTrueRem = false;
    vector<uint32_t> remTerms;
    for (uint32_t mask : S) {
        uint32_t r = mask & ~(1u << bestVar);
        if (r == 0) hasTrueRem = true;
        else remTerms.push_back(r);
    }

    Result remResult;
    if (hasTrueRem) {
        remResult = Result(0, "T");
    } else {
        remResult = buildOR(remTerms, n, memo);
    }

    Result leftResult;
    if (remResult.expr == "T") {
        leftResult = Result(0, string(1, 'a' + bestVar));
    } else if (remResult.expr == "F") {
        leftResult = Result(0, "F");
    } else {
        string expr = "(" + string(1, 'a' + bestVar) + "&" + remResult.expr + ")";
        leftResult = Result(remResult.size + 1, expr);
    }

    Result rightResult = buildOR(R, n, memo);

    // Simplify the combination
    if (leftResult.expr == "F") {
        memo[terms] = rightResult;
        return rightResult;
    }
    if (rightResult.expr == "F") {
        memo[terms] = leftResult;
        return leftResult;
    }
    if (leftResult.expr == "T" || rightResult.expr == "T") {
        memo[terms] = Result(0, "T");
        return memo[terms];
    }
    if (leftResult.expr == rightResult.expr) {
        memo[terms] = leftResult;
        return leftResult;
    }
    string expr = "(" + leftResult.expr + "|" + rightResult.expr + ")";
    Result res(leftResult.size + rightResult.size + 1, expr);
    memo[terms] = res;
    return res;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T;
    if (!(cin >> T)) return 0;
    while (T--) {
        int n;
        string s;
        cin >> n;
        cin >> s;
        int N = 1 << n;
        vector<char> f(N);
        for (int i = 0; i < N; ++i) f[i] = s[i];

        // Check constant functions
        bool allZero = true, allOne = true;
        for (int i = 0; i < N; ++i) {
            if (f[i] == '1') allZero = false;
            else allOne = false;
        }
        if (allZero) {
            cout << "Yes\nF\n";
            continue;
        }
        if (allOne) {
            cout << "Yes\nT\n";
            continue;
        }

        // Monotonicity check
        bool monotone = true;
        for (int i = 0; i < N && monotone; ++i) {
            for (int j = 0; j < n; ++j) {
                if (!(i & (1 << j))) {
                    int i2 = i | (1 << j);
                    if (f[i] == '1' && f[i2] == '0') {
                        monotone = false;
                        break;
                    }
                }
            }
        }
        if (!monotone) {
            cout << "No\n";
            continue;
        }

        // Collect true assignments
        vector<int> trueAssignments;
        for (int i = 0; i < N; ++i) {
            if (f[i] == '1') trueAssignments.push_back(i);
        }

        // Find minimal true vectors (prime implicants)
        sort(trueAssignments.begin(), trueAssignments.end(),
            [](int a, int b) { return __builtin_popcount(a) < __builtin_popcount(b); });
        vector<uint32_t> minimal;
        uint32_t allMask = (1u << n) - 1;
        for (int a : trueAssignments) {
            bool isMinimal = true;
            for (uint32_t m : minimal) {
                if ((m & ~(a & allMask)) == 0) { // m <= a
                    isMinimal = false;
                    break;
                }
            }
            if (isMinimal) minimal.push_back((uint32_t)a);
        }

        // If any minimal term is 0 (all-zero), function is constant T (already handled)
        // Build expression from minimal terms
        unordered_map<vector<uint32_t>, Result, VectorHash> memo;
        Result ans = buildOR(minimal, n, memo);
        cout << "Yes\n" << ans.expr << "\n";
    }
    return 0;
}