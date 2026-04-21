#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int si, sj;
    cin >> si >> sj;
    const int N = 50;
    vector<vector<int>> tile(N, vector<int>(N));
    vector<vector<int>> p(N, vector<int>(N));
    int max_id = -1;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            cin >> tile[i][j];
            max_id = max(max_id, tile[i][j]);
        }
    }
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            cin >> p[i][j];
        }
    }
    int M = max_id + 1;
    vector<char> visited(M);
    const int di[4] = {-1, 1, 0, 0};
    const int dj[4] = {0, 0, -1, 1};
    vector<vector<vector<pair<int,int>>>> neigh(N, vector<vector<pair<int,int>>>(N));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            for (int d = 0; d < 4; ++d) {
                int ni = i + di[d];
                int nj = j + dj[d];
                if (ni >= 0 && ni < N && nj >= 0 && nj < N) {
                    neigh[i][j].push_back({ni, nj});
                }
            }
        }
    }

    vector<pair<int,int>> best_path;
    int best_score = -1;
    const int ITER = 1000;
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

    for (int it = 0; it < ITER; ++it) {
        fill(visited.begin(), visited.end(), 0);
        visited[tile[si][sj]] = 1;
        vector<pair<int,int>> path;
        path.emplace_back(si, sj);
        int score = p[si][sj];
        while (true) {
            int i = path.back().first;
            int j = path.back().second;
            vector<tuple<int,int,int>> cands;
            for (auto &dir : neigh[i][j]) {
                int ni = dir.first;
                int nj = dir.second;
                int t_cand = tile[ni][nj];
                if (visited[t_cand]) continue;
                int own = p[ni][nj];
                int cnt = 0;
                int max_next = 0;
                for (auto &dir2 : neigh[ni][nj]) {
                    int ni2 = dir2.first;
                    int nj2 = dir2.second;
                    int t2 = tile[ni2][nj2];
                    if (t2 == t_cand) continue;
                    if (visited[t2]) continue;
                    cnt++;
                    max_next = max(max_next, p[ni2][nj2]);
                }
                int score_cand = own + cnt * 10 + max_next * 2 + (int)(rng() % 10);
                cands.emplace_back(score_cand, ni, nj);
            }
            if (cands.empty()) break;
            sort(cands.begin(), cands.end(), greater<tuple<int,int,int>>());
            int k = min(3, (int)cands.size());
            int idx = rng() % k;
            int ni = get<1>(cands[idx]);
            int nj = get<2>(cands[idx]);
            visited[tile[ni][nj]] = 1;
            path.emplace_back(ni, nj);
            score += p[ni][nj];
        }
        if (score > best_score) {
            best_score = score;
            best_path = move(path);
        }
    }

    fill(visited.begin(), visited.end(), 0);
    for (auto &pos : best_path) {
        visited[tile[pos.first][pos.second]] = 1;
    }

    vector<pair<int,int>> &path = best_path;
    bool improved = true;
    int improve_iter = 0;
    const int MAX_IMPROVE = 1000;
    while (improved && improve_iter < MAX_IMPROVE) {
        improved = false;
        int best_score_inc = -1;
        int best_i = -1;
        pair<int,int> best_B;
        for (int i = 0; i < (int)path.size() - 1; ++i) {
            auto A = path[i];
            auto C = path[i+1];
            for (auto &B : neigh[A.first][A.second]) {
                bool is_common = false;
                for (auto &Nb : neigh[C.first][C.second]) {
                    if (Nb == B) {
                        is_common = true;
                        break;
                    }
                }
                if (!is_common) continue;
                int tB = tile[B.first][B.second];
                if (visited[tB]) continue;
                int scoreB = p[B.first][B.second];
                if (scoreB > best_score_inc) {
                    best_score_inc = scoreB;
                    best_i = i;
                    best_B = B;
                }
            }
        }
        if (best_score_inc > 0) {
            path.insert(path.begin() + best_i + 1, best_B);
            visited[tile[best_B.first][best_B.second]] = 1;
            improved = true;
            improve_iter++;
        }
    }

    string moves;
    for (int i = 0; i < (int)path.size() - 1; ++i) {
        int i1 = path[i].first, j1 = path[i].second;
        int i2 = path[i+1].first, j2 = path[i+1].second;
        if (i2 == i1 - 1) moves.push_back('U');
        else if (i2 == i1 + 1) moves.push_back('D');
        else if (j2 == j1 - 1) moves.push_back('L');
        else if (j2 == j1 + 1) moves.push_back('R');
    }
    cout << moves << "\n";
    return 0;
}