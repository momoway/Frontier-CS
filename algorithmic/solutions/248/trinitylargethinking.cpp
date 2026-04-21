#include <bits/stdc++.h>
using namespace std;

const double INF = 1e100;
const double EPS = 1e-12;

int M;
vector<int> cityX;
vector<vector<int>> cityY;
vector<vector<int>> cityPoints;
vector<int> pointToCity;
vector<int> pointToIdx;
vector<int> pointX;
vector<int> pointY;
vector<vector<double>> costMat;

vector<int> order;
vector<int> selectedId;
double totalCost;

mt19937 rng(random_device{}());

double computeCost(const vector<int>& ord, const vector<int>& sel) {
    double sum = 0;
    for (int i = 0; i < M; ++i) {
        int u = ord[i];
        int v = ord[(i+1)%M];
        sum += costMat[sel[u]][sel[v]];
    }
    return sum;
}

double deltaNonAdjacent(int i, int j) {
    int a = order[i];
    int b = order[j];
    int pi = (i-1+M)%M;
    int si = (i+1)%M;
    int pj = (j-1+M)%M;
    int sj = (j+1)%M;
    double oldCost = costMat[selectedId[order[pi]]][selectedId[a]]
                   + costMat[selectedId[a]][selectedId[order[si]]]
                   + costMat[selectedId[order[pj]]][selectedId[b]]
                   + costMat[selectedId[b]][selectedId[order[sj]]];
    double newCost = costMat[selectedId[order[pi]]][selectedId[b]]
                   + costMat[selectedId[b]][selectedId[order[si]]]
                   + costMat[selectedId[order[pj]]][selectedId[a]]
                   + costMat[selectedId[a]][selectedId[order[sj]]];
    return newCost - oldCost;
}

double deltaAdjacent(int i, int j) {
    int u = order[i];
    int v = order[j];
    if ((i+1)%M == j) {
        int pi = (i-1+M)%M;
        int sj = (j+1)%M;
        double oldCost = costMat[selectedId[order[pi]]][selectedId[u]]
                       + costMat[selectedId[u]][selectedId[v]]
                       + costMat[selectedId[v]][selectedId[order[sj]]];
        double newCost = costMat[selectedId[order[pi]]][selectedId[v]]
                       + costMat[selectedId[v]][selectedId[u]]
                       + costMat[selectedId[u]][selectedId[order[sj]]];
        return newCost - oldCost;
    } else if ((j+1)%M == i) {
        int pj = (j-1+M)%M;
        int si = (i+1)%M;
        double oldCost = costMat[selectedId[order[pj]]][selectedId[v]]
                       + costMat[selectedId[v]][selectedId[u]]
                       + costMat[selectedId[u]][selectedId[order[si]]];
        double newCost = costMat[selectedId[order[pj]]][selectedId[u]]
                       + costMat[selectedId[u]][selectedId[v]]
                       + costMat[selectedId[v]][selectedId[order[si]]];
        return newCost - oldCost;
    } else {
        swap(order[i], order[j]);
        double newTotal = computeCost(order, selectedId);
        swap(order[i], order[j]);
        return newTotal - totalCost;
    }
}

void improve(Solution &sol) {
    order = sol.order;
    selectedId = sol.selectedId;
    totalCost = computeCost(order, selectedId);
    bool changed = true;
    while (changed) {
        changed = false;
        bool pointImproved = false;
        for (int i = 0; i < M; ++i) {
            int city = order[i];
            int prevCity = order[(i-1+M)%M];
            int nextCity = order[(i+1)%M];
            int currId = selectedId[city];
            double oldEdge = costMat[selectedId[prevCity]][currId] + costMat[currId][selectedId[nextCity]];
            int bestId = currId;
            double bestCost = oldEdge;
            for (int pt : cityPoints[city]) {
                double newCost = costMat[selectedId[prevCity]][pt] + costMat[pt][selectedId[nextCity]];
                if (newCost < bestCost - EPS) {
                    bestCost = newCost;
                    bestId = pt;
                }
            }
            if (bestId != currId) {
                selectedId[city] = bestId;
                totalCost += (bestCost - oldEdge);
                pointImproved = true;
                changed = true;
            }
        }
        if (M > 2) {
            bool swapLoopImproved = true;
            while (swapLoopImproved) {
                swapLoopImproved = false;
                for (int i = 0; i < M && !swapLoopImproved; ++i) {
                    for (int j = i+2; j < M && !swapLoopImproved; ++j) {
                        double d = deltaNonAdjacent(i, j);
                        if (d < -EPS) {
                            swap(order[i], order[j]);
                            totalCost += d;
                            swapLoopImproved = true;
                            changed = true;
                        }
                    }
                }
                if (swapLoopImproved) continue;
                for (int i = 0; i < M && !swapLoopImproved; ++i) {
                    int j = (i+1)%M;
                    double d = deltaAdjacent(i, j);
                    if (d < -EPS) {
                        swap(order[i], order[j]);
                        totalCost += d;
                        swapLoopImproved = true;
                        changed = true;
                    }
                }
            }
        }
    }
    sol.order = order;
    sol.selectedId = selectedId;
    sol.cost = totalCost;
}

struct Solution {
    vector<int> order;
    vector<int> selectedId;
    double cost;
};

Solution greedy(bool randomStart = true) {
    Solution sol;
    sol.order.resize(M);
    sol.selectedId.assign(M, -1);
    vector<bool> visited(M, false);
    int startCity;
    if (randomStart) {
        uniform_int_distribution<int> dist(0, M-1);
        startCity = dist(rng);
    } else {
        startCity = 0;
    }
    int bestStartPt = -1;
    int miny = INT_MAX;
    for (int pt : cityPoints[startCity]) {
        if (pointY[pt] < miny) {
            miny = pointY[pt];
            bestStartPt = pt;
        }
    }
    sol.selectedId[startCity] = bestStartPt;
    sol.order[0] = startCity;
    visited[startCity] = true;
    int currentCity = startCity;
    int currentId = bestStartPt;
    for (int step = 1; step < M; ++step) {
        double bestCost = 1e100;
        vector<pair<int,int>> mins;
        for (int j = 0; j < M; ++j) {
            if (visited[j]) continue;
            for (int pt : cityPoints[j]) {
                double c = costMat[currentId][pt];
                if (c < bestCost - EPS) {
                    bestCost = c;
                    mins.clear();
                    mins.emplace_back(j, pt);
                } else if (fabs(c - bestCost) <= EPS) {
                    mins.emplace_back(j, pt);
                }
            }
        }
        uniform_int_distribution<int> dist(0, (int)mins.size()-1);
        int idx = dist(rng);
        int bestCity = mins[idx].first;
        int bestPt = mins[idx].second;
        sol.order[step] = bestCity;
        sol.selectedId[bestCity] = bestPt;
        visited[bestCity] = true;
        currentCity = bestCity;
        currentId = bestPt;
    }
    sol.cost = computeCost(sol.order, sol.selectedId);
    return sol;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    double base;
    cin >> base;
    cin >> M;
    cityX.resize(M);
    cityY.resize(M);
    for (int i = 0; i < M; ++i) {
        int n, x;
        cin >> n >> x;
        cityX[i] = x;
        cityY[i].resize(n);
        for (int j = 0; j < n; ++j) {
            cin >> cityY[i][j];
        }
    }
    int D, S;
    cin >> D >> S;
    const double k = 0.6;
    double weight1 = (1 - k) / D;
    double weight2 = k / S;
    int totalPoints = 0;
    for (int i = 0; i < M; ++i) totalPoints += cityY[i].size();
    pointToCity.resize(totalPoints);
    pointToIdx.resize(totalPoints);
    pointX.resize(totalPoints);
    pointY.resize(totalPoints);
    cityPoints.assign(M, vector<int>());
    int id = 0;
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < cityY[i].size(); ++j) {
            pointToCity[id] = i;
            pointToIdx[id] = j;
            pointX[id] = cityX[i];
            pointY[id] = cityY[i][j];
            cityPoints[i].push_back(id);
            ++id;
        }
    }
    costMat.assign(totalPoints, vector<double>(totalPoints, 0));
    for (int i = 0; i < totalPoints; ++i) {
        for (int j = 0; j < totalPoints; ++j) {
            double dx = pointX[i] - pointX[j];
            double dy = pointY[i] - pointY[j];
            double dist = sqrt(dx*dx + dy*dy);
            double slope = 0.0;
            if (pointY[j] > pointY[i]) {
                if (dx == 0) slope = INF;
                else slope = (pointY[j] - pointY[i]) / fabs(dx);
            }
            costMat[i][j] = weight1 * dist + weight2 * slope;
        }
    }
    vector<Solution> candidates;
    int numInitial = 5;
    for (int t = 0; t < numInitial; ++t) {
        Solution sol = greedy(true);
        candidates.push_back(sol);
    }
    Solution best = candidates[0];
    for (Solution &sol : candidates) {
        improve(sol);
        if (sol.cost < best.cost - EPS) {
            best = sol;
        }
    }
    string out;
    for (int i = 0; i < M; ++i) {
        int cityId = best.order[i];
        int ptId = best.selectedId[cityId];
        int ptIdx = pointToIdx[ptId] + 1;
        char buf[50];
        sprintf(buf, "(%d,%d)", cityId+1, ptIdx);
        if (i > 0) out += "@";
        out += buf;
    }
    cout << out << "\n";
    return 0;
}