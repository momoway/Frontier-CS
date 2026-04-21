#include <bits/stdc++.h>
using namespace std;

struct Node {
    int val;
    int pri;
    int sz;
    Node *l, *r, *p;
    Node(int v, int pr) : val(v), pri(pr), sz(1), l(nullptr), r(nullptr), p(nullptr) {}
};

int getSize(Node* t) { return t ? t->sz : 0; }

void upd(Node* t) {
    if(!t) return;
    t->sz = 1;
    if(t->l) {
        t->sz += t->l->sz;
        t->l->p = t;
    }
    if(t->r) {
        t->sz += t->r->sz;
        t->r->p = t;
    }
}

void split(Node* t, int k, Node* &left, Node* &right) {
    if(!t) { left = right = nullptr; return; }
    int ls = t->l ? t->l->sz : 0;
    if(ls >= k) {
        split(t->l, k, left, t->l);
        if(t->l) t->l->p = t;
        right = t;
        right->p = nullptr;
        upd(right);
    } else {
        split(t->r, k - ls - 1, t->r, right);
        if(t->r) t->r->p = t;
        left = t;
        left->p = nullptr;
        upd(left);
    }
    upd(t);
}

Node* merge(Node* left, Node* right) {
    if(!left || !right) return left ? left : right;
    if(left->pri > right->pri) {
        left->r = merge(left->r, right);
        if(left->r) left->r->p = left;
        upd(left);
        return left;
    } else {
        right->l = merge(left, right->l);
        if(right->l) right->l->p = right;
        upd(right);
        return right;
    }
}

int getPosition(Node* root, Node* target) {
    int pos = (target->l ? target->l->sz : 0) + 1;
    Node* cur = target;
    while(cur->p) {
        Node* par = cur->p;
        if(cur == par->r) {
            pos += (par->l ? par->l->sz : 0) + 1;
        }
        cur = par;
    }
    return pos;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    if(!(cin >> n)) return 0;
    vector<int> a(n);
    for(int i = 0; i < n; ++i) cin >> a[i];

    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    vector<Node*> nodePtr(n+1);
    Node* root = nullptr;
    for(int i = 0; i < n; ++i) {
        Node* node = new Node(a[i], (int)rng());
        nodePtr[a[i]] = node;
        root = merge(root, node);
    }

    long long totalCost = 0;
    int moveCount = 0;
    vector<pair<int,int>> moves;

    for(int i = 1; i <= n; ++i) {
        Node* node = nodePtr[i];
        int curPos = getPosition(root, node);
        if(curPos != i) {
            // Remove node from current position
            Node *left, *midRight;
            split(root, curPos-1, left, midRight);
            Node *mid, *right;
            split(midRight, 1, mid, right);
            // mid is the node to move
            // Insert at position i
            Node *left1, *left2;
            split(left, i-1, left1, left2);
            Node* temp = merge(mid, left2);
            root = merge(merge(left1, temp), right);
            // Record move
            moves.push_back({curPos, i});
            totalCost += i;
            moveCount++;
        }
    }

    long long min_cost = (totalCost + 1) * (moveCount + 1);
    cout << min_cost << " " << moveCount << "\n";
    for(auto &p : moves) {
        cout << p.first << " " << p.second << "\n";
    }
    return 0;
}