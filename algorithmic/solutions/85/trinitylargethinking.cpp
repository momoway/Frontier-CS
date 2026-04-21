#include <iostream>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int initialDeep;
    cin >> initialDeep;
    int curDist = initialDeep;
    int inc = -1; // incoming color, -1 means no incoming edge
    if (curDist == 0) return 0;
    while (curDist > 0) {
        bool foundParent = false;
        for (int c = 0; c < 3; ++c) {
            if (c == inc) continue;
            cout << "move " << c << endl;
            int res;
            cin >> res;
            if (res == 1) {
                return 0;
            }
            cout << "query" << endl;
            int newDist;
            cin >> newDist;
            if (newDist == curDist - 1) {
                curDist = newDist;
                inc = c;
                foundParent = true;
                break;
            } else if (newDist == curDist + 1) {
                // Move back to original node
                cout << "move " << c << endl;
                cin >> res; // ignore response
            } else {
                // Unexpected distance change; should not happen in a tree
                return 0;
            }
        }
        if (!foundParent) {
            // Could not find parent; should not happen
            return 0;
        }
    }
    return 0;
}