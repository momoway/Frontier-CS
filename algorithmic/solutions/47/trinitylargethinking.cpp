#include <bits/stdc++.h>
using namespace std;

struct Bin {
    int W, H;
    bool allow_rotate;
};

struct Item {
    string type;
    int w, h, v, limit;
};

struct Placement {
    string type;
    int x, y, rot;
};

class Parser {
public:
    string s;
    size_t pos;
    Parser(const string& str) : s(str), pos(0) {}

    void skipWhitespace() {
        while (pos < s.size() && isspace(s[pos])) pos++;
    }

    char peek() {
        skipWhitespace();
        return pos < s.size() ? s[pos] : 0;
    }

    string parseString() {
        skipWhitespace();
        if (s[pos] != '"') throw runtime_error("Expected string");
        pos++;
        string res;
        while (pos < s.size() && s[pos] != '"') {
            res.push_back(s[pos]);
            pos++;
        }
        if (pos >= s.size()) throw runtime_error("Unterminated string");
        pos++; // skip closing quote
        return res;
    }

    int parseInt() {
        skipWhitespace();
        long long val = 0;
        bool neg = false;
        if (s[pos] == '-') { neg = true; pos++; }
        while (pos < s.size() && isdigit(s[pos])) {
            val = val * 10 + (s[pos] - '0');
            pos++;
        }
        if (neg) val = -val;
        return (int)val;
    }

    bool parseBool() {
        skipWhitespace();
        if (pos + 4 <= s.size() && s.compare(pos, 4, "true") == 0) {
            pos += 4;
            return true;
        } else if (pos + 5 <= s.size() && s.compare(pos, 5, "false") == 0) {
            pos += 5;
            return false;
        } else {
            throw runtime_error("Expected boolean");
        }
    }

    void expect(char c) {
        skipWhitespace();
        if (pos >= s.size() || s[pos] != c) throw runtime_error(string("Expected ") + c);
        pos++;
    }

    void parseObject() {
        expect('{');
        while (true) {
            skipWhitespace();
            if (pos >= s.size()) break;
            if (s[pos] == '}') { pos++; break; }
            expect('"');
            string key = parseString();
            expect(':');
            parseValue();
            skipWhitespace();
            if (pos < s.size() && s[pos] == ',') { pos++; continue; }
            else break;
        }
        expect('}');
    }

    void parseArray() {
        expect('[');
        while (true) {
            skipWhitespace();
            if (pos >= s.size()) break;
            if (s[pos] == ']') { pos++; break; }
            parseValue();
            skipWhitespace();
            if (pos < s.size() && s[pos] == ',') { pos++; continue; }
            else break;
        }
        expect(']');
    }

    void parseValue() {
        skipWhitespace();
        if (pos >= s.size()) return;
        char c = s[pos];
        if (c == '{') {
            parseObject();
        } else if (c == '[') {
            parseArray();
        } else if (c == '"') {
            parseString();
        } else if (c == 't' || c == 'f') {
            parseBool();
        } else if (c == '-' || isdigit(c)) {
            parseInt();
        } else if (c == 'n') {
            // null
            pos += 4;
        } else {
            throw runtime_error("Unexpected value");
        }
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string input_str((istreambuf_iterator<char>(cin)), istreambuf_iterator<char>());
    Parser p(input_str);

    Bin bin;
    vector<Item> items;

    try {
        p.expect('{');
        while (true) {
            p.skipWhitespace();
            if (p.peek() == '}') { p.pos++; break; }
            p.expect('"');
            string key = p.parseString();
            p.expect(':');
            if (key == "bin") {
                p.expect('{');
                while (true) {
                    p.skipWhitespace();
                    if (p.peek() == '}') { p.pos++; break; }
                    p.expect('"');
                    string innerKey = p.parseString();
                    p.expect(':');
                    if (innerKey == "W") {
                        bin.W = p.parseInt();
                    } else if (innerKey == "H") {
                        bin.H = p.parseInt();
                    } else if (innerKey == "allow_rotate") {
                        bin.allow_rotate = p.parseBool();
                    } else {
                        p.parseValue();
                    }
                    p.skipWhitespace();
                    if (p.peek() == ',') { p.pos++; continue; }
                    else break;
                }
                p.expect('}');
            } else if (key == "items") {
                p.expect('[');
                while (true) {
                    p.skipWhitespace();
                    if (p.peek() == ']') { p.pos++; break; }
                    p.expect('{');
                    Item item;
                    bool gotType = false, gotW = false, gotH = false, gotV = false, gotLimit = false;
                    while (true) {
                        p.skipWhitespace();
                        if (p.peek() == '}') { p.pos++; break; }
                        p.expect('"');
                        string innerKey = p.parseString();
                        p.expect(':');
                        if (innerKey == "type") {
                            item.type = p.parseString();
                            gotType = true;
                        } else if (innerKey == "w") {
                            item.w = p.parseInt();
                            gotW = true;
                        } else if (innerKey == "h") {
                            item.h = p.parseInt();
                            gotH = true;
                        } else if (innerKey == "v") {
                            item.v = p.parseInt();
                            gotV = true;
                        } else if (innerKey == "limit") {
                            item.limit = p.parseInt();
                            gotLimit = true;
                        } else {
                            p.parseValue();
                        }
                        p.skipWhitespace();
                        if (p.peek() == ',') { p.pos++; continue; }
                        else break;
                    }
                    p.expect('}');
                    if (gotType && gotW && gotH && gotV && gotLimit) {
                        items.push_back(item);
                    }
                    p.skipWhitespace();
                    if (p.peek() == ',') { p.pos++; continue; }
                    else break;
                }
                p.expect(']');
            } else {
                p.parseValue();
            }
            p.skipWhitespace();
            if (p.peek() == ',') { p.pos++; continue; }
            else break;
        }
        p.expect('}');
    } catch (const exception& e) {
        // Invalid JSON, but per problem input is valid; just exit.
        return 1;
    }

    // Sort items by profit density (value / area) descending.
    sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
        double da = (double)a.v / ((long long)a.w * a.h);
        double db = (double)b.v / ((long long)b.w * b.h);
        if (da != db) return da > db;
        return a.v > b.v; // tie-breaker: higher profit first
    });

    vector<int> sky(bin.W, 0);
    vector<Placement> placements;

    for (const auto& item : items) {
        int remaining = item.limit;
        while (remaining > 0) {
            bool placed = false;
            int best_x = -1, best_y = INT_MAX, best_width, best_height, best_orient;
            // Try both orientations if rotation allowed
            for (int orient = 0; orient <= (bin.allow_rotate ? 1 : 0); ++orient) {
                int width = (orient == 0) ? item.w : item.h;
                int height = (orient == 0) ? item.h : item.w;
                if (width > bin.W || height > bin.H) continue;
                int n = bin.W - width + 1;
                if (n <= 0) continue;
                vector<int> max_win(n);
                deque<int> dq;
                for (int i = 0; i < bin.W; ++i) {
                    if (!dq.empty() && dq.front() <= i - width) dq.pop_front();
                    while (!dq.empty() && sky[dq.back()] <= sky[i]) dq.pop_back();
                    dq.push_back(i);
                    if (i >= width - 1) {
                        int start = i - width + 1;
                        max_win[start] = sky[dq.front()];
                    }
                }
                int min_max = INT_MAX;
                int best_x_orient = -1;
                for (int x = 0; x < n; ++x) {
                    if (max_win[x] < min_max) {
                        min_max = max_win[x];
                        best_x_orient = x;
                    }
                }
                if (best_x_orient == -1) continue;
                if (min_max + height <= bin.H) {
                    if (!placed || min_max < best_y || (min_max == best_y && width < best_width)) {
                        placed = true;
                        best_x = best_x_orient;
                        best_y = min_max;
                        best_width = width;
                        best_height = height;
                        best_orient = orient;
                    }
                }
            }
            if (placed) {
                placements.push_back({item.type, best_x, best_y, best_orient});
                remaining--;
                int new_height = best_y + best_height;
                for (int x = best_x; x < best_x + best_width; ++x) {
                    sky[x] = new_height;
                }
            } else {
                break;
            }
        }
    }

    // Output placements as JSON
    cout << "{ \"placements\": [\n";
    for (size_t i = 0; i < placements.size(); ++i) {
        if (i > 0) cout << ",\n";
        cout << "  {\"type\":\"" << placements[i].type
             << "\",\"x\":" << placements[i].x
             << ",\"y\":" << placements[i].y
             << ",\"rot\":" << placements[i].rot << "}";
    }
    cout << "\n] }\n";

    return 0;
}