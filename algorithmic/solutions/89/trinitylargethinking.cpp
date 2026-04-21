#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;

    if (n == 1) {
        cout << "!\n";
        cout.flush();
        return 0;
    }

    vector<vector<char>> onPath(n+1, vector<char>(n+1, 0));

    // For each v (2..n), ask for all u (1..n, u != v) whether u is on the path 1-v
    for (int v = 2; v <= n; ++v) {
        for (int u = 1; u <= n; ++u) {
            if (u == v) continue;
            cout << "? 2 " << u << " 1 " << v << "\n";
            cout << flush;
            int ans;
            cin >> ans;
            if (ans == -1) {
                // Error, exit
                return 0;
            }
            if (ans == 1) {
                onPath[u][v] = 1;
            }
        }
    }

    vector<int> dist(n+1, 0);
    vector<int> parent(n+1, 0);
    dist[1] = 0;
    for (int v = 2; v <= n; ++v) {
        int cnt = 0;
        for (int u = 1; u <= n; ++u) {
            if (u == v) continue;
            if (onPath[u][v]) ++cnt;
        }
        dist[v] = cnt;
    }

    for (int v = 2; v <= n; ++v) {
        for (int u = 1; u <= n; ++u) {
            if (u == v) continue;
            if (onPath[u][v] && dist[u] == dist[v] - 1) {
                parent[v] = u;
                break;
            }
        }
    }

    cout << "!\n";
    for (int v = 2; v <= n; ++v) {
        cout << parent[v] << " " << v << "\n";
    }
    cout << flush;

    return 0;
}