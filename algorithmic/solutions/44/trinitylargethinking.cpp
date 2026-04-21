#include <bits/stdc++.h>
using namespace std;

struct Point {
    long long x, y;
    int id;
};

double dist(const vector<Point>& pts, int a, int b) {
    long long dx = pts[a].x - pts[b].x;
    long long dy = pts[a].y - pts[b].y;
    return sqrt((double)dx*dx + (double)dy*dy);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int N;
    if (!(cin >> N)) return 0;
    vector<Point> pts(N);
    for (int i = 0; i < N; ++i) {
        long long x, y;
        cin >> x >> y;
        pts[i] = {x, y, i};
    }

    const long long SHIFT = 1000000000LL;
    vector<pair<uint64_t, int>> codes;
    codes.reserve(N);
    for (const auto& p : pts) {
        uint32_t ux = (uint32_t)(p.x + SHIFT);
        uint32_t uy = (uint32_t)(p.y + SHIFT);
        uint64_t code = 0;
        for (int i = 0; i < 31; ++i) {
            uint64_t mask = 1ULL << i;
            if (ux & mask) code |= (1ULL << (2*i));
            if (uy & mask) code |= (1ULL << (2*i + 1));
        }
        codes.emplace_back(code, p.id);
    }

    sort(codes.begin(), codes.end(),
        [](const pair<uint64_t,int>& a, const pair<uint64_t,int>& b) {
            return a.first < b.first;
        });

    vector<int> order;
    for (const auto& pr : codes) {
        if (pr.second != 0) order.push_back(pr.second);
    }

    vector<int> tour1;
    tour1.reserve(N+1);
    tour1.push_back(0);
    for (int id : order) tour1.push_back(id);
    tour1.push_back(0);

    vector<int> order_rev = order;
    reverse(order_rev.begin(), order_rev.end());
    vector<int> tour2;
    tour2.reserve(N+1);
    tour2.push_back(0);
    for (int id : order_rev) tour2.push_back(id);
    tour2.push_back(0);

    vector<bool> is_prime(N, false);
    if (N > 2) {
        for (int i = 2; i < N; ++i) is_prime[i] = true;
        for (int p = 2; p * p < N; ++p) {
            if (is_prime[p]) {
                for (int q = p * p; q < N; q += p) {
                    is_prime[q] = false;
                }
            }
        }
    }

    auto penalizedLength = [&](const vector<int>& tour) -> double {
        double total = 0.0;
        int steps = (int)tour.size() - 1;
        for (int t = 1; t <= steps; ++t) {
            int src = tour[t-1];
            int dst = tour[t];
            double d = dist(pts, src, dst);
            if (t % 10 == 0 && !is_prime[src]) {
                d *= 1.1;
            }
            total += d;
        }
        return total;
    };

    double len1 = penalizedLength(tour1);
    double len2 = penalizedLength(tour2);
    const vector<int>& best_tour = (len1 <= len2) ? tour1 : tour2;

    cout << N+1 << "\n";
    for (int id : best_tour) {
        cout << id << "\n";
    }

    return 0;
}