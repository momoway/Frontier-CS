#include <bits/stdc++.h>
using namespace std;

int n, total;
vector<vector<int>> children;
vector<bool> selected;

int race(const vector<int>& v) {
    cout << "?";
    for (int i = 0; i < (int)v.size(); ++i) {
        if (i) cout << " ";
        cout << v[i];
    }
    cout << endl;
    cout.flush();
    int winner;
    cin >> winner;
    return winner;
}

int findMax(vector<int> cand) {
    int m = cand.size();
    if (m == 0) return -1;
    if (m == 1) return cand[0];
    int current = -1;
    vector<int> inactive_list;
    vector<bool> in_inactive(total + 1, false);
    int idx = 0;
    while (true) {
        if (current == -1) {
            vector<int> batch;
            for (int i = 0; i < n; ++i) {
                batch.push_back(cand[idx++]);
            }
            int w = race(batch);
            current = w;
            for (int x : batch) {
                if (x != w && !in_inactive[x]) {
                    inactive_list.push_back(x);
                    in_inactive[x] = true;
                }
            }
        } else {
            int remaining = (int)cand.size() - idx;
            if (remaining == 0) break;
            int take = min(n - 1, remaining);
            vector<int> batch_new;
            for (int i = 0; i < take; ++i) {
                batch_new.push_back(cand[idx++]);
            }
            int filler_needed = (n - 1) - take;
            vector<int> filler;
            for (int i = 0; i < filler_needed; ++i) {
                filler.push_back(inactive_list[i]);
            }
            vector<int> batch;
            batch.push_back(current);
            for (int x : batch_new) batch.push_back(x);
            for (int x : filler) batch.push_back(x);
            int w = race(batch);
            current = w;
            for (int x : batch) {
                if (x == w) continue;
                if (!in_inactive[x]) {
                    inactive_list.push_back(x);
                    in_inactive[x] = true;
                }
            }
        }
    }
    return current;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int t;
    cin >> t;
    while (t--) {
        cin >> n;
        total = n * n;
        children.assign(total + 1, vector<int>());
        selected.assign(total + 1, false);
        vector<vector<int>> groups(n);
        for (int i = 0; i < total; ++i) {
            groups[i % n].push_back(i + 1);
        }
        vector<int> group_winners;
        for (int i = 0; i < n; ++i) {
            int w = race(groups[i]);
            group_winners.push_back(w);
            for (int pepe : groups[i]) {
                if (pepe != w) children[w].push_back(pepe);
            }
        }
        int W = race(group_winners);
        for (int w : group_winners) {
            if (w != W) children[W].push_back(w);
        }
        vector<int> result;
        result.push_back(W);
        selected[W] = true;
        vector<int> candidates = children[W];
        int k = total - n + 1;
        while ((int)result.size() < k) {
            int m = findMax(candidates);
            vector<int> new_candidates;
            for (int x : candidates) if (x != m) new_candidates.push_back(x);
            for (int child : children[m]) {
                if (!selected[child]) new_candidates.push_back(child);
            }
            candidates = move(new_candidates);
            selected[m] = true;
            result.push_back(m);
        }
        cout << "!";
        for (int i = 0; i < (int)result.size(); ++i) {
            cout << " " << result[i];
        }
        cout << endl;
        cout.flush();
    }
    return 0;
}