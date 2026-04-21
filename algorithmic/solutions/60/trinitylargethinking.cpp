#include <bits/stdc++.h>
using namespace std;

const int BOX = 100000;
const double THRESH = 1e-3;

double query_vertical(int X) {
    cout << "query " << X << " " << 0 << " " << X << " " << BOX << endl;
    cout.flush();
    double resp;
    cin >> resp;
    return resp;
}

double query_horizontal(int Y) {
    cout << "query " << 0 << " " << Y << " " << BOX << " " << Y << endl;
    cout.flush();
    double resp;
    cin >> resp;
    return resp;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int low = 0, high = BOX;
    while (low < high) {
        int mid = (low + high) / 2;
        double resp = query_vertical(mid);
        if (resp > THRESH) high = mid;
        else low = mid + 1;
    }
    int L = low;

    low = 0; high = BOX;
    int R = -1;
    while (low <= high) {
        int mid = (low + high) / 2;
        double resp = query_vertical(mid);
        if (resp > THRESH) {
            R = mid;
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    int x = (L + R) / 2;
    int r = (R - L) / 2 + 1;

    low = 0; high = BOX;
    while (low < high) {
        int mid = (low + high) / 2;
        double resp = query_horizontal(mid);
        if (resp > THRESH) high = mid;
        else low = mid + 1;
    }
    int B = low;

    low = 0; high = BOX;
    int T = -1;
    while (low <= high) {
        int mid = (low + high) / 2;
        double resp = query_horizontal(mid);
        if (resp > THRESH) {
            T = mid;
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    int y = (B + T) / 2;

    cout << "answer " << x << " " << y << " " << r << endl;
    return 0;
}