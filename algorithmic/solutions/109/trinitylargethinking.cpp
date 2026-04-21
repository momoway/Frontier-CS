#include <bits/stdc++.h>
using namespace std;

struct Candidate {
    int r, c;
    int d1;
    int d2;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    if (!(cin >> N)) return 0;
    int sr, sc;
    cin >> sr >> sc;
    sr--; sc--;

    const int dr[8] = {-2, -2, -1, -1, 1, 1, 2, 2};
    const int dc[8] = {-1, 1, -2, 2, -2, 2, -1, 1};

    auto find_path = [&](auto comp) -> vector<pair<int,int>> {
        vector<char> visited(N * N, 0);
        vector<pair<int,int>> path;
        path.reserve(N * N);
        int r = sr, c = sc;
        visited[r * N + c] = 1;
        path.emplace_back(r, c);

        while (true) {
            vector<Candidate> candidates;
            for (int k = 0; k < 8; ++k) {
                int nr = r + dr[k];
                int nc = c + dc[k];
                if (nr >= 0 && nr < N && nc >= 0 && nc < N && !visited[nr * N + nc]) {
                    candidates.push_back({nr, nc, 0, 0});
                }
            }
            if (candidates.empty()) break;

            for (auto &cand : candidates) {
                int d1 = 0;
                for (int k = 0; k < 8; ++k) {
                    int nr = cand.r + dr[k];
                    int nc = cand.c + dc[k];
                    if (nr >= 0 && nr < N && nc >= 0 && nc < N && !visited[nr * N + nc]) {
                        ++d1;
                    }
                }
                cand.d1 = d1;

                int d2 = 0;
                for (int k = 0; k < 8; ++k) {
                    int nr = cand.r + dr[k];
                    int nc = cand.c + dc[k];
                    if (nr >= 0 && nr < N && nc >= 0 && nc < N && !visited[nr * N + nc]) {
                        int deg_n = 0;
                        for (int kk = 0; kk < 8; ++kk) {
                            int nnr = nr + dr[kk];
                            int nnc = nc + dc[kk];
                            if (nnr >= 0 && nnr < N && nnc >= 0 && nnc < N && !visited[nnr * N + nnc]) {
                                ++deg_n;
                            }
                        }
                        d2 += (deg_n - 1);
                    }
                }
                cand.d2 = d2;
            }

            auto best = candidates.begin();
            for (auto it = candidates.begin() + 1; it != candidates.end(); ++it) {
                if (comp(*it, *best)) best = it;
            }

            r = best->r;
            c = best->c;
            visited[r * N + c] = 1;
            path.emplace_back(r, c);
        }
        return path;
    };

    // Attempt 1: improved heuristic (d1, d2, row, column)
    auto path1 = find_path([](const Candidate& a, const Candidate& b) {
        if (a.d1 != b.d1) return a.d1 < b.d1;
        if (a.d2 != b.d2) return a.d2 < b.d2;
        if (a.r != b.r) return a.r < b.r;
        return a.c < b.c;
    });
    if (path1.size() == N * N) {
        cout << path1.size() << "\n";
        for (auto &p : path1) {
            cout << p.first + 1 << " " << p.second + 1 << "\n";
        }
        return 0;
    }

    // Attempt 2: simple heuristic (d1, row, column)
    auto path2 = find_path([](const Candidate& a, const Candidate& b) {
        if (a.d1 != b.d1) return a.d1 < b.d1;
        if (a.r != b.r) return a.r < b.r;
        return a.c < b.c;
    });
    if (path2.size() == N * N) {
        cout << path2.size() << "\n";
        for (auto &p : path2) {
            cout << p.first + 1 << " " << p.second + 1 << "\n";
        }
        return 0;
    }

    const vector<pair<int,int>>& best_path = (path1.size() >= path2.size()) ? path1 : path2;
    cout << best_path.size() << "\n";
    for (auto &p : best_path) {
        cout << p.first + 1 << " " << p.second + 1 << "\n";
    }

    return 0;
}