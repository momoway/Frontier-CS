#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    if (!(cin >> N)) return 0;
    vector<int> S(N);
    for (int i = 0; i < N; ++i) cin >> S[i];

    int M;
    cin >> M;
    vector<pair<int,int>> jerry(M);
    for (int i = 0; i < M; ++i) {
        cin >> jerry[i].first >> jerry[i].second;
    }

    // If already sorted, output R=0, V=0
    bool alreadySorted = true;
    for (int i = 0; i < N; ++i) {
        if (S[i] != i) { alreadySorted = false; break; }
    }
    if (alreadySorted) {
        cout << 0 << "\n\n" << 0 << "\n";
        return 0;
    }

    vector<int> pos(N);
    vector<char> correct(N, 0);
    int misplacedCount = 0;
    for (int i = 0; i < N; ++i) {
        pos[S[i]] = i;
        if (S[i] == i) correct[i] = 1;
        else ++misplacedCount;
    }

    // min-heap: (distance, value)
    using P = pair<int,int>;
    priority_queue<P, vector<P>, greater<P>> heap;
    for (int i = 0; i < N; ++i) {
        if (!correct[i]) {
            int d = abs(i - pos[i]);
            heap.emplace(d, i);
        }
    }

    vector<pair<int,int>> ourSwaps;
    long long sumDist = 0;
    int last_u = -1, last_v = -1;
    int R = 0;

    for (int r = 0; r < M; ++r) {
        // Jerry's move
        int X = jerry[r].first;
        int Y = jerry[r].second;
        int a = S[X];
        int b = S[Y];
        // swap
        swap(S[X], S[Y]);
        pos[a] = Y;
        pos[b] = X;
        // update correct flags for a and b
        bool wasCorrectA = correct[a];
        correct[a] = (pos[a] == a);
        if (wasCorrectA && !correct[a]) ++misplacedCount;
        else if (!wasCorrectA && correct[a]) --misplacedCount;

        bool wasCorrectB = correct[b];
        correct[b] = (pos[b] == b);
        if (wasCorrectB && !correct[b]) ++misplacedCount;
        else if (!wasCorrectB && correct[b]) --misplacedCount;

        // push new entries for a and b if misplaced
        if (!correct[a]) {
            int d = abs(a - pos[a]);
            heap.emplace(d, a);
        }
        if (!correct[b]) {
            int d = abs(b - pos[b]);
            heap.emplace(d, b);
        }

        // If sorted after Jerry's move, we still need to perform a dummy swap this round
        if (misplacedCount == 0) {
            // dummy swap (0,0)
            ourSwaps.emplace_back(0, 0);
            sumDist += 0;
            R = r + 1;
            break;
        }

        // Choose our swap
        pair<int,int> fallback = {-1, -1};
        pair<int,int> chosen = {-1, -1};
        while (!heap.empty()) {
            auto [d, i] = heap.top(); heap.pop();
            if (correct[i]) continue;
            int curDist = abs(i - pos[i]);
            if (curDist != d) continue; // stale
            int j = pos[i];
            // check if same as last swap
            bool isLast = (i == last_u && j == last_v) || (i == last_v && j == last_u);
            // check if same as Jerry's swap
            bool isJerry = (i == X && j == Y) || (i == Y && j == X);
            if (isLast || isJerry) {
                if (fallback.first == -1) fallback = {i, j};
                continue;
            }
            chosen = {i, j};
            break;
        }
        if (chosen.first == -1) {
            if (fallback.first != -1) {
                chosen = fallback;
            } else {
                // emergency: scan for any misplaced value
                for (int i = 0; i < N; ++i) {
                    if (!correct[i]) {
                        chosen = {i, pos[i]};
                        break;
                    }
                }
            }
        }

        int u = chosen.first;
        int v = chosen.second;
        // perform swap
        int val_u = S[u];
        int val_v = S[v];
        swap(S[u], S[v]);
        pos[val_u] = v;
        pos[val_v] = u;
        // update correct flags for val_u and val_v
        bool wasCorrectU = correct[val_u];
        correct[val_u] = (pos[val_u] == val_u);
        if (wasCorrectU && !correct[val_u]) ++misplacedCount;
        else if (!wasCorrectU && correct[val_u]) --misplacedCount;

        bool wasCorrectV = correct[val_v];
        correct[val_v] = (pos[val_v] == val_v);
        if (wasCorrectV && !correct[val_v]) ++misplacedCount;
        else if (!wasCorrectV && correct[val_v]) --misplacedCount;

        // push new entries for val_u and val_v if misplaced
        if (!correct[val_u]) {
            int d = abs(val_u - pos[val_u]);
            heap.emplace(d, val_u);
        }
        if (!correct[val_v]) {
            int d = abs(val_v - pos[val_v]);
            heap.emplace(d, val_v);
        }

        ourSwaps.emplace_back(u, v);
        sumDist += abs(u - v);
        last_u = u; last_v = v;
        ++R;

        // check if sorted after our move
        if (misplacedCount == 0) {
            break;
        }
    }

    // Output
    cout << R << "\n";
    for (auto &p : ourSwaps) {
        cout << p.first << " " << p.second << "\n";
    }
    long long V = (long long)R * sumDist;
    cout << V << "\n";

    return 0;
}