#include <bits/stdc++.h>
using namespace std;

struct DLX {
    struct Node {
        int up, down, left, right, col, row;
        Node(): up(0), down(0), left(0), right(0), col(0), row(0) {}
    };
    int nCol;
    vector<Node> nodes;
    vector<int> colSize;
    vector<int> solutionRows;
    int root;

    DLX(int numCols) {
        nCol = numCols;
        nodes.resize(nCol + 1);
        root = 0;
        nodes[root].right = 1;
        nodes[root].left = nCol;
        for (int i = 1; i <= nCol; ++i) {
            nodes[i].col = i;
            nodes[i].row = -1;
            nodes[i].up = i;
            nodes[i].down = i;
            nodes[i].left = i - 1;
            nodes[i].right = (i == nCol) ? root : i + 1;
        }
        nodes[root].left = nCol;
        colSize.assign(nCol + 1, 0);
        solutionRows.clear();
    }

    void addRow(const vector<int>& cols, int rowId) {
        int first = nodes.size();
        nodes.resize(first + (int)cols.size());
        for (int j = 0; j < (int)cols.size(); ++j) {
            int colIdx = cols[j] + 1;
            Node& node = nodes[first + j];
            node.col = colIdx;
            node.row = rowId;
            int up = nodes[colIdx].up;
            int down = colIdx;
            nodes[colIdx].up = first + j;
            nodes[up].down = first + j;
            nodes[first + j].up = up;
            nodes[first + j].down = down;
            colSize[colIdx]++;
        }
        for (int j = 0; j < (int)cols.size(); ++j) {
            int cur = first + j;
            int prev = first + (j == 0 ? (int)cols.size() - 1 : j - 1);
            int next = first + (j == (int)cols.size() - 1 ? 0 : j + 1);
            nodes[cur].left = prev;
            nodes[cur].right = next;
            nodes[prev].right = cur;
            nodes[next].left = cur;
        }
    }

    void cover(int col) {
        nodes[nodes[col].right].left = nodes[col].left;
        nodes[nodes[col].left].right = nodes[col].right;
        for (int i = nodes[col].down; i != col; i = nodes[i].down) {
            for (int j = nodes[i].right; j != i; j = nodes[j].right) {
                nodes[nodes[j].down].up = nodes[j].up;
                nodes[nodes[j].up].down = nodes[j].down;
                colSize[nodes[j].col]--;
            }
        }
    }

    void uncover(int col) {
        for (int i = nodes[col].up; i != col; i = nodes[i].up) {
            for (int j = nodes[i].left; j != i; j = nodes[j].left) {
                colSize[nodes[j].col]++;
                nodes[nodes[j].down].up = j;
                nodes[nodes[j].up].down = j;
            }
        }
        nodes[nodes[col].left].right = col;
        nodes[nodes[col].right].left = col;
    }

    bool solve() {
        if (nodes[root].right == root) return true;
        int c = -1;
        int minSize = INT_MAX;
        for (int j = nodes[root].right; j != root; j = nodes[j].right) {
            if (colSize[j] < minSize) {
                minSize = colSize[j];
                c = j;
            }
        }
        if (minSize == 0) return false;
        cover(c);
        for (int r = nodes[c].down; r != c; r = nodes[r].down) {
            solutionRows.push_back(nodes[r].row);
            for (int j = nodes[r].right; j != r; j = nodes[j].right) {
                cover(nodes[j].col);
            }
            if (solve()) return true;
            solutionRows.pop_back();
            for (int j = nodes[r].left; j != r; j = nodes[j].left) {
                uncover(nodes[j].col);
            }
        }
        uncover(c);
        return false;
    }

    const vector<int>& getSolution() const {
        return solutionRows;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int b, k, w;
    if (!(cin >> b >> k >> w)) return 0;
    vector<pair<int,int>> corners = {{b,b}, {b,-b}, {-b,-b}, {-b,b}};
    int m = min(w, 4);
    vector<vector<int>> lists(m, vector<int>(k));
    for (int i = 0; i < m; ++i) {
        int s = corners[i].first;
        int t = corners[i].second;
        cout << "? 1 " << s << " " << t << "\n";
        cout.flush();
        for (int j = 0; j < k; ++j) {
            cin >> lists[i][j];
        }
    }

    vector<pair<int,int>> points;

    if (m == 4) {
        unordered_map<int, vector<int>> mapIdx2, mapIdx3;
        for (int i = 0; i < k; ++i) {
            mapIdx2[lists[2][i]].push_back(i);
            mapIdx3[lists[3][i]].push_back(i);
        }
        int cols = 4 * k;
        DLX dlx(cols);
        struct RowInfo { int i0,i1,i2,i3,x,y; };
        vector<RowInfo> rowInfos;
        int rowId = 0;
        for (int i0 = 0; i0 < k; ++i0) {
            int d0 = lists[0][i0];
            for (int i1 = 0; i1 < k; ++i1) {
                int d1 = lists[1][i1];
                int needed_c = 4*b - d0;
                int needed_d = 4*b - d1;
                if ((d1 - d0) % 2 != 0) continue;
                int x = (needed_d - d0) / 2;
                int y = (d1 - d0) / 2;
                if (x < -b || x > b || y < -b || y > b) continue;
                auto it_c = mapIdx2.find(needed_c);
                auto it_d = mapIdx3.find(needed_d);
                if (it_c != mapIdx2.end() && it_d != mapIdx3.end()) {
                    for (int i2 : it_c->second) {
                        for (int i3 : it_d->second) {
                            vector<int> colsRow = {i0, k+i1, 2*k+i2, 3*k+i3};
                            dlx.addRow(colsRow, rowId);
                            rowInfos.push_back({i0,i1,i2,i3,x,y});
                            rowId++;
                        }
                    }
                }
            }
        }
        if (dlx.solve()) {
            const vector<int>& sol = dlx.getSolution();
            for (int rid : sol) {
                points.push_back({rowInfos[rid].x, rowInfos[rid].y});
            }
        } else {
            for (int i = 0; i < k; ++i) points.emplace_back(0,0);
        }
    } else if (m == 3) {
        unordered_map<int, vector<int>> mapIdx2;
        for (int i = 0; i < k; ++i) {
            mapIdx2[lists[2][i]].push_back(i);
        }
        int cols = 3 * k;
        DLX dlx(cols);
        struct RowInfo { int i0,i1,i2,x,y; };
        vector<RowInfo> rowInfos;
        int rowId = 0;
        for (int i0 = 0; i0 < k; ++i0) {
            int a = lists[0][i0];
            for (int i1 = 0; i1 < k; ++i1) {
                int bval = lists[1][i1];
                int needed_c = 4*b - a;
                if ((bval - a) % 2 != 0) continue;
                int y = (bval - a) / 2;
                int x = 2*b - (a + bval) / 2;
                if (x < -b || x > b || y < -b || y > b) continue;
                auto it_c = mapIdx2.find(needed_c);
                if (it_c != mapIdx2.end()) {
                    for (int i2 : it_c->second) {
                        vector<int> colsRow = {i0, k+i1, 2*k+i2};
                        dlx.addRow(colsRow, rowId);
                        rowInfos.push_back({i0,i1,i2,x,y});
                        rowId++;
                    }
                }
            }
        }
        if (dlx.solve()) {
            const vector<int>& sol = dlx.getSolution();
            for (int rid : sol) {
                points.push_back({rowInfos[rid].x, rowInfos[rid].y});
            }
        } else {
            for (int i = 0; i < k; ++i) points.emplace_back(0,0);
        }
    } else if (m == 2) {
        vector<vector<int>> adj(k);
        for (int i0 = 0; i0 < k; ++i0) {
            int a = lists[0][i0];
            for (int i1 = 0; i1 < k; ++i1) {
                int bval = lists[1][i1];
                if ((bval - a) % 2 != 0) continue;
                int y = (bval - a) / 2;
                int x = 2*b - (a + bval) / 2;
                if (x < -b || x > b || y < -b || y > b) continue;
                adj[i0].push_back(i1);
            }
        }
        vector<int> matchR(k, -1), matchL(k, -1);
        function<bool(int)> dfs = [&](int u) {
            for (int v : adj[u]) {
                if (matchR[v] == -1 || dfs(matchR[v])) {
                    matchR[v] = u;
                    matchL[u] = v;
                    return true;
                }
            }
            return false;
        };
        int matched = 0;
        for (int u = 0; u < k; ++u) {
            fill(matchR.begin(), matchR.end(), -1);
            if (dfs(u)) matched++;
        }
        if (matched == k) {
            for (int u = 0; u < k; ++u) {
                int i1 = matchL[u];
                int a = lists[0][u];
                int bval = lists[1][i1];
                int y = (bval - a) / 2;
                int x = 2*b - (a + bval) / 2;
                points.push_back({x, y});
            }
        } else {
            for (int i = 0; i < k; ++i) points.emplace_back(0,0);
        }
    } else {
        for (int i = 0; i < k; ++i) points.emplace_back(0,0);
    }

    cout << "! ";
    for (int i = 0; i < k; ++i) {
        cout << points[i].first << " " << points[i].second;
        if (i < k-1) cout << " ";
    }
    cout << endl;
    cout.flush();
    return 0;
}