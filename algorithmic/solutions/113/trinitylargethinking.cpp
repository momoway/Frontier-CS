#include <bits/stdc++.h>
using namespace std;

void transfer(int l, int r, int src, int dst, int aux, bool dstHasSmaller, vector<pair<int,int>>& moves) {
    if (l > r) return;
    if (l == r) {
        moves.push_back({src, dst});
        return;
    }
    int size = r - l + 1;
    if (size == 2 && dstHasSmaller) {
        // Move larger then smaller
        moves.push_back({src, dst}); // move larger (r)
        moves.push_back({src, dst}); // move smaller (l)
        return;
    }
    int m = l + (size + 2) / 2 - 1;
    // Move median to auxiliary
    moves.push_back({src, aux});
    if (!dstHasSmaller) {
        // Move left part first
        transfer(l, m-1, src, dst, aux, false);
        // After left, dst has left part (smaller than right), so for right, dstHasSmaller = true
        transfer(m+1, r, src, dst, aux, true);
    } else {
        // dstHasSmaller: move right part first, then left part
        transfer(m+1, r, src, dst, aux, true);
        transfer(l, m-1, src, dst, aux, true);
    }
    // Move median from auxiliary to destination
    moves.push_back({aux, dst});
}

int main() {
    int N;
    cin >> N;
    vector<pair<int,int>> moves;
    if (N == 1) {
        moves.push_back({1, 3});
    } else {
        // Transfer balls 2..N from basket1 to basket2 using basket3 as auxiliary (basket2 empty)
        transfer(2, N, 1, 2, 3, false, moves);
        // Move ball 1 from basket1 to basket3
        moves.push_back({1, 3});
        // Transfer balls 2..N from basket2 to basket3 using basket1 as auxiliary (basket3 contains ball 1, so dstHasSmaller = true)
        transfer(2, N, 2, 3, 1, true, moves);
    }
    cout << moves.size() << "\n";
    for (auto &p : moves) {
        cout << p.first << " " << p.second << "\n";
    }
    return 0;
}