// 二维几何手打版 R3：按题取专题；不要把整个目录当作每题前置。
// 接在约定公共头后，GNU++20 -O2，不使用第三方数值库。
// 原始整数坐标每维绝对值、非负圆半径不超过 10^9；浮点结果没有统一绝对误差保证。
// 合并版和专题版不要同时包含，否则会重复定义。
#pragma once
namespace Geo {

// 整数点与向量。输入坐标每维绝对值不超过 10^9；排序按字典序，不加 EPS。缩放乘积及后续调用的范围由调用方保证。
struct P {
    ll x = 0, y = 0;
    bool operator==(const P&) const = default;
    bool operator<(P b) const { return tie(x, y) < tie(b.x, b.y); }
    P operator+(P b) const { return {x + b.x, y + b.y}; }
    P operator-(P b) const { return {x - b.x, y - b.y}; }
    P operator-() const { return {-x, -y}; }
    P operator*(ll k) const { return {x * k, y * k}; }
};

// 整数点积。
i128 dot(P a, P b) { return (i128)a.x * b.x + (i128)a.y * b.y; }

// 整数叉积。三点叉积为正：c 在 a→b 左侧；零：共线；负：右侧。先转 i128 再乘。
i128 cross(P a, P b) { return (i128)a.x * b.y - (i128)a.y * b.x; }
i128 cross(P a, P b, P c) { return cross(b - a, c - a); }

// 距离平方。
i128 dist2(P a, P b) { return dot(a - b, a - b); }

// 整数符号。
int sgn(i128 x) { return (x > 0) - (x < 0); }

// 整数绝对值。本库数值范围内使用，不接受 i128 的最小值。
i128 abs128(i128 x) { return x < 0 ? -x : x; }

// 整数向量逆时针转九十度。
P perp(P p) { return {-p.y, p.x}; }

// 整数向量取反。
P neg(P p) { return -p; }

// 浮点类型。
using ld = long double;

// 圆周率。不手抄长小数。只在用到角度、周长或面积时需要。
const ld PI = acosl(-1);

// 浮点点与向量。只作近似坐标与数值运算；不是整数点 P 的替代品。除法分母非零，旋转单位为弧度。
struct FP {
    ld x = 0, y = 0;
    FP operator+(FP b) const { return {x + b.x, y + b.y}; }
    FP operator-(FP b) const { return {x - b.x, y - b.y}; }
    FP operator*(ld k) const { return {x * k, y * k}; }
    FP operator/(ld k) const { return {x / k, y / k}; }
    FP perp() const { return {-y, x}; }
    ld norm() const { return hypotl(x, y); }
    FP rotate(ld a) const {
        ld c = cosl(a), s = sinl(a);
        return {x * c - y * s, x * s + y * c};
    }
};

// 整数点转浮点点。
FP toFP(P p) { return {(ld)p.x, (ld)p.y}; }

// 浮点点积。
ld dot(FP a, FP b) { return a.x * b.x + a.y * b.y; }

// 浮点叉积。
ld cross(FP a, FP b) { return a.x * b.y - a.y * b.x; }

// 点的输入输出。可选；不用这段时直接读写 p.x、p.y。
istream& operator>>(istream& in, P& p) { return in >> p.x >> p.y; }
ostream& operator<<(ostream& out, P p) { return out << p.x << ' ' << p.y; }
ostream& operator<<(ostream& out, FP p) { return out << p.x << ' ' << p.y; }

// 定点小数解析。合法普通十进制字符串；0≤k≤9，小数最多 k 位。返回精确放大 10^k 后的整数，放大后仍须满足坐标界。
ll parseFixed(const string& s, int k) {
    ll x = 0;
    int f = -1;
    for (char c : s) {
        if (c == '+' || c == '-') continue;
        if (c == '.') f = 0;
        else {
            x = x * 10 + c - '0';
            if (f >= 0) f++;
        }
    }
    if (f < 0) f = 0;
    for (; f < k; f++) x *= 10;
    return s[0] == '-' ? -x : x;
}

// 方向排序。非零整数向量，从正 x 轴起逆时针；同射线等价，不用 EPS。
bool angleLess(P a, P b) {
    auto half = [](P p) { return p.y < 0 || (p.y == 0 && p.x < 0); };
    if (half(a) != half(b)) return half(a) < half(b);
    return cross(a, b) > 0;
}

// 极角与长度排序。允许零向量并把它排在最前；其他向量按极角，同射线按长度。
bool polarLess(P a, P b) {
    if (a == P{} || b == P{}) return a == P{} && b != P{};
    auto half = [](P p) { return p.y < 0 || (p.y == 0 && p.x < 0); };
    if (half(a) != half(b)) return half(a) < half(b);
    i128 c = cross(a, b);
    return c ? c > 0 : dot(a, a) < dot(b, b);
}

// 向量夹角。夹角要求两个向量非零，返回 [0,π] 内弧度；rotate 的后两参为余弦、正弦。
ld angle(P a, P b) { return atan2l((ld)abs128(cross(a, b)), (ld)dot(a, b)); }
ld angle(FP a, FP b) { return atan2l(fabsl(cross(a, b)), dot(a, b)); }
FP rotate(FP p, ld c, ld s) { return {p.x * c - p.y * s, p.x * s + p.y * c}; }

// 点在闭线段上。包含端点，允许 a==b。点积版本在整数输入界内精确。
bool onSegment(P p, P a, P b) {
    return cross(a, b, p) == 0 && dot(p - a, p - b) <= 0;
}

// 闭线段相交。端点接触、共线重叠、零长度都支持；返回是否相交。
bool segIntersect(P a, P b, P c, P d) {
    if (max(min(a.x, b.x), min(c.x, d.x)) > min(max(a.x, b.x), max(c.x, d.x)))
        return false;
    if (max(min(a.y, b.y), min(c.y, d.y)) > min(max(a.y, b.y), max(c.y, d.y)))
        return false;
    return sgn(cross(a, b, c)) * sgn(cross(a, b, d)) <= 0 &&
           sgn(cross(c, d, a)) * sgn(cross(c, d, b)) <= 0;
}

// 有向直线。Line{a,b} 是两个不同的点，不是点加方向；左侧为正。
struct Line {
    P a, b; // 要求 a != b，表示从 a 指向 b 的无限直线
    P dir() const { return b - a; }
    int side(P p) const { return sgn(cross(a, b, p)); }
    bool operator==(const Line& l) const {
        return cross(dir(), l.dir()) == 0 && side(l.a) == 0;
    }
};

// 闭线段结构。允许 a==b。只需四点版判交时不用此结构。
struct Segment { P a, b; };

// 点在线段上的分类。0 不在，1 严格内部，-1 端点。
int onSegmentType(P p, Segment s) {
    if (p == s.a || p == s.b) return -1;
    return onSegment(p, s.a, s.b) ? 1 : 0;
}

// 线段与直线关系。0 无交，1 严格穿过，-1 经过端点或包含整段。
int segmentLineType(Segment s, Line l) {
    int a = l.side(s.a), b = l.side(s.b);
    if (a == 0 || b == 0) return -1;
    return a != b;
}

// 两线段关系分类。0 无交，1 内部严格交叉，-1 端点接触或共线重叠。
int segmentType(Segment s, Segment t) {
    if (!segIntersect(s.a, s.b, t.a, t.b)) return 0;
    if (onSegment(t.a, s.a, s.b) || onSegment(t.b, s.a, s.b) ||
        onSegment(s.a, t.a, t.b) || onSegment(s.b, t.a, t.b)) return -1;
    return 1;
}

// 严格凸包。O(n log n)，逆时针，不重复首点、不保留边上中间共线点；支持空集、重复点和全共线。
vector<P> convexHull(vector<P> p) {
    sort(p.begin(), p.end());
    p.erase(unique(p.begin(), p.end()), p.end());
    int n = p.size();
    if (n <= 1) return p;
    vector<P> h;
    auto add = [&](P q, int limit) {
        while ((int)h.size() > limit &&
               cross(h[h.size() - 2], h.back(), q) <= 0) h.pop_back();
        h.pb(q);
    };
    for (P q : p) add(q, 1);
    int k = h.size();
    for (int i = n - 2; i >= 0; i--) add(p[i], k);
    h.pop_back();
    return h;
}

// 多边形二倍有向面积。O(n)，顶点按边界顺序，不重复首点；逆时针为正。
i128 area2(const vector<P>& p) {
    i128 s = 0;
    int n = p.size();
    for (int i = 0; i < n; i++) s += cross(p[i], p[(i + 1) % n]);
    return s;
}

// 回转数。O(n)，返回 {是否在边界, 回转数}；边界时回转数按 0 返回。
pair<bool, int> winding(const vector<P>& p, P q) {
    int w = 0, n = p.size();
    for (int i = 0; i < n; i++) {
        P a = p[i], b = p[(i + 1) % n];
        if (onSegment(q, a, b)) return {true, 0};
        i128 c = cross(a, b, q);
        if (a.y <= q.y && q.y < b.y && c > 0) w++;
        if (b.y <= q.y && q.y < a.y && c < 0) w--;
    }
    return {false, w};
}

// 点在简单多边形内。O(n)，-1 外部、0 边界、1 内部；顺时针、逆时针均可。
int inPolygon(const vector<P>& p, P q) {
    auto [boundary, w] = winding(p, q);
    return boundary ? 0 : (w == 0 ? -1 : 1);
}

// 点在严格凸包内。O(log n)，要求严格逆时针凸包；-1 外部、0 边界、1 内部，支持空集、点和线段。
int inConvex(const vector<P>& h, P q) {
    int n = h.size();
    if (n == 0) return -1;
    if (n <= 2) return onSegment(q, h[0], h.back()) ? 0 : -1;
    i128 a = cross(h[0], h[1], q), b = cross(h[0], h.back(), q);
    if (a < 0 || b > 0) return -1;
    if (a == 0) return onSegment(q, h[0], h[1]) ? 0 : -1;
    if (b == 0) return onSegment(q, h[0], h.back()) ? 0 : -1;
    int l = 1, r = n - 1;
    while (r - l > 1) {
        int m = l + (r - l) / 2;
        if (cross(h[0], h[m], q) >= 0) l = m;
        else r = m;
    }
    return sgn(cross(h[l], h[r], q));
}

// 面积补偿求和。仅减轻累加舍入误差，不恢复已经算丢的有效数字。
struct Sum {
    ld s = 0, c = 0;
    void add(ld x) {
        ld t = s + x;
        if (abs(s) >= abs(x)) c += (s - t) + x;
        else c += (x - t) + s;
        s = t;
    }
    ld value() const { return s + c; }
};

// 边界周长。O(n)，空集、单点为 0，两点为线段长的两倍；浮点近似，补偿累加。
ld perimeter(const vector<P>& p) {
    Sum ans;
    for (int i = 0, n = p.size(); i < n; i++)
        ans.add(sqrtl((ld)dist2(p[i], p[(i + 1) % n])));
    return ans.value();
}

// 点对结果。d2 为距离平方；点下标属于哪个数组由调用函数说明。
struct PairResult { i128 d2 = -1; int a = -1, b = -1; };

// 凸包直径。O(h)，严格逆时针凸包；返回凸包下标。空集 d2=-1，单点 d2=0。
PairResult diameter(const vector<P>& h) {
    int n = h.size();
    if (n == 0) return {};
    PairResult ans{0, 0, 0};
    auto update = [&](int a, int b) {
        i128 d = dist2(h[a], h[b]);
        if (d > ans.d2) ans = {d, a, b};
    };
    if (n <= 2) { update(0, n - 1); return ans; }
    for (int i = 0, j = 1; i < n; i++) {
        int k = (i + 1) % n;
        auto value = [&](int t) { return cross(h[i], h[k], h[t]); };
        while (value((j + 1) % n) > value(j)) j = (j + 1) % n;
        update(i, j);
        update(k, j);
        if (value((j + 1) % n) == value(j)) {
            update(i, (j + 1) % n);
            update(k, (j + 1) % n);
        }
    }
    return ans;
}

// 平面最近点对。O(n log n) 时间、O(n) 空间；返回原输入下标，不修改输入；不足两点 d2=-1。
// 内部坐标用 int、距离平方用 ll：只在每维原坐标绝对值不超过 10^9 时使用；平方和最多 8×10^18。
PairResult closestPair(const vector<P>& p) {
    int n = p.size();
    if (n < 2) return {};
    struct Node { int x, y, id; };
    vector<Node> a(n), tmp(n);
    for (int i = 0; i < n; i++) a[i] = {(int)p[i].x, (int)p[i].y, i};
    sort(a.begin(), a.end(), [](Node a, Node b) { return tie(a.x, a.y) < tie(b.x, b.y); });
    for (int i = 1; i < n; i++)
        if (a[i].x == a[i - 1].x && a[i].y == a[i - 1].y) return {0, a[i - 1].id, a[i].id};
    PairResult ans{dist2(p[0], p[1]), 0, 1};
    auto update = [&](Node a, Node b) {
        ll x = (ll)a.x - b.x, y = (ll)a.y - b.y;
        ll d = x * x + y * y;
        if (d < ans.d2) ans = {d, a.id, b.id};
    };
    auto byY = [](Node a, Node b) { return tie(a.y, a.x) < tie(b.y, b.x); };
    auto solve = [&](auto&& self, int l, int r) -> void {
        if (r - l <= 3) {
            for (int i = l; i < r; i++)
                for (int j = i + 1; j < r; j++) update(a[i], a[j]);
            sort(a.begin() + l, a.begin() + r, byY);
            return;
        }
        int m = l + (r - l) / 2, x = a[m].x;
        // 递归返回后按 y 有序，必须提前保存分割线 x。
        self(self, l, m);
        self(self, m, r);
        merge(a.begin() + l, a.begin() + m, a.begin() + m, a.begin() + r,
              tmp.begin() + l, byY);
        copy(tmp.begin() + l, tmp.begin() + r, a.begin() + l);
        int k = 0;
        for (int i = l; i < r; i++) {
            ll dx = (ll)a[i].x - x;
            if (dx * dx >= ans.d2) continue;
            for (int j = k - 1; j >= 0; j--) {
                ll dy = (ll)a[i].y - tmp[j].y;
                if (dy * dy >= ans.d2) break;
                update(a[i], tmp[j]);
            }
            tmp[k++] = a[i];
        }
    };
    solve(solve, 0, n);
    return ans;
}

// 有理点。(x/d,y/d)，分母转为正；不约分。仅需要精确交点等构造时才写，不是常驻基础。
struct Q {
    i128 x = 0, y = 0, d = 1;
    Q(i128 x = 0, i128 y = 0, i128 d = 1) : x(x), y(y), d(d) {
        if (d < 0) {
            this->x = -x;
            this->y = -y;
            this->d = -d;
        }
    }
    // 把有理坐标转为 long double 近似值；需要比较时使用 qEqual、qLess。
    FP value() const {
        // 先精确分离整数部分，避免大分子转浮点时提前丢掉低位。
        auto part = [&](i128 a) { return (ld)(a / d) + (ld)(a % d) / (ld)d; };
        return {part(x), part(y)};
    }
};

// 两直线交点。a!=b、c!=d。type：0 平行不重合，1 唯一交点，-1 重合；仅 1 时 p 有效。
struct LineHit { int type; Q p; };
LineHit lineInter(P a, P b, P c, P d) {
    P v = b - a, w = d - c;
    i128 den = cross(v, w), t = cross(c - a, w);
    if (den == 0) return {cross(v, c - a) == 0 ? -1 : 0, {}};
    return {1, {(i128)a.x * den + (i128)v.x * t,
                (i128)a.y * den + (i128)v.y * t, den}};
}

// 线段的交集。长度 0 无交、1 单点、2 重叠段两端点；允许零长度。
vector<Q> segInter(P a, P b, P c, P d) {
    if (!segIntersect(a, b, c, d)) return {};
    if (cross(b - a, d - c) != 0) return {lineInter(a, b, c, d).p};
    vector<P> v;
    for (P p : {a, b}) if (onSegment(p, c, d)) v.pb(p);
    for (P p : {c, d}) if (onSegment(p, a, b)) v.pb(p);
    sort(v.begin(), v.end());
    vector<Q> ans{{v.front().x, v.front().y}};
    if (v.front() != v.back()) ans.pb({v.back().x, v.back().y});
    return ans;
}

// 点到直线的投影。a!=b，返回精确有理点；不是投影到线段。
Q projection(P p, P a, P b) {
    P v = b - a;
    i128 d = dot(v, v), t = dot(p - a, v);
    return {(i128)a.x * d + (i128)v.x * t,
            (i128)a.y * d + (i128)v.y * t, d};
}

// 关于直线的对称点。a!=b，返回精确有理点。
Q reflection(P p, P a, P b) {
    Q q = projection(p, a, b);
    return {2 * q.x - (i128)p.x * q.d, 2 * q.y - (i128)p.y * q.d, q.d};
}

// 点到直线距离。a!=b，返回近似长度。
ld lineDistance(P p, P a, P b) {
    return fabsl((ld)cross(a, b, p)) / sqrtl((ld)dist2(a, b));
}

// 点到线段距离。允许零长度，返回近似长度。
ld segmentDistance(P p, P a, P b) {
    if (a == b || dot(p - a, b - a) <= 0) return sqrtl((ld)dist2(p, a));
    if (dot(p - b, a - b) <= 0) return sqrtl((ld)dist2(p, b));
    return lineDistance(p, a, b);
}

// 线段间距离。允许零长度；相交时为 0。
ld segmentDistance(P a, P b, P c, P d) {
    if (segIntersect(a, b, c, d)) return 0;
    return min({segmentDistance(a, c, d), segmentDistance(b, c, d),
                segmentDistance(c, a, b), segmentDistance(d, a, b)});
}

// 三点外心。共线（含重复点）返回 nullopt，否则精确有理外心。
optional<Q> circumcenter(P a, P b, P c) {
    P v = b - a, w = c - a;
    i128 d = 2 * cross(v, w), u = dot(v, v), t = dot(w, w);
    if (d == 0) return nullopt;
    return Q{(i128)a.x * d + u * w.y - t * v.y,
             (i128)a.y * d + t * v.x - u * w.x, d};
}

// 闵可夫斯基和。O(n+m)，输入、输出均为严格逆时针凸包；允许退化。相加后坐标界会扩大。
vector<P> minkowski(vector<P> a, vector<P> b) {
    if (a.empty() || b.empty()) return {};
    auto prepare = [](vector<P>& p) {
        auto it = min_element(p.begin(), p.end(), [](P a, P b) {
            return tie(a.y, a.x) < tie(b.y, b.x);
        });
        rotate(p.begin(), it, p.end());
    };
    if (a.size() == 1) { for (P& p : b) p = p + a[0]; return b; }
    if (b.size() == 1) { for (P& p : a) p = p + b[0]; return a; }
    prepare(a); prepare(b);
    int n = a.size(), m = b.size(), i = 0, j = 0;
    P cur = a[0] + b[0];
    vector<P> res;
    while (i < n || j < m) {
        res.pb(cur);
        P u = a[(i + 1) % n] - a[i % n];
        P v = b[(j + 1) % m] - b[j % m];
        if (j == m || (i < n && angleLess(u, v))) cur = cur + u, i++;
        else if (i == n || angleLess(v, u)) cur = cur + v, j++;
        else cur = cur + u + v, i++, j++;
    }
    rotate(res.begin(), min_element(res.begin(), res.end()), res.end());
    return res;
}

// 凸包极点二分。内部辅助，方向规则须使用下方包装；不要把任意函数当作 dir。
template<class F> int extreme(const vector<P>& h, F dir) {
    int n = h.size();
    if (n == 0) return -1;
    if (n <= 2) return n == 2 && cross(dir(h[0]), h[1] - h[0]) > 0 ? 1 : 0;
    auto check = [&](int i) { return cross(dir(h[i]), h[(i + 1) % n] - h[i]) >= 0; };
    P d = dir(h[0]);
    bool c0 = check(0);
    if (!c0 && check(n - 1)) return 0;
    auto before = [&](int i) {
        if (i == 0) return true;
        bool c = check(i);
        i128 t = cross(d, h[i] - h[0]);
        if (i == 1 && c == c0 && t == 0) return true;
        return bool(c ^ (c == c0 && t <= 0));
    };
    int l = 0, r = n;
    while (l < r) {
        int m = l + (r - l) / 2;
        if (before(m)) l = m + 1;
        else r = m;
    }
    return l % n;
}

// 给定方向的极点。O(log h)，非零方向、严格凸包；返回最大点积下标，空集 -1，平局任取。
int support(const vector<P>& h, P v) {
    return extreme(h, [&](P) { return P{v.y, -v.x}; });
}

// 平行方向的凸包切线。O(log h)，返回最大、最小叉积的下标；空集 {-1,-1}。
pair<int, int> parallelTangents(const vector<P>& h, P v) {
    return {extreme(h, [&](P) { return v; }), extreme(h, [&](P) { return neg(v); })};
}

// 过外点的凸包切线。O(log h)，严格凸包；返回两切点下标，点在内部或边界返回 {-1,-1}。
pair<int, int> tangentFrom(const vector<P>& h, P q) {
    if (h.empty() || inConvex(h, q) >= 0) return {-1, -1};
    return {extreme(h, [&](P p) { return p - q; }),
            extreme(h, [&](P p) { return q - p; })};
}

// 整数圆。整数圆心，0≤r≤10^9；几何关系精确，坐标和面积输出另用浮点。
struct Circle { P c; ll r; };

// 圆的周长与面积。
ld circumference(Circle c) { return 2 * PI * c.r; }
ld circleArea(Circle c) { return PI * c.r * c.r; }

// 点与圆盘。-1 圆外、0 圆上、1 圆内。
int inCircle(Circle c, P p) { return sgn((i128)c.r * c.r - dist2(c.c, p)); }

// 直线与圆周。0 相离、1 相切、2 两点相交；a!=b；四次中间量须遵守 10^9 输入界。
int circleLineRelation(Circle c, Line l) {
    P v = l.dir();
    i128 z = cross(v, c.c - l.a);
    return 1 + sgn((i128)c.r * c.r * dot(v, v) - z * z);
}

// 圆周关系枚举。相同、相离、外切、相交、内切、严格内含；与交点个数无关。
enum class CircleRelation { Equal, Separate, ExternalTouch, Intersect, InternalTouch, Contained };

// 两圆周的位置。先判断相同，再外切，再内切；零半径也遵循此顺序。
CircleRelation circleRelation(Circle a, Circle b) {
    i128 d = dist2(a.c, b.c);
    if (d == 0 && a.r == b.r) return CircleRelation::Equal;
    i128 sum = (i128)(a.r + b.r) * (a.r + b.r);
    i128 dif = (i128)(a.r - b.r) * (a.r - b.r);
    if (d > sum) return CircleRelation::Separate;
    if (d == sum) return CircleRelation::ExternalTouch;
    if (d < dif) return CircleRelation::Contained;
    if (d == dif) return CircleRelation::InternalTouch;
    return CircleRelation::Intersect;
}

// 圆交点结果。count 为 0/1/2；-1 表示正半径圆重合，此时 p 为空。坐标近似。
struct CircleHit { int count = 0; vector<FP> p; };

// 圆与直线交点。a!=b；交点个数精确，坐标近似。
CircleHit circleLine(P o, ll r, P a, P b) {
    P v = b - a;  // 要求 a != b，直线不能退化为一个点
    i128 d = dot(v, v), c = cross(v, o - a);
    i128 h = (i128)r * r * d - c * c;
    if (h < 0) return {};
    FP mid = projection(o, a, b).value();
    if (h == 0) return {1, {mid}};
    FP off = toFP(v) * (sqrtl((ld)h) / (ld)d);
    return {2, {mid - off, mid + off}};
}

// 两圆交点。同一零半径圆返回一个点；不同交点的顺序不保证。
CircleHit circleCircle(P a, ll r, P b, ll s) {
    i128 d = dist2(a, b);
    if (d == 0) {
        if (r != s) return {};
        if (r == 0) return {1, {toFP(a)}};
        return {-1, {}};
    }
    i128 sum = (i128)(r + s) * (r + s), dif = (i128)(r - s) * (r - s);
    if (d > sum || d < dif) return {};
    P v = b - a;
    i128 t = d + (i128)r * r - (i128)s * s;
    FP mid = Q{2 * (i128)a.x * d + (i128)v.x * t,
               2 * (i128)a.y * d + (i128)v.y * t, 2 * d}.value();
    if (d == sum || d == dif) return {1, {mid}};
    ld k = sqrtl((ld)(sum - d) * (ld)(d - dif)) / (2 * (ld)d);
    FP off = toFP(v).perp() * k;
    return {2, {mid - off, mid + off}};
}

// 浮点构造直线。点 p 加非零方向 v；与整数 Line 的两点表示不同。
struct FLine { FP p, v; };

// 浮点构造圆。只作近似构造结果，不直接传给整数 Circle 的接口。
struct FCircle { FP c; ld r; };

// 公切线结果。a、b 是两圆切点；相切时可重合，应使用独立的 line.v。count=-1 表示无限多条。
struct Tangent { FP a, b; FLine line; };
struct TangentResult { int count = 0; vector<Tangent> t; };

// 两圆公切线。O(1)，支持零半径；返回切点与独立方向，均为近似值。
TangentResult commonTangents(Circle a, Circle b) {
    P v = b.c - a.c;
    i128 d = dot(v, v);
    if (d == 0) return {a.r == b.r ? -1 : 0, {}};
    if (a.r == 0 && b.r == 0)
        return {1, {{toFP(a.c), toFP(b.c), {toFP(a.c), toFP(v)}}}};
    vector<Tangent> ans;
    for (int k : {1, -1}) {
        if (k == -1 && (a.r == 0 || b.r == 0)) break;
        ll dr = a.r - k * b.r;
        i128 h = d - (i128)dr * dr;
        if (h < 0) continue;
        for (int sign : {-1, 1}) {
            FP normal = (toFP(v) * (ld)dr + toFP(perp(v)) * (sign * sqrtl((ld)h))) / (ld)d;
            FP p = toFP(a.c) + normal * a.r;
            FP q = toFP(b.c) + normal * (k * b.r);
            ans.pb({p, q, {p, normal.perp()}});
            if (h == 0) break;
        }
    }
    return {(int)ans.size(), ans};
}

// 过一点的圆切线。圆内无切线，圆上通常一条，圆外通常两条；零半径按公切线约定。
TangentResult tangentsFrom(Circle c, P p) { return commonTangents(c, {p, 0}); }

// 反演结果。type：0 无有限结果，1 读取 circle，2 读取 line；反演圆半径须为正。
struct InversionResult { int type = 0; FCircle circle{}; FLine line{}; };

// 点反演。反演中心本身无有限像，返回 nullopt；其他点返回精确有理坐标。
optional<Q> invertPoint(Circle o, P p) {
    P v = p - o.c;
    i128 d = dot(v, v), r2 = (i128)o.r * o.r;
    if (d == 0) return nullopt;
    return Q{(i128)o.c.x * d + (i128)v.x * r2,
             (i128)o.c.y * d + (i128)v.y * r2, d};
}

// 直线反演。反演半径为正；经过反演中心仍为直线，但应排除该中心；结果近似。
InversionResult invertLine(Circle o, Line l) {
    P v = l.dir(), n = perp(v);
    i128 h = cross(v, l.a - o.c), r2 = (i128)o.r * o.r;
    if (h == 0) return {2, {}, {toFP(l.a), toFP(v)}};
    FP c = Q{2 * (i128)o.c.x * h + (i128)n.x * r2,
             2 * (i128)o.c.y * h + (i128)n.y * r2, 2 * h}.value();
    ld r = (ld)r2 * sqrtl((ld)dot(v, v)) / (2 * fabsl((ld)h));
    return {1, {c, r}, {}};
}

// 圆反演。反演半径为正；圆经过中心变为直线，中心处零半径圆无有限像；结果近似。
InversionResult invertCircle(Circle o, Circle a) {
    P v = a.c - o.c;
    i128 d = dot(v, v), den = d - (i128)a.r * a.r, r2 = (i128)o.r * o.r;
    if (den == 0) {
        if (d == 0) return {};
        FP p = Q{2 * (i128)o.c.x * d + (i128)v.x * r2,
                 2 * (i128)o.c.y * d + (i128)v.y * r2, 2 * d}.value();
        return {2, {}, {p, toFP(perp(v))}};
    }
    FP c = Q{(i128)o.c.x * den + (i128)v.x * r2,
             (i128)o.c.y * den + (i128)v.y * r2, den}.value();
    return {1, {c, (ld)r2 * a.r / fabsl((ld)den)}, {}};
}

// 小圆弓形因子。计算 t-sin(t)cos(t)，本库调用 t∈[0,π]；小角度级数避免直接相消。它不是高精度库。
ld circularSegment(ld theta) {
    // 计算 theta - sin(theta) * cos(theta)；小角度用级数避免相消，只在数值输出部分使用。
    if (fabsl(theta) < 1.L / 8) {
        ld term = 2 * theta * theta * theta / 3, ans = term;
        for (int k = 1; ; k++) {
            term *= -4 * theta * theta / ((2 * k + 2) * (2 * k + 3));
            ld next = ans + term;
            if (next == ans) break;
            ans = next;
        }
        return ans;
    }
    ld s = sinl(theta), c = cosl(theta);
    return theta - s * c;
}

// 两圆盘交面积。O(1)，long double 近似；关系判定用整数。没有统一的绝对误差保证。
ld circleIntersectionArea(Circle a, Circle b) {
    if (a.r == 0 || b.r == 0) return 0;
    auto type = circleRelation(a, b);
    if (type == CircleRelation::Separate || type == CircleRelation::ExternalTouch) return 0;
    if (type != CircleRelation::Intersect) {
        ll r = min(a.r, b.r);
        return PI * r * r;
    }
    i128 d = dist2(a.c, b.c), x = (i128)a.r * a.r, y = (i128)b.r * b.r;
    i128 sum = (i128)(a.r + b.r) * (a.r + b.r);
    i128 dif = (i128)(a.r - b.r) * (a.r - b.r);
    ld h = sqrt(ld(sum - d) * ld(d - dif));
    ld alpha = atan2(h, ld(d + x - y));
    ld beta = atan2(h, ld(d + y - x));
    return ld(x) * circularSegment(alpha) + ld(y) * circularSegment(beta);
}

// 浮点两乘积之差的补偿计算。只减轻本次乘法相消，不能修复输入误差，也不是精确几何谓词。
ld crossAccurate(FP a, FP b) {
    ld t = a.y * b.x;
    return fmal(a.x, b.y, -t) + fmal(-a.y, b.x, t);
}

// 只在符号远离舍入误差时走快分支，否则回退补偿叉积；不是用 EPS 宣布共线。
int crossSign(FP a, FP b) {
    ld x = a.x * b.y, y = a.y * b.x, z = x - y;
    if (fabsl(z) <= 4 * numeric_limits<ld>::epsilon() * (fabsl(x) + fabsl(y)))
        z = crossAccurate(a, b);
    return (z > 0) - (z < 0);
}

// 圆与简单无洞多边形交面积。O(n) 时间、O(1) 工作空间；逆时针正、顺时针负，数值近似。
// 按边界顺序连接圆内线段，圆外路径换成短圆弧，再用整数绕数补整圈；弦项与弓形项分开求和。
ld circlePolygonArea(Circle c, const vector<P>& p) {
    if (c.r == 0) return 0;
    i128 rr = (i128)c.r * c.r;
    // 整个多边形都在圆内时只算整数面积，不生成交点、圆弧或补偿乘积。
    if (all_of(p.begin(), p.end(), [&](P a) { return dist2(a, c.c) <= rr; }))
        return (ld)area2(p) / 2;
    int turns = 0, pieces = 0;
    FP origin{}, first{}, last{};
    ld onlyHalf = 0;
    Sum ans;
    // 极角从正 x 轴之后开始，正 x 轴本身排在末尾，与下方半开区间绕数约定一致。
    auto before = [](FP a, FP b) {
        auto half = [](FP p) { return p.y < 0 || (p.y == 0 && p.x >= 0); };
        if (half(a) != half(b)) return half(a) < half(b);
        return crossSign(a, b) > 0;
    };
    auto chord = [&](FP a, FP b) {
        a = a - origin; b = b - origin;
        if (tie(a.x, a.y) < tie(b.x, b.y)) return crossAccurate(a, b) / 2;
        return -crossAccurate(b, a) / 2;
    };
    auto arc = [&](FP a, FP b, ld half) {
        if (a.x == b.x && a.y == b.y) return;
        // 共用圆内顶点时不需要圆弧；通过上面的检查后才计算夹角。
        if (half == 0) {
            ld z = crossAccurate(a, b);
            half = atan2l(z == 0 ? 0 : z, dot(a, b)) / 2;
        }
        if (half > 0 && before(b, a)) turns--;
        if (half < 0 && before(a, b)) turns++;
        ans.add(chord(a, b));
        ans.add((ld)rr * circularSegment(half));
    };
    for (int i = 0, n = p.size(); i < n; i++) {
        P a = p[i] - c.c, b = p[(i + 1) % n] - c.c, v = b - a;
        i128 z = cross(a, b), aa = dot(a, a), bb = dot(b, b);
        if (a.y <= 0 && b.y > 0 && z > 0) turns++;
        if (b.y <= 0 && a.y > 0 && z < 0) turns--;
        if (a == b) continue;
        i128 d = dot(v, v), av = dot(a, v), bv = dot(b, v), h = rr * d - z * z;
        if (!(aa < rr || bb < rr || (h > 0 && av < 0 && bv > 0))) continue;
        ld root = sqrtl((ld)h);
        FP mid = toFP(perp(v)) * (-(ld)z / (ld)d);
        FP off = toFP(v) * (root / (ld)d);
        FP u = aa <= rr ? toFP(a) : mid - off;
        FP w = bb <= rr ? toFP(b) : mid + off;
        // 原边的绕数减去圆内部分，剩下圆外路径的绕数；共线穿心边两项均为零。
        if (u.y <= 0 && w.y > 0 && z > 0) turns--;
        if (w.y <= 0 && u.y > 0 && z < 0) turns++;
        if (pieces == 0) {
            origin = first = u;
            onlyHalf = atan2l(root, (ld)abs128(z));
            if (z > 0) onlyHalf = -onlyHalf;
        } else arc(last, u, 0);
        ans.add(chord(u, w));
        last = w;
        pieces++;
    }
    if (pieces == 0) return turns * PI * (ld)rr;
    // 只有一段圆内边时，直接用它的整数判别式求半角，不从两个近似交点反推小角度。
    arc(last, first, pieces == 1 ? onlyHalf : 0);
    ans.add(turns * PI * (ld)rr);
    return ans.value();
}

// 线性参数分数。仅给原始整数直线参数使用，分子分母绝对值≤8×10^18；该界内交叉比较不溢出。
struct Fraction {
    i128 n = 0, d = 1;
    Fraction(i128 n = 0, i128 d = 1) : n(n), d(d) {
        if (d < 0) this->n = -n, this->d = -d;
    }
    bool operator<(const Fraction& b) const { return n * b.d < b.n * d; }
    bool operator==(const Fraction& b) const { return n * b.d == b.n * d; }
    ld value() const { return (ld)n / (ld)d; }
};

// 分数截到单位区间。
Fraction clamp01(Fraction x) {
    if (x.n < 0) return 0;
    if (x.n > x.d) return 1;
    return x;
}

// 参数点。只用于原始整数直线及受限 Fraction，不是通用有理构造。
Q pointAt(Line l, Fraction t) {
    P v = l.dir();
    return {(i128)l.a.x * t.d + (i128)v.x * t.n,
            (i128)l.a.y * t.d + (i128)v.y * t.n, t.d};
}

// 交点与直线的侧别。只接受原始整数直线产生的交点；不要传反复复合得到的任意 Q。
i128 lineSide(Line l, Q p) {
    P v = l.dir();
    return (i128)v.x * (p.y - (i128)l.a.y * p.d) -
           (i128)v.y * (p.x - (i128)l.a.x * p.d);
}

// 半平面交结果。dim=-1 空、0 点、1 线段、2 有面积多边形；p 为精确有理顶点。
struct HalfPlaneResult {
    int dim = -1; // 维数：-1 空集，0 单点，1 线段，2 有面积多边形
    vector<Q> p;  // 精确有理顶点，不重复首点；dim 为 2 时按逆时针排列
};

// 盒内闭半平面交。O(n log n)，额外与 [-lim,lim]^2 相交，0<lim≤10^9；不判断盒外无界性。
HalfPlaneResult halfPlaneIntersection(vector<Line> a, ll lim) {
    a.pb({{-lim, -lim}, {lim, -lim}});
    a.pb({{lim, -lim}, {lim, lim}});
    a.pb({{lim, lim}, {-lim, lim}});
    a.pb({{-lim, lim}, {-lim, -lim}});
    sort(a.begin(), a.end(), [](Line a, Line b) { return angleLess(a.dir(), b.dir()); });
    vector<Line> v;
    for (Line l : a) {
        if (!v.empty() && cross(v.back().dir(), l.dir()) == 0 && dot(v.back().dir(), l.dir()) > 0) {
            if (v.back().side(l.a) > 0) v.back() = l;
        } else v.pb(l);
    }
    // 方向相反且重合的边界把可行域限制在线上，退化为沿该直线的一维区间求交。
    auto clipLine = [&](Line l) -> HalfPlaneResult {
        optional<Fraction> lo, hi;
        for (Line b : v) {
            i128 c = cross(b.dir(), l.a - b.a), d = cross(b.dir(), l.dir());
            if (d == 0) { if (c < 0) return {}; continue; }
            Fraction t(-c, d);
            if (d > 0) { if (!lo || *lo < t) lo = t; }
            else { if (!hi || t < *hi) hi = t; }
        }
        // 已经加入有限盒子，因此只要交集存在，参数上下界都应为有限值。
        if (!lo || !hi || *hi < *lo) return {};
        if (*lo == *hi) return {0, {pointAt(l, *lo)}};
        return {1, {pointAt(l, *lo), pointAt(l, *hi)}};
    };
    for (Line l : v) {
        P d = neg(l.dir());
        auto it = lower_bound(v.begin(), v.end(), d, [](Line a, P d) { return angleLess(a.dir(), d); });
        if (it == v.end() || cross(it->dir(), d) != 0 || dot(it->dir(), d) <= 0) continue;
        int side = l.side(it->a);
        if (side < 0) return {};
        if (side == 0) return clipLine(l);
    }
    auto inter = [](Line a, Line b) { return lineInter(a.a, a.b, b.a, b.b).p; };
    auto out = [&](Line a, Line b, Line c) { return lineSide(a, inter(b, c)) < 0; };
    deque<Line> q;
    for (Line l : v) {
        while (q.size() > 1 && out(l, q[q.size() - 2], q.back())) q.pop_back();
        while (q.size() > 1 && out(l, q[0], q[1])) q.pop_front();
        if (!q.empty() && cross(q.back().dir(), l.dir()) <= 0) return {};
        q.pb(l);
    }
    while (q.size() > 2 && out(q.front(), q[q.size() - 2], q.back())) q.pop_back();
    while (q.size() > 2 && out(q.back(), q[0], q[1])) q.pop_front();
    int n = q.size();
    if (n < 3 || cross(q.back().dir(), q.front().dir()) <= 0) return {};
    vector<Q> p;
    for (int i = 0; i < n; i++) {
        Line next = q[(i + 1) % n];
        if (p.empty() || lineSide(next, p.back()) != 0) p.pb(inter(q[i], next));
    }
    if (p.size() > 1 && lineSide(q[1], p.back()) == 0) p.pop_back();
    return {(int)p.size() <= 2 ? (int)p.size() - 1 : 2, p};
}

// 有理多边形有向面积。O(n)，浮点近似。整数商之差须在 i128 内（本板原始输入构造满足）。
ld rationalPolygonArea(const vector<Q>& p) {
    if (p.size() < 3) return 0;
    i128 ox = p[0].x / p[0].d, oy = p[0].y / p[0].d;
    // 整数域平移；把整数与余数调成同号，避免 -1+(1-极小量) 的再次相消。
    auto part = [](i128 x, i128 d, i128 origin) {
        i128 q = x / d - origin, r = x % d;
        if (q > 0 && r < 0) q--, r += d;
        if (q < 0 && r > 0) q++, r -= d;
        return (ld)q + (ld)r / (ld)d;
    };
    auto local = [&](Q q) { return FP{part(q.x, q.d, ox), part(q.y, q.d, oy)}; };
    FP o = local(p[0]);
    Sum ans;
    for (int i = 1; i + 1 < (int)p.size(); i++)
        ans.add(crossAccurate(local(p[i]) - o, local(p[i + 1]) - o) / 2);
    return ans.value();
}

// 多线段找相交对。O(n log n)，支持竖直、零长、共线与端点；原输入下标，无交 {-1,-1}，不能直接改成枚举全部交点。
pair<int, int> anySegmentIntersection(vector<Segment> s) {
    int n = s.size();
    // 同一横坐标按“插入非竖直线段、检查竖直线段、删除非竖直线段”处理，保留端点接触。
    struct Event { ll x; int type, id; };
    vector<Event> ev;
    for (int i = 0; i < n; i++) {
        if (s[i].b < s[i].a) swap(s[i].a, s[i].b);
        if (s[i].a.x == s[i].b.x) ev.pb({s[i].a.x, 1, i});
        else {
            ev.pb({s[i].a.x, 0, i});
            ev.pb({s[i].b.x, 2, i});
        }
    }
    sort(ev.begin(), ev.end(), [&](Event a, Event b) {
        if (tie(a.x, a.type) != tie(b.x, b.type)) return tie(a.x, a.type) < tie(b.x, b.type);
        if (a.type == 1 && s[a.id].a.y != s[b.id].a.y) return s[a.id].a.y < s[b.id].a.y;
        return a.id < b.id;
    });
    ll x = 0;
    struct Compare {
        using is_transparent = void;
        const vector<Segment>* s;
        const ll* x;
        pair<i128, ll> height(int i) const {
            auto [a, b] = (*s)[i];
            ll d = b.x - a.x;
            return {(i128)a.y * d + (i128)(b.y - a.y) * (*x - a.x), d};
        }
        bool operator()(int i, int j) const {
            auto [a, b] = height(i); auto [c, d] = height(j);
            if (a * d != c * b) return a * d < c * b;
            return i < j;
        }
        bool operator()(int i, ll y) const {
            auto [a, b] = height(i); return a < (i128)y * b;
        }
        bool operator()(ll y, int i) const {
            auto [a, b] = height(i); return (i128)y * b < a;
        }
    };
    // 活动集合按当前高度排序；相邻线段一旦发现相交就立即返回，不能直接改成枚举所有交点。
    set<int, Compare> active(Compare{&s, &x});
    vector<decltype(active)::iterator> where(n);
    auto hit = [&](int i, int j) { return segIntersect(s[i].a, s[i].b, s[j].a, s[j].b); };
    int lastVertical = -1;
    for (Event e : ev) {
        x = e.x;
        int i = e.id;
        if (e.type == 0) {
            auto it = active.lower_bound(i);
            if (it != active.end() && hit(i, *it)) return {i, *it};
            if (it != active.begin() && hit(i, *prev(it))) return {i, *prev(it)};
            where[i] = active.insert(it, i);
        } else if (e.type == 1) {
            if (lastVertical != -1 && s[lastVertical].a.x == x &&
                s[lastVertical].b.y >= s[i].a.y) return {lastVertical, i};
            lastVertical = i;
            auto it = active.lower_bound(s[i].a.y);
            if (it != active.end() && hit(i, *it)) return {i, *it};
        } else {
            auto it = where[i], right = next(it);
            if (it != active.begin() && right != active.end() && hit(*prev(it), *right))
                return {*prev(it), *right};
            active.erase(it);
        }
    }
    return {-1, -1};
}

// 点集最小及最大三角形。O(n²log n) 时间、O(n²) 空间；三个不同输入下标，允许零面积；返回二倍面积。
pair<i128, i128> minmaxTriangle(vector<P> p) {
    if (p.size() < 3) return {0, 0};
    sort(p.begin(), p.end());
    bool duplicate = adjacent_find(p.begin(), p.end()) != p.end();
    p.erase(unique(p.begin(), p.end()), p.end());
    int n = p.size();
    if (n < 3) return {0, 0};
    vector<pair<int, int>> events;
    events.reserve((size_t)n * (n - 1) / 2);
    for (int i = 0; i < n; i++) for (int j = i + 1; j < n; j++) events.pb({i, j});
    auto direction = [&](pair<int, int> e) { return p[e.second] - p[e.first]; };
    sort(events.begin(), events.end(), [&](auto a, auto b) { return cross(direction(a), direction(b)) > 0; });
    vector<int> order(n), pos(n);
    iota(order.begin(), order.end(), 0); iota(pos.begin(), pos.end(), 0);
    i128 low = duplicate ? 0 : ((i128)1 << 126), high = 0;
    for (size_t l = 0; l < events.size();) {
        size_t r = l + 1;
        while (r < events.size() && cross(direction(events[l]), direction(events[r])) == 0) r++;
        vector<pair<int, int>> ranges;
        for (size_t i = l; i < r; i++) {
            auto [a, b] = events[i];
            ranges.pb(minmax(pos[a], pos[b]));
        }
        sort(ranges.begin(), ranges.end());
        for (int i = 0; i < (int)ranges.size();) {
            auto [a, b] = ranges[i++];
            while (i < (int)ranges.size() && ranges[i].first <= b) b = max(b, ranges[i++].second);
            P u = p[order[a]], v = p[order[b]];
            auto area = [&](int k) { return abs128(cross(u, v, p[order[k]])); };
            high = max({high, area(0), area(n - 1)});
            if (b - a >= 2) low = 0;
            else {
                if (a > 0) low = min(low, area(a - 1));
                if (b + 1 < n) low = min(low, area(b + 1));
            }
            reverse(order.begin() + a, order.begin() + b + 1);
            for (int k = a; k <= b; k++) pos[order[k]] = k;
        }
        l = r;
    }
    return {low, high};
}

// 凸包最大三角形。O(h²)，严格逆时针凸包；返回二倍面积，少于三点为 0。
i128 maxTriangle2(const vector<P>& h) {
    int n = h.size();
    i128 ans = 0;
    for (int i = 0; i + 2 < n; i++) {
        int k = i + 2;
        for (int j = i + 1; j + 1 < n; j++) {
            k = max(k, j + 1);
            while (k + 1 < n && cross(h[i], h[j], h[k + 1]) >= cross(h[i], h[j], h[k])) k++;
            ans = max(ans, cross(h[i], h[j], h[k]));
        }
    }
    return ans;
}

// 多边形分层面积。E 条边：O(E²log E)；简单无洞多边形，顺逆皆可，允许重合与共边。ans[k-1] 为至少覆盖 k 次的面积，数值近似。
vector<ld> polygonCoverageAreas(vector<vector<P>> p) {
    int n = p.size();
    P origin{};
    for (auto& a : p) {
        if (!a.empty()) origin = a[0];
        i128 ar = area2(a);
        if (ar == 0) a.clear(); // 零面积退化多边形不贡献面积
        else if (ar < 0) reverse(a.begin(), a.end());
    }
    vector<Sum> sums(n);
    for (int i = 0; i < n; i++) for (int k = 0; k < (int)p[i].size(); k++) {
        P a = p[i][k], b = p[i][(k + 1) % p[i].size()], v = b - a;
        if (a == b) continue;
        vector<pair<Fraction, int>> ev{{Fraction(0), 0}, {Fraction(1), 0}};
        auto add = [&](Fraction t, int delta) { ev.pb({clamp01(t), delta}); };
        auto parameter = [&](P c) { return v.x != 0 ? Fraction(c.x - a.x, v.x) : Fraction(c.y - a.y, v.y); };
        for (int j = 0; j < n; j++) if (j != i) {
            for (int t = 0; t < (int)p[j].size(); t++) {
                P c = p[j][t], d = p[j][(t + 1) % p[j].size()];
                int sc = sgn(cross(a, b, c)), sd = sgn(cross(a, b, d));
                if (sc != sd && min(sc, sd) < 0) {
                    add(Fraction(cross(c - a, d - c), cross(v, d - c)), sc > sd ? 1 : -1);
                } else if (sc == 0 && sd == 0 && j < i && dot(v, d - c) > 0) {
                    add(parameter(c), 1); add(parameter(d), -1);
                }
            }
        }
        sort(ev.begin(), ev.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
        int count = 0;
        Fraction last = 0;
        ld base = ld(cross(a - origin, b - origin)) / 2;
        for (auto [t, delta] : ev) {
            if (last < t) sums[count].add(base * ld(t.n * last.d - last.n * t.d) / ld(last.d * t.d));
            count += delta;
            last = t;
        }
    }
    vector<ld> ans(n);
    for (int i = 0; i < n; i++) ans[i] = sums[i].value();
    return ans;
}

// 圆分层面积。O(n²log n) 时间、O(n) 工作空间；ans[k-1] 为至少覆盖 k 次的面积，重复圆计重。
// 数值近似：共享交点保持公共弦项一致，同一圆对的小圆弓形直接用半角；仍不保证所有输入的绝对误差。
vector<ld> circleCoverageAreas(vector<Circle> input) {
    int total = input.size();
    sort(input.begin(), input.end(), [](Circle a, Circle b) {
        return tie(a.c.x, a.c.y, a.r) < tie(b.c.x, b.c.y, b.r);
    });
    vector<Circle> c;
    vector<int> weight;
    for (Circle a : input) {
        if (a.r == 0) continue;
        if (!c.empty() && c.back().c == a.c && c.back().r == a.r) weight.back()++;
        else c.pb(a), weight.pb(1);
    }
    // 同一对圆按固定顺序构造交点，弦叉积也固定顺序求值，保证公共弦互为相反项。
    struct Event { FP p, v; int add, other; ld half; };
    auto before = [](FP a, FP b) {
        auto half = [](FP p) { return p.y < 0 || (p.y == 0 && p.x < 0); };
        if (half(a) != half(b)) return half(a) < half(b);
        return crossSign(a, b) > 0;
    };
    // 不相交的圆盘连通分量各取自己的整数原点，避免无关的远处圆放大弦积分误差。
    vector<int> parent(c.size());
    iota(parent.begin(), parent.end(), 0);
    auto find = [&](int x) {
        while (parent[x] != x) x = parent[x] = parent[parent[x]];
        return x;
    };
    for (int i = 0; i < (int)c.size(); i++) for (int j = 0; j < i; j++) {
        ll r = c[i].r + c[j].r;
        if (dist2(c[i].c, c[j].c) <= (i128)r * r)
            parent[find(i)] = find(j);
    }
    vector<Sum> diff(total + 1);
    for (int i = 0; i < (int)c.size(); i++) {
        vector<Event> ev;
        P origin = c[find(i)].c;
        int count = 0;
        for (int j = 0; j < (int)c.size(); j++) if (i != j) {
            ll ri = c[i].r, rj = c[j].r;
            i128 d = dist2(c[i].c, c[j].c);
            i128 sum = (i128)(ri + rj) * (ri + rj), dif = (i128)(ri - rj) * (ri - rj);
            if (rj >= ri && d <= dif) { count += weight[j]; continue; }
            if (d >= sum || d <= dif) continue;
            ld root = sqrtl((ld)(sum - d) * (ld)(d - dif));
            int lo = min(i, j), hi = max(i, j);
            P v = c[hi].c - c[lo].c, o = c[lo].c - origin;
            i128 t = d + (i128)c[lo].r * c[lo].r - (i128)c[hi].r * c[hi].r;
            FP mid = Q{2 * (i128)o.x * d + (i128)v.x * t,
                       2 * (i128)o.y * d + (i128)v.y * t, 2 * d}.value();
            FP off = toFP(perp(v)) * (root / (2 * (ld)d));
            FP a = mid - off, b = mid + off;
            if (i == hi) swap(a, b);
            v = c[j].c - c[i].c;
            t = d + (i128)ri * ri - (i128)rj * rj;
            FP base = toFP(v) * ((ld)t / (2 * (ld)d));
            off = toFP(perp(v)) * (root / (2 * (ld)d));
            FP u = base - off, w = base + off;
            if (before(w, u)) count += weight[j];
            ev.pb({a, u, weight[j], j, atan2l(root, (ld)t)});
            ev.pb({b, w, -weight[j], j, atan2l(root, -(ld)t)});
        }
        auto add = [&](int level, ld value) {
            diff[level].add(value);
            diff[level + weight[i]].add(-value);
        };
        if (ev.empty()) { add(count, PI * c[i].r * c[i].r); continue; }
        sort(ev.begin(), ev.end(), [&](const Event& a, const Event& b) { return before(a.v, b.v); });
        for (int k = 0, m = ev.size(); k < m; k++) {
            const Event &a = ev[k], &b = ev[(k + 1) % m];
            count += a.add;
            ld half;
            if (a.other == b.other) half = a.half;
            else {
                ld angle = atan2l(crossAccurate(a.v, b.v), dot(a.v, b.v));
                if (angle < 0) angle += 2 * PI;
                half = angle / 2;
            }
            // 弦项和弓形项分开补偿累加；不要先把大弦项与小面积相加。
            FP u = a.p, v = b.p;
            ld chord = tie(u.x, u.y) < tie(v.x, v.y) ? crossAccurate(u, v) : -crossAccurate(v, u);
            add(count, chord / 2);
            add(count, (ld)c[i].r * c[i].r * circularSegment(half));
        }
    }
    vector<ld> ans(total);
    Sum sum;
    for (int i = 0; i < total; i++) { sum.add(diff[i].s); sum.add(diff[i].c); ans[i] = sum.value(); }
    return ans;
}

// 不交叉相乘的分数比较。比较 a/b、c/d，b,d>0；返回 -1/0/1。欧几里得法 O(log M)，用于大分数比较，普通凸包不需要。
int compareFractions(i128 a, i128 b, i128 c, i128 d) {
    if ((a < 0) != (c < 0)) return a < 0 ? -1 : 1;
    int sign = a < 0 ? -1 : 1;
    u128 x = a < 0 ? u128(-(a + 1)) + 1 : u128(a);
    u128 y = b, z = c < 0 ? u128(-(c + 1)) + 1 : u128(c), w = d;
    for (;;) {
        u128 q = x / y, r = z / w;
        if (q != r) return sign * (q < r ? -1 : 1);
        x %= y; z %= w;
        if (x == 0 || z == 0) return sign * ((x > 0) - (z > 0));
        swap(x, y); swap(z, w); sign = -sign;
    }
}

// 有理点判等。
bool qEqual(Q a, Q b) {
    return compareFractions(a.x, a.d, b.x, b.d) == 0 &&
           compareFractions(a.y, a.d, b.y, b.d) == 0;
}

// 有理点排序。按真实坐标字典序，不转浮点。
bool qLess(Q a, Q b) {
    int t = compareFractions(a.x, a.d, b.x, b.d);
    return t ? t < 0 : compareFractions(a.y, a.d, b.y, b.d) < 0;
}

// 整数圆内判定。abc 非共线；1 圆内、0 圆上、-1 圆外。原始每维 |坐标|≤10^9，四次中间量在 i128 范围内。
int inCircumcircle(P a, P b, P c, P p) {
    int o = sgn(cross(a, b, c));
    a = a - p; b = b - p; c = c - p;
    i128 z = dot(a, a) * cross(b, c) + dot(b, b) * cross(c, a)
           + dot(c, c) * cross(a, b);
    return o * sgn(z);
}

// 最小覆盖圆结果。空集 count=0、radius=-1；非空支持点 1..3。圆心精确，半径近似，contains 用原始支持点精确判断。
struct EnclosingCircle {
    Q center;
    ld radius = -1; // 空输入时为 -1；非空时为近似半径，包含判断不要依赖它
    int count = 0;
    array<int, 3> id{-1, -1, -1}; // id[0..count) 为原输入的支持点下标；其余为 -1
    array<P, 3> basis{};          // 支持点副本；原输入销毁后，仍可进行包含判断
    // 精确判断整数点是否在闭圆盘内，包含圆周；不使用近似圆心或半径，空圆返回 false。
    bool contains(P p) const {
        if (count == 0) return false;
        if (count == 1) return p == basis[0];
        if (count == 2) return dot(p - basis[0], p - basis[1]) <= 0;
        return inCircumcircle(basis[0], basis[1], basis[2], p) >= 0;
    }
};

// 随机增量最小圆覆盖。独立随机排列下期望 O(n)，最坏 O(n³)。seed 便于复现；不依赖近似圆心做包含判断。
EnclosingCircle minimumEnclosingCircle(const vector<P>& p, ull seed) {
    int n = p.size();
    vector<int> order(n);
    iota(order.begin(), order.end(), 0);
    mt19937_64 rng(seed);
    shuffle(order.begin(), order.end(), rng);
    EnclosingCircle ans;
    // 循环中只维护支持点，不反复构造浮点圆；三支持点共线时约简为最远的两个端点。
    auto setBasis = [&](int a, int b = -1, int c = -1) {
        ans.id = {a, b, c}; ans.count = 1 + (b >= 0) + (c >= 0);
        for (int i = 0; i < ans.count; i++) ans.basis[i] = p[ans.id[i]];
        if (ans.count == 3 && cross(p[a], p[b], p[c]) == 0) {
            if (dist2(p[a], p[c]) > dist2(p[a], p[b])) swap(b, c);
            if (dist2(p[b], p[c]) > dist2(p[a], p[b])) a = c;
            ans.id = {a, b, -1}; ans.basis = {p[a], p[b], P{}}; ans.count = 2;
        }
    };
    for (int i = 0; i < n; i++) if (!ans.contains(p[order[i]])) {
        setBasis(order[i]);
        for (int j = 0; j < i; j++) if (!ans.contains(p[order[j]])) {
            setBasis(order[i], order[j]);
            for (int k = 0; k < j; k++) if (!ans.contains(p[order[k]]))
                setBasis(order[i], order[j], order[k]);
        }
    }
    if (ans.count == 0) return ans;
    P a = ans.basis[0], b = ans.basis[1];
    if (ans.count == 1) { ans.center = {a.x, a.y}; ans.radius = 0; }
    if (ans.count == 2) {
        ans.center = {(i128)a.x + b.x, (i128)a.y + b.y, 2};
        ans.radius = sqrtl((ld)dist2(a, b)) / 2;
    }
    if (ans.count == 3) {
        ans.center = *circumcenter(a, b, ans.basis[2]);
        Q q = ans.center;
        ans.radius = hypotl((ld)(q.x - (i128)a.x * q.d) / (ld)q.d,
                            (ld)(q.y - (i128)a.y * q.d) / (ld)q.d);
    }
    return ans;
}

// 最小宽度结果。num/den 是宽度平方；edge、opposite 是凸包下标；value 才开平方。
struct WidthResult {
    i128 num = 0, den = 1; // 精确宽度平方为 num/den，不是宽度本身
    int edge = -1, opposite = -1;
    ld value() const { return sqrtl((ld)num / (ld)den); }
};

// 凸包最小宽度。严格逆时针凸包；O(h) 卡壳推进，每次分数比较另需 O(log B)；空集、点、线段宽度为零。
WidthResult minimumWidth(const vector<P>& h) {
    int n = h.size();
    WidthResult ans;
    if (n <= 2) { if (n) ans.edge = 0, ans.opposite = n - 1; return ans; }
    for (int i = 0, j = 1; i < n; i++) {
        int k = (i + 1) % n;
        while (cross(h[i], h[k], h[(j + 1) % n]) > cross(h[i], h[k], h[j])) j = (j + 1) % n;
        i128 v = cross(h[i], h[k], h[j]), a = v * v, b = dist2(h[i], h[k]);
        if (ans.edge < 0 || compareFractions(a, b, ans.num, ans.den) < 0)
            ans = {a, b, i, j};
    }
    return ans;
}

// 外接矩形结果。num/den 是精确面积；corner 是四个有理角点；空集 edge=-1。
struct RectangleResult {
    i128 num = 0, den = 1; // 精确面积为 num/den，不是面积平方
    int edge = -1;
    array<Q, 4> corner{}; // 四个精确有理顶点按逆时针排列；退化为点或线段时允许重复
    ld area() const { return (ld)num / (ld)den; }
};

// 最小面积外接矩形。严格逆时针凸包；O(h) 卡壳推进，每次分数比较另需 O(log B)；支持点、线段退化。
RectangleResult minimumRectangle(const vector<P>& h) {
    int n = h.size();
    RectangleResult ans;
    if (n == 0) return ans;
    if (n == 1) { ans.edge = 0; ans.corner.fill(Q(h[0].x, h[0].y)); return ans; }
    int top = 0, lo = 0, hi = 0;
    P first = h[1] - h[0];
    for (int j = 0; j < n; j++) {
        if (cross(first, h[j] - h[0]) > cross(first, h[top] - h[0])) top = j;
        if (dot(first, h[j]) < dot(first, h[lo])) lo = j;
        if (dot(first, h[j]) > dot(first, h[hi])) hi = j;
    }
    for (int i = 0; i < n; i++) {
        P v = h[(i + 1) % n] - h[i], w = perp(v);
        while (cross(v, h[(top + 1) % n] - h[i]) > cross(v, h[top] - h[i])) top = (top + 1) % n;
        while (dot(v, h[(lo + 1) % n]) < dot(v, h[lo])) lo = (lo + 1) % n;
        while (dot(v, h[(hi + 1) % n]) > dot(v, h[hi])) hi = (hi + 1) % n;
        i128 l = dot(v, h[lo] - h[i]), r = dot(v, h[hi] - h[i]);
        i128 height = cross(v, h[top] - h[i]), den = dot(v, v), num = (r - l) * height;
        if (ans.edge >= 0 && compareFractions(num, den, ans.num, ans.den) >= 0) continue;
        ans.num = num; ans.den = den; ans.edge = i;
        auto point = [&](i128 x, i128 y) {
            return Q((i128)h[i].x * den + (i128)v.x * x + (i128)w.x * y,
                     (i128)h[i].y * den + (i128)v.y * x + (i128)w.y * y, den);
        };
        ans.corner = {point(l, 0), point(r, 0), point(r, height), point(l, height)};
    }
    return ans;
}

// 简单多边形重心。O(n)，有向面积为零返回 nullopt；不是顶点平均值，返回精确有理重心。
optional<Q> centroid(const vector<P>& p) {
    i128 a = area2(p), x = 0, y = 0;
    if (a == 0) return nullopt;
    for (int i = 0, n = p.size(); i < n; i++) {
        P u = p[i], v = p[(i + 1) % n];
        i128 c = cross(u, v);
        x += (i128)(u.x + v.x) * c;
        y += (i128)(u.y + v.y) * c;
    }
    return Q(x, y, 3 * a);
}

// 格点计数。O(n)，无洞、简单、有面积的整数多边形；返回 {严格内部格点,边界格点}。
pair<i128, i128> latticePoints(const vector<P>& p) {
    i128 b = 0;
    for (int i = 0, n = p.size(); i < n; i++) {
        P v = p[(i + 1) % n] - p[i];
        b += gcd(llabs(v.x), llabs(v.y));
    }
    return {(abs128(area2(p)) - b + 2) / 2, b};
}

// 一次凸多边形裁剪。O(n)，保留直线左侧。输入整数点、输出有理点；不支持把输出传回本函数反复裁剪。
vector<Q> convexCut(const vector<P>& h, Line l) {
    vector<Q> q;
    for (int i = 0, n = h.size(); i < n; i++) {
        P a = h[i], b = h[(i + 1) % n];
        int u = l.side(a), v = l.side(b);
        if (u >= 0) q.pb(Q(a.x, a.y));
        if (u * v < 0) q.pb(lineInter(a, b, l.a, l.b).p);
    }
    vector<Q> ans;
    for (Q p : q) if (ans.empty() || !qEqual(ans.back(), p)) ans.pb(p);
    if (ans.size() > 1 && qEqual(ans.front(), ans.back())) ans.pop_back();
    return ans;
}

// 圆与闭线段的交点个数。返回 0/1/2，包含端点；允许零长度。不求交点，不使用 EPS；整数输入范围同主板。
int circleSegmentCount(Circle c, P a, P b) {
    int x = inCircle(c, a), y = inCircle(c, b);
    if (a == b) return x == 0;
    if (x >= 0 && y >= 0) return (x == 0) + (y == 0);
    if (x > 0 || y > 0) return 1;
    P v = b - a, u = c.c - a;
    i128 d = dot(v, v), t = dot(u, v);
    // 两端均不在圆内，垂足又不在线段内部，只可能交于端点。
    if (t <= 0 || t >= d) return (x == 0) + (y == 0);
    i128 z = cross(v, u);
    return 1 + sgn((i128)c.r * c.r * d - z * z);
}

// 整条闭线段是否在闭多边形内。O(n log n) 时间、O(n) 空间。简单多边形，至少三点、面积非零、无洞，顶点不重复；两种方向均可。允许沿边、过顶点及零长度查询。
bool segmentInPolygon(const vector<P>& p, P a, P b) {
    if (a == b) return inPolygon(p, a) >= 0;
    // t/d 为查询直线 a+t*(b-a) 上的参数，d 始终为正。
    struct Event { i128 t, d; int add; };
    vector<Event> e{{0, 1, 0}, {1, 1, 0}};
    P v = b - a;
    int n = p.size();
    for (int i = 0; i < n; i++) {
        P u = p[i], w = p[(i + 1) % n];
        int x = sgn(cross(v, u - a)), y = sgn(cross(v, w - a));
        if (x == y) continue;
        i128 t = cross(u - a, w - u), d = cross(v, w - u);
        if (d < 0) t = -t, d = -d;
        e.pb({t, d, y - x});
    }
    // 这里只比较原始整数直线的参数；10^9 输入界内，交叉乘积可放进 i128。
    sort(e.begin(), e.end(), [](Event x, Event y) { return x.t * y.d < y.t * x.d; });
    int sum = 0;
    for (int i = 0; i + 1 < (int)e.size(); i++) {
        sum += e[i].add;
        Event x = e[i], y = e[i + 1];
        // 同位置事件之间没有区间；检查与 (0,1) 有正长度重合的空白区间。
        if (sum == 0 && x.t < x.d && y.t > 0 && x.t * y.d < y.t * x.d)
            return false;
    }
    return true;
}

// 简单多边形三角剖分。O(n^2) 时间、O(n) 额外空间。简单多边形，无洞、无重复顶点、面积非零；允许共线顶点。两种方向均可，返回 n-2 个逆时针三角形的原输入下标；不足三点返回空。
vector<array<int, 3>> triangulate(const vector<P>& p) {
    int n = p.size();
    if (n < 3) return {};
    vector<int> pre(n), nxt(n);
    vector<char> ear(n);
    int step = area2(p) > 0 ? 1 : -1;
    for (int i = 0; i < n; i++) {
        pre[i] = (i - step + n) % n;
        nxt[i] = (i + step + n) % n;
    }
    auto isEar = [&](int i) {
        int a = pre[i], c = nxt[i];
        if (cross(p[a], p[i], p[c]) <= 0) return false;
        // 闭三角形里不能有其他剩余顶点，避免对角线穿过顶点。
        for (int j = nxt[c]; j != a; j = nxt[j]) {
            if (cross(p[a], p[i], p[j]) >= 0 &&
                cross(p[i], p[c], p[j]) >= 0 &&
                cross(p[c], p[a], p[j]) >= 0) return false;
        }
        return true;
    };
    for (int i = 0; i < n; i++) ear[i] = isEar(i);
    vector<array<int, 3>> ans;
    int cur = 0;
    for (int left = n; left > 3; left--) {
        while (!ear[cur]) cur = nxt[cur];
        int a = pre[cur], c = nxt[cur];
        ans.pb({a, cur, c});
        nxt[a] = c;
        pre[c] = a;
        // 删除耳尖后，只重算左右邻点，不能每次重新找全部耳朵。
        ear[a] = isEar(a);
        ear[c] = isEar(c);
        cur = c;
    }
    ans.pb({pre[cur], cur, nxt[cur]});
    return ans;
}

// 三角形重心。返回精确有理坐标；也允许三点共线。
Q triangleCentroid(P a, P b, P c) {
    return {(i128)a.x + b.x + c.x, (i128)a.y + b.y + c.y, 3};
}

// 三角形垂心。返回精确有理坐标；共线时返回 nullopt。利用垂心=三个顶点之和-两倍外心。
optional<Q> orthocenter(P a, P b, P c) {
    auto o = circumcenter(a, b, c);
    if (!o) return nullopt;
    return Q{((i128)a.x + b.x + c.x) * o->d - 2 * o->x,
             ((i128)a.y + b.y + c.y) * o->d - 2 * o->y, o->d};
}

// 三角形内心。要求三点不共线；返回近似坐标。三个顶点的权重分别为对应的对边长度。
FP incenter(P a, P b, P c) {
    ld x = sqrtl((ld)dist2(b, c)), y = sqrtl((ld)dist2(c, a)), z = sqrtl((ld)dist2(a, b));
    return toFP(a) + (toFP(b - a) * y + toFP(c - a) * z) / (x + y + z);
}

// 三角形的旁心。要求三点不共线，返回与顶点 a 对应的旁心近似坐标；换参数顺序求其他旁心。近退化时坐标可远大于输入范围，不承诺统一绝对误差。
FP excenter(P a, P b, P c) {
    P u = b - a, v = c - a;
    ld x = sqrtl((ld)dist2(b, c)), y = sqrtl((ld)dot(v, v)), z = sqrtl((ld)dot(u, u));
    i128 d = dot(u, v), k = cross(u, v);
    // 稳定计算有向半角正切，避免直接相减“两个边长之和减去第三边”。
    ld t = d < 0 ? (y * z - (ld)d) / (ld)k : (ld)k / (y * z + (ld)d);
    FP e = toFP(u) / z;
    return toFP(a) + (e + e.perp() * t) * ((x + y + z) / 2);
}

// 精确分数直接输出 k 位小数，向零截断，绝对误差严格小于 10^(-k)。默认 12 位。
// b>0 且 b<=10^36，k>=0；不经过浮点，不需要高精度库。适用于本板 Q 和宽度平方、矩形面积的分数。
string fractionString(i128 a, i128 b, int k = 12) {
    bool negative = a < 0;
    u128 x = negative ? u128(-(a + 1)) + 1 : u128(a), d = b;
    u128 q = x / d, r = x % d;
    string s;
    do { s += char('0' + q % 10); q /= 10; } while (q);
    reverse(s.begin(), s.end());
    if (k) s += '.';
    while (k--) {
        r *= 10;
        s += char('0' + r / d);
        r %= d;
    }
    return (negative ? "-" : "") + s;
}

// 精确输出 sqrt(a/b) 的 k 位小数，向下截断，绝对误差严格小于 10^(-k)。
// 0<=a<=i128 上限，0<b<=10^36，0<=k<=15；仅在需要精确长度输出时写，不是常驻数值类型。
string sqrtFractionString(i128 a, i128 b = 1, int k = 12) {
    string integer = fractionString(a, b, 0), answer;
    if (integer.size() % 2) integer = '0' + integer;
    u128 root = 0, rem = 0, decimal = (u128)a % b;
    auto step = [&](int pair) {
        rem = rem * 100 + pair;
        int digit = 0;
        while (digit < 9 && (20 * root + digit + 1) * (digit + 1) <= rem) digit++;
        rem -= (20 * root + digit) * digit;
        root = root * 10 + digit;
        answer += char('0' + digit);
    };
    for (int i = 0; i < (int)integer.size(); i += 2)
        step(10 * (integer[i] - '0') + integer[i + 1] - '0');
    if (k) answer += '.';
    while (k--) {
        decimal *= 100;
        step((int)(decimal / b));
        decimal %= b;
    }
    return answer;
}

} // 命名空间结束。
