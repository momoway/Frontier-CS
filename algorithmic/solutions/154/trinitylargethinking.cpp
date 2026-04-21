#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Read initial data
    int N;
    cin >> N;
    vector<int> pet_x(N), pet_y(N), pet_type(N);
    for (int i = 0; i < N; i++) {
        cin >> pet_x[i] >> pet_y[i] >> pet_type[i];
    }
    int M;
    cin >> M;
    vector<int> human_x(M), human_y(M);
    for (int i = 0; i < M; i++) {
        cin >> human_x[i] >> human_y[i];
    }

    // Initialize grid: 0=passable, 1=impassable
    int grid[31][31] = {0};
    // inR: whether cell is inside the chosen rectangle
    bool inR[31][31] = {false};

    // Find largest pet-free rectangle
    int best_area = 0;
    int best_r1 = 1, best_c1 = 1, best_r2 = 30, best_c2 = 30;
    for (int r1 = 1; r1 <= 30; r1++) {
        for (int c1 = 1; c1 <= 30; c1++) {
            for (int r2 = r1; r2 <= 30; r2++) {
                for (int c2 = c1; c2 <= 30; c2++) {
                    bool has_pet = false;
                    for (int i = 0; i < N; i++) {
                        if (pet_x[i] >= r1 && pet_x[i] <= r2 && pet_y[i] >= c1 && pet_y[i] <= c2) {
                            has_pet = true;
                            break;
                        }
                    }
                    if (!has_pet) {
                        int area = (r2 - r1 + 1) * (c2 - c1 + 1);
                        if (area > best_area) {
                            best_area = area;
                            best_r1 = r1; best_c1 = c1; best_r2 = r2; best_c2 = c2;
                        }
                    }
                }
            }
        }
    }
    int r1 = best_r1, c1 = best_c1, r2 = best_r2, c2 = best_c2;
    // Mark inR
    for (int i = 1; i <= 30; i++) {
        for (int j = 1; j <= 30; j++) {
            inR[i][j] = (i >= r1 && i <= r2 && j >= c1 && j <= c2);
        }
    }

    // Compute wall cells (outside neighbors of rectangle)
    vector<pair<int,int>> wall_cells;
    for (int i = r1; i <= r2; i++) {
        for (int j = c1; j <= c2; j++) {
            int dx[4] = {-1,1,0,0};
            int dy[4] = {0,0,-1,1};
            for (int d = 0; d < 4; d++) {
                int nx = i + dx[d], ny = j + dy[d];
                if (nx >= 1 && nx <= 30 && ny >= 1 && ny <= 30 && !inR[nx][ny]) {
                    wall_cells.push_back({nx, ny});
                }
            }
        }
    }
    sort(wall_cells.begin(), wall_cells.end());
    wall_cells.erase(unique(wall_cells.begin(), wall_cells.end()), wall_cells.end());

    // Choose entrance (one of the wall cells, to be left unbuilt until end)
    pair<int,int> entrance;
    if (r1 > 1) entrance = {r1-1, c1};
    else if (r2 < 30) entrance = {r2+1, c1};
    else if (c1 > 1) entrance = {r1, c1-1};
    else entrance = {r1, c2+1};
    // Remove entrance from wall_cells
    wall_cells.erase(remove(wall_cells.begin(), wall_cells.end(), entrance), wall_cells.end());

    // Phase management
    enum Phase { MOVING_IN, BUILDING, SEALING, DONE } current_phase = MOVING_IN;
    int next_wall_index = 0; // index in wall_cells for next wall to build
    bool entrance_built = false;

    // Distance to rectangle (for moving in) - computed once
    int dist_to_R[31][31];
    {
        queue<pair<int,int>> q;
        for (int i = 1; i <= 30; i++) for (int j = 1; j <= 30; j++) dist_to_R[i][j] = -1;
        for (int i = r1; i <= r2; i++) {
            for (int j = c1; j <= c2; j++) {
                dist_to_R[i][j] = 0;
                q.push({i, j});
            }
        }
        while (!q.empty()) {
            auto [x,y] = q.front(); q.pop();
            int d = dist_to_R[x][y];
            int dx[4] = {-1,1,0,0};
            int dy[4] = {0,0,-1,1};
            for (int dir=0; dir<4; dir++) {
                int nx = x+dx[dir], ny = y+dy[dir];
                if (nx>=1 && nx<=30 && ny>=1 && ny<=30 && dist_to_R[nx][ny]==-1) {
                    dist_to_R[nx][ny] = d+1;
                    q.push({nx, ny});
                }
            }
        }
    }

    // Main loop for 300 turns
    for (int turn = 0; turn < 300; turn++) {
        // Precompute safe_walls for this turn (based on pet positions at start of turn)
        vector<bool> safe_walls(wall_cells.size(), false);
        for (int i = 0; i < wall_cells.size(); i++) {
            int wx = wall_cells[i].first, wy = wall_cells[i].second;
            bool safe = true;
            int dx[4] = {-1,1,0,0};
            int dy[4] = {0,0,-1,1};
            for (int d = 0; d < 4; d++) {
                int nx = wx + dx[d], ny = wy + dy[d];
                if (nx >= 1 && nx <= 30 && ny >= 1 && ny <= 30) {
                    for (int p = 0; p < N; p++) {
                        if (pet_x[p] == nx && pet_y[p] == ny) {
                            safe = false;
                            break;
                        }
                    }
                    if (!safe) break;
                }
            }
            safe_walls[i] = safe;
        }

        // Determine entrance safety if needed
        bool entrance_safe = true;
        if (current_phase == SEALING && !entrance_built) {
            int ex = entrance.first, ey = entrance.second;
            int dx[4] = {-1,1,0,0};
            int dy[4] = {0,0,-1,1};
            for (int d = 0; d < 4; d++) {
                int nx = ex + dx[d], ny = ey + dy[d];
                if (nx >= 1 && nx <= 30 && ny >= 1 && ny <= 30) {
                    for (int p = 0; p < N; p++) {
                        if (pet_x[p] == nx && pet_y[p] == ny) {
                            entrance_safe = false;
                            break;
                        }
                    }
                    if (!entrance_safe) break;
                }
            }
        }

        vector<char> actions(M, '.');
        vector<bool> human_assigned(M, false);
        vector<pair<int,int>> human_new_pos(M);

        if (current_phase == MOVING_IN) {
            // Move humans towards rectangle
            for (int i = 0; i < M; i++) {
                int hx = human_x[i], hy = human_y[i];
                if (inR[hx][hy]) {
                    actions[i] = '.';
                    human_assigned[i] = true;
                } else {
                    // Find a neighbor with smaller distance to R
                    int dx[4] = {-1,1,0,0};
                    int dy[4] = {0,0,-1,1};
                    char dir_char[4] = {'U','D','L','R'};
                    int best_dir = -1;
                    for (int d = 0; d < 4; d++) {
                        int nx = hx + dx[d], ny = hy + dy[d];
                        if (nx >= 1 && nx <= 30 && ny >= 1 && ny <= 30) {
                            if (dist_to_R[nx][ny] < dist_to_R[hx][hy]) {
                                best_dir = d;
                                break;
                            }
                        }
                    }
                    if (best_dir != -1) {
                        int nx = hx + dx[best_dir], ny = hy + dy[best_dir];
                        actions[i] = dir_char[best_dir];
                        human_new_pos[i] = {nx, ny};
                        human_assigned[i] = true;
                    } else {
                        actions[i] = '.';
                        human_assigned[i] = true;
                    }
                }
            }
        } else if (current_phase == BUILDING) {
            // Try to build walls in order, as long as safe
            while (next_wall_index < (int)wall_cells.size() && safe_walls[next_wall_index]) {
                // Find an unassigned human adjacent to this wall cell
                int found = -1;
                int wx = wall_cells[next_wall_index].first, wy = wall_cells[next_wall_index].second;
                for (int i = 0; i < M; i++) {
                    if (human_assigned[i]) continue;
                    int hx = human_x[i], hy = human_y[i];
                    if ((hx == wx-1 && hy == wy) || (hx == wx+1 && hy == wy) ||
                        (hx == wx && hy == wy-1) || (hx == wx && hy == wy+1)) {
                        found = i;
                        break;
                    }
                }
                if (found == -1) break; // no human adjacent, stop for now

                // Assign this human to build
                int hx = human_x[found], hy = human_y[found];
                char build_char;
                if (hx == wx-1 && hy == wy) build_char = 'd';
                else if (hx == wx+1 && hy == wy) build_char = 'u';
                else if (hx == wx && hy == wy-1) build_char = 'r';
                else if (hx == wx && hy == wy+1) build_char = 'l';
                else build_char = '.';
                actions[found] = build_char;
                human_assigned[found] = true;
                next_wall_index++;
            }

            // For remaining unassigned humans, move them to become adjacent to the current target (if any)
            pair<int,int> current_target;
            bool has_target = false;
            if (next_wall_index < (int)wall_cells.size()) {
                current_target = wall_cells[next_wall_index];
                has_target = true;
            }

            if (has_target) {
                // Find adj_cell: the cell in R adjacent to current_target
                int tx = current_target.first, ty = current_target.second;
                int adj_x = -1, adj_y = -1;
                if (tx-1 >= 1 && inR[tx-1][ty]) { adj_x = tx-1; adj_y = ty; }
                else if (tx+1 <= 30 && inR[tx+1][ty]) { adj_x = tx+1; adj_y = ty; }
                else if (ty-1 >= 1 && inR[tx][ty-1]) { adj_x = tx; adj_y = ty-1; }
                else if (ty+1 <= 30 && inR[tx][ty+1]) { adj_x = tx; adj_y = ty+1; }

                if (adj_x != -1) {
                    // Compute distances within R from adj_cell
                    int dist[31][31];
                    for (int i = 1; i <= 30; i++) for (int j = 1; j <= 30; j++) dist[i][j] = -1;
                    queue<pair<int,int>> q;
                    dist[adj_x][adj_y] = 0;
                    q.push({adj_x, adj_y});
                    while (!q.empty()) {
                        auto [x,y] = q.front(); q.pop();
                        int d = dist[x][y];
                        int dx[4] = {-1,1,0,0};
                        int dy[4] = {0,0,-1,1};
                        for (int dir=0; dir<4; dir++) {
                            int nx = x+dx[dir], ny = y+dy[dir];
                            if (nx>=1 && nx<=30 && ny>=1 && ny<=30 && inR[nx][ny] && dist[nx][ny]==-1) {
                                dist[nx][ny] = d+1;
                                q.push({nx, ny});
                            }
                        }
                    }
                    // Move unassigned humans towards adj_cell
                    for (int i = 0; i < M; i++) {
                        if (human_assigned[i]) continue;
                        int hx = human_x[i], hy = human_y[i];
                        if (!inR[hx][hy]) {
                            actions[i] = '.';
                            human_assigned[i] = true;
                            continue;
                        }
                        if (hx == adj_x && hy == adj_y) {
                            actions[i] = '.';
                            human_assigned[i] = true;
                            continue;
                        }
                        int best_dir = -1;
                        int dx[4] = {-1,1,0,0};
                        int dy[4] = {0,0,-1,1};
                        char dir_char[4] = {'U','D','L','R'};
                        for (int d = 0; d < 4; d++) {
                            int nx = hx + dx[d], ny = hy + dy[d];
                            if (nx>=1 && nx<=30 && ny>=1 && ny<=30 && inR[nx][ny]) {
                                if (dist[nx][ny] != -1 && dist[nx][ny] < dist[hx][hy]) {
                                    best_dir = d;
                                    break;
                                }
                            }
                        }
                        if (best_dir != -1) {
                            int nx = hx + dx[best_dir], ny = hy + dy[best_dir];
                            actions[i] = dir_char[best_dir];
                            human_new_pos[i] = {nx, ny};
                            human_assigned[i] = true;
                        } else {
                            actions[i] = '.';
                            human_assigned[i] = true;
                        }
                    }
                } else {
                    for (int i = 0; i < M; i++) {
                        if (!human_assigned[i]) {
                            actions[i] = '.';
                            human_assigned[i] = true;
                        }
                    }
                }
            } else {
                for (int i = 0; i < M; i++) {
                    if (!human_assigned[i]) actions[i] = '.';
                }
            }
        } else if (current_phase == SEALING) {
            if (!entrance_built && entrance_safe) {
                // Find an unassigned human adjacent to entrance
                int ex = entrance.first, ey = entrance.second;
                int found = -1;
                for (int i = 0; i < M; i++) {
                    if (human_assigned[i]) continue;
                    int hx = human_x[i], hy = human_y[i];
                    if ((hx == ex-1 && hy == ey) || (hx == ex+1 && hy == ey) ||
                        (hx == ex && hy == ey-1) || (hx == ex && hy == ey+1)) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    int hx = human_x[found], hy = human_y[found];
                    char build_char;
                    if (hx == ex-1 && hy == ey) build_char = 'd';
                    else if (hx == ex+1 && hy == ey) build_char = 'u';
                    else if (hx == ex && hy == ey-1) build_char = 'r';
                    else if (hx == ex && hy == ey+1) build_char = 'l';
                    else build_char = '.';
                    actions[found] = build_char;
                    human_assigned[found] = true;
                    entrance_built = true;
                }
            }
            // Others do nothing
            for (int i = 0; i < M; i++) {
                if (!human_assigned[i]) actions[i] = '.';
            }
        } else if (current_phase == DONE) {
            for (int i = 0; i < M; i++) actions[i] = '.';
        }

        // Output actions
        for (int i = 0; i < M; i++) {
            cout << actions[i];
        }
        cout << endl;
        cout.flush();

        // Read pet movements
        vector<string> pet_moves(N);
        for (int i = 0; i < N; i++) {
            cin >> pet_moves[i];
        }

        // Update pet positions
        for (int i = 0; i < N; i++) {
            int x = pet_x[i], y = pet_y[i];
            for (char c : pet_moves[i]) {
                if (c == 'U') x--;
                else if (c == 'D') x++;
                else if (c == 'L') y--;
                else if (c == 'R') y++;
            }
            pet_x[i] = x;
            pet_y[i] = y;
        }

        // Update human positions for moves
        for (int i = 0; i < M; i++) {
            if (actions[i] == 'U') human_x[i]--;
            else if (actions[i] == 'D') human_x[i]++;
            else if (actions[i] == 'L') human_y[i]--;
            else if (actions[i] == 'R') human_y[i]++;
        }

        // Update grid for build actions
        for (int i = 0; i < M; i++) {
            if (actions[i] == 'u' || actions[i] == 'd' || actions[i] == 'l' || actions[i] == 'r') {
                int hx = human_x[i], hy = human_y[i];
                int bx, by;
                if (actions[i] == 'u') { bx = hx-1; by = hy; }
                else if (actions[i] == 'd') { bx = hx+1; by = hy; }
                else if (actions[i] == 'l') { bx = hx; by = hy-1; }
                else if (actions[i] == 'r') { bx = hx; by = hy+1; }
                if (bx >= 1 && bx <= 30 && by >= 1 && by <= 30) {
                    grid[bx][by] = 1;
                }
            }
        }

        // Check if all humans are in R to switch phase
        if (current_phase == MOVING_IN) {
            bool all_in = true;
            for (int i = 0; i < M; i++) {
                if (!inR[human_x[i]][human_y[i]]) {
                    all_in = false;
                    break;
                }
            }
            if (all_in) {
                current_phase = BUILDING;
            }
        }

        // Switch to SEALING when all walls built
        if (current_phase == BUILDING && next_wall_index == (int)wall_cells.size()) {
            current_phase = SEALING;
        }

        // Switch to DONE when entrance built
        if (current_phase == SEALING && entrance_built) {
            current_phase = DONE;
        }
    }

    return 0;
}