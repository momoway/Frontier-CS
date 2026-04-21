#include <bits/stdc++.h>
using namespace std;

struct Orientation {
    vector<pair<int,int>> cells; // sorted for deduplication, order doesn't matter for placement
    int width;
    int height;
    int R;
    int F;
};

struct Shape {
    int id;
    int k;
    int min_area;
    vector<pair<int,int>> orig_cells;
    vector<Orientation> orientations;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    if (!(cin >> n)) return 0;
    vector<Shape> shapes(n);
    for (int i = 0; i < n; ++i) {
        int k;
        cin >> k;
        shapes[i].id = i;
        shapes[i].k = k;
        shapes[i].orig_cells.resize(k);
        for (int j = 0; j < k; ++j) {
            int x, y;
            cin >> x >> y;
            shapes[i].orig_cells[j] = {x, y};
        }
    }

    // Precompute distinct orientations for each shape
    for (auto &shape : shapes) {
        for (int R = 0; R < 4; ++R) {
            for (int F = 0; F < 2; ++F) {
                vector<pair<int,int>> trans;
                for (auto &p : shape.orig_cells) {
                    int x = p.first;
                    int y = p.second;
                    if (F) x = -x; // reflect across y-axis
                    int nx, ny;
                    switch (R) {
                        case 0: nx = x; ny = y; break;
                        case 1: nx = y; ny = -x; break;
                        case 2: nx = -x; ny = -y; break;
                        case 3: nx = -y; ny = x; break;
                    }
                    trans.emplace_back(nx, ny);
                }
                // compute bounding box from trans
                int minx = INT_MAX, miny = INT_MAX;
                int maxx = INT_MIN, maxy = INT_MIN;
                for (auto &p : trans) {
                    minx = min(minx, p.first);
                    miny = min(miny, p.second);
                    maxx = max(maxx, p.first);
                    maxy = max(maxy, p.second);
                }
                int width = maxx - minx + 1;
                int height = maxy - miny + 1;
                // normalize
                vector<pair<int,int>> norm;
                norm.reserve(trans.size());
                for (auto &p : trans) {
                    norm.emplace_back(p.first - minx, p.second - miny);
                }
                // sort for deduplication
                vector<pair<int,int>> sorted = norm;
                sort(sorted.begin(), sorted.end());
                // check duplicate
                bool duplicate = false;
                for (const auto &existing : shape.orientations) {
                    if (existing.cells == sorted) {
                        duplicate = true;
                        break;
                    }
                }
                if (!duplicate) {
                    Orientation o;
                    o.cells = sorted; // store sorted cells
                    o.width = width;
                    o.height = height;
                    o.R = R;
                    o.F = F;
                    shape.orientations.push_back(o);
                }
            }
        }
        // compute minimal bounding box area for this shape
        shape.min_area = INT_MAX;
        for (const auto &o : shape.orientations) {
            int area = o.width * o.height;
            if (area < shape.min_area) shape.min_area = area;
        }
    }

    // Sort shapes by minimal bounding box area descending, then by number of cells descending
    sort(shapes.begin(), shapes.end(), [](const Shape &a, const Shape &b) {
        if (a.min_area != b.min_area) return a.min_area > b.min_area;
        return a.k > b.k;
    });

    vector<int> col_height; // col_height[x] = next free row (max y+1 of placed cells in column x)
    int W = 0, H = 0;
    vector<Placement> placements(n);

    for (const auto &shape : shapes) {
        long long best_area = LLONG_MAX;
        int best_tx = -1, best_ty = -1;
        int best_orient_idx = -1;
        int best_newW = -1, best_newH = -1;
        for (int oi = 0; oi < (int)shape.orientations.size(); ++oi) {
            const auto &o = shape.orientations[oi];
            int w = o.width;
            int h = o.height;
            // try translations tx from 0 to W (inclusive)
            for (int tx = 0; tx <= W; ++tx) {
                // compute required ty
                int max_val = INT_MIN;
                for (const auto &cell : o.cells) {
                    int nx = cell.first;
                    int ny = cell.second;
                    int idx = tx + nx;
                    int ch = (idx < (int)col_height.size()) ? col_height[idx] : 0;
                    int val = ch - ny;
                    if (val > max_val) max_val = val;
                }
                int ty = max(0, max_val);
                // compute new dimensions if placed at (tx, ty)
                int newW = W;
                int newH = H;
                for (const auto &cell : o.cells) {
                    int nx = cell.first;
                    int ny = cell.second;
                    int x = tx + nx;
                    int y = ty + ny;
                    if (x + 1 > newW) newW = x + 1;
                    if (y + 1 > newH) newH = y + 1;
                }
                long long area = 1LL * newW * newH;
                if (area < best_area || (area == best_area && newH < best_newH) ||
                    (area == best_area && newH == best_newH && newW < best_newW)) {
                    best_area = area;
                    best_tx = tx;
                    best_ty = ty;
                    best_orient_idx = oi;
                    best_newW = newW;
                    best_newH = newH;
                }
            }
        }
        // place the shape
        const auto &best_o = shape.orientations[best_orient_idx];
        for (const auto &cell : best_o.cells) {
            int nx = cell.first;
            int ny = cell.second;
            int x = best_tx + nx;
            int y = best_ty + ny;
            if (x >= (int)col_height.size()) col_height.resize(x + 1, 0);
            if (y + 1 > col_height[x]) col_height[x] = y + 1;
            if (x + 1 > W) W = x + 1;
            if (y + 1 > H) H = y + 1;
        }
        placements[shape.id] = {best_tx, best_ty, best_o.R, best_o.F};
    }

    // Output
    cout << W << " " << H << "\n";
    for (int i = 0; i < n; ++i) {
        const auto &p = placements[i];
        cout << p.X << " " << p.Y << " " << p.R << " " << p.F << "\n";
    }
    return 0;
}