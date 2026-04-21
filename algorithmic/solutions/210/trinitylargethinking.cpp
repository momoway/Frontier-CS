#include <bits/stdc++.h>
using namespace std;

struct Point { int x, y; };

struct BlueBase {
    int x, y;
    int fuel;
    int missiles;
};

struct RedBase {
    int x, y;
    int defense;
    int value;
    int damage = 0;
    bool destroyed = false;
};

struct Fighter {
    int id;
    int x, y;
    int G; // fuel capacity
    int C; // missile capacity
    int fuel = 0;
    int missiles = 0;
    enum State { IDLE, MOVING, ARRIVED } state = IDLE;
    char target_type = 0; // 'R' or 'B'
    int target_id = -1;
    vector<Point> path;
    int path_step = 0;
    char arrived_type = 0;
    int arrived_id = -1;
    int reload_for_red_id = -1;
};

const int dx[4] = {-1, 1, 0, 0}; // up, down, left, right
const int dy[4] = {0, 0, -1, 1};
const int INF = 1e9;

vector<vector<bool>> blocked_grid;
vector<BlueBase> blue_bases;
vector<RedBase> red_bases;
vector<Fighter> fighters;
int n_global, m_global;

struct BfsResult {
    vector<vector<int>> dist;
    vector<vector<Point>> parent;
};

BfsResult bfs(int start_x, int start_y) {
    int n = n_global, m = m_global;
    vector<vector<int>> dist(n, vector<int>(m, -1));
    vector<vector<Point>> parent(n, vector<Point>(m, Point{-1,-1}));
    queue<Point> q;
    dist[start_x][start_y] = 0;
    parent[start_x][start_y] = {start_x, start_y};
    q.push({start_x, start_y});
    while (!q.empty()) {
        auto cur = q.front(); q.pop();
        int x = cur.x, y = cur.y;
        for (int dir = 0; dir < 4; ++dir) {
            int nx = x + dx[dir], ny = y + dy[dir];
            if (nx < 0 || nx >= n || ny < 0 || ny >= m) continue;
            if (blocked_grid[nx][ny]) continue;
            if (dist[nx][ny] != -1) continue;
            dist[nx][ny] = dist[x][y] + 1;
            parent[nx][ny] = {x, y};
            q.push({nx, ny});
        }
    }
    return {dist, parent};
}

vector<Point> reconstruct_path(const vector<vector<Point>>& parent, Point start, Point goal) {
    vector<Point> path;
    Point cur = goal;
    while (!(cur.x == start.x && cur.y == start.y)) {
        path.push_back(cur);
        cur = parent[cur.x][cur.y];
        if (cur.x == -1) break;
    }
    path.push_back(start);
    reverse(path.begin(), path.end());
    return path;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m;
    if (!(cin >> n >> m)) return 0;
    n_global = n; m_global = m;
    vector<string> grid(n);
    for (int i = 0; i < n; ++i) {
        cin >> grid[i];
    }
    blocked_grid.assign(n, vector<bool>(m, false));
    vector<vector<int>> blue_base_index(n, vector<int>(m, -1));
    vector<vector<int>> red_base_index(n, vector<int>(m, -1));
    int N_blue;
    cin >> N_blue;
    blue_bases.resize(N_blue);
    for (int i = 0; i < N_blue; ++i) {
        cin >> blue_bases[i].x >> blue_bases[i].y;
        cin >> blue_bases[i].fuel >> blue_bases[i].missiles;
        int d, v; cin >> d >> v; // ignore
        blue_base_index[blue_bases[i].x][blue_bases[i].y] = i;
    }
    int N_red;
    cin >> N_red;
    red_bases.resize(N_red);
    for (int i = 0; i < N_red; ++i) {
        cin >> red_bases[i].x >> red_bases[i].y;
        cin >> red_bases[i].defense >> red_bases[i].value;
        int g, c; cin >> g >> c; // ignore
        red_base_index[red_bases[i].x][red_bases[i].y] = i;
        blocked_grid[red_bases[i].x][red_bases[i].y] = true;
    }
    int k;
    cin >> k;
    fighters.resize(k);
    for (int i = 0; i < k; ++i) {
        cin >> fighters[i].x >> fighters[i].y >> fighters[i].G >> fighters[i].C;
        fighters[i].id = i;
        int blue_idx = blue_base_index[fighters[i].x][fighters[i].y];
        if (blue_idx != -1) {
            fighters[i].home_base_id = blue_idx;
        }
    }

    // Simulation loop
    for (int frame = 0; frame < 15000; ++frame) {
        vector<string> commands;
        bool all_destroyed = true;
        for (auto &rb : red_bases) {
            if (!rb.destroyed) { all_destroyed = false; break; }
        }
        if (all_destroyed) break;

        for (int i = 0; i < k; ++i) {
            Fighter &f = fighters[i];
            if (f.state == Fighter::MOVING) {
                if (f.path.empty()) {
                    f.state = Fighter::IDLE;
                    continue;
                }
                Point next = f.path[f.path_step];
                int dir = -1;
                if (f.x == next.x) {
                    if (f.y < next.y) dir = 3;
                    else if (f.y > next.y) dir = 2;
                } else if (f.y == next.y) {
                    if (f.x < next.x) dir = 1;
                    else if (f.x > next.x) dir = 0;
                }
                if (dir == -1) {
                    f.state = Fighter::IDLE;
                    continue;
                }
                commands.push_back("move " + to_string(i) + " " + to_string(dir));
                f.x = next.x;
                f.y = next.y;
                f.fuel--;
                f.path_step++;
                if (f.path_step == (int)f.path.size()) {
                    f.state = Fighter::ARRIVED;
                    f.arrived_type = f.target_type;
                    f.arrived_id = f.target_id;
                    f.path.clear();
                    f.path_step = 0;
                }
            } else if (f.state == Fighter::ARRIVED) {
                if (f.arrived_type == 'R') {
                    RedBase &rb = red_bases[f.arrived_id];
                    if (rb.destroyed) {
                        f.state = Fighter::IDLE;
                        continue;
                    }
                    if (f.missiles > 0) {
                        int needed = rb.defense - rb.damage;
                        int count = min(f.missiles, needed);
                        int dir;
                        if (f.x == rb.x - 1 && f.y == rb.y) dir = 1;
                        else if (f.x == rb.x + 1 && f.y == rb.y) dir = 0;
                        else if (f.x == rb.x && f.y == rb.y - 1) dir = 3;
                        else if (f.x == rb.x && f.y == rb.y + 1) dir = 2;
                        else {
                            f.state = Fighter::IDLE;
                            continue;
                        }
                        commands.push_back("attack " + to_string(i) + " " + to_string(dir) + " " + to_string(count));
                        f.missiles -= count;
                        rb.damage += count;
                        if (rb.damage >= rb.defense) {
                            rb.destroyed = true;
                            blocked_grid[rb.x][rb.y] = false;
                            f.state = Fighter::IDLE;
                            f.reload_for_red_id = -1;
                        } else {
                            if (f.missiles == 0) {
                                f.state = Fighter::MOVING;
                                f.target_type = 'B';
                                BfsResult bfs_res = bfs(f.x, f.y);
                                int best_dist = INF;
                                int best_blue_id = -1;
                                Point best_pos;
                                for (int j = 0; j < (int)blue_bases.size(); ++j) {
                                    if (blue_bases[j].missiles > 0) {
                                        if (bfs_res.dist[blue_bases[j].x][blue_bases[j].y] != -1 && bfs_res.dist[blue_bases[j].x][blue_bases[j].y] <= f.fuel) {
                                            if (bfs_res.dist[blue_bases[j].x][blue_bases[j].y] < best_dist) {
                                                best_dist = bfs_res.dist[blue_bases[j].x][blue_bases[j].y];
                                                best_blue_id = j;
                                                best_pos = {blue_bases[j].x, blue_bases[j].y};
                                            }
                                        }
                                    }
                                }
                                if (best_blue_id != -1) {
                                    vector<Point> full_path = reconstruct_path(bfs_res.parent, {f.x, f.y}, best_pos);
                                    f.path = vector<Point>(full_path.begin()+1, full_path.end());
                                    f.path_step = 0;
                                    f.target_id = best_blue_id;
                                    f.reload_for_red_id = f.arrived_id;
                                } else {
                                    f.state = Fighter::IDLE;
                                    f.reload_for_red_id = -1;
                                }
                            } else {
                                // stay in ARRIVED to attack again next frame
                            }
                        }
                    } else {
                        // no missiles, need to reload
                        f.state = Fighter::MOVING;
                        f.target_type = 'B';
                        BfsResult bfs_res = bfs(f.x, f.y);
                        int best_dist = INF;
                        int best_blue_id = -1;
                        Point best_pos;
                        for (int j = 0; j < (int)blue_bases.size(); ++j) {
                            if (blue_bases[j].missiles > 0) {
                                if (bfs_res.dist[blue_bases[j].x][blue_bases[j].y] != -1 && bfs_res.dist[blue_bases[j].x][blue_bases[j].y] <= f.fuel) {
                                    if (bfs_res.dist[blue_bases[j].x][blue_bases[j].y] < best_dist) {
                                        best_dist = bfs_res.dist[blue_bases[j].x][blue_bases[j].y];
                                        best_blue_id = j;
                                        best_pos = {blue_bases[j].x, blue_bases[j].y};
                                    }
                                }
                            }
                        }
                        if (best_blue_id != -1) {
                            vector<Point> full_path = reconstruct_path(bfs_res.parent, {f.x, f.y}, best_pos);
                            f.path = vector<Point>(full_path.begin()+1, full_path.end());
                            f.path_step = 0;
                            f.target_id = best_blue_id;
                            f.reload_for_red_id = f.arrived_id;
                        } else {
                            f.state = Fighter::IDLE;
                            f.reload_for_red_id = -1;
                        }
                    }
                } else if (f.arrived_type == 'B') {
                    BlueBase &bb = blue_bases[f.arrived_id];
                    int fuel_take = min(f.G - f.fuel, bb.fuel);
                    int missile_take = min(f.C - f.missiles, bb.missiles);
                    if (fuel_take > 0) {
                        commands.push_back("fuel " + to_string(i) + " " + to_string(fuel_take));
                        f.fuel += fuel_take;
                        bb.fuel -= fuel_take;
                    }
                    if (missile_take > 0) {
                        commands.push_back("missile " + to_string(i) + " " + to_string(missile_take));
                        f.missiles += missile_take;
                        bb.missiles -= missile_take;
                    }
                    if (f.reload_for_red_id != -1) {
                        int red_id = f.reload_for_red_id;
                        f.target_type = 'R';
                        f.target_id = red_id;
                        BfsResult bfs_res = bfs(f.x, f.y);
                        RedBase &rb = red_bases[red_id];
                        int best_dist = INF;
                        Point best_adj;
                        for (int dir = 0; dir < 4; ++dir) {
                            int nx = rb.x + dx[dir], ny = rb.y + dy[dir];
                            if (nx < 0 || nx >= n || ny < 0 || ny >= m) continue;
                            if (blocked_grid[nx][ny]) continue;
                            if (bfs_res.dist[nx][ny] != -1) {
                                if (bfs_res.dist[nx][ny] < best_dist) {
                                    best_dist = bfs_res.dist[nx][ny];
                                    best_adj = {nx, ny};
                                }
                            }
                        }
                        if (best_dist != INF) {
                            vector<Point> full_path = reconstruct_path(bfs_res.parent, {f.x, f.y}, best_adj);
                            f.path = vector<Point>(full_path.begin()+1, full_path.end());
                            f.path_step = 0;
                            f.state = Fighter::MOVING;
                        } else {
                            f.state = Fighter::IDLE;
                        }
                        f.reload_for_red_id = -1;
                    } else {
                        f.state = Fighter::IDLE;
                    }
                }
            } else if (f.state == Fighter::IDLE) {
                int blue_idx = blue_base_index[f.x][f.y];
                if (blue_idx != -1) {
                    BlueBase &bb = blue_bases[blue_idx];
                    bool need_fuel = (f.fuel < f.G);
                    bool need_missile = (f.missiles < f.C);
                    if (need_fuel || need_missile) {
                        int fuel_take = min(f.G - f.fuel, bb.fuel);
                        int missile_take = min(f.C - f.missiles, bb.missiles);
                        if (fuel_take > 0) {
                            commands.push_back("fuel " + to_string(i) + " " + to_string(fuel_take));
                            f.fuel += fuel_take;
                            bb.fuel -= fuel_take;
                        }
                        if (missile_take > 0) {
                            commands.push_back("missile " + to_string(i) + " " + to_string(missile_take));
                            f.missiles += missile_take;
                            bb.missiles -= missile_take;
                        }
                        continue;
                    }
                }
                if (f.missiles > 0) {
                    bool attacked = false;
                    for (int dir = 0; dir < 4; ++dir) {
                        int nx = f.x + dx[dir], ny = f.y + dy[dir];
                        if (nx < 0 || nx >= n || ny < 0 || ny >= m) continue;
                        int rid = red_base_index[nx][ny];
                        if (rid != -1 && !red_bases[rid].destroyed) {
                            RedBase &rb = red_bases[rid];
                            int count = min(f.missiles, rb.defense - rb.damage);
                            if (count > 0) {
                                commands.push_back("attack " + to_string(i) + " " + to_string(dir) + " " + to_string(count));
                                f.missiles -= count;
                                rb.damage += count;
                                if (rb.damage >= rb.defense) {
                                    rb.destroyed = true;
                                    blocked_grid[rb.x][rb.y] = false;
                                }
                                attacked = true;
                                break;
                            }
                        }
                    }
                    if (attacked) continue;
                }
                if (f.fuel == 0) {
                    continue;
                }
                if (f.missiles == 0) {
                    BfsResult bfs_res = bfs(f.x, f.y);
                    int best_dist = INF;
                    int best_blue_id = -1;
                    Point best_pos;
                    for (int j = 0; j < (int)blue_bases.size(); ++j) {
                        if (blue_bases[j].missiles > 0) {
                            if (bfs_res.dist[blue_bases[j].x][blue_bases[j].y] != -1 && bfs_res.dist[blue_bases[j].x][blue_bases[j].y] <= f.fuel) {
                                if (bfs_res.dist[blue_bases[j].x][blue_bases[j].y] < best_dist) {
                                    best_dist = bfs_res.dist[blue_bases[j].x][blue_bases[j].y];
                                    best_blue_id = j;
                                    best_pos = {blue_bases[j].x, blue_bases[j].y};
                                }
                            }
                        }
                    }
                    if (best_blue_id != -1) {
                        vector<Point> full_path = reconstruct_path(bfs_res.parent, {f.x, f.y}, best_pos);
                        f.path = vector<Point>(full_path.begin()+1, full_path.end());
                        f.path_step = 0;
                        f.target_type = 'B';
                        f.target_id = best_blue_id;
                        f.state = Fighter::MOVING;
                    }
                    continue;
                }
                BfsResult bfs_res = bfs(f.x, f.y);
                int best_score = -1;
                int best_red_id = -1;
                Point best_adj;
                for (int r = 0; r < (int)red_bases.size(); ++r) {
                    if (red_bases[r].destroyed) continue;
                    if (red_bases[r].value <= 0) continue;
                    int best_dist = INF;
                    Point best_adj_r;
                    for (int dir = 0; dir < 4; ++dir) {
                        int nx = red_bases[r].x + dx[dir], ny = red_bases[r].y + dy[dir];
                        if (nx < 0 || nx >= n || ny < 0 || ny >= m) continue;
                        if (blocked_grid[nx][ny]) continue;
                        if (bfs_res.dist[nx][ny] != -1) {
                            if (bfs_res.dist[nx][ny] < best_dist) {
                                best_dist = bfs_res.dist[nx][ny];
                                best_adj_r = {nx, ny};
                            }
                        }
                    }
                    if (best_dist == INF) continue;
                    if (f.fuel < best_dist) continue;
                    int score = red_bases[r].value / (best_dist + 1);
                    if (score > best_score) {
                        best_score = score;
                        best_red_id = r;
                        best_adj = best_adj_r;
                    }
                }
                if (best_red_id != -1) {
                    f.target_type = 'R';
                    f.target_id = best_red_id;
                    vector<Point> full_path = reconstruct_path(bfs_res.parent, {f.x, f.y}, best_adj);
                    f.path = vector<Point>(full_path.begin()+1, full_path.end());
                    f.path_step = 0;
                    f.state = Fighter::MOVING;
                } else {
                    // No red base reachable; try to refuel fuel if not full
                    if (f.fuel < f.G) {
                        int best_dist = INF;
                        int best_blue_id = -1;
                        Point best_pos;
                        for (int j = 0; j < (int)blue_bases.size(); ++j) {
                            if (blue_bases[j].fuel > 0) {
                                if (bfs_res.dist[blue_bases[j].x][blue_bases[j].y] != -1 && bfs_res.dist[blue_bases[j].x][blue_bases[j].y] <= f.fuel) {
                                    if (bfs_res.dist[blue_bases[j].x][blue_bases[j].y] < best_dist) {
                                        best_dist = bfs_res.dist[blue_bases[j].x][blue_bases[j].y];
                                        best_blue_id = j;
                                        best_pos = {blue_bases[j].x, blue_bases[j].y};
                                    }
                                }
                            }
                        }
                        if (best_blue_id != -1) {
                            vector<Point> full_path = reconstruct_path(bfs_res.parent, {f.x, f.y}, best_pos);
                            f.path = vector<Point>(full_path.begin()+1, full_path.end());
                            f.path_step = 0;
                            f.target_type = 'B';
                            f.target_id = best_blue_id;
                            f.state = Fighter::MOVING;
                        }
                    }
                }
            }
        }
        for (const string &cmd : commands) {
            cout << cmd << "\n";
        }
        cout << "OK\n";
    }
    return 0;
}