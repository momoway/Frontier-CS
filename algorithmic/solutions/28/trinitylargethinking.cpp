#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T;
    if (!(cin >> T)) return 0;
    while (T--) {
        int N;
        cin >> N;
        vector<string> allWords;
        // For each letter 'a' to 'z'
        for (char ch = 'a'; ch <= 'z' && (int)allWords.size() < N; ++ch) {
            // First, check if there is any word starting with this letter
            cout << "query " << ch << " 1" << endl;
            cout.flush();
            int k;
            cin >> k;
            vector<string> words(k);
            for (int i = 0; i < k; ++i) {
                cin >> words[i];
            }
            if (k == 0) continue; // no words, skip
            // This letter has words, retrieve all using exponential search
            unsigned int cur = 1;
            while (true) {
                cout << "query " << ch << " " << cur << endl;
                cout.flush();
                int got;
                cin >> got;
                vector<string> batch(got);
                for (int i = 0; i < got; ++i) {
                    cin >> batch[i];
                }
                // Add to allWords
                for (const string& w : batch) {
                    allWords.push_back(w);
                }
                if (got == (int)cur) {
                    // Might have more, double
                    if (cur > (unsigned int)N) break; // safety
                    cur *= 2;
                } else {
                    break;
                }
            }
        }
        // Output answer
        cout << "answer";
        for (const string& w : allWords) {
            cout << " " << w;
        }
        cout << endl;
        cout.flush();
    }
    return 0;
}