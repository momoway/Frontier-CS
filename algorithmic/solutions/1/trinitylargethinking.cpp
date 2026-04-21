#include <bits/stdc++.h>
using namespace std;

const uint64_t MAX_MASS = 20000000ULL;
const uint64_t MAX_VOL = 25000000ULL;

struct Item {
    int q;
    uint64_t v;
    uint64_t m;
    uint64_t l;
};

struct BinaryItem {
    int count;
    uint64_t mass;
    uint64_t vol;
    uint64_t value;
    int typeIndex;
};

struct State {
    uint64_t mass;
    uint64_t vol;
    uint64_t value;
    array<uint16_t, 6> counts;
    State() : mass(0), vol(0), value(0) { counts.fill(0); }
};

class FenwickMax {
public:
    int n;
    vector<uint64_t> tree;
    void init(int n_) {
        n = n_;
        tree.assign(n + 1, 0);
    }
    void update(int idx, uint64_t val) {
        for (int i = idx; i <= n; i += i & -i)
            if (val > tree[i]) tree[i] = val;
    }
    uint64_t query(int idx) const {
        uint64_t res = 0;
        for (int i = idx; i > 0; i -= i & -i)
            if (tree[i] > res) res = tree[i];
        return res;
    }
};

vector<State> pruneStates(vector<State> states) {
    if (states.empty()) return {};
    sort(states.begin(), states.end(), [](const State& a, const State& b) {
        if (a.mass != b.mass) return a.mass < b.mass;
        if (a.vol != b.vol) return a.vol < b.vol;
        return a.value > b.value;
    });
    vector<uint32_t> volumes;
    volumes.reserve(states.size());
    for (const auto& s : states) volumes.push_back((uint32_t)s.vol);
    sort(volumes.begin(), volumes.end());
    volumes.erase(unique(volumes.begin(), volumes.end()), volumes.end());
    int numVols = volumes.size();
    FenwickMax bit;
    bit.init(numVols);
    vector<State> pruned;
    size_t i = 0;
    while (i < states.size()) {
        size_t j = i;
        while (j < states.size() && states[j].mass == states[i].mass) ++j;
        vector<State> candidates;
        for (size_t k = i; k < j; ++k) {
            const State& s = states[k];
            int volIdx = lower_bound(volumes.begin(), volumes.end(), s.vol) - volumes.begin();
            uint64_t bestPrev = bit.query(volIdx + 1);
            if (bestPrev >= s.value) continue;
            candidates.push_back(s);
        }
        sort(candidates.begin(), candidates.end(), [](const State& a, const State& b) {
            if (a.vol != b.vol) return a.vol < b.vol;
            return a.value > b.value;
        });
        vector<State> nonDominated;
        uint64_t lastVal = 0;
        for (const auto& s : candidates) {
            if (!nonDominated.empty()) {
                if (s.vol == nonDominated.back().vol) continue;
                if (s.value <= lastVal) continue;
            }
            nonDominated.push_back(s);
            lastVal = s.value;
        }
        for (const auto& s : nonDominated) {
            pruned.push_back(s);
            int volIdx = lower_bound(volumes.begin(), volumes.end(), s.vol) - volumes.begin();
            bit.update(volIdx + 1, s.value);
        }
        i = j;
    }
    return pruned;
}

vector<State> solveGroup(const vector<Item>& items, uint64_t maxMass, uint64_t maxVol) {
    vector<BinaryItem> binItems;
    for (int i = 0; i < (int)items.size(); ++i) {
        int q = items[i].q;
        int remaining = q;
        for (int k = 1; k <= remaining; k <<= 1) {
            int take = k;
            if (take > remaining) take = remaining;
            BinaryItem bi;
            bi.count = take;
            bi.mass = (uint64_t)take * items[i].m;
            bi.vol = (uint64_t)take * items[i].l;
            bi.value = (uint64_t)take * items[i].v;
            bi.typeIndex = i;
            binItems.push_back(bi);
            remaining -= take;
        }
    }
    vector<State> states;
    states.emplace_back();
    for (const auto& bin : binItems) {
        vector<State> newStates;
        newStates.reserve(states.size());
        for (const auto& cur : states) {
            uint64_t newMass = cur.mass + bin.mass;
            uint64_t newVol = cur.vol + bin.vol;
            if (newMass > maxMass || newVol > maxVol) continue;
            uint64_t newVal = cur.value + bin.value;
            State ns;
            ns.mass = newMass;
            ns.vol = newVol;
            ns.value = newVal;
            ns.counts = cur.counts;
            ns.counts[bin.typeIndex] += bin.count;
            newStates.push_back(ns);
        }
        states.insert(states.end(), newStates.begin(), newStates.end());
        states = pruneStates(states);
    }
    states = pruneStates(states);
    return states;
}

pair<vector<uint16_t>, uint64_t> combine(const vector<State>& A, const vector<State>& B) {
    uint64_t bestValue = 0;
    vector<uint16_t> bestCounts(12, 0);
    uint64_t maxB = 0;
    for (const auto& b : B) if (b.value > maxB) maxB = b.value;
    for (const auto& a : A) {
        uint64_t remMass = MAX_MASS - a.mass;
        uint64_t remVol = MAX_VOL - a.vol;
        if (a.value + maxB <= bestValue) continue;
        for (const auto& b : B) {
            if (b.mass <= remMass && b.vol <= remVol) {
                uint64_t totalVal = a.value + b.value;
                if (totalVal > bestValue) {
                    bestValue = totalVal;
                    bestCounts.assign(12, 0);
                    for (int i = 0; i < 6; ++i) bestCounts[i] = a.counts[i];
                    for (int i = 0; i < 6; ++i) bestCounts[i + 6] = b.counts[i];
                }
            }
        }
    }
    return {bestCounts, bestValue};
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string input((istreambuf_iterator<char>(cin)), istreambuf_iterator<char>());
    vector<string> keysOrder;
    map<string, Item> itemMap;
    size_t pos = 0;
    while (pos < input.size() && isspace(input[pos])) ++pos;
    if (pos >= input.size() || input[pos] != '{') return 0;
    ++pos;
    while (true) {
        while (pos < input.size() && isspace(input[pos])) ++pos;
        if (pos >= input.size()) break;
        if (input[pos] == '}') break;
        if (input[pos] != '"') return 0;
        ++pos;
        string key;
        while (pos < input.size() && input[pos] != '"') key.push_back(input[pos++]);
        ++pos;
        while (pos < input.size() && (isspace(input[pos]) || input[pos] == ':')) ++pos;
        if (pos >= input.size() || input[pos] != '[') return 0;
        ++pos;
        vector<uint64_t> nums;
        while (true) {
            while (pos < input.size() && isspace(input[pos])) ++pos;
            uint64_t num = 0;
            while (pos < input.size() && isdigit(input[pos])) {
                num = num * 10 + (input[pos] - '0');
                ++pos;
            }
            nums.push_back(num);
            while (pos < input.size() && isspace(input[pos])) ++pos;
            if (pos >= input.size()) break;
            if (input[pos] == ']') { ++pos; break; }
            if (input[pos] == ',') { ++pos; continue; }
        }
        if (nums.size() != 4) return 0;
        Item it;
        it.q = (int)nums[0];
        it.v = nums[1];
        it.m = nums[2];
        it.l = nums[3];
        itemMap[key] = it;
        keysOrder.push_back(key);
        while (pos < input.size() && isspace(input[pos])) ++pos;
        if (pos >= input.size()) break;
        if (input[pos] == ',') { ++pos; continue; }
        else if (input[pos] == '}') break;
    }
    vector<Item> items;
    for (auto& kv : keysOrder) items.push_back(itemMap[kv]);
    if (items.size() != 12) return 0;
    vector<Item> groupA(items.begin(), items.begin() + 6);
    vector<Item> groupB(items.begin() + 6, items.end());
    auto statesA = solveGroup(groupA, MAX_MASS, MAX_VOL);
    auto statesB = solveGroup(groupB, MAX_MASS, MAX_VOL);
    auto result = combine(statesA, statesB);
    const auto& bestCounts = result.first;
    cout << "{\n";
    for (size_t i = 0; i < keysOrder.size(); ++i) {
        cout << " \"" << keysOrder[i] << "\": " << bestCounts[i];
        if (i != keysOrder.size() - 1) cout << ",";
        cout << "\n";
    }
    cout << "}\n";
    return 0;
}