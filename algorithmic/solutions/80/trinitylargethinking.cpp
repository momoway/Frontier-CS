#include <iostream>
#include <string>
using namespace std;

int main() {
    int m;
    if (!(cin >> m)) return 0;
    string line;
    getline(cin, line); // consume newline after m
    while (true) {
        getline(cin, line);
        if (line == "treasure") break;
        // Always output the same action: move stone 1 step clockwise, set side left, take passage 0
        cout << "1 left 0" << endl;
        cout.flush();
    }
    return 0;
}