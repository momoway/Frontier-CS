#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <tuple>
#include <algorithm>
#include <cfloat>

using namespace std;

int n;
int query_count = 0;

double query(double x, double y) {
    cout << "? " << x << " " << y << endl;
    cout.flush();
    double res;
    cin >> res;
    query_count++;
    return res;
}

// For x = 0
map<long long, double> f0_cache;

double get_f0(long long y) {
    if (fX_cache.count(y)) return fX_cache[y];
    double val = query(0.0, (double)y);
    f0_cache[y] = val;
    return val;
}

double get_slope0(long long y) {
    double fy = get_f0(y);
    double fy1 = get_f0(y + 1);
    return fy1 - fy;
}

vector<pair<long long, double>> b_breakpoints;

void find_b_breakpoints(long long l, long long r, double ref_slope) {
    if (l >= r) return;
    double S_l = (ref_slope >= 0 ? ref_slope : get_slope0(l));
    double S_r = get_slope0(r - 1);
    if (fabs(S_r - S_l) < 1e-7) return;
    long long low = l + 1, high = r - 1;
    while (high - low > 1) {
        long long mid = (low + high) / 2;
        double s_mid = get_slope0(mid);
        if (s_mid > S_l + 1e-7) {
            high = mid;
        } else {
            low = mid + 1;
        }
    }
    long long b = low;
    double w = (get_slope0(b) - S_l) / 2.0;
    b_breakpoints.push_back({b, w});
    find_b_breakpoints(l, b, S_l);
    find_b_breakpoints(b + 1, r, get_slope0(b));
}

// For x = sqrt(2)
double X = sqrt(2.0);
const double Y_minX = -30000.0;
const double Y_maxX = 30000.0;
const double coarse_step = 200.0;
map<double, double> fX_cache;

double get_fX(double y) {
    if (fX_cache.count(y)) return fX_cache[y];
    double val = query(X, y);
    fX_cache[y] = val;
    return val;
}

vector<pair<double, double>> c_breakpoints;

double get_slope_fine(double y) {
    double y1 = y - 1e-5;
    double y2 = y + 1e-5;
    double f1 = get_fX(y1);
    double f2 = get_fX(y2);
    return (f2 - f1) / (2e-5);
}

void find_c_breakpoints(double L, double R, double slope_L, double slope_R) {
    if (fabs(slope_L - slope_R) < 1e-7) return;
    double low = L, high = R;
    while (high - low > 1e-4) {
        double mid = (low + high) / 2.0;
        double s_mid = get_slope_fine(mid);
        if (s_mid > slope_L + 1e-7) {
            high = mid;
        } else {
            low = mid;
        }
    }
    double c = (low + high) / 2.0;
    double s_left = get_slope_fine(c - 1e-5);
    double s_right = get_slope_fine(c + 1e-5);
    double w = (s_right - s_left) / 2.0;
    c_breakpoints.push_back({c, w});
    find_c_breakpoints(c, R, s_right, slope_R);
}

int main() {
    cin >> n;
    // Step 1: Recover b_i from x=0
    find_b_breakpoints(-10000, 10000, -1.0);
    // Step 2: Recover c_i from x = sqrt(2)
    vector<double> coarse_y;
    for (double y = Y_minX; y <= Y_maxX; y += coarse_step) {
        coarse_y.push_back(y);
    }
    // Ensure Y_maxX is included
    if (coarse_y.back() < Y_maxX) coarse_y.push_back(Y_maxX);
    for (double y : coarse_y) {
        get_fX(y);
    }
    vector<double> coarse_slope;
    for (size_t i = 0; i + 1 < coarse_y.size(); ++i) {
        double y1 = coarse_y[i], y2 = coarse_y[i+1];
        double f1 = get_fX(y1), f2 = get_fX(y2);
        coarse_slope.push_back((f2 - f1) / (y2 - y1));
    }
    vector<pair<double, double>> intervals;
    for (size_t i = 0; i + 1 < coarse_slope.size(); ++i) {
        if (i + 2 < coarse_slope.size() && fabs(coarse_slope[i+1] - coarse_slope[i]) > 1e-7) {
            intervals.push_back({coarse_y[i], coarse_y[i+1]});
        }
    }
    // Also consider the last interval if needed
    if (coarse_slope.size() >= 1) {
        intervals.push_back({coarse_y[coarse_y.size()-2], coarse_y[coarse_y.size()-1]});
    }
    for (auto &inter : intervals) {
        double L = inter.first, R = inter.second;
        double sL = get_slope_fine(L + 1e-5);
        double sR = get_slope_fine(R - 1e-5);
        find_c_breakpoints(L, R, sL, sR);
    }
    // Matching
    vector<tuple<long long, long long, double>> lines;
    for (auto &cp : c_breakpoints) {
        double c = cp.first;
        double w = cp.second;
        bool matched = false;
        for (auto &bp : b_breakpoints) {
            long long b = bp.first;
            double a_guess = (c - b) / X;
            long long a_round = llround(a_guess);
            if (fabs(a_guess - a_round) < 0.01 && a_round >= -10000 && a_round <= 10000) {
                double w_expected = 1.0 / sqrt((double)a_round * a_round + 1.0);
                if (fabs(w - w_expected) < 0.01) {
                    double c_expected = (double)a_round * X + (double)b;
                    if (fabs(c - c_expected) < 0.01) {
                        lines.emplace_back(a_round, b, w);
                        matched = true;
                        break;
                    }
                }
            }
        }
        if (!matched) {
            // In case of no match, try with larger tolerance or all b
            for (auto &bp : b_breakpoints) {
                long long b = bp.first;
                double a_guess = (c - b) / X;
                long long a_round = llround(a_guess);
                if (fabs(a_guess - a_round) < 0.1 && a_round >= -10000 && a_round <= 10000) {
                    double w_expected = 1.0 / sqrt((double)a_round * a_round + 1.0);
                    if (fabs(w - w_expected) < 0.1) {
                        double c_expected = (double)a_round * X + (double)b;
                        if (fabs(c - c_expected) < 0.1) {
                            lines.emplace_back(a_round, b, w);
                            matched = true;
                            break;
                        }
                    }
                }
            }
        }
    }
    // Output
    cout << "!";
    for (auto &line : lines) {
        long long a = get<0>(line);
        long long b = get<1>(line);
        cout << " " << a << " " << b;
    }
    cout << endl;
    cout.flush();
    return 0;
}