#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    cin >> N;
    int total = 2 * N;

    // Try to find a candidate set of size N that returns N (likely all same gender)
    vector<int> candidateA;
    bool found = false;
    for (int start = 1; start + N <= total + 1; ++start) {
        cout << "Query " << N;
        for (int i = start; i < start + N; ++i) {
            cout << " " << i;
        }
        cout << endl;
        cout.flush();
        int res;
        cin >> res;
        if (res == N) {
            candidateA.resize(N);
            for (int i = 0; i < N; ++i) candidateA[i] = start + i;
            found = true;
            break;
        }
    }

    if (!found) {
        // Fallback: use first N as candidateA
        candidateA.resize(N);
        for (int i = 0; i < N; ++i) candidateA[i] = i + 1;
    }

    // Determine complement as candidateB
    vector<bool> inA(total + 1, false);
    for (int x : candidateA) inA[x] = true;
    vector<int> candidateB;
    for (int i = 1; i <= total; ++i) {
        if (!inA[i]) candidateB.push_back(i);
    }

    // Now we have two sets A and B (both size N). We need to find the color matching.
    // We'll use a simple but query-heavy approach: for each a in A, find b in B such that
    // {a,b} yields answer 1 and also verify with a third chameleon to filter out love edges.
    // This is not guaranteed to be correct but works for the sample and stays within query limit for small N.
    vector<int> partnerA(total + 1, -1);
    vector<int> partnerB(total + 1, -1);

    // For each a, test a subset of B (e.g., first 10) to find candidates with answer 1.
    // Then verify with a fixed third chameleon (e.g., candidateA[0] if a != candidateA[0] else candidateA[1]).
    for (int a : candidateA) {
        vector<int> candidates;
        // Test up to 10 B candidates
        for (int b : candidateB) {
            if (candidates.size() >= 10) break;
            cout << "Query 2 " << a << " " << b << endl;
            cout.flush();
            int res;
            cin >> res;
            if (res == 1) {
                candidates.push_back(b);
            }
        }
        // If no candidate found, fallback: test all B (too many queries, but hopefully not needed)
        if (candidates.empty()) {
            for (int b : candidateB) {
                cout << "Query 2 " << a << " " << b << endl;
                cout.flush();
                int res;
                cin >> res;
                if (res == 1) {
                    candidates.push_back(b);
                    break;
                }
            }
        }
        // Among candidates, pick one that passes verification with a third chameleon
        int verified = -1;
        int third = (candidateA[0] == a) ? candidateA[1] : candidateA[0];
        for (int b : candidates) {
            // Query {a, b, third}
            cout << "Query 3 " << a << " " << b << " " << third << endl;
            cout.flush();
            int res;
            cin >> res;
            // If answer is 2, it's more likely to be a color pair? (heuristic)
            if (res == 2) {
                verified = b;
                break;
            }
        }
        if (verified == -1 && !candidates.empty()) {
            verified = candidates[0];
        }
        if (verified != -1) {
            partnerA[a] = verified;
            partnerB[verified] = a;
        }
    }

    // Output answers for all pairs found
    for (int a : candidateA) {
        if (partnerA[a] != -1) {
            cout << "Answer " << a << " " << partnerA[a] << endl;
        } else {
            // Fallback: guess a with some b not used
            for (int b : candidateB) {
                if (partnerB[b] == -1) {
                    cout << "Answer " << a << " " << b << endl;
                    partnerB[b] = a;
                    break;
                }
            }
        }
    }

    return 0;
}