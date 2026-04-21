#include <bits/stdc++.h>
using namespace std;

using ull = unsigned long long;

vector<int> primes;

void sieve(int n = 1000000) {
    vector<bool> is_prime(n+1, true);
    is_prime[0] = is_prime[1] = false;
    for (int i = 2; i <= n; ++i) {
        if (is_prime[i]) {
            primes.push_back(i);
            if ((ull)i * i <= n) {
                for (int j = i * i; j <= n; j += i)
                    is_prime[j] = false;
            }
        }
    }
}

vector<pair<ull, int>> factorize(ull x) {
    vector<pair<ull, int>> factors;
    ull tmp = x;
    for (int p : primes) {
        if ((ull)p * p > tmp) break;
        if (tmp % p == 0) {
            int cnt = 0;
            while (tmp % p == 0) {
                tmp /= p;
                ++cnt;
            }
            factors.emplace_back(p, cnt);
        }
    }
    if (tmp > 1) factors.emplace_back(tmp, 1);
    return factors;
}

void generate_divisors(const vector<pair<ull, int>>& factors, size_t idx, ull cur, vector<ull>& divisors) {
    if (idx == factors.size()) {
        divisors.push_back(cur);
        return;
    }
    ull p = factors[idx].first;
    int exp = factors[idx].second;
    ull val = 1;
    for (int i = 0; i <= exp; ++i) {
        generate_divisors(factors, idx + 1, cur * val, divisors);
        val *= p;
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    ull n;
    if (!(cin >> n)) return 0;
    if (n == 1) {
        cout << "1\n1\n";
        return 0;
    }
    sieve();
    ull best_v = 0;
    vector<ull> best_seq;
    int max_a = (n < 500 ? (int)n : 500);
    for (int a = 2; a <= max_a; ++a) {
        if ((ull)a > n) break;
        auto factors_a = factorize(a);
        vector<ull> divisors_a;
        generate_divisors(factors_a, 0, 1, divisors_a);
        sort(divisors_a.begin(), divisors_a.end());
        for (ull d : divisors_a) {
            if (d >= a) continue;
            vector<ull> seq;
            seq.push_back(d);
            seq.push_back(a);
            ull cur = a;
            ull prev_gcd = d;
            ull sum = d + a;
            while (true) {
                auto fac = factorize(cur);
                vector<ull> divs;
                generate_divisors(fac, 0, 1, divs);
                sort(divs.begin(), divs.end());
                ull e = cur; // default to cur itself
                for (ull div : divs) {
                    if (div > prev_gcd) {
                        e = div;
                        break;
                    }
                }
                ull new_cur = cur + e;
                if (new_cur > n) break;
                seq.push_back(new_cur);
                sum += new_cur;
                cur = new_cur;
                prev_gcd = e;
            }
            ull v = (ull)seq.size() * sum;
            if (v > best_v || (v == best_v && seq.size() > best_seq.size())) {
                best_v = v;
                best_seq = seq;
            }
        }
    }
    cout << best_seq.size() << "\n";
    for (size_t i = 0; i < best_seq.size(); ++i) {
        if (i) cout << ' ';
        cout << best_seq[i];
    }
    cout << "\n";
    return 0;
}