#include <bits/stdc++.h>
using namespace std;

struct Point {
    double x, y, z;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    if (!(cin >> n)) return 0;
    vector<Point> points(n);
    if (n == 2) {
        points[0] = {0, 0, 1};
        points[1] = {0, 0, -1};
    } else if (n == 3) {
        const double s3 = sqrt(3.0);
        points[0] = {1, 0, 0};
        points[1] = {-0.5, s3 / 2, 0};
        points[2] = {-0.5, -s3 / 2, 0};
    } else if (n == 4) {
        const double inv_sqrt3 = 1.0 / sqrt(3.0);
        points[0] = {1, 1, 1};
        points[1] = {1, -1, -1};
        points[2] = {-1, 1, -1};
        points[3] = {-1, -1, 1};
        for (auto &p : points) {
            p.x *= inv_sqrt3;
            p.y *= inv_sqrt3;
            p.z *= inv_sqrt3;
        }
    } else if (n == 5) {
        const double s3 = sqrt(3.0);
        points[0] = {0, 0, 1};
        points[1] = {0, 0, -1};
        points[2] = {1, 0, 0};
        points[3] = {-0.5, s3 / 2, 0};
        points[4] = {-0.5, -s3 / 2, 0};
    } else if (n == 6) {
        points[0] = {1, 0, 0};
        points[1] = {-1, 0, 0};
        points[2] = {0, 1, 0};
        points[3] = {0, -1, 0};
        points[4] = {0, 0, 1};
        points[5] = {0, 0, -1};
    } else {
        const double PI = acos(-1.0);
        const double phi = (1.0 + sqrt(5.0)) / 2.0;
        for (int i = 0; i < n; ++i) {
            double theta = 2.0 * PI * i / phi;
            double z = 1.0 - 2.0 * i / static_cast<double>(n);
            double r = sqrt(1.0 - z * z);
            points[i].x = cos(theta) * r;
            points[i].y = sin(theta) * r;
            points[i].z = z;
        }
        int max_iter = 100;
        double eta = 0.1;
        for (int iter = 0; iter < max_iter; ++iter) {
            for (int i = 0; i < n; ++i) {
                double fx = 0, fy = 0, fz = 0;
                for (int j = 0; j < n; ++j) {
                    if (i == j) continue;
                    double dx = points[i].x - points[j].x;
                    double dy = points[i].y - points[j].y;
                    double dz = points[i].z - points[j].z;
                    double dist_sq = dx*dx + dy*dy + dz*dz;
                    if (dist_sq < 1e-12) continue;
                    double dist = sqrt(dist_sq);
                    double factor = 1.0 / (dist_sq * dist);
                    fx += dx * factor;
                    fy += dy * factor;
                    fz += dz * factor;
                }
                points[i].x += eta * fx;
                points[i].y += eta * fy;
                points[i].z += eta * fz;
                double len = sqrt(points[i].x*points[i].x + points[i].y*points[i].y + points[i].z*points[i].z);
                points[i].x /= len;
                points[i].y /= len;
                points[i].z /= len;
            }
            eta *= 0.95;
        }
    }
    double min_dist = 1e30;
    for (int i = 0; i < n; ++i) {
        for (int j = i+1; j < n; ++j) {
            double dx = points[i].x - points[j].x;
            double dy = points[i].y - points[j].y;
            double dz = points[i].z - points[j].z;
            double dist = sqrt(dx*dx + dy*dy + dz*dz);
            if (dist < min_dist) min_dist = dist;
        }
    }
    cout << fixed << setprecision(10) << min_dist << "\n";
    for (int i = 0; i < n; ++i) {
        cout << fixed << setprecision(10) << points[i].x << " " << points[i].y << " " << points[i].z << "\n";
    }
    return 0;
}