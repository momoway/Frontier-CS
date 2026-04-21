#include <bits/stdc++.h>
using namespace std;

struct Point {
    double x, y;
};

struct Segment {
    int a, b;
};

const int leafSize = 10;
const double INF = 1e100;

vector<Point> pts;
vector<Segment> allSegments;
vector<BVHNode> tree; // defined later

struct BVHNode {
    double xmin, xmax, ymin, ymax;
    int left, right; // children indices; -1 for leaf
    vector<int> segIndices; // for leaf
};

double dist2_point_segment(const Point& p, const Point& a, const Point& b) {
    double len2 = (b.x - a.x)*(b.x - a.x) + (b.y - a.y)*(b.y - a.y);
    if (len2 == 0.0) {
        double dx = p.x - a.x, dy = p.y - a.y;
        return dx*dx + dy*dy;
    }
    double t = ((p.x - a.x)*(b.x - a.x) + (p.y - a.y)*(b.y - a.y)) / len2;
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;
    double projx = a.x + t * (b.x - a.x);
    double projy = a.y + t * (b.y - a.y);
    double dx = p.x - projx;
    double dy = p.y - projy;
    return dx*dx + dy*dy;
}

double distance_point_to_bbox(const Point& p, const BVHNode& node) {
    double dx = 0.0;
    if (p.x < node.xmin) dx = node.xmin - p.x;
    else if (p.x > node.xmax) dx = p.x - node.xmax;
    double dy = 0.0;
    if (p.y < node.ymin) dy = node.ymin - p.y;
    else if (p.y > node.ymax) dy = p.y - node.ymax;
    return dx*dx + dy*dy;
}

int build_bvh(vector<int>& segIndices, int l, int r) {
    if (r - l <= leafSize) {
        BVHNode node;
        node.xmin = INF; node.xmax = -INF; node.ymin = INF; node.ymax = -INF;
        for (int i = l; i < r; ++i) {
            int segId = segIndices[i];
            const Segment& seg = allSegments[segId];
            const Point& p0 = pts[seg.a];
            const Point& p1 = pts[seg.b];
            node.xmin = min(node.xmin, min(p0.x, p1.x));
            node.xmax = max(node.xmax, max(p0.x, p1.x));
            node.ymin = min(node.ymin, min(p0.y, p1.y));
            node.ymax = max(node.ymax, max(p0.y, p1.y));
        }
        node.left = node.right = -1;
        node.segIndices.assign(segIndices.begin() + l, segIndices.begin() + r);
        tree.push_back(node);
        return (int)tree.size() - 1;
    }

    // Compute spread of midpoints
    double midXmin = INF, midXmax = -INF, midYmin = INF, midYmax = -INF;
    for (int i = l; i < r; ++i) {
        int segId = segIndices[i];
        const Segment& seg = allSegments[segId];
        const Point& p0 = pts[seg.a];
        const Point& p1 = pts[seg.b];
        double mx = (p0.x + p1.x) * 0.5;
        double my = (p0.y + p1.y) * 0.5;
        midXmin = min(midXmin, mx);
        midXmax = max(midXmax, mx);
        midYmin = min(midYmin, my);
        midYmax = max(midYmax, my);
    }
    double spreadX = midXmax - midXmin;
    double spreadY = midYmax - midYmin;
    int dim = (spreadX >= spreadY) ? 0 : 1;

    // Sort by chosen dimension
    sort(segIndices.begin() + l, segIndices.begin() + r, [&](int i, int j) {
        const Segment& si = allSegments[i];
        const Segment& sj = allSegments[j];
        const Point& pi0 = pts[si.a];
        const Point& pi1 = pts[si.b];
        const Point& pj0 = pts[sj.a];
        const Point& pj1 = pts[sj.b];
        double mxi = (pi0.x + pi1.x) * 0.5;
        double myi = (pi0.y + pi1.y) * 0.5;
        double mxj = (pj0.x + pj1.x) * 0.5;
        double myj = (pj0.y + pj1.y) * 0.5;
        if (dim == 0) return mxi < mxj;
        else return myi < myj;
    });

    int m = (l + r) / 2;
    int leftIdx = build_bvh(segIndices, l, m);
    int rightIdx = build_bvh(segIndices, m, r);

    const BVHNode& leftNode = tree[leftIdx];
    const BVHNode& rightNode = tree[rightIdx];
    BVHNode node;
    node.xmin = min(leftNode.xmin, rightNode.xmin);
    node.xmax = max(leftNode.xmax, rightNode.xmax);
    node.ymin = min(leftNode.ymin, rightNode.ymin);
    node.ymax = max(leftNode.ymax, rightNode.ymax);
    node.left = leftIdx;
    node.right = rightIdx;
    node.segIndices.clear();
    tree.push_back(node);
    return (int)tree.size() - 1;
}

bool query(const Point& p, double r) {
    double r2 = r * r;
    using Pair = pair<double, int>;
    priority_queue<Pair, vector<Pair>, greater<Pair>> pq;
    double rootDist2 = distance_point_to_bbox(p, tree[0]);
    pq.push({rootDist2, 0});
    while (!pq.empty()) {
        auto [dist2, idx] = pq.top(); pq.pop();
        if (dist2 > r2) break;
        const BVHNode& node = tree[idx];
        if (node.left == -1 && node.right == -1) {
            for (int segId : node.segIndices) {
                const Segment& seg = allSegments[segId];
                const Point& a = pts[seg.a];
                const Point& b = pts[seg.b];
                double d2 = dist2_point_segment(p, a, b);
                if (d2 <= r2) return true;
            }
        } else {
            int left = node.left;
            int right = node.right;
            double dL2 = distance_point_to_bbox(p, tree[left]);
            double dR2 = distance_point_to_bbox(p, tree[right]);
            pq.push({dL2, left});
            pq.push({dR2, right});
        }
    }
    return false;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    if (!(cin >> n)) return 0;
    pts.resize(n);
    for (int i = 0; i < n; ++i) {
        cin >> pts[i].x >> pts[i].y;
    }
    int m;
    cin >> m;
    allSegments.resize(m);
    for (int i = 0; i < m; ++i) {
        cin >> allSegments[i].a >> allSegments[i].b;
        --allSegments[i].a; --allSegments[i].b;
    }
    double r;
    cin >> r;
    double p1, p2, p3, p4;
    cin >> p1 >> p2 >> p3 >> p4; // p2, p4 ignored

    if (m == 0) {
        cout << "0.0000000000\n";
        return 0;
    }

    // Compute overall bounding box of points
    double minX = pts[0].x, maxX = pts[0].x;
    double minY = pts[0].y, maxY = pts[0].y;
    for (int i = 1; i < n; ++i) {
        minX = min(minX, pts[i].x);
        maxX = max(maxX, pts[i].x);
        minY = min(minY, pts[i].y);
        maxY = max(maxY, pts[i].y);
    }
    // Expand by r
    double bboxMinX = minX - r;
    double bboxMaxX = maxX + r;
    double bboxMinY = minY - r;
    double bboxMaxY = maxY + r;
    double totalArea = (bboxMaxX - bboxMinX) * (bboxMaxY - bboxMinY);

    // Build BVH
    vector<int> segIndices(m);
    iota(segIndices.begin(), segIndices.end(), 0);
    tree.reserve(4 * m); // approximate
    build_bvh(segIndices, 0, m);

    // Monte Carlo integration
    const long long N = 20000000; // 20 million samples
    static std::mt19937_64 rng(( unsigned long long ) chrono::steady_clock::now().time_since_epoch().count());
    uniform_real_distribution<double> distX(bboxMinX, bboxMaxX);
    uniform_real_distribution<double> distY(bboxMinY, bboxMaxY);
    long long count = 0;
    for (long long i = 0; i < N; ++i) {
        Point p;
        p.x = distX(rng);
        p.y = distY(rng);
        if (query(p, r)) ++count;
    }
    double area = (double)count / N * totalArea;
    cout << fixed << setprecision(10) << area << "\n";
    return 0;
}