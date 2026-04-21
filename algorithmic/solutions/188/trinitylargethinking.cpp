#include <bits/stdc++.h>
using namespace std;

int charToIndex(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    else return 10 + (c - 'A');
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string S1, S2;
    cin >> S1 >> S2;
    int N = S1.size();
    int M = S2.size();

    vector<int> posList[36];
    for (int i = 0; i < M; ++i) {
        int idx = charToIndex(S2[i]);
        posList[idx].push_back(i);
    }

    int idxPtr[36] = {0};
    int pos = 0;
    auto getNext = [&](char c) -> int {
        int idx = charToIndex(c);
        auto &vec = posList[idx];
        int &p = idxPtr[idx];
        while (p < (int)vec.size() && vec[p] < pos) ++p;
        if (p < (int)vec.size()) return vec[p];
        else return M;
    };

    const int W = 100;
    vector<char> result;
    result.reserve(N);

    int i = 0;
    while (i < N) {
        int nxt = getNext(S1[i]);
        if (nxt < M) {
            result.push_back(S1[i]);
            pos = nxt + 1;
            i++;
            continue;
        }
        int bestNext = M;
        char bestChar = 0;
        int bestIdx = -1;
        int end = min(i + W, N);
        for (int j = i + 1; j < end; ++j) {
            if (bestNext == pos) break;
            char c = S1[j];
            int nxt2 = getNext(c);
            if (nxt2 < bestNext) {
                bestNext = nxt2;
                bestChar = c;
                bestIdx = j;
                if (bestNext == pos) break;
            }
        }
        if (bestNext < M) {
            result.push_back(bestChar);
            pos = bestNext + 1;
            i = bestIdx + 1;
        } else {
            i = end;
        }
    }

    for (char c : result) cout << c;
    cout << '\n';
    return 0;
}