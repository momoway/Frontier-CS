#include <bits/stdc++.h>
using namespace std;

const int N = 20;
bool canGo[N][N][4]; // 0: up, 1: right, 2: down, 3: left

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int si, sj, ti, tj;
    double p;
    if (!(cin >> si >> sj >> ti >> tj >> p)) return 0;

    vector<string> hstr(N);
    for (int i = 0; i < N; i++) cin >> hstr[i];
    vector<string> vstr(N - 1);
    for (int i = 0; i < N - 1; i++) cin >> vstr[i];

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            for (int d = 0; d < 4; d++)
                canGo[i][j][d] = false;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < 19; j++) {
            if (hstr[i][j] == '0') {
                canGo[i][j][1] = true;
                canGo[i][j + 1][3] = true;
            }
        }
    }
    for (int i = 0; i < N - 1; i++) {
        for (int j = 0; j < N; j++) {
            if (vstr[i][j] == '0') {
                canGo[i][j][2] = true;
                canGo[i + 1][j][0] = true;
            }
        }
    }

    int par_i[N][N], par_j[N][N];
    char par_dir[N][N];
    bool visited[N][N];
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            visited[i][j] = false;

    queue<pair<int, int>> q;
    visited[si][sj] = true;
    par_i[si][sj] = -1;
    par_j[si][sj] = -1;
    par_dir[si][sj] = 0;
    q.push({si, sj});

    const int dr[4] = {-1, 0, 1, 0};
    const int dc[4] = {0, 1, 0, -1};
    const char dirChar[4] = {'U', 'R', 'D', 'L'};

    while (!q.empty()) {
        auto [i, j] = q.front(); q.pop();
        if (i == ti && j == tj) break;
        for (int d = 0; d < 4; d++) {
            int ni = i + dr[d];
            int nj = j + dc[d];
            if (ni >= 0 && ni < N && nj >= 0 && nj < N && canGo[i][j][d] && !visited[ni][nj]) {
                visited[ni][nj] = true;
                par_i[ni][nj] = i;
                par_j[ni][nj] = j;
                par_dir[ni][nj] = dirChar[d];
                q.push({ni, nj});
            }
        }
    }

    string path = "";
    int ci = ti, cj = tj;
    while (ci != si || cj != sj) {
        char d = par_dir[ci][cj];
        path.push_back(d);
        int pi = par_i[ci][cj];
        int pj = par_j[ci][cj];
        ci = pi;
        cj = pj;
    }
    reverse(path.begin(), path.end());
    int D = path.size();
    if (D > 200) {
        path = path.substr(0, 200);
        D = 200;
    }

    auto evaluate = [&](const string& seq) -> double {
        int L = seq.size();
        double dist[N][N];
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                dist[i][j] = 0.0;
        dist[si][sj] = 1.0;
        double total = 0.0;
        for (int step = 0; step < L; step++) {
            double newDist[N][N];
            for (int i = 0; i < N; i++)
                for (int j = 0; j < N; j++)
                    newDist[i][j] = 0.0;
            double reach = 0.0;
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    if (i == ti && j == tj) continue;
                    double dval = dist[i][j];
                    if (dval == 0.0) continue;
                    newDist[i][j] += dval * p;
                    char dirChar = seq[step];
                    int di = 0, dj = 0, dirIdx = -1;
                    if (dirChar == 'U') { di = -1; dj = 0; dirIdx = 0; }
                    else if (dirChar == 'R') { di = 0; dj = 1; dirIdx = 1; }
                    else if (dirChar == 'D') { di = 1; dj = 0; dirIdx = 2; }
                    else if (dirChar == 'L') { di = 0; dj = -1; dirIdx = 3; }
                    int ni = i + di;
                    int nj = j + dj;
                    if (ni >= 0 && ni < N && nj >= 0 && nj < N && canGo[i][j][dirIdx]) {
                        if (ni == ti && nj == tj) {
                            reach += dval * (1 - p);
                        } else {
                            newDist[ni][nj] += dval * (1 - p);
                        }
                    } else {
                        newDist[i][j] += dval * (1 - p);
                    }
                }
            }
            total += (401.0 - (step + 1)) * reach;
            for (int i = 0; i < N; i++)
                for (int j = 0; j < N; j++)
                    dist[i][j] = newDist[i][j];
        }
        return total;
    };

    string currentSeq = path;
    double currentScore = evaluate(currentSeq);
    string bestSeq = currentSeq;
    double bestScore = currentScore;

    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> dirDist(0, 3);
    auto randomDir = [&]() -> char {
        int r = dirDist(rng);
        if (r == 0) return 'U';
        if (r == 1) return 'R';
        if (r == 2) return 'D';
        return 'L';
    };

    const int MAX_ITER = 2000;
    for (int iter = 0; iter < MAX_ITER; iter++) {
        string candidate = currentSeq;
        int len = candidate.size();
        bool changed = false;
        uniform_int_distribution<int> mutDist(0, 4);
        int mut = mutDist(rng);
        if (mut == 0 && len < 200) {
            uniform_int_distribution<int> posDist(0, len);
            int pos = posDist(rng);
            char d = randomDir();
            candidate.insert(pos, 1, d);
            changed = true;
        } else if (mut == 1 && len > 1) {
            uniform_int_distribution<int> posDist(0, len - 1);
            int pos = posDist(rng);
            candidate.erase(pos, 1);
            changed = true;
        } else if (mut == 2) {
            if (len == 0) {
                if (len < 200) {
                    uniform_int_distribution<int> posDist(0, len);
                    int pos = posDist(rng);
                    char d = randomDir();
                    candidate.insert(pos, 1, d);
                    changed = true;
                }
            } else {
                uniform_int_distribution<int> posDist(0, len - 1);
                int pos = posDist(rng);
                char d = randomDir();
                candidate[pos] = d;
                changed = true;
            }
        } else if (mut == 3 && len < 200) {
            if (len > 0) {
                uniform_int_distribution<int> srcDist(0, len - 1);
                int srcPos = srcDist(rng);
                char d = candidate[srcPos];
                uniform_int_distribution<int> posDist(0, len);
                int pos = posDist(rng);
                candidate.insert(pos, 1, d);
                changed = true;
            } else {
                uniform_int_distribution<int> posDist(0, len);
                int pos = posDist(rng);
                char d = randomDir();
                candidate.insert(pos, 1, d);
                changed = true;
            }
        } else if (mut == 4 && len >= 2) {
            uniform_int_distribution<int> posDist(0, len - 1);
            int i = posDist(rng);
            int j = posDist(rng);
            if (i != j) {
                swap(candidate[i], candidate[j]);
                changed = true;
            }
        }
        if (!changed) {
            if (len < 200) {
                candidate.push_back(randomDir());
                changed = true;
            } else if (len > 1) {
                candidate.pop_back();
                changed = true;
            }
        }
        if (!changed) continue;
        double candScore = evaluate(candidate);
        if (candScore > currentScore) {
            currentSeq = candidate;
            currentScore = candScore;
        } else {
            uniform_real_distribution<double> accDist(0.0, 1.0);
            if (accDist(rng) < 0.05) {
                currentSeq = candidate;
                currentScore = candScore;
            }
        }
        if (currentScore > bestScore) {
            bestScore = currentScore;
            bestSeq = currentSeq;
        }
    }

    cout << bestSeq << endl;
    return 0;
}