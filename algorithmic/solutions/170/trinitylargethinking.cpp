#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, L;
    if (!(cin >> N >> L)) return 0;
    vector<int> T(N);
    for (int i = 0; i < N; ++i) cin >> T[i];

    int a[100], b[100];
    for (int i = 0; i < N; ++i) {
        a[i] = (i + 1) % N;
        b[i] = (i + 1) % N;
    }

    auto simulate = [&](int a[], int b[], int count[]) {
        memset(count, 0, N * sizeof(int));
        int cur = 0;
        count[cur] = 1;
        for (int week = 2; week <= L; ++week) {
            if (count[cur] % 2 == 1) cur = a[cur];
            else cur = b[cur];
            count[cur]++;
        }
    };

    int count[100];
    simulate(a, b, count);
    long long error = 0;
    for (int i = 0; i < N; ++i) error += abs(count[i] - T[i]);
    long long best_error = error;
    int best_a[100], best_b[100];
    for (int i = 0; i < N; ++i) {
        best_a[i] = a[i];
        best_b[i] = b[i];
    }

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist_int(0, N - 1);
    std::uniform_int_distribution<int> dist_move(0, 2);
    std::uniform_real_distribution<double> dist_real(0, 1);

    double temp = 1000.0;
    const int MAX_ITER = 1000;
    for (int iter = 0; iter < MAX_ITER; ++iter) {
        int i = dist_int(rng);
        int move = dist_move(rng);
        int old_a = a[i];
        int old_b = b[i];
        bool swapped = false;
        if (move == 0) {
            a[i] = dist_int(rng);
        } else if (move == 1) {
            b[i] = dist_int(rng);
        } else {
            swap(a[i], b[i]);
            swapped = true;
        }

        simulate(a, b, count);
        long long new_error = 0;
        for (int j = 0; j < N; ++j) new_error += abs(count[j] - T[j]);

        if (new_error < best_error) {
            best_error = new_error;
            for (int j = 0; j < N; ++j) {
                best_a[j] = a[j];
                best_b[j] = b[j];
            }
        } else {
            double prob = exp((best_error - new_error) / temp);
            if (dist_real(rng) < prob) {
                best_error = new_error;
                for (int j = 0; j < N; ++j) {
                    best_a[j] = a[j];
                    best_b[j] = b[j];
                }
            } else {
                if (swapped) {
                    swap(a[i], b[i]);
                } else {
                    if (move == 0) a[i] = old_a;
                    else b[i] = old_b;
                }
            }
        }
        temp *= 0.995;
    }

    for (int i = 0; i < N; ++i) {
        cout << best_a[i] << " " << best_b[i] << "\n";
    }

    return 0;
}