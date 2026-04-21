#include <bits/stdc++.h>
using namespace std;

struct Node {
    char type; // 'L', 'M', 'S'
    unsigned long long len;
    // leaf
    char sub_type; // 'D' or 'I'
    int L;
    // composite
    Node *left;
    Node *right;
    // leaf constructor
    Node(char t, unsigned long long l, char st, int Ll) : type(t), len(l), sub_type(st), L(Ll), left(nullptr), right(nullptr) {}
    // composite constructor
    Node(char t, unsigned long long l, Node* lft, Node* rgt) : type(t), len(l), left(lft), right(rgt) {
        sub_type = 0; L = 0;
    }
};

unordered_map<unsigned long long, Node*> memo;

Node* build(unsigned long long k) {
    auto it = memo.find(k);
    if (it != memo.end()) return it->second;

    // Power of two -> increasing block
    if ((k & (k-1)) == 0) {
        int a = __builtin_ctzll(k);
        Node* node = new Node('L', a, 'I', a);
        memo[k] = node;
        return node;
    }
    // Small numbers -> decreasing block (length k-1)
    if (k <= 91) {
        int L = k - 1;
        Node* node = new Node('L', L, 'D', L);
        memo[k] = node;
        return node;
    }

    Node* best_node = nullptr;
    unsigned long long best_len = ULLONG_MAX;

    // Helper to update best candidate
    auto consider = [&](Node* candidate, unsigned long long len) {
        if (len < best_len) {
            best_node = candidate;
            best_len = len;
        }
    };

    // 1. Addition with power of two leaf (m=1)
    for (int a = 1; a <= 60; ++a) {
        unsigned long long pow2 = 1ULL << a;
        if (pow2 >= k) break; // need pow2 <= k-1 so that B = k - pow2 + 1 >= 2
        unsigned long long B = k - pow2 + 1;
        if (B < 2) continue;
        Node* right = build(B);
        unsigned long long len = a + right->len;
        if (len <= 90) {
            Node* left = new Node('L', a, 'I', a);
            Node* node = new Node('S', len, left, right);
            memo[k] = node;
            return node;
        }
        if (len < best_len) {
            Node* left = new Node('L', a, 'I', a);
            Node* node = new Node('S', len, left, right);
            best_node = node;
            best_len = len;
        }
    }

    // 2. Mixed: k = m * pow2 + B - 1, with m >= 2
    for (int a = 1; a <= 60; ++a) {
        unsigned long long pow2 = 1ULL << a;
        if (pow2 > k) break;
        for (int m = 2; m <= 91; ++m) {
            if (m > k / pow2) break; // avoid overflow
            unsigned long long prod = (unsigned long long)m * pow2;
            unsigned long long B = k - prod + 1;
            if (B < 2) continue;
            Node* left_m = build(m);
            Node* left_pow2 = new Node('L', a, 'I', a);
            Node* left = new Node('M', left_m->len + a, left_m, left_pow2);
            Node* right = build(B);
            unsigned long long len = left->len + right->len;
            if (len <= 90) {
                Node* node = new Node('S', len, left, right);
                memo[k] = node;
                return node;
            }
            if (len < best_len) {
                Node* node = new Node('S', len, left, right);
                best_node = node;
                best_len = len;
            }
        }
    }

    // 3. Multiplication by power of two (if divisible)
    for (int a = 1; a <= 60; ++a) {
        unsigned long long pow2 = 1ULL << a;
        if (pow2 > k) break;
        if (k % pow2 != 0) continue;
        unsigned long long m = k / pow2;
        Node* left = new Node('L', a, 'I', a);
        Node* right = build(m);
        unsigned long long len = a + right->len;
        if (len <= 90) {
            Node* node = new Node('M', len, left, right);
            memo[k] = node;
            return node;
        }
        if (len < best_len) {
            Node* node = new Node('M', len, left, right);
            best_node = node;
            best_len = len;
        }
    }

    // 4. Multiplication by small factor (2..91)
    for (int f = 2; f <= 91; ++f) {
        if (k % f != 0) continue;
        unsigned long long m = k / f;
        Node* left = build(f);
        Node* right = build(m);
        unsigned long long len = left->len + right->len;
        if (len <= 90) {
            Node* node = new Node('M', len, left, right);
            memo[k] = node;
            return node;
        }
        if (len < best_len) {
            Node* node = new Node('M', len, left, right);
            best_node = node;
            best_len = len;
        }
    }

    // 5. Addition with small leaf (2..91)
    for (int l = 2; l <= 91; ++l) {
        if (l > k) break;
        unsigned long long B = k + 1 - l;
        if (B < 2) continue;
        Node* left = build(l);
        Node* right = build(B);
        unsigned long long len = left->len + right->len;
        if (len <= 90) {
            Node* node = new Node('S', len, left, right);
            memo[k] = node;
            return node;
        }
        if (len < best_len) {
            Node* node = new Node('S', len, left, right);
            best_node = node;
            best_len = len;
        }
    }

    // If we have any candidate (even with len > 90), return it (should not happen often)
    if (best_node) {
        memo[k] = best_node;
        return best_node;
    }

    // Fallback: decreasing block of length k-1 (will be huge, but should not occur)
    int L = k - 1;
    Node* node = new Node('L', L, 'D', L);
    memo[k] = node;
    return node;
}

vector<int> generate(Node* node, int start) {
    if (!node) return {};
    if (node->type == 'L') {
        vector<int> res(node->L);
        if (node->sub_type == 'D') {
            for (int i = 0; i < node->L; ++i) {
                res[i] = start + node->L - 1 - i;
            }
        } else { // 'I'
            for (int i = 0; i < node->L; ++i) {
                res[i] = start + i;
            }
        }
        return res;
    } else if (node->type == 'M') {
        vector<int> left_vec = generate(node->left, start);
        int left_len = node->left->len;
        vector<int> right_vec = generate(node->right, start + left_len);
        left_vec.insert(left_vec.end(), right_vec.begin(), right_vec.end());
        return left_vec;
    } else { // 'S'
        int left_len = node->left->len;
        int right_len = node->right->len;
        vector<int> right_vec = generate(node->right, start);
        vector<int> left_vec = generate(node->left, start + right_len);
        left_vec.insert(left_vec.end(), right_vec.begin(), right_vec.end());
        return left_vec;
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int q;
    if (!(cin >> q)) return 0;
    vector<unsigned long long> ks(q);
    for (int i = 0; i < q; ++i) cin >> ks[i];
    for (int i = 0; i < q; ++i) {
        unsigned long long k = ks[i];
        Node* root = build(k);
        vector<int> perm = generate(root, 0);
        cout << perm.size() << "\n";
        for (size_t j = 0; j < perm.size(); ++j) {
            if (j) cout << " ";
            cout << perm[j];
        }
        cout << "\n";
    }
    return 0;
}