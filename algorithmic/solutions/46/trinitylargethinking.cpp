#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int J, M;
    if (!(cin >> J >> M)) return 0;
    vector<vector<int>> route_machines(J, vector<int>(M));
    vector<vector<int>> route_proc(J, vector<int>(M));
    for (int j = 0; j < J; ++j) {
        for (int k = 0; k < M; ++k) {
            cin >> route_machines[j][k] >> route_proc[j][k];
        }
    }

    // Processing time matrix: proc_time[j][m]
    vector<vector<int>> proc_time(J, vector<int>(M, 0));
    for (int j = 0; j < J; ++j) {
        for (int k = 0; k < M; ++k) {
            int m = route_machines[j][k];
            proc_time[j][m] = route_proc[j][k];
        }
    }

    // Precompute predecessor in job chain for each operation node (id = j*M + m)
    int V = J * M;
    vector<int> pred_job(V, -1);
    for (int j = 0; j < J; ++j) {
        for (int k = 0; k < M; ++k) {
            int m = route_machines[j][k];
            int id = j * M + m;
            if (k > 0) {
                int prev_m = route_machines[j][k - 1];
                pred_job[id] = j * M + prev_m;
            } else {
                pred_job[id] = -1;
            }
        }
    }

    // Precompute processing time for each node
    vector<long long> proc(V);
    for (int j = 0; j < J; ++j) {
        for (int m = 0; m < M; ++m) {
            int id = j * M + m;
            proc[id] = proc_time[j][m];
        }
    }

    // Giffler-Thompson algorithm with SPT priority rule
    vector<int> job_progress(J, 0);
    vector<long long> machine_free_time(M, 0);
    vector<long long> job_ready_time(J, 0);
    vector<vector<int>> machine_orders(M);

    int total_ops = J * M;
    int ops_scheduled = 0;
    while (ops_scheduled < total_ops) {
        struct Candidate {
            long long priority; // processing time (smaller is better)
            int job;
            int machine;
            int proc;
        };
        vector<Candidate> candidates;
        candidates.reserve(J);
        for (int j = 0; j < J; ++j) {
            if (job_progress[j] < M) {
                int k = job_progress[j];
                int m = route_machines[j][k];
                int p = route_proc[j][k];
                candidates.push_back({(long long)p, j, m, p});
            }
        }
        if (candidates.empty()) break; // should not happen
        sort(candidates.begin(), candidates.end(), [](const Candidate& a, const Candidate& b) {
            if (a.proc != b.proc) return a.proc < b.proc;
            return a.job < b.job;
        });
        const Candidate& best = candidates[0];
        int j = best.job;
        int m = best.machine;
        int p = best.proc;
        long long start = max(machine_free_time[m], job_ready_time[j]);
        long long finish = start + p;
        machine_free_time[m] = finish;
        job_ready_time[j] = finish;
        job_progress[j]++;
        ops_scheduled++;
        machine_orders[m].push_back(j);
    }

    // Evaluation function: compute makespan for given machine orders, return -1 if infeasible
    auto evaluate = [&](const vector<vector<int>>& orders) -> long long {
        int V = J * M;
        vector<int> indegree(V, 0);
        vector<vector<int>> adj(V);
        vector<int> pred_machine(V, -1);
        // Build machine predecessor from orders
        for (int m = 0; m < M; ++m) {
            const auto& ord = orders[m];
            for (int i = 0; i < (int)ord.size(); ++i) {
                int job = ord[i];
                int id = job * M + m;
                if (i > 0) {
                    int prev_job = ord[i - 1];
                    pred_machine[id] = prev_job * M + m;
                } else {
                    pred_machine[id] = -1;
                }
            }
        }
        // Build graph edges
        for (int id = 0; id < V; ++id) {
            if (pred_job[id] != -1) {
                int u = pred_job[id];
                adj[u].push_back(id);
                indegree[id]++;
            }
            if (pred_machine[id] != -1) {
                int u = pred_machine[id];
                adj[u].push_back(id);
                indegree[id]++;
            }
        }
        // Topological sort to compute earliest finish times
        vector<long long> earliest_start(V, 0);
        vector<long long> finish(V, 0);
        queue<int> q;
        for (int id = 0; id < V; ++id) {
            if (indegree[id] == 0) {
                q.push(id);
                finish[id] = proc[id]; // start at time 0
            }
        }
        int processed = 0;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            processed++;
            for (int v : adj[u]) {
                if (earliest_start[v] < finish[u]) {
                    earliest_start[v] = finish[u];
                }
                indegree[v]--;
                if (indegree[v] == 0) {
                    finish[v] = earliest_start[v] + proc[v];
                    q.push(v);
                }
            }
        }
        if (processed != V) return -1; // cycle -> infeasible
        long long makespan = 0;
        for (int id = 0; id < V; ++id) {
            if (finish[id] > makespan) makespan = finish[id];
        }
        return makespan;
    };

    long long current_make = evaluate(machine_orders);

    // Local search: adjacent swaps on each machine to improve makespan
    bool improved = true;
    while (improved) {
        improved = false;
        for (int m = 0; m < M && !improved; ++m) {
            for (int i = 0; i < J - 1 && !improved; ++i) {
                auto new_orders = machine_orders;
                swap(new_orders[m][i], new_orders[m][i + 1]);
                long long new_make = evaluate(new_orders);
                if (new_make > 0 && new_make < current_make) {
                    machine_orders = move(new_orders);
                    current_make = new_make;
                    improved = true;
                }
            }
        }
    }

    // Output the machine orders
    for (int m = 0; m < M; ++m) {
        for (int idx = 0; idx < J; ++idx) {
            if (idx) cout << ' ';
            cout << machine_orders[m][idx];
        }
        cout << '\n';
    }

    return 0;
}