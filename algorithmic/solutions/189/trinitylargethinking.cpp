#include <bits/stdc++.h>
using namespace std;

int char_to_index(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    return 26 + (c - '0');
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string S1, S2;
    cin >> S1 >> S2;
    int N = S1.size();
    int M = S2.size();

    // Compute trivial alignment and its cost
    string trivial_transcript;
    trivial_transcript.reserve(N + M);
    int cost_trivial = 0;
    int i = 0, j = 0;
    while (i < N && j < M) {
        if (S1[i] != S2[j]) cost_trivial++;
        trivial_transcript.push_back('M');
        i++; j++;
    }
    while (i < N) {
        trivial_transcript.push_back('D');
        cost_trivial++;
        i++;
    }
    while (j < M) {
        trivial_transcript.push_back('I');
        cost_trivial++;
        j++;
    }

    // Preprocess positions for greedy
    vector<vector<int>> pos1(36), pos2(36);
    for (int idx = 0; idx < N; ++idx) {
        pos1[char_to_index(S1[idx])].push_back(idx);
    }
    for (int idx = 0; idx < M; ++idx) {
        pos2[char_to_index(S2[idx])].push_back(idx);
    }

    int idx1[36] = {0};
    int idx2[36] = {0};
    const int INF = 1e9;

    string greedy_transcript;
    greedy_transcript.reserve(N + M);
    int cost_greedy = 0;
    i = 0; j = 0;
    while (i < N && j < M) {
        if (S1[i] == S2[j]) {
            greedy_transcript.push_back('M');
            i++; j++;
        } else {
            int c2 = char_to_index(S2[j]);
            while (idx1[c2] < (int)pos1[c2].size() && pos1[c2][idx1[c2]] < i) idx1[c2]++;
            int next_i = (idx1[c2] < (int)pos1[c2].size()) ? pos1[c2][idx1[c2]] : INF;
            int c1 = char_to_index(S1[i]);
            while (idx2[c1] < (int)pos2[c1].size() && pos2[c1][idx2[c1]] < j) idx2[c1]++;
            int next_j = (idx2[c1] < (int)pos2[c1].size()) ? pos2[c1][idx2[c1]] : INF;

            if (next_i == INF && next_j == INF) {
                greedy_transcript.push_back('M');
                cost_greedy++;
                i++; j++;
            } else if (next_i == INF) {
                greedy_transcript.push_back('I');
                cost_greedy++;
                j++;
            } else if (next_j == INF) {
                greedy_transcript.push_back('D');
                cost_greedy++;
                i++;
            } else {
                if (next_i - i <= next_j - j) {
                    greedy_transcript.push_back('D');
                    cost_greedy++;
                    i++;
                } else {
                    greedy_transcript.push_back('I');
                    cost_greedy++;
                    j++;
                }
            }
        }
    }
    while (i < N) {
        greedy_transcript.push_back('D');
        cost_greedy++;
        i++;
    }
    while (j < M) {
        greedy_transcript.push_back('I');
        cost_greedy++;
        j++;
    }

    if (cost_greedy < cost_trivial) {
        cout << greedy_transcript << '\n';
    } else {
        cout << trivial_transcript << '\n';
    }
    return 0;
}