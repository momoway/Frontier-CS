#include <bits/stdc++.h>
using namespace std;

int N;
vector<bool> present;
int cur_response;

void toggle(int x) {
    cout << "? " << x << endl;
    cout.flush();
    cin >> cur_response;
    present[x].flip();
}

void clear_device() {
    for (int i = 1; i <= 2*N; ++i) {
        if (present[i]) toggle(i);
    }
}

void set_device(const vector<int>& v) {
    // assumes device is empty
    for (int x : v) toggle(x);
}

vector<int> solve(const vector<int>& seg, vector<pair<int,int>>& pairs);

void match(vector<int> L, vector<int> R, vector<pair<int,int>>& pairs) {
    if (L.empty()) return;
    if (L.size() == 1) {
        pairs.emplace_back(L[0], R[0]);
        return;
    }
    int k = L.size();
    int k1 = k/2, k2 = k - k1;
    vector<int> R1(R.begin(), R.begin()+k1);
    vector<int> R2(R.begin()+k1, R.end());

    // Build S = R1
    clear_device();
    for (int x : R1) toggle(x);
    int baseline = cur_response;

    vector<int> L1, L2;
    for (int x : L) {
        toggle(x);
        if (cur_response == baseline) L1.push_back(x);
        else L2.push_back(x);
        toggle(x); // restore
    }

    // Recursively match L1 with R1
    match(L1, R1, pairs);
    // After match, device is empty

    // Build S = R2
    for (int x : R2) toggle(x);
    match(L2, R2, pairs);
    // device empty again
}

vector<int> solve(const vector<int>& seg, vector<pair<int,int>>& pairs) {
    int n = seg.size();
    if (n == 2) {
        // Build device with the two slices
        clear_device();
        for (int x : seg) toggle(x);
        int r = cur_response;
        vector<int> unpaired;
        if (r == 1) {
            pairs.emplace_back(seg[0], seg[1]);
        } else {
            unpaired = seg;
        }
        // clear device
        for (int x : seg) toggle(x);
        return unpaired;
    }

    // split seg into two halves
    int mid = n/2;
    vector<int> left(seg.begin(), seg.begin()+mid);
    vector<int> right(seg.begin()+mid, seg.end());

    // Process left
    clear_device();
    for (int x : left) toggle(x);
    vector<int> left_unpaired = solve(left, pairs);
    // after solve, device is empty

    // Process right
    for (int x : right) toggle(x);
    vector<int> right_unpaired = solve(right, pairs);
    // after solve, device is empty

    // Match unpaired from left and right
    match(left_unpaired, right_unpaired, pairs);
    return vector<int>(); // all paired
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> N;
    present.assign(2*N+1, false);
    cur_response = 0;

    vector<int> all(2*N);
    for (int i = 0; i < 2*N; ++i) all[i] = i+1;

    vector<pair<int,int>> pairs;
    solve(all, pairs);

    // Output all pairs
    for (auto &p : pairs) {
        cout << "! " << p.first << " " << p.second << endl;
        cout.flush();
    }
    return 0;
}