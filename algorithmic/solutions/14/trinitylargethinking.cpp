#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Random number generator seeded with time to avoid predictability
    mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
    const long long MAX_X = 1000000000LL;

    // Initial walk to get starting vertex label
    cout << "walk 0" << endl;
    long long currentLabel;
    cin >> currentLabel;
    unordered_map<long long, long long> firstSeen;
    firstSeen[currentLabel] = 0;
    long long curOffset = 0;
    long long g = 0; // gcd of differences from collisions

    const long long COLLECTION_LIMIT = 196000; // number of walks for data collection
    for (long long i = 0; i < COLLECTION_LIMIT; ++i) {
        long long x = uniform_int_distribution<long long>(1, MAX_X)(rng);
        cout << "walk " << x << endl;
        cin >> currentLabel;
        curOffset += x;
        auto it = firstSeen.find(currentLabel);
        if (it != firstSeen.end()) {
            long long diff = curOffset - it->second;
            g = std::gcd(g, diff);
        } else {
            firstSeen[currentLabel] = curOffset;
        }
    }

    // If no collision yet, try walking 1e9 repeatedly to force a collision
    if (g == 0) {
        for (int attempt = 0; attempt < 1000; ++attempt) {
            long long x = MAX_X;
            cout << "walk " << x << endl;
            cin >> currentLabel;
            curOffset += x;
            auto it = firstSeen.find(currentLabel);
            if (it != firstSeen.end()) {
                long long diff = curOffset - it->second;
                g = std::gcd(g, diff);
                if (g != 0) break;
            } else {
                firstSeen[currentLabel] = curOffset;
            }
        }
    }

    // If still no collision (extremely unlikely), guess arbitrarily
    if (g == 0) {
        cout << "guess 1" << endl;
        return 0;
    }

    // Factor g and generate all divisors
    long long temp = g;
    vector<long long> primes;
    vector<int> exponents;
    for (long long p = 2; p * p <= temp; ++p) {
        if (temp % p == 0) {
            int e = 0;
            while (temp % p == 0) {
                temp /= p;
                ++e;
            }
            primes.push_back(p);
            exponents.push_back(e);
        }
    }
    if (temp > 1) {
        primes.push_back(temp);
        exponents.push_back(1);
    }

    vector<long long> divisors = {1};
    for (size_t i = 0; i < primes.size(); ++i) {
        long long p = primes[i];
        int e = exponents[i];
        size_t sz = divisors.size();
        long long p_pow = 1;
        for (int j = 1; j <= e; ++j) {
            p_pow *= p;
            for (size_t k = 0; k < sz; ++k) {
                divisors.push_back(divisors[k] * p_pow);
            }
        }
    }

    sort(divisors.begin(), divisors.end());
    divisors.erase(unique(divisors.begin(), divisors.end()), divisors.end());

    // Test divisors in increasing order; the smallest divisor that returns to the same vertex is n
    for (long long d : divisors) {
        if (d > MAX_X) continue; // cannot walk more than 1e9
        cout << "walk " << d << endl;
        long long newLabel;
        cin >> newLabel;
        if (currentLabel == newLabel) {
            cout << "guess " << d << endl;
            return 0;
        } else {
            currentLabel = newLabel;
        }
    }

    // Should not happen if g is correct, but guess 1 as last resort
    cout << "guess 1" << endl;
    return 0;
}