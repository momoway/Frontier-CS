#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T;
    if(!(cin >> T)) return 0;
    // Precompute primes up to 1000
    const int MAXP = 1000;
    vector<int> primes;
    vector<bool> is_prime(MAXP+1, true);
    is_prime[0] = is_prime[1] = false;
    for(int i = 2; i <= MAXP; ++i) {
        if(is_prime[i]) {
            primes.push_back(i);
            for(int j = i*i; j <= MAXP; j += i)
                is_prime[j] = false;
        }
    }
    // Build groups of primes such that product of each group <= 1e18
    vector<vector<int>> groups;
    vector<unsigned long long> groupProducts;
    for(int p : primes) {
        if(groups.empty()) {
            groups.push_back({p});
            groupProducts.push_back(p);
        } else {
            unsigned long long lastProd = groupProducts.back();
            if(lastProd <= (unsigned long long)1e18 / p) {
                groups.back().push_back(p);
                groupProducts.back() = lastProd * p;
            } else {
                groups.push_back({p});
                groupProducts.push_back(p);
            }
        }
    }
    // Play T games
    for(int game = 0; game < T; ++game) {
        vector<int> factors;
        // Query each group
        for(size_t i = 0; i < groups.size(); ++i) {
            unsigned long long Q = groupProducts[i];
            cout << "0 " << Q << endl;
            cout.flush();
            unsigned long long g;
            cin >> g;
            // Check each prime in the group
            for(int p : groups[i]) {
                if(g % p == 0) {
                    factors.push_back(p);
                }
            }
        }
        // Determine exponents for each found prime
        unsigned long long d_small = 1;
        for(int p : factors) {
            // Compute max exponent e_max such that p^e <= 1e9
            unsigned long long power = 1;
            int e_max = 0;
            while(power <= (unsigned long long)1e9 / p) {
                power *= p;
                e_max++;
            }
            // Query p^ e_max
            unsigned long long Q = power;
            cout << "0 " << Q << endl;
            cout.flush();
            unsigned long long g;
            cin >> g;
            // Extract exponent e from g = p^e
            int e = 0;
            unsigned long long temp = g;
            while(temp > 1) {
                temp /= p;
                e++;
            }
            d_small *= (e + 1);
        }
        unsigned long long ans = 2 * d_small;
        cout << "1 " << ans << endl;
        cout.flush();
    }
    return 0;
}