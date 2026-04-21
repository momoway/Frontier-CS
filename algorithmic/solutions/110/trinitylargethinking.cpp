#include <bits/stdc++.h>
using namespace std;

const int ROWS = 8;
const int COLS = 14;
const int N = ROWS * COLS;
const int B = 500; // search limit

vector<int> neighbors[N];
bitset<N> neighBits[N];
vector<string> numStrs;

void initNeighbors() {
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            int idx = r * COLS + c;
            for (int dr = -1; dr <= 1; ++dr) {
                for (int dc = -1; dc <= 1; ++dc) {
                    if (dr == 0 && dc == 0) continue;
                    int nr = r + dr, nc = c + dc;
                    if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS) {
                        int nidx = nr * COLS + nc;
                        neighbors[idx].push_back(nidx);
                        neighBits[idx].set(nidx);
                    }
                }
            }
        }
    }
}

array<bitset<N>, 10> computeDigitBits(const char* g) {
    array<bitset<N>, 10> digitBits;
    for (int d = 0; d < 10; ++d) digitBits[d].reset();
    for (int i = 0; i < N; ++i) {
        int d = g[i] - '0';
        digitBits[d].set(i);
    }
    return digitBits;
}

bool canRead(const string &s, const char* g, const array<bitset<N>, 10> &digitBits) {
    int L = s.size();
    if (L == 0) return false;
    bitset<N> cur, nxt;
    int firstDigit = s[0] - '0';
    cur = digitBits[firstDigit];
    if (cur.none()) return false;
    for (int pos = 1; pos < L; ++pos) {
        nxt.reset();
        int d = s[pos] - '0';
        for (int u = 0; u < N; ++u) {
            if (cur.test(u)) {
                nxt |= (neighBits[u] & digitBits[d]);
            }
        }
        cur = nxt;
        if (cur.none()) return false;
    }
    return true;
}

int computeScore(const char* g) {
    auto digitBits = computeDigitBits(g);
    for (int i = 0; i < B; ++i) {
        const string &s = numStrs[i];
        if (!canRead(s, g, digitBits)) {
            return i;
        }
    }
    return B;
}

int main() {
    // Precompute number strings
    for (int i = 1; i <= B; ++i) {
        numStrs.push_back(to_string(i));
    }

    // Initialize neighbors
    initNeighbors();

    // Random generator
    random_device rd;
    mt19937 rng(rd());
    uniform_int_distribution<int> distCell(0, N - 1);
    uniform_int_distribution<int> distDigit(0, 9);
    uniform_int_distribution<int> dist100(0, 99);
    uniform_int_distribution<int> distRow(0, ROWS - 1);
    uniform_real_distribution<double> dist01(0, 1);

    // Initial grid: i % 10
    char currentGrid[N];
    char bestGrid[N];
    for (int i = 0; i < N; ++i) {
        currentGrid[i] = '0' + (i % 10);
    }
    int currentScore = computeScore(currentGrid);
    int bestScore = currentScore;
    memcpy(bestGrid, currentGrid, N);

    // Simulated annealing
    auto start = chrono::steady_clock::now();
    auto end = start + chrono::seconds(50);
    const double totalTime = 50.0;
    const double T0 = 200.0;

    while (chrono::steady_clock::now() < end) {
        double elapsed = chrono::duration<double>(chrono::steady_clock::now() - start).count();
        double T = max(0.1, T0 * (1.0 - elapsed / totalTime));

        // Propose new grid
        char proposal[N];
        memcpy(proposal, currentGrid, N);

        int r = dist100(rng);
        if (r < 70) {
            // Change a cell
            int idx = distCell(rng);
            proposal[idx] = '0' + distDigit(rng);
        } else if (r < 95) {
            // Swap two cells
            int idx1 = distCell(rng);
            int idx2 = distCell(rng);
            while (idx2 == idx1) idx2 = distCell(rng);
            swap(proposal[idx1], proposal[idx2]);
        } else {
            // Randomize a whole row
            int row = distRow(rng);
            for (int c = 0; c < COLS; ++c) {
                proposal[row * COLS + c] = '0' + distDigit(rng);
            }
        }

        int propScore = computeScore(proposal);

        // Acceptance probability
        double prob;
        if (propScore > currentScore) prob = 1.0;
        else if (propScore == currentScore) prob = 0.5;
        else prob = exp((propScore - currentScore) / T);

        if (dist01(rng) < prob) {
            memcpy(currentGrid, proposal, N);
            currentScore = propScore;
            if (currentScore > bestScore) {
                bestScore = currentScore;
                memcpy(bestGrid, currentGrid, N);
            }
        }
    }

    // Output best grid
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            cout << bestGrid[r * COLS + c];
        }
        cout << '\n';
    }

    return 0;
}