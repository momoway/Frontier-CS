#include <bits/stdc++.h>
using namespace std;

int main() {
    const int N = 50, M = 50;
    vector<string> grid(N, string(M, '#'));

    // Define the super cell grid: top-left corners from (1,1) to (48,48)
    int top = 1, bottom = 48, left = 1, right = 48;

    vector<pair<int,int>> path;
    while (top < bottom && left < right) {
        // Top row: left -> right
        for (int c = left; c <= right; c++) {
            path.push_back({top, c});
        }
        // Right column: top+1 -> bottom
        for (int r = top+1; r <= bottom; r++) {
            path.push_back({r, right});
        }
        if (top+1 <= bottom) {
            // Bottom row: right-1 -> left
            for (int c = right-1; c >= left; c--) {
                path.push_back({bottom, c});
            }
        }
        if (left+1 <= right) {
            // Left column: bottom-1 -> top+1
            for (int r = bottom-1; r >= top+1; r--) {
                path.push_back({r, left});
            }
        }
        // Move to next inner layer
        top++; left++; right--; bottom--;
    }

    // Mark all positions in the path as free (2x2 blocks)
    for (auto &p : path) {
        int r = p.first, c = p.second;
        // The 2x2 block must be within grid
        if (r >= 0 && r+1 < N && c >= 0 && c+1 < M) {
            grid[r][c] = '.';
            grid[r+1][c] = '.';
            grid[r][c+1] = '.';
            grid[r+1][c+1] = '.';
        }
    }

    // Mark player behind the first box (first move is to the right)
    // First box position is path[0] = (1,1). Behind is left: (1,0)
    grid[1][0] = 'P';

    // Mark storage at the last box position
    auto last = path.back();
    int r = last.first, c = last.second;
    grid[r][c] = 'S';
    grid[r+1][c] = 'S';
    grid[r][c+1] = 'S';
    grid[r+1][c+1] = 'S';

    // Mark extra cells at corners to allow turning
    // We need to add cells so the player can get to the pushing side at each turn.
    // Since the path is a simple spiral, we add the necessary cells manually.
    // For a right-to-down turn at the top-right corner, we need a cell above the corner.
    // For a down-to-left turn at the bottom-right corner, we need a cell to the right.
    // For a left-to-up turn at the bottom-left corner, we need a cell below.
    // For an up-to-right turn at the top-left corner, we need a cell to the left.
    // We'll add these cells at the appropriate locations.

    // The corners occur at the following indices in the path (for the standard spiral):
    // After top row: index = (right-left+1) - 1
    // After right column: index = (right-left+1) + (bottom - (top+1) + 1) - 1
    // After bottom row: similar
    // We'll compute and add cells.

    // Since the path is generated in order, we can simply add cells around the four corners of each rectangle.
    // We'll add a 3x3 block at each corner? Actually a 2x2 block might suffice, but to be safe we add a few cells.
    // For simplicity, we'll add the cells that are adjacent to the corner in the direction needed.

    // We'll iterate through the path and detect direction changes.
    vector<tuple<int,int,int>> extras; // (r,c, type) but we just add cells.

    for (size_t i = 0; i+1 < path.size(); i++) {
        int r1 = path[i].first, c1 = path[i].second;
        int r2 = path[i+1].first, c2 = path[i+1].second;
        int dr = r2 - r1, dc = c2 - c1;
        // If this is a corner (direction changes)
        if (i > 0) {
            int pr = path[i-1].first, pc = path[i-1].second;
            int pdr = r1 - pr, pdc = c1 - pc;
            if (pdr != dr || pdc != dc) {
                // Corner at (r1,c1)
                // Determine needed extra cells based on incoming and outgoing directions.
                if (pdr == 0 && pdc > 0) { // came from left (moving right)
                    if (dr < 0) { // turning up
                        // Need cell above: (r1-1, c1) and maybe (r1-1, c1+1)
                        extras.push_back({r1-1, c1, 0});
                        extras.push_back({r1-1, c1+1, 0});
                    } else if (dr > 0) { // turning down
                        // Need cell below: (r1+1, c1) and maybe (r1+1, c1+1)
                        extras.push_back({r1+1, c1, 0});
                        extras.push_back({r1+1, c1+1, 0});
                    }
                } else if (pdr > 0 && pdc == 0) { // came from top (moving down)
                    if (dc < 0) { // turning left
                        // Need cell left: (r1, c1-1) and maybe (r1+1, c1-1)
                        extras.push_back({r1, c1-1, 0});
                        extras.push_back({r1+1, c1-1, 0});
                    } else if (dc > 0) { // turning right
                        // Need cell right: (r1, c1+1) and maybe (r1+1, c1+1)
                        extras.push_back({r1, c1+1, 0});
                        extras.push_back({r1+1, c1+1, 0});
                    }
                } else if (pdr == 0 && pdc < 0) { // came from right (moving left)
                    if (dr < 0) { // turning up
                        // Need cell above: (r1-1, c1) and maybe (r1-1, c1-1)
                        extras.push_back({r1-1, c1, 0});
                        extras.push_back({r1-1, c1-1, 0});
                    } else if (dr > 0) { // turning down
                        // Need cell below: (r1+1, c1) and maybe (r1+1, c1-1)
                        extras.push_back({r1+1, c1, 0});
                        extras.push_back({r1+1, c1-1, 0});
                    }
                } else if (pdr < 0 && pdc == 0) { // came from bottom (moving up)
                    if (dc < 0) { // turning left
                        // Need cell left: (r1, c1-1) and maybe (r1-1, c1-1)
                        extras.push_back({r1, c1-1, 0});
                        extras.push_back({r1-1, c1-1, 0});
                    } else if (dc > 0) { // turning right
                        // Need cell right: (r1, c1+1) and maybe (r1-1, c1+1)
                        extras.push_back({r1, c1+1, 0});
                        extras.push_back({r1-1, c1+1, 0});
                    }
                }
            }
        }
    }

    // Add extra cells (ensure they are within grid and not already free)
    for (auto &e : extras) {
        int r = get<0>(e), c = get<1>(e);
        if (r >= 0 && r < N && c >= 0 && c < M) {
            grid[r][c] = '.';
        }
    }

    // Output the grid
    cout << N << " " << M << "\n";
    for (int i = 0; i < N; i++) {
        cout << grid[i] << "\n";
    }

    return 0;
}