// ICPC 几何定型稿：单一命名空间，0-base，无通用分数类。
// 接约定公共头后；GNU++20 -O2，不依赖 Boost，不启用 fast-math。
// 这是按题摘取的索引，不要求整份手打。章节次序按常用程度的经验判断并兼顾依赖，非频率统计。
// 整数原始点每维 |坐标|<=1e9，整数半径 0..1e9；更大或复合结果须检查各专题范围。
// P 做整数判定，FP 只作近似构造/输出；禁止把 FP 取整后反馈为原始点。
// 不设置全局 EPS，不给排序比较器加 EPS；实数输入需要题目自己的数值模型。
// 无分数类不等于所有计算都改为浮点：事件参数、面积矩等必要系数仍保留整数。
// 重要变化：构造返回 FP；半平面交返回有面积区域的支撑线，零维/一维用 halfPlanePoint 判可行。
#pragma once
namespace Geo {

// 检索目录（搜索方括号编号）；详细接口/行号见 目录与迁移.md。
// [01] 基础：整数点、实数点、输入与数值工具
// [02] 线段判交、距离、直线交点与投影
// [03] 凸包、面积、点包含、格点与重心
// [04] 最近点对与凸包直径
// [05] 圆关系、交点、切线与两圆面积
// [06] 卡壳扩展、三角形各心与最小圆
// [07] 极角、浮点面积与半平面交／凸裁剪／凸交
// [08] 闵可夫斯基和差、两个／多个凸包合并
// [09] 多次切片重心与整条线段包含
// [10] 三角剖分与三角形面积极值
// [11] 凸包极点与切线
// [12] 多线段扫描线找任意相交对
// [13] 无盒半平面可行点（含点／线退化）
// [14] 圆与简单多边形的交面积
// [15] 多个多边形并面积与所有覆盖层
// [16] 多个圆的并面积与所有覆盖层
// [17] 圆反演（低频构造）

// ==================== [01] 基础：整数点、实数点、输入与数值工具 ====================
// 依赖：公共头

// P：整数原始点，用于拓扑判定；FP：近似构造和长度，不互相隐式转换。
struct P {
    ll x = 0, y = 0;
    bool operator==(const P&) const = default;
    bool operator<(P b) const { return tie(x, y) < tie(b.x, b.y); }
    P operator+(P b) const { return {x + b.x, y + b.y}; }
    P operator-(P b) const { return {x - b.x, y - b.y}; }
    P operator-() const { return {-x, -y}; }
    P operator*(ll k) const { return {x * k, y * k}; }
};
i128 dot(P a, P b) { return (i128)a.x * b.x + (i128)a.y * b.y; }
i128 cross(P a, P b) { return (i128)a.x * b.y - (i128)a.y * b.x; }
i128 cross(P a, P b, P c) { return cross(b - a, c - a); }
i128 dist2(P a, P b) { return dot(a - b, a - b); }
int sgn(i128 x) { return (x > 0) - (x < 0); }
i128 abs128(i128 x) { return x < 0 ? -x : x; } // 不接受 i128 最小值。
P perp(P p) { return {-p.y, p.x}; }
P neg(P p) { return -p; }
using ld = long double;
const ld PI = acosl(-1);
struct FP {
    ld x = 0, y = 0;
    bool operator==(const FP&) const = default; // 只比较存储值，不是几何近似相等。
    bool operator<(FP b) const { return tie(x, y) < tie(b.x, b.y); } // 不加 EPS。
    FP operator+(FP b) const { return {x + b.x, y + b.y}; }
    FP operator-(FP b) const { return {x - b.x, y - b.y}; }
    FP operator-() const { return {-x, -y}; }
    FP operator*(ld k) const { return {x * k, y * k}; }
    FP operator/(ld k) const { return {x / k, y / k}; }
    FP perp() const { return {-y, x}; }
    ld norm() const { return hypotl(x, y); }
    FP rotate(ld a) const {
        ld c = cosl(a), s = sinl(a);
        return {x * c - y * s, x * s + y * c};
    }
};
FP toFP(P p) { return {(ld)p.x, (ld)p.y}; }
ld dot(FP a, FP b) { return a.x * b.x + a.y * b.y; }
ld cross(FP a, FP b) { return a.x * b.y - a.y * b.x; }
istream& operator>>(istream& in, P& p) { return in >> p.x >> p.y; }
istream& operator>>(istream& in, FP& p) { return in >> p.x >> p.y; }
ostream& operator<<(ostream& out, P p) { return out << p.x << ' ' << p.y; }
ostream& operator<<(ostream& out, FP p) { return out << p.x << ' ' << p.y; }
// 仅在输出构造坐标时需要：先作整数除法与取余，减少大分子转换的损失。
// d!=0；本库使用范围内不会触发最小负数除以 -1。
ld realDiv(i128 n, i128 d) { return (ld)(n / d) + (ld)(n % d) / (ld)d; }
FP realPoint(i128 x, i128 y, i128 d) { return {realDiv(x, d), realDiv(y, d)}; }

// 合法十进制固定小数，最多 k 位、0<=k<=9；返回精确放大后的整数。
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

// 后面的距离、面积用此补偿累加；它不是任意精度类型。
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

// ==================== [02] 线段判交、距离、直线交点与投影 ====================
// 依赖：01

// 有向直线统一为两个不同端点 Line{a,b}，保留左侧；不是点加方向。
struct Line {
    P a, b; // 要求 a != b，表示从 a 指向 b 的无限直线
    P dir() const { return b - a; }
    int side(P p) const { return sgn(cross(a, b, p)); }
    bool operator==(const Line& l) const {
        return cross(dir(), l.dir()) == 0 && side(l.a) == 0;
    }
};

// 闭线段，允许两端点重合。
struct Segment { P a, b; };

// 点在闭线段上，精确包含端点。
bool onSegment(P p, P a, P b) {
    return cross(a, b, p) == 0 && dot(p - a, p - b) <= 0;
}

// 闭线段判交：相交为真，包括接触、重叠和零长度；O(1)。
bool segIntersect(P a, P b, P c, P d) {
    if (max(min(a.x, b.x), min(c.x, d.x)) > min(max(a.x, b.x), max(c.x, d.x)))
        return false;
    if (max(min(a.y, b.y), min(c.y, d.y)) > min(max(a.y, b.y), max(c.y, d.y)))
        return false;
    i128 x = cross(a, b, c), y = cross(a, b, d);
    if (sgn(x) * sgn(y) > 0) return false;
    i128 z = cross(c, d, a);
    return sgn(z) * sgn(z + x - y) <= 0; // 第四个方向值由前三个恢复。
}

// 0 不在，1 严格内部，-1 端点。
int onSegmentType(P p, Segment s) {
    if (p == s.a || p == s.b) return -1;
    return onSegment(p, s.a, s.b) ? 1 : 0;
}

// 0 无交，1 严格穿过，-1 过端点或包含整段。
int segmentLineType(Segment s, Line l) {
    int a = l.side(s.a), b = l.side(s.b);
    if (a == 0 || b == 0) return -1;
    return a != b;
}

// 0 无交，1 内部交叉，-1 端点接触或共线重叠。
int segmentType(Segment s, Segment t) {
    P a = s.a, b = s.b, c = t.a, d = t.b;
    if (max(min(a.x, b.x), min(c.x, d.x)) > min(max(a.x, b.x), max(c.x, d.x)) ||
        max(min(a.y, b.y), min(c.y, d.y)) > min(max(a.y, b.y), max(c.y, d.y))) return 0;
    i128 x = cross(a, b, c), y = cross(a, b, d);
    int u = sgn(x) * sgn(y);
    if (u > 0) return 0;
    i128 z = cross(c, d, a);
    int v = sgn(z) * sgn(z + x - y);
    if (v > 0) return 0;
    return u < 0 && v < 0 ? 1 : -1;
}

// 点到无限直线的近似距离；a!=b。
ld lineDistance(P p, P a, P b) {
    return fabsl((ld)cross(a, b, p)) / sqrtl((ld)dist2(a, b));
}

// 点到闭线段的近似距离，允许零长度。
ld segmentDistance(P p, P a, P b) {
    P v = b - a, u = p - a;
    i128 t = dot(u, v);
    if (t <= 0) return sqrtl((ld)dot(u, u)); // 也覆盖 a==b。
    i128 d = dot(v, v);
    if (t >= d) return sqrtl((ld)dist2(p, b));
    return fabsl((ld)cross(v, u)) / sqrtl((ld)d);
}

// 两闭线段的近似距离，相交为零。
ld segmentDistance(P a, P b, P c, P d) {
    if (segIntersect(a, b, c, d)) return 0;
    return min({segmentDistance(a, c, d), segmentDistance(b, c, d),
                segmentDistance(c, a, b), segmentDistance(d, a, b)});
}

// 0 平行、-1 重合、1 唯一交点；仅 type==1 时读取近似坐标 p。
struct LineHit { int type; FP p; };
LineHit lineInter(P a, P b, P c, P d) {
    P v = b - a, w = d - c;
    i128 den = cross(v, w), t = cross(c - a, w);
    if (den == 0) return {cross(v, c - a) == 0 ? -1 : 0, {}};
    return {1, realPoint((i128)a.x * den + (i128)v.x * t,
                         (i128)a.y * den + (i128)v.y * t, den)};
}
// 闭线段交集：空、一个点或重叠段两端点；只构造近似输出，不拿它再判共线。
vector<FP> segInter(P a, P b, P c, P d) {
    if (!segIntersect(a, b, c, d)) return {};
    if (cross(b - a, d - c) != 0) return {lineInter(a, b, c, d).p};
    if (b < a) swap(a, b);
    if (d < c) swap(c, d);
    P l = max(a, c), r = min(b, d);
    if (l == r) return {toFP(l)};
    return {toFP(l), toFP(r)};
}
// 投影与对称：a!=b，返回 FP；输入整数的中间乘加仍用 i128。
FP projection(P p, P a, P b) {
    P v = b - a;
    i128 d = dot(v, v), t = dot(p - a, v);
    return realPoint((i128)a.x * d + (i128)v.x * t,
                     (i128)a.y * d + (i128)v.y * t, d);
}
FP reflection(P p, P a, P b) {
    P v = b - a;
    i128 d = dot(v, v), c = 2 * cross(v, p - a);
    return realPoint((i128)p.x * d + (i128)v.y * c,
                     (i128)p.y * d - (i128)v.x * c, d);
}

// ==================== [03] 凸包、面积、点包含、格点与重心 ====================
// 依赖：01、02 的 onSegment

// 已按 (x,y) 字典序排序的点列；O(n) 严格逆时针凸包，从最小点开始。
vector<P> convexHullSorted(vector<P> p) {
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

// 无序点集的严格凸包，O(n log n)；空、重复、全共线均支持。
vector<P> convexHull(vector<P> p) {
    sort(p.begin(), p.end());
    return convexHullSorted(move(p));
}

// 有向二倍面积：逆时针正；边界点按顺序，不重复首点。
i128 area2(const vector<P>& p) {
    i128 s = 0;
    int n = p.size();
    for (int i = 0; i < n; i++) s += cross(p[i], p[i + 1 == n ? 0 : i + 1]);
    return s;
}

// 闭边界周长；两点时为线段长度的两倍；O(n)。
ld perimeter(const vector<P>& p) {
    Sum ans;
    for (int i = 0, n = p.size(); i < n; i++)
        ans.add(sqrtl((ld)dist2(p[i], p[i + 1 == n ? 0 : i + 1])));
    return ans.value();
}

// {是否在边界,有符号回转数}，边界时回转数为0；O(n)。
pair<bool, int> winding(const vector<P>& p, P q) {
    int w = 0, n = p.size();
    for (int i = 0, j = n - 1; i < n; j = i++) {
        P a = p[j], b = p[i];
        // 纵坐标严格同侧：既不穿过水平射线，也不可能包含查询点。
        if (q.y < min(a.y, b.y) || q.y > max(a.y, b.y)) continue;
        i128 c = cross(a, b, q);
        // 纵坐标已在闭区间内；共线时只需补查横坐标。
        if (c == 0 && min(a.x, b.x) <= q.x && q.x <= max(a.x, b.x)) return {true, 0};
        if (a.y <= q.y && q.y < b.y && c > 0) w++;
        if (b.y <= q.y && q.y < a.y && c < 0) w--;
    }
    return {false, w};
}

// 简单多边形包含：-1 外、0 边界、1 内；方向任意；O(n)。
int inPolygon(const vector<P>& p, P q) {
    auto [boundary, w] = winding(p, q);
    return boundary ? 0 : (w == 0 ? -1 : 1);
}

// 严格逆时针凸包包含：-1 外、0 边界、1 内；O(log n)，支持退化。
int inConvex(const vector<P>& h, P q) {
    int n = h.size();
    if (n == 0) return -1;
    if (n <= 2) return onSegment(q, h[0], h.back()) ? 0 : -1;
    P v = q - h[0];
    i128 a = cross(h[1] - h[0], v), b = cross(h.back() - h[0], v);
    if (a < 0 || b > 0) return -1;
    if (a == 0) return dot(v, q - h[1]) <= 0 ? 0 : -1;
    if (b == 0) return dot(v, q - h.back()) <= 0 ? 0 : -1;
    int l = 1, r = n - 1;
    while (r - l > 1) {
        int m = l + (r - l) / 2;
        if (cross(h[m] - h[0], v) >= 0) l = m;
        else r = m;
    }
    return sgn(cross(h[l], h[r], q));
}

// 简单无洞整点多边形：{严格内部格点,边界格点}；Pick 定理，O(n)。
pair<i128, i128> latticePoints(const vector<P>& p) {
    i128 a = 0, b = 0;
    for (int i = 0, n = p.size(); i < n; i++) {
        P u = p[i], v = p[i + 1 == n ? 0 : i + 1], d = v - u;
        a += cross(u, v);
        b += gcd(llabs(d.x), llabs(d.y));
    }
    return {(abs128(a) - b + 2) / 2, b};
}

// 多边形重心，不是顶点平均；零面积返回空；只在最后除法转为 FP。
optional<FP> centroid(const vector<P>& p) {
    i128 a = 0, x = 0, y = 0;
    for (int i = 0, n = p.size(); i < n; i++) {
        P u = p[i], v = p[i + 1 == n ? 0 : i + 1];
        i128 c = cross(u, v);
        a += c;
        x += ((i128)u.x + v.x) * c;
        y += ((i128)u.y + v.y) * c;
    }
    if (a == 0) return nullopt;
    return realPoint(x, y, 3 * a);
}

// ==================== [04] 最近点对与凸包直径 ====================
// 依赖：01

// 点对：距离平方 d2、原数组下标 a/b；无点对时 d2=-1。
struct PairResult { i128 d2 = -1; int a = -1, b = -1; };

// 严格逆时针凸包直径，O(n)；下标指向凸包。
PairResult diameter(const vector<P>& h) {
    int n = h.size();
    if (n == 0) return {};
    PairResult ans{0, 0, 0};
    auto update = [&](int a, int b) {
        i128 d = dist2(h[a], h[b]);
        if (d > ans.d2) ans = {d, a, b};
    };
    if (n <= 2) { update(0, n - 1); return ans; }
    auto next = [&](int i) { return i + 1 == n ? 0 : i + 1; };
    for (int i = 0, j = 1; i < n; i++) {
        int k = next(i);
        P v = h[k] - h[i];
        while (cross(v, h[next(j)] - h[j]) > 0) j = next(j);
        update(i, j);
        update(k, j);
        if (cross(v, h[next(j)] - h[j]) == 0) {
            update(i, next(j));
            update(k, next(j));
        }
    }
    return ans;
}

// 最近点对，O(n log n)；原输入下标；每维 |坐标|<=1e9。
PairResult closestPair(const vector<P>& p) {
    int n = p.size();
    if (n < 2) return {};
    if (n == 2) return {dist2(p[0], p[1]), 0, 1};
    struct Node { int x, y, id; };
    vector<Node> a(n);
    for (int i = 0; i < n; i++) a[i] = {(int)p[i].x, (int)p[i].y, i};
    sort(a.begin(), a.end(), [](Node a, Node b) { return tie(a.x, a.y) < tie(b.x, b.y); });
    PairResult ans{dist2(p[0], p[1]), 0, 1};
    auto update = [&](Node a, Node b) {
        ll x = (ll)a.x - b.x, y = (ll)a.y - b.y, d = x * x + y * y;
        if (d < ans.d2) ans = {d, a.id, b.id};
    };
    // 相邻点给出上界；先查完重复点，才能凭整数距离下界 1 返回。
    for (int i = 1; i < n; i++) {
        update(a[i - 1], a[i]);
        if (ans.d2 == 0) return ans;
    }
    if (ans.d2 == 1) return ans;
    vector<Node> tmp(n);
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

// 只求值时复用同一份算法，不再背另一份分治；仍保存点号。
i128 diameter2(const vector<P>& h) { return diameter(h).d2; }
ll closest2(const vector<P>& p) { return (ll)closestPair(p).d2; }

// ==================== [05] 圆关系、交点、切线与两圆面积 ====================
// 依赖：01、02

// 整数圆；0<=r<=1e9；圆关系精确，坐标输出近似。
struct Circle { P c; ll r; };

// 圆周长。
ld circumference(Circle c) { return 2 * PI * c.r; }

// 圆盘面积。
ld circleArea(Circle c) { return PI * c.r * c.r; }

// -1 圆外、0 圆上、1 圆内；整数精确。
int inCircle(Circle c, P p) { return sgn((i128)c.r * c.r - dist2(c.c, p)); }

// 圆周位置：重合、相离、外切、两点交、内切、内含。
enum class CircleRelation { Equal, Separate, ExternalTouch, Intersect, InternalTouch, Contained };

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

// 无限直线与圆周交点数 0/1/2；a!=b。
int circleLineCount(Circle c, P a, P b) {
    P v = b - a;
    i128 z = cross(v, c.c - a);
    return 1 + sgn((i128)c.r * c.r * dot(v, v) - z * z);
}

// 两圆周交点数；-1 为同一正半径圆，零半径按点处理。
int circleCircleCount(Circle a, Circle b) {
    i128 d = dist2(a.c, b.c);
    if (d == 0) return a.r != b.r ? 0 : (a.r == 0 ? 1 : -1);
    i128 hi = (i128)(a.r + b.r) * (a.r + b.r);
    i128 lo = (i128)(a.r - b.r) * (a.r - b.r);
    if (d > hi || d < lo) return 0;
    return d == hi || d == lo ? 1 : 2;
}

// 闭线段与圆周的不同交点数，精确包含端点，允许零长度。
int circleSegmentCount(Circle c, P a, P b) {
    P u = a - c.c, v = b - a;
    // f(t)=A*t²+2*B*t+C；C、D 分别是两个端点的圆幂。
    i128 C = dot(u, u) - (i128)c.r * c.r;
    if (a == b) return C == 0;
    i128 A = dot(v, v), B = dot(u, v), D = A + 2 * B + C;
    if (C <= 0 && D <= 0) return (C == 0) + (D == 0);
    if (C < 0 || D < 0) return 1;
    if (B >= 0 || -B >= A) return (C == 0) + (D == 0);
    // 两端均不在圆内，且最低点在 (0,1)；判别式决定 0/1/2 个交点。
    return 1 + sgn(B * B - A * C);
}

// count 为0/1/2，-1表示无限多交点；p是近似坐标。
struct CircleHit { int count = 0; vector<FP> p; };

// 整数圆与非退化直线交点，个数精确，坐标近似。
CircleHit circleLine(P o, ll r, P a, P b) {
    P v = b - a;  // 要求 a != b，直线不能退化为一个点
    i128 d = dot(v, v), c = cross(v, o - a);
    i128 h = (i128)r * r * d - c * c;
    if (h < 0) return {};
    // 用已有叉积沿法向投影；分子与原 projection 完全相同。
    FP mid = realPoint((i128)o.x * d + (i128)v.y * c,
               (i128)o.y * d - (i128)v.x * c, d);
    if (h == 0) return {1, {mid}};
    FP off = toFP(v) * (sqrtl((ld)h) / (ld)d);
    return {2, {mid - off, mid + off}};
}

// 整数两圆交点，个数精确；不保证两点的先后次序。
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
    // 相切点直接按圆心连线的半径比构造，不先生成一般公共弦中点。
    if (d == sum || d == dif) {
        ll k = d == sum ? r + s : r - s; // 内切时允许为负；同心已处理。
        return {1, {realPoint((i128)a.x * k + (i128)v.x * r,
                      (i128)a.y * k + (i128)v.y * r, k)}};
    }
    i128 t = d + (i128)r * r - (i128)s * s;
    FP mid;
    // 等半径时公共弦中点就是圆心中点，整数或半整数能精确表示。
    if (r == s) mid = {(ld)(a.x + b.x) / 2, (ld)(a.y + b.y) / 2};
    else mid = realPoint(2 * (i128)a.x * d + (i128)v.x * t,
                 2 * (i128)a.y * d + (i128)v.y * t, 2 * d);
    ld k = sqrtl((ld)(sum - d) * (ld)(d - dif)) / (2 * (ld)d);
    FP off = toFP(v).perp() * k;
    return {2, {mid - off, mid + off}};
}

// a+t(b-a) 与圆的交点参数；a!=b；根数精确、根值近似。
vector<ld> circleLineParameters(Circle c, P a, P b) {
    P u = a - c.c, v = b - a;
    i128 A = dot(v, v), B = dot(u, v);
    i128 C = dot(u, u) - (i128)c.r * c.r;
    i128 H = B * B - A * C; // 系数已齐，复用二次式判别式，不另算叉积。
    if (H < 0) return {};
    if (H == 0) return {-(ld)B / (ld)A};
    // 先算不发生相消的根；另一根由两根之积 C/A 求出。
    ld q = -(ld)B - copysignl(sqrtl((ld)H), (ld)B);
    ld x = q / (ld)A, y = (ld)C / q;
    if (x > y) swap(x, y);
    return {x, y};
}

// 近似构造直线：点 p + 方向 v；注意与整数 Line{a,b} 的区别。
struct FLine { FP p, v; };

// 实数圆，不能强制转为整数 Circle。
struct FCircle { FP c; ld r; };

// 实数圆交点，无统一容差；同心返回空，不能据此判断是否重合。
vector<FP> circleInter(FCircle a, FCircle b) {
    FP v = b.c - a.c;
    ld d = dot(v, v), s = a.r + b.r, t = a.r - b.r;
    if (d == 0) return {};
    ld x = s * s - d, y = d - t * t;
    if (x < 0 || y < 0) return {};
    FP mid = a.c + v * ((d + s * t) / (2 * d));
    if (x == 0 || y == 0) return {mid};
    FP off = v.perp() * (sqrtl(x * y) / (2 * d));
    return {mid - off, mid + off};
}

// 圆外点切点；r>0；圆内空、圆上一点、圆外两点。
vector<FP> tangentPoints(Circle c, P p) {
    P v = p - c.c;
    i128 d = dot(v, v), r2 = (i128)c.r * c.r;
    if (d < r2) return {};
    if (d == r2) return {toFP(p)};
    FP mid = toFP(c.c) + toFP(v) * ((ld)r2 / (ld)d);
    FP off = toFP(perp(v)) * (c.r * sqrtl((ld)(d - r2)) / (ld)d);
    return {mid - off, mid + off};
}

// 两圆切点及独立切线方向；相切时不能用 b-a 恢复方向。
struct Tangent { FP a, b; FLine line; };

// count=-1 表示无限多条，其余为切线条数。
struct TangentResult { int count = 0; vector<Tangent> t; };

// 公切线：支持点圆和重合退化，坐标近似。
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
        if (ans.empty()) ans.reserve(4); // 确认有解后才分配，最多四条。
        ld root = sqrtl((ld)h); // 同类两条切线共用平方根。
        for (int sign : {-1, 1}) {
            FP normal = (toFP(v) * (ld)dr + toFP(perp(v)) * (sign * root)) / (ld)d;
            FP p = toFP(a.c) + normal * a.r;
            FP q = toFP(b.c) + normal * (k * b.r);
            ans.pb({p, q, {p, normal.perp()}});
            if (h == 0) break;
        }
    }
    return {(int)ans.size(), move(ans)}; // 转交结果缓冲，不复制整个数组。
}

// 只需切点优先用 tangentPoints；需要方向或 r=0 用此函数。
TangentResult tangentsFrom(Circle c, P p) { return commonTangents(c, {p, 0}); }

// 圆弓形因子 t-sin(t)cos(t)；小角度级数避免相消。
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

// 两圆盘交面积，整数分类；长双精度近似，没有全域绝对误差保证。
ld circleIntersectionArea(Circle a, Circle b) {
    if (a.r == 0 || b.r == 0) return 0;
    i128 d = dist2(a.c, b.c);
    i128 sum = (i128)(a.r + b.r) * (a.r + b.r);
    i128 dif = (i128)(a.r - b.r) * (a.r - b.r);
    if (d >= sum) return 0;
    if (d <= dif) {
        ll r = min(a.r, b.r);
        return PI * r * r;
    }
    i128 x = (i128)a.r * a.r, y = (i128)b.r * b.r;
    ld h = sqrt(ld(sum - d) * ld(d - dif));
    if (x == y) return ld(2 * x) * circularSegment(atan2(h, ld(d)));
    ld alpha = atan2(h, ld(d + x - y));
    ld beta = atan2(h, ld(d + y - x));
    return ld(x) * circularSegment(alpha) + ld(y) * circularSegment(beta);
}

int circleLineRelation(Circle c, Line l) { return circleLineCount(c, l.a, l.b); }

// ==================== [06] 卡壳扩展、三角形各心与最小圆 ====================
// 依赖：01、03、05


// 三角形重心；允许共线，返回近似坐标。
FP triangleCentroid(P a, P b, P c) {
    return realPoint((i128)a.x + b.x + c.x, (i128)a.y + b.y + c.y, 3);
}
// 共线时外心／垂心无定义；非共线返回 FP，不是可继续精确比较的有理点。
optional<FP> circumcenter(P a, P b, P c) {
    P v = b - a, w = c - a;
    i128 d = 2 * cross(v, w), u = dot(v, v), t = dot(w, w);
    if (d == 0) return nullopt;
    return realPoint((i128)a.x * d + u * w.y - t * v.y,
                     (i128)a.y * d + t * v.x - u * w.x, d);
}
optional<FP> orthocenter(P a, P b, P c) {
    P u = b - a, v = c - a;
    i128 d = cross(u, v), t = dot(u, v);
    if (d == 0) return nullopt;
    return realPoint((i128)a.x * d + (v.y - u.y) * t,
                     (i128)a.y * d + (u.x - v.x) * t, d);
}


// 内心，三点不共线。
FP incenter(P a, P b, P c) {
    ld x = sqrtl((ld)dist2(b, c)), y = sqrtl((ld)dist2(c, a)), z = sqrtl((ld)dist2(a, b));
    return toFP(a) + (toFP(b - a) * y + toFP(c - a) * z) / (x + y + z);
}

// 对应顶点 a 的旁心；三点不共线，极瘦三角形结果可能很大。
FP excenter(P a, P b, P c) {
    P u = b - a, v = c - a;
    ld x = sqrtl((ld)dist2(b, c)), y = sqrtl((ld)dot(v, v)), z = sqrtl((ld)dot(u, u));
    i128 d = dot(u, v), k = cross(u, v);
    // 稳定计算有向半角正切，避免直接相减“两个边长之和减去第三边”。
    ld t = d < 0 ? (y * z - (ld)d) / (ld)k : (ld)k / (y * z + (ld)d);
    FP e = toFP(u) / z;
    return toFP(a) + (e + e.perp() * t) * ((x + y + z) / 2);
}

// 凸包最小宽度，返回近似长度；空/点/线段为0；严格逆时针凸包，O(n)。
ld minimumWidth(const vector<P>& h) {
    int n = h.size();
    if (n <= 2) return 0;
    auto next = [&](int i) { return i + 1 == n ? 0 : i + 1; };
    ld ans = numeric_limits<ld>::infinity();
    for (int i = 0, j = 1; i < n; i++) {
        P v = h[next(i)] - h[i];
        while (cross(v, h[next(j)] - h[j]) > 0) j = next(j);
        i128 a = cross(v, h[j] - h[i]);
        chmin(ans, (ld)a * (ld)a / (ld)dot(v, v));
    }
    return sqrtl(ans);
}
// 最小面积外接矩形；返回近似面积，O(n)，严格逆时针凸包。
// 只要面积就不传第二参数；需要角点时传 &corner，四点逆时针，退化时可重复。
ld minimumRectangle(const vector<P>& h, array<FP, 4>* corner = nullptr) {
    int n = h.size();
    if (corner) corner->fill({});
    if (n == 0) return 0;
    if (n == 1) { if (corner) corner->fill(toFP(h[0])); return 0; }
    auto next = [&](int i) { return i + 1 == n ? 0 : i + 1; };
    int top = 0, lo = 0, hi = 0;
    P v = h[1] - h[0];
    for (int j = 0; j < n; j++) {
        if (cross(v, h[j] - h[top]) > 0) top = j;
        if (dot(v, h[j] - h[lo]) < 0) lo = j;
        if (dot(v, h[j] - h[hi]) > 0) hi = j;
    }
    ld ans = numeric_limits<ld>::infinity();
    for (int i = 0; i < n; i++) {
        v = h[next(i)] - h[i];
        while (cross(v, h[next(top)] - h[top]) > 0) top = next(top);
        while (dot(v, h[next(lo)] - h[lo]) < 0) lo = next(lo);
        while (dot(v, h[next(hi)] - h[hi]) > 0) hi = next(hi);
        i128 l = dot(v, h[lo] - h[i]), r = dot(v, h[hi] - h[i]);
        i128 height = cross(v, h[top] - h[i]), d = dot(v, v);
        ld area = realDiv((r - l) * height, d);
        if (area >= ans) continue;
        ans = area;
        if (corner) {
            P w = perp(v);
            auto point = [&](i128 x, i128 y) {
                return realPoint((i128)h[i].x * d + (i128)v.x * x + (i128)w.x * y,
                                 (i128)h[i].y * d + (i128)v.y * x + (i128)w.y * y, d);
            };
            *corner = {point(l, 0), point(r, 0), point(r, height), point(l, height)};
        }
    }
    return ans;
}


// 非共线 abc 的外接圆：1 内、0 上、-1 外；原始每维 |坐标|<=1e9。
int inCircumcircle(P a, P b, P c, P p) {
    b = b - a; c = c - a; p = p - a;
    i128 s = cross(b, c);
    i128 z = dot(b, b) * cross(p, c) + dot(c, c) * cross(b, p) - dot(p, p) * s;
    return sgn(s) * sgn(z);
}

// 最小圆：中心、半径近似；原支持点与 contains 的整数判断保留。
struct EnclosingCircle {
    FP center;
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

// 随机增量最小圆：期望 O(n)，最坏 O(n³)；固定 seed 复现，输入不变。
EnclosingCircle minimumEnclosingCircle(const vector<P>& p, ull seed) {
    int n = p.size();
    vector<int> order(n);
    iota(order.begin(), order.end(), 0);
    mt19937_64 rng(seed);
    shuffle(order.begin(), order.end(), rng);
    EnclosingCircle ans;
    i128 sx = 0, sy = 0, den = 0;
    // 三个支持点不变时复用圆内式的系数；少于三点仍走原精确判定。
    auto contains = [&](P q) {
        if (ans.count < 3) return ans.contains(q);
        q = q - ans.basis[0];
        return sx * q.x + sy * q.y - den * dot(q, q) >= 0;
    };
    // 循环中只维护支持点，不反复构造浮点圆；三支持点共线时约简为最远的两个端点。
    auto setBasis = [&](int a, int b = -1, int c = -1) {
        ans.id = {a, b, c}; ans.count = 1 + (b >= 0) + (c >= 0);
        for (int i = 0; i < ans.count; i++) ans.basis[i] = p[ans.id[i]];
        if (ans.count == 3 && cross(p[a], p[b], p[c]) == 0) {
            if (dist2(p[a], p[c]) > dist2(p[a], p[b])) swap(b, c);
            if (dist2(p[b], p[c]) > dist2(p[a], p[b])) a = c;
            ans.id = {a, b, -1}; ans.basis = {p[a], p[b], P{}}; ans.count = 2;
        }
        if (ans.count == 3) {
            P u = p[b] - p[a], v = p[c] - p[a];
            i128 x = dot(u, u), y = dot(v, v);
            den = cross(u, v); sx = x * v.y - y * u.y; sy = y * u.x - x * v.x;
            if (den < 0) den = -den, sx = -sx, sy = -sy;
        }
    };
    for (int i = 0; i < n; i++) if (!contains(p[order[i]])) {
        setBasis(order[i]);
        for (int j = 0; j < i; j++) if (!contains(p[order[j]])) {
            setBasis(order[i], order[j]);
            for (int k = 0; k < j; k++) if (!contains(p[order[k]]))
                setBasis(order[i], order[j], order[k]);
        }
    }
    if (ans.count == 0) return ans;
    P a = ans.basis[0], b = ans.basis[1];
    if (ans.count == 1) { ans.center = toFP(a); ans.radius = 0; }
    if (ans.count == 2) {
        ans.center = realPoint((i128)a.x + b.x, (i128)a.y + b.y, 2);
        ans.radius = sqrtl((ld)dist2(a, b)) / 2;
    }
    if (ans.count == 3) {
        // 最后一次三支持点更新的系数仍有效，不再重新求外心。
        i128 d = 2 * den;
        ans.center = realPoint((i128)a.x * d + sx, (i128)a.y * d + sy, d);
        ans.radius = hypotl((ld)sx / (ld)d, (ld)sy / (ld)d);
    }
    return ans;
}

// ==================== [07] 极角、浮点面积与半平面交／凸裁剪／凸交 ====================
// 依赖：01、02、03

// 非零向量按极角，同射线等价；排序不加 EPS。
bool angleLess(P a, P b) {
    auto half = [](P p) { return p.y < 0 || (p.y == 0 && p.x < 0); };
    if (half(a) != half(b)) return half(a) < half(b);
    return cross(a, b) > 0;
}

// 允许零向量（最前），同射线再按长度排序。
bool polarLess(P a, P b) {
    if (a == P{} || b == P{}) return a == P{} && b != P{};
    auto half = [](P p) { return p.y < 0 || (p.y == 0 && p.x < 0); };
    if (half(a) != half(b)) return half(a) < half(b);
    i128 c = cross(a, b);
    return c ? c > 0 : llabs(a.x) + llabs(a.y) < llabs(b.x) + llabs(b.y);
}

// 两个非零向量夹角，弧度 [0,π]。
ld angle(P a, P b) { return atan2l((ld)abs128(cross(a, b)), (ld)dot(a, b)); }

ld angle(FP a, FP b) { return atan2l(fabsl(cross(a, b)), dot(a, b)); }

FP rotate(FP p, ld c, ld s) { return {p.x * c - p.y * s, p.x * s + p.y * c}; }

// 乘积差补偿；不等于完整精确谓词，不能恢复先前构造误差。
ld crossAccurate(FP a, FP b) {
    ld t = a.y * b.x;
    return fmal(a.x, b.y, -t) + fmal(-a.y, b.x, t);
}

// 近似叉积符号过滤；用于已有圆弧数值算法，不提供精确共线保证。
int crossSign(FP a, FP b) {
    ld x = a.x * b.y, y = a.y * b.x, z = x - y;
    if (fabsl(z) <= 4 * numeric_limits<ld>::epsilon() * (fabsl(x) + fabsl(y)))
        z = crossAccurate(a, b);
    return (z > 0) - (z < 0);
}

// FP 多边形有向面积，O(n)，近似；大坐标薄交集优先直接使用 linePolygonArea。
ld polygonArea(const vector<FP>& p) {
    if (p.size() < 3) return 0;
    FP o = p[0], a = p[1] - o;
    Sum ans;
    for (int i = 2; i < (int)p.size(); i++) {
        FP b = p[i] - o;
        ans.add(crossAccurate(a, b) / 2);
        a = b;
    }
    return ans.value();
}



// 三条原始整数直线：b,c 的交点相对 a 的侧别。b,c 不平行。
// 精确返回 -1/0/1，不构造浮点交点；本节仍限每维原始坐标 <=1e9。
int intersectionSide(Line a, Line b, Line c) {
    P u = a.dir(), v = b.dir(), w = c.dir();
    i128 d = cross(v, w), t = cross(c.a - b.a, w);
    i128 s = cross(u, b.a - a.a) * d + cross(u, v) * t;
    return sgn(s) * sgn(d);
}

// 有序半平面交核心：angleLess 从正 x 轴起有序；交集必须为空或有界。
// 返回逆时针支撑线；空、点、线段统一返回空，只用于有面积区域。
// 不加盒子，不排序；O(n)。闭交集是否非空请用第13节 halfPlanePoint。
vector<Line> halfPlaneSorted(vector<Line> a) {
    int m = 0;
    for (Line l : a) {
        if (m && cross(a[m - 1].dir(), l.dir()) == 0 && dot(a[m - 1].dir(), l.dir()) > 0) {
            if (a[m - 1].side(l.a) > 0) a[m - 1] = l;
        } else a[m++] = l;
    }
    a.resize(m);
    // 反方向仍循环有序；相反的重合约束只能形成零面积区域。
    for (int i = 0, j = 0; i < m; i++) {
        P d = -a[i].dir();
        if (i && angleLess(d, -a[i - 1].dir())) j = 0;
        while (j < m && angleLess(a[j].dir(), d)) j++;
        if (j < m && cross(a[j].dir(), d) == 0 && dot(a[j].dir(), d) > 0 &&
            a[i].side(a[j].a) <= 0) return {};
    }
    auto out = [](Line l, Line b, Line c) { return intersectionSide(l, b, c) < 0; };
    int head = 0, tail = 0;
    for (Line l : a) {
        while (tail - head > 1 && out(l, a[tail - 2], a[tail - 1])) --tail;
        while (tail - head > 1 && out(l, a[head], a[head + 1])) ++head;
        if (tail > head && cross(a[tail - 1].dir(), l.dir()) <= 0) return {};
        a[tail++] = l;
    }
    while (tail - head > 2 && out(a[head], a[tail - 2], a[tail - 1])) --tail;
    while (tail - head > 2 && out(a[tail - 1], a[head], a[head + 1])) ++head;
    if (tail - head < 3 || cross(a[tail - 1].dir(), a[head].dir()) <= 0) return {};
    bool positive = false;
    for (int i = head + 2; i < tail; i++)
        positive |= intersectionSide(a[i], a[head], a[head + 1]) > 0;
    if (!positive) return {}; // 全部边界共点，不能当成有面积多边形。
    a.resize(tail);
    a.erase(a.begin(), a.begin() + head);
    return a;
}
// 与 [-lim,lim]² 相交，0<lim<=1e9；不能把盒内空解释成全平面无解。
vector<Line> halfPlaneIntersection(vector<Line> a, ll lim) {
    a.reserve(a.size() + 4);
    a.pb({{-lim,-lim},{lim,-lim}}); a.pb({{lim,-lim},{lim,lim}});
    a.pb({{lim,lim},{-lim,lim}}); a.pb({{-lim,lim},{-lim,-lim}});
    sort(a.begin(), a.end(), [](Line a, Line b) { return angleLess(a.dir(), b.dir()); });
    return halfPlaneSorted(move(a));
}
// 构造支撑线相邻交点，得到近似坐标；共点冗余约束可能产生重复顶点。
vector<FP> boundaryPoints(const vector<Line>& h) {
    vector<FP> p;
    for (int i = 0, n = h.size(); i < n; i++) {
        Line a = h[i], b = h[i + 1 == n ? 0 : i + 1];
        p.pb(lineInter(a.a, a.b, b.a, b.b).p);
    }
    return p;
}
// 支撑线围成多边形的面积；优先用于整数半平面交、连续裁剪结果。
// 只在最后输出用浮点；每条线保留原始整数端点，相邻不平行。
// 结果有界，顶点及原始线端点每维绝对值<=1e9；O(n)。
ld linePolygonArea(const vector<Line>& h) {
    int n = h.size();
    if (n < 3) return 0;
    P origin = h[0].a;
    i128 whole = 0;
    Sum fraction;
    for (int i = 0; i < n; i++) {
        Line a = h[i];
        i128 base = cross(a.a - origin, a.b - origin);
        auto add = [&](Line b, int sign) {
            i128 d = cross(a.dir(), b.dir()), t = cross(b.a - a.a, b.dir());
            i128 z = base * t;
            whole += sign * (z / d);
            fraction.add(sign * ((ld)(z % d) / (ld)d));
        };
        add(h[i + 1 == n ? 0 : i + 1], 1);
        add(h[i ? i - 1 : n - 1], -1);
    }
    return ((ld)whole + fraction.value()) / 2;
}
// 一次整数凸多边形裁剪：左侧及边界保留，输出 FP 仅供数值计算。
// 不把输出取整反馈；需要多次裁剪使用下面的 convexCutLines。
vector<FP> convexCut(const vector<P>& h, Line l) {
    vector<FP> q;
    int u = h.empty() ? 0 : l.side(h[0]);
    for (int i = 0, n = h.size(); i < n; i++) {
        P a = h[i], b = h[i + 1 == n ? 0 : i + 1];
        int v = l.side(b);
        if (u >= 0) q.pb(toFP(a));
        if (u * v < 0) q.pb(lineInter(a, b, l.a, l.b).p);
        u = v;
    }
    q.erase(unique(q.begin(), q.end()), q.end());
    if (q.size() > 1 && q.front() == q.back()) q.pop_back();
    return q;
}


// 连续裁剪：逆时针原始支撑线，保留左侧；空/点/段统一为空；O(n)。
vector<Line> convexCutLines(const vector<Line>& h, Line l) {
    vector<Line> ans;
    int n = h.size();
    if (n == 0) return ans;
    int a = intersectionSide(l, h.back(), h[0]);
    bool positive = false;
    for (int i = 0; i < n; i++) {
        int b = intersectionSide(l, h[i], h[(i + 1) % n]);
        positive |= a > 0;
        if (a >= 0 || b >= 0) ans.pb(h[i]);
        if (a >= 0 && b < 0) ans.pb(l);
        a = b;
    }
    if (!positive) ans.clear();
    return ans;
}

// 两个严格逆时针凸包的交面积；退化为零，O(n+m)，输入不变。
ld convexIntersectionArea(const vector<P>& a, const vector<P>& b) {
    int n = a.size(), m = b.size();
    if (n < 3 || m < 3) return 0;
    auto start = [](const vector<P>& p) {
        return int(min_element(p.begin(), p.end(), [](P a, P b) {
            return tie(a.y, a.x) < tie(b.y, b.x);
        }) - p.begin());
    };
    int x = start(a), y = start(b);
    vector<Line> lines;
    lines.reserve(n + m);
    // 从最低、再最左的点出发，两条凸边界的边方向分别已有序。
    for (int i = 0, j = 0; i < n || j < m;) {
        int xx = x + 1 == n ? 0 : x + 1, yy = y + 1 == m ? 0 : y + 1;
        Line u{a[x], a[xx]}, v{b[y], b[yy]};
        if (j == m || (i < n && !angleLess(v.dir(), u.dir()))) {
            lines.pb(u); x = xx; i++;
        } else {
            lines.pb(v); y = yy; j++;
        }
    }
    // 交集包含于两个有界凸包，不需人为盒子，也不需重新排序。
    auto r = halfPlaneSorted(move(lines));
    return fabsl(linePolygonArea(r));
}

// ==================== [08] 闵可夫斯基和差、两个／多个凸包合并 ====================
// 依赖：01、03

// 严格逆时针凸包和，允许空/点/段；O(n+m)，坐标范围可能翻倍。
vector<P> minkowski(const vector<P>& a, const vector<P>& b) {
    if (a.empty() || b.empty()) return {};
    // 点平移仍保留另一输入的循环起点和顺序。
    if (a.size() == 1) { auto r = b; for (P& p : r) p = p + a[0]; return r; }
    if (b.size() == 1) { auto r = a; for (P& p : r) p = p + b[0]; return r; }
    int n = a.size(), m = b.size();
    int x = min_element(a.begin(), a.end()) - a.begin();
    int y = min_element(b.begin(), b.end()) - b.begin();
    vector<P> res;
    res.reserve(n + m);
    // 游标循环回绕；i、j 分别记录已经走过多少条边，不复制或补点。
    for (int i = 0, j = 0; i < n || j < m;) {
        int xx = x + 1 == n ? 0 : x + 1, yy = y + 1 == m ? 0 : y + 1;
        res.pb(a[x] + b[y]);
        i128 c = cross(a[xx] - a[x], b[yy] - b[y]);
        if (c >= 0 && i < n) x = xx, i++;
        if (c <= 0 && j < m) y = yy, j++;
    }
    return res;
}

// 点对差 A+(-B)，不是集合差；取负仍为逆时针。
vector<P> minkowskiDifference(const vector<P>& a, vector<P> b) {
    for (P& p : b) p = -p; // 关于原点中心对称，仍为逆时针，不要 reverse。
    return minkowski(a, b);
}


// 包含两个凸包的最小凸包，非普通并集；顺逆时针、任意起点，允许空/点/段。
// O(n+m) 时间和空间。选短的有序点列归并，舍弃复杂的四链直接写栈版本。
vector<P> mergeConvexHulls(const vector<P>& a, const vector<P>& b) {
    auto ordered = [](const vector<P>& p) {
        int n = p.size();
        vector<P> s(n);
        if (!n) return s;
        int l = min_element(p.begin(), p.end()) - p.begin(), r = l ? l - 1 : n - 1;
        for (P& q : s) {
            if (p[r] < p[l]) { q = p[r]; r = r ? r - 1 : n - 1; }
            else { q = p[l]; l = l + 1 == n ? 0 : l + 1; }
        }
        return s;
    };
    auto p = ordered(a), q = ordered(b);
    vector<P> s(p.size() + q.size());
    merge(p.begin(), p.end(), q.begin(), q.end(), s.begin());
    return convexHullSorted(move(s));
}


// 多个凸包平衡合并；总 N 点、k 组，O(k+N log(k+1))。
vector<P> mergeConvexHulls(const vector<vector<P>>& hulls) {
    auto solve = [&](auto&& self, int l, int r) -> vector<P> {
        if (l == r) return {};
        if (r - l == 1) {
            auto p = hulls[l];
            if (p.size() > 2 && cross(p[0], p[1], p.back()) < 0)
                reverse(p.begin() + 1, p.end());
            if (!p.empty()) rotate(p.begin(), min_element(p.begin(), p.end()), p.end());
            return p;
        }
        int m = l + (r - l) / 2;
        auto a = self(self, l, m), b = self(self, m, r);
        if (a.empty()) return b;
        if (b.empty()) return a;
        return mergeConvexHulls(a, b);
    };
    return solve(solve, 0, (int)hulls.size());
}

// ==================== [09] 多次切片重心与整条线段包含 ====================
// 依赖：01、02、03

// 循环边的面积与面积矩前缀，s[i] 对应 [0,i)；O(n) 预处理。
vector<array<i128, 3>> polygonPrefix(const vector<P>& p) {
    int n = p.size();
    vector<array<i128, 3>> s(n + 1);
    for (int i = 0; i < n; i++) {
        P a = p[i], b = p[i + 1 == n ? 0 : i + 1];
        i128 c = cross(a, b);
        s[i + 1] = {s[i][0] + c, s[i][1] + ((i128)a.x + b.x) * c,
                   s[i][2] + ((i128)a.y + b.y) * c};
    }
    return s;
}

// 严格逆时针凸多边形；u,v 不同且不相邻；返回 u→v 右侧切片重心，O(1)。
// s 必须由同一 p 构造，构造后不要修改 p；返回近似 FP。
FP sliceCentroid(const vector<P>& p, const vector<array<i128, 3>>& s, int u, int v) {
    int n = p.size();
    i128 c = cross(p[v], p[u]);
    array<i128, 3> a{c, ((i128)p[u].x + p[v].x) * c, ((i128)p[u].y + p[v].y) * c};
    for (int k = 0; k < 3; k++) {
        a[k] += s[v][k] - s[u][k];
        if (u > v) a[k] += s[n][k];
    }
    return realPoint(a[1], a[2], 3 * a[0]);
}

// 简单无洞多边形，允许沿边/过顶点/零长度；精确闭包含，O(n+k log k)。
bool segmentInPolygon(const vector<P>& p, P a, P b) {
    if (a == b) return inPolygon(p, a) >= 0;
    // 只保留 (0,1) 内的事件；左侧事件直接计入初始覆盖。
    struct Event { ll t, d; int add; }; // 存储 <=8e18，比较时转 i128。
    vector<Event> e{{0, 1, 0}, {1, 1, 0}};
    P v = b - a;
    int n = p.size(), sum = 0;
    i128 x = cross(v, p[0] - a);
    for (int i = 0; i < n; i++) {
        P u = p[i], w = p[i + 1 == n ? 0 : i + 1];
        i128 y = cross(v, w - a);
        int delta = sgn(y) - sgn(x);
        if (delta) {
            i128 t = cross(u - a, w - u), d = y - x;
            if (d < 0) t = -t, d = -d;
            if (t <= 0) sum += delta;
            else if (t < d) e.pb({(ll)t, (ll)d, delta});
        }
        x = y;
    }
    sort(e.begin(), e.end(), [](Event x, Event y) { return (i128)x.t * y.d < (i128)y.t * x.d; });
    for (int i = 0; i + 1 < (int)e.size(); i++) {
        sum += e[i].add;
        Event x = e[i], y = e[i + 1];
        if (sum == 0 && (i128)x.t * y.d < (i128)y.t * x.d) return false;
    }
    return true;
}

// ==================== [10] 三角剖分与三角形面积极值 ====================
// 依赖：01、03

// 凸边界扇形，0-base；不检查凸性；n<3 为空，O(n)。
vector<array<int, 3>> triangulateConvex(int n) {
    vector<array<int, 3>> ans(max(0, n - 2));
    for (int i = 1; i + 1 < n; i++) ans[i - 1] = {0, i, i + 1};
    return ans;
}

// 简单无洞、无重复顶点、非零面积；允许共线，方向任意；O(n²)。
vector<array<int, 3>> triangulate(const vector<P>& p) {
    int n = p.size();
    if (n < 3) return {};
    vector<int> pre(n), nxt(n), bad;
    vector<char> ear(n);
    int step = area2(p) > 0 ? 1 : -1;
    for (int i = 0; i < n; i++) {
        pre[i] = (i - step + n) % n;
        nxt[i] = (i + step + n) % n;
        // 平角点也收进名单，不能只收集严格凹点。
        if (cross(p[pre[i]], p[i], p[nxt[i]]) <= 0) bad.pb(i);
    }
    auto isEar = [&](int i) {
        int a = pre[i], c = nxt[i];
        P u = p[i] - p[a], v = p[c] - p[a];
        i128 area = cross(u, v);
        if (area <= 0) return false;
        // 包围盒外的点不用算叉积；边界上的点仍要检查。
        ll x0 = min({p[a].x, p[i].x, p[c].x}), x1 = max({p[a].x, p[i].x, p[c].x});
        ll y0 = min({p[a].y, p[i].y, p[c].y}), y1 = max({p[a].y, p[i].y, p[c].y});
        // 已知耳尖必为凸点；已删耳尖保持 ear=true，不必另记删除状态。
        for (int j : bad) if (!ear[j] && j != a && j != i && j != c) {
            if (p[j].x < x0 || p[j].x > x1 || p[j].y < y0 || p[j].y > y1) continue;
            P q = p[j] - p[a];
            i128 x = cross(u, q), y = cross(q, v);
            // 闭三角形：两个子面积非负，且和不超过总面积。
            if (x >= 0 && y >= 0 && x + y <= area) return false;
        }
        return true;
    };
    for (int i = 0; i < n; i++) ear[i] = isEar(i);
    vector<array<int, 3>> ans;
    ans.reserve(n - 2); // 结果恰好 n-2 项，只分配一次。
    int cur = 0;
    for (int left = n; left > 3; left--) {
        while (!ear[cur]) cur = nxt[cur];
        int a = pre[cur], c = nxt[cur];
        ans.pb({a, cur, c});
        nxt[a] = c;
        pre[c] = a;
        // 删除耳尖后，只重算左右邻点，不能每次重新找全部耳朵。
        // 凸性不会退回凹性；已成为耳尖的名单项以后不再阻挡。
        erase_if(bad, [&](int j) { return ear[j]; });
        ear[a] = isEar(a);
        ear[c] = isEar(c);
        cur = c;
    }
    ans.pb({pre[cur], cur, nxt[cur]});
    return ans;
}

// 严格逆时针凸包最大三角形二倍面积；O(h²)。
i128 maxTriangle2(const vector<P>& h) {
    int n = h.size();
    i128 ans = 0;
    for (int i = 0; i + 2 < n; i++) {
        int k = i + 2;
        for (int j = i + 1; j + 1 < n; j++) {
            k = max(k, j + 1);
            P v = h[j] - h[i];
            // 面积差等于边方向的叉积；保留 >=，平局时推进。
            while (k + 1 < n && cross(v, h[k + 1] - h[k]) >= 0) k++;
            ans = max(ans, cross(v, h[k] - h[i]));
        }
    }
    return ans;
}

// 输入三个不同下标可同坐标；返回最小/最大二倍面积，最坏 O(n²log n)。
pair<i128, i128> minmaxTriangle(vector<P> p) {
    if (p.size() < 3) return {0, 0};
    sort(p.begin(), p.end());
    int total = p.size();
    p.erase(unique(p.begin(), p.end()), p.end());
    int n = p.size();
    if (n < 3) return {0, 0};
    // 三个不同下标允许坐标重复；此时最小值已为零，只算最大值。
    if (n < total) return {0, maxTriangle2(convexHullSorted(move(p)))};
    // 全共线时两个答案都是零，不生成二次方数量的事件。
    if (all_of(p.begin() + 2, p.end(), [&](P q) { return cross(p[0], p[1], q) == 0; }))
        return {0, 0};
    // 找到任意一组三点共线就足够：最小值已为零，只需求最大值。
    for (int i = 2; i < n; i++)
        if (cross(p[i - 2], p[i - 1], p[i]) == 0)
            return {0, maxTriangle2(convexHullSorted(move(p)))};
    vector<pair<int, int>> events;
    events.reserve((size_t)n * (n - 1) / 2);
    for (int i = 0; i < n; i++) for (int j = i + 1; j < n; j++) events.pb({i, j});
    auto direction = [&](pair<int, int> e) { return p[e.second] - p[e.first]; };
    // 这里只比较原始点差：分量绝对值<=2e9，乘积之差可放入 ll。
    auto turn = [&](auto a, auto b) {
        P u = direction(a), v = direction(b);
        return u.x * v.y - u.y * v.x;
    };
    sort(events.begin(), events.end(), [&](auto a, auto b) { return turn(a, b) > 0; });
    vector<pair<int, int>> ranges; // 各方向组复用容量，不在每组重新分配。
    vector<int> order(n), pos(n);
    iota(order.begin(), order.end(), 0); iota(pos.begin(), pos.end(), 0);
    i128 low = (i128)1 << 126, high = 0;
    for (size_t l = 0; l < events.size();) {
        size_t r = l + 1;
        while (r < events.size() && turn(events[l], events[r]) == 0) r++;
        ranges.clear();
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
            // 扫描中找到共线三点，最小值已为零；不再处理其余方向。
            if (b - a >= 2) return {0, maxTriangle2(convexHullSorted(move(p)))};
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

// ==================== [11] 凸包极点与切线 ====================
// 依赖：01、03、07

// 以下极点二分只使用三个包装的方向规则，不传任意 lambda。
template<class F> int extreme(const vector<P>& h, F dir) {
    int n = h.size();
    if (n == 0) return -1;
    if (n <= 2) return n == 2 && cross(dir(h[0]), h[1] - h[0]) > 0 ? 1 : 0;
    auto check = [&](int i) { return cross(dir(h[i]), h[i + 1 == n ? 0 : i + 1] - h[i]) >= 0; };
    P d = dir(h[0]);
    bool c0 = check(0);
    if (!c0 && check(n - 1)) return 0;
    auto before = [&](int i) {
        bool c = check(i);
        if (c != c0) return c; // 已经能决定二分方向，不再计算位置叉积。
        i128 t = cross(d, h[i] - h[0]);
        if (i == 1 && t == 0) return true;
        return bool(c ^ (t <= 0));
    };
    int l = 1, r = n; // 原判定在下标 0 恒为真，直接排除。
    while (l < r) {
        int m = l + (r - l) / 2;
        if (before(m)) l = m + 1;
        else r = m;
    }
    return l == n ? 0 : l;
}

// 非零方向的最大点积下标；空 -1、平局任取；O(log n)。
int support(const vector<P>& h, P v) {
    return extreme(h, [&](P) { return P{v.y, -v.x}; });
}

// 给定非零平行方向，返回最大/最小叉积点；O(log n)。
pair<int, int> parallelTangents(const vector<P>& h, P v) {
    return {extreme(h, [&](P) { return v; }), extreme(h, [&](P) { return neg(v); })};
}

// 外点两切点；内部或边界返回 {-1,-1}；O(log n)。
pair<int, int> tangentFrom(const vector<P>& h, P q) {
    if (h.empty() || inConvex(h, q) >= 0) return {-1, -1};
    return {extreme(h, [&](P p) { return p - q; }),
            extreme(h, [&](P p) { return q - p; })};
}

// ==================== [12] 多线段扫描线找任意相交对 ====================
// 依赖：01、02

// O(n log n)，原下标，无交 {-1,-1}；不能改成枚举全部交点。
pair<int, int> anySegmentIntersection(vector<Segment> s) {
    int n = s.size();
    if (n < 2) return {-1, -1};
    // 同一横坐标按“插入非竖直线段、检查竖直线段、删除非竖直线段”处理，保留端点接触。
    struct Event { int x, type, id; }; // 原始横坐标在 1e9 范围内。
    vector<Event> ev;
    for (int i = 0; i < n; i++) {
        if (s[i].b < s[i].a) swap(s[i].a, s[i].b);
        if (s[i].a.x == s[i].b.x) ev.pb({(int)s[i].a.x, 1, i});
        else {
            ev.pb({(int)s[i].a.x, 0, i});
            ev.pb({(int)s[i].b.x, 2, i});
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
        bool operator()(int i, int j) const {
            auto a = (*s)[i], b = (*s)[j];
            // 只比较同时活动的非竖直段，在较晚的左端点处判上下。
            i128 z = a.a.x < b.a.x ? cross(a.a, a.b, b.a) : -cross(b.a, b.b, a.a);
            return z != 0 ? z > 0 : i < j;
        }
        bool operator()(int i, ll y) const {
            auto a = (*s)[i]; return cross(a.a, a.b, P{*x, y}) > 0;
        }
        bool operator()(ll y, int i) const {
            auto a = (*s)[i]; return cross(a.a, a.b, P{*x, y}) < 0;
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

// ==================== [13] 无盒半平面可行点（含点／线退化） ====================
// 依赖：01、02

// 无盒闭半平面可行性：空交返回 nullopt，否则返回距原点最近点的近似坐标。
// 是否有解的所有分支仍用整数；不要把返回 FP 代回去做精确边界判断。
// 内部 x,y,d 只是当前点的三个系数；不公开、不构造分数类型。
// 随机顺序期望 O(n)、最坏 O(n²)，原始整数端点界 1e9；输入不变。
optional<FP> halfPlanePoint(vector<Line> a, ull seed = chrono::steady_clock::now().time_since_epoch().count()) {
    if (all_of(a.begin(), a.end(), [](Line l) { return cross(l.a, l.b) >= 0; })) return FP{};
    mt19937_64 rng(seed);
    shuffle(a.begin(), a.end(), rng);
    i128 x = 0, y = 0, den = 1;
    for (int i = 0; i < (int)a.size(); i++) {
        P v = a[i].dir();
        if ((i128)v.x * (y - (i128)a[i].a.y * den) -
            (i128)v.y * (x - (i128)a[i].a.x * den) >= 0) continue;
        i128 loN = 0, loD = 0, hiN = 0, hiD = 0; // 分母0表示该侧无界。
        for (int j = 0; j < i; j++) {
            P u = a[j].dir();
            i128 c = cross(u, a[i].a - a[j].a), d = cross(u, v);
            if (d == 0) { if (c < 0) return nullopt; continue; }
            i128 t = -c;
            if (d > 0) {
                if (!loD || t * loD > loN * d) loN = t, loD = d;
            } else {
                t = -t; d = -d;
                if (!hiD || t * hiD < hiN * d) hiN = t, hiD = d;
            }
        }
        if (loD && hiD && loN * hiD > hiN * loD) return nullopt;
        i128 t = -dot(a[i].a, v), d = dot(v, v); // 垂足参数。
        if (loD && t * loD < loN * d) t = loN, d = loD;
        if (hiD && t * hiD > hiN * d) t = hiN, d = hiD;
        x = (i128)a[i].a.x * d + (i128)v.x * t;
        y = (i128)a[i].a.y * d + (i128)v.y * t;
        den = d;
    }
    return realPoint(x, y, den);
}

// ==================== [14] 圆与简单多边形的交面积 ====================
// 依赖：01、03、05、07 的补偿叉积

// 简单无洞整数多边形，逆时针正、顺时针负；O(n)，稳定数值公式。
ld circlePolygonArea(Circle c, const vector<P>& p) {
    if (c.r == 0) return 0;
    i128 rr = (i128)c.r * c.r;
    // 整个多边形都在圆内时只算整数面积，不生成交点、圆弧或补偿乘积。
    if (all_of(p.begin(), p.end(), [&](P a) { return dist2(a, c.c) <= rr; }))
        return (ld)area2(p) / 2;
    int turns = 0, pieces = 0, anchor = -1;
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
        i128 d = dot(v, v), av = dot(a, v), bv = av + d, h = rr * d - z * z;
        if (!(aa < rr || bb < rr || (h > 0 && av < 0 && bv > 0))) continue;
        FP u = toFP(a), w = toFP(b);
        ld root = 0;
        // 两端都在圆内时，后续片段不用构造直线交点。
        // 第一段仍保留 root，用于仅有一条弦时的稳定半角。
        if (aa > rr || bb > rr || pieces == 0) root = sqrtl((ld)h);
        if (aa > rr || bb > rr) {
            FP mid = toFP(perp(v)) * (-(ld)z / (ld)d);
            FP off = toFP(v) * (root / (ld)d);
            if (aa > rr) u = mid - off;
            if (bb > rr) w = mid + off;
        }
        // 原边的绕数减去圆内部分，剩下圆外路径的绕数；共线穿心边两项均为零。
        if (u.y <= 0 && w.y > 0 && z > 0) turns--;
        if (w.y <= 0 && u.y > 0 && z < 0) turns++;
        if (pieces == 0) {
            origin = first = u;
            if (aa <= rr) anchor = i; // 局部原点就是原输入顶点，可用整数弦积分。
            onlyHalf = atan2l(root, (ld)abs128(z));
            if (z > 0) onlyHalf = -onlyHalf;
        } else arc(last, u, 0);
        if (anchor >= 0 && aa <= rr && bb <= rr)
            ans.add((ld)cross(p[anchor], p[i], p[(i + 1) % n]) / 2);
        else ans.add(chord(u, w));
        last = w;
        pieces++;
    }
    if (pieces == 0) return turns * PI * (ld)rr;
    // 只有一段圆内边时，直接用它的整数判别式求半角，不从两个近似交点反推小角度。
    arc(last, first, pieces == 1 ? onlyHalf : 0);
    ans.add(turns * PI * (ld)rr);
    return ans.value();
}

// ==================== [15] 多个多边形并面积与所有覆盖层 ====================
// 依赖：01、03

// ans[k-1] 为至少覆盖 k 次的面积；简单无洞、方向任意、重合共边允许。
// 边数 E：最坏 O(E² log E)；本节较长，只在题目需要时摘取。
// 局部事件保存整数参数，避免重新引入浮点事件顺序和已修复的零层残差。
vector<ld> polygonCoverageAreas(vector<vector<P>> p) {
    int total = p.size();
    if (!total) return {};
    if (total == 1) return {(ld)abs128(area2(p[0])) / 2};
    P lo{1000000000, 1000000000}, hi{-1000000000, -1000000000};
    for (const auto& a : p) for (P q : a) {
        chmin(lo.x, q.x); chmin(lo.y, q.y);
        chmax(hi.x, q.x); chmax(hi.y, q.y);
    }
    // 面积在交换坐标轴后不变；随后统一方向，继续复用横向筛选。
    if (hi.y - lo.y > hi.x - lo.x)
        for (auto& a : p) for (P& q : a) swap(q.x, q.y);
    P origin{};
    for (auto& a : p) {
        if (!a.empty()) origin = a[0];
        i128 ar = area2(a);
        if (ar == 0) a.clear();
        else {
            // 只去掉同一条边上的冗余顶点，不做凸包、不填平凹口。
            a.erase(unique(a.begin(), a.end()), a.end());
            if (a.front() == a.back()) a.pop_back();
            int m = a.size(), k = 0;
            P prev = a.back(), first = a.front();
            for (int i = 0; i < m; i++) {
                P cur = a[i], next = i + 1 == m ? first : a[i + 1];
                if (cross(prev, cur, next) != 0) a[k++] = cur;
                prev = cur; // 必须保存原前驱，不能改用已经压紧的 a[i-1]。
            }
            a.resize(k);
            if (ar < 0) reverse(a.begin(), a.end());
            rotate(a.begin(), min_element(a.begin(), a.end()), a.end());
        }
    }
    // 去除共线细分后规范化，压缩相同轮廓并保留覆盖重数。
    sort(p.begin(), p.end());
    vector<int> weight;
    int n = 0;
    for (int i = 0; i < total; i++) if (!p[i].empty()) {
        if (n && p[n - 1] == p[i]) ++weight.back();
        else {
            if (n != i) p[n] = move(p[i]); // 避免 vector 自移动。
            ++n; weight.pb(1);
        }
    }
    p.resize(n);
    // 只有一种非退化轮廓时，前 weight[0] 层都是它本身，无需事件扫描。
    if (n < 2) {
        vector<ld> ans(total);
        if (n) fill_n(ans.begin(), weight[0], (ld)area2(p[0]) / 2);
        return ans;
    }
    vector<array<ll, 4>> box(n);
    vector<ll> right(n); // 前缀包围盒的最大横坐标，单调不减。
    for (int i = 0; i < n; i++) {
        box[i] = {p[i][0].x, p[i][0].x, p[i][0].y, p[i][0].y};
        for (P q : p[i]) {
            chmin(box[i][0], q.x); chmax(box[i][1], q.x);
            chmin(box[i][2], q.y); chmax(box[i][3], q.y);
        }
        right[i] = i ? max(right[i - 1], box[i][1]) : box[i][1];
    }
    // 差分覆盖层：当前边同时贡献给 [count,count+weight[i])。
    vector<Sum> sums(total + 1);
    vector<i128> whole(total + 1);
    struct Event { ll t, d; int add; };
    vector<Event> ev;
    for (int i = 0; i < n; i++) for (int k = 0, m = p[i].size(); k < m; k++) {
        P a = p[i][k], b = p[i][k + 1 == m ? 0 : k + 1], v = b - a;
        i128 base = cross(a - origin, b - origin);
        if (base == 0) continue;
        ll x0 = min(a.x, b.x), x1 = max(a.x, b.x);
        ll y0 = min(a.y, b.y), y1 = max(a.y, b.y);
        int count = 0;
        ev = {{1, 1, 0}};
        auto add = [&](i128 t, i128 d, int delta) {
            if (d < 0) t = -t, d = -d;
            if (t <= 0) count += delta;
            else if (t < d) ev.pb({(ll)t, (ll)d, delta});
        };
        auto endpoint = [&](P c, int delta) {
            if (v.x) add(c.x - a.x, v.x, delta);
            else add(c.y - a.y, v.y, delta);
        };
        // 只跳过确定在横向无交的前后缀；剩余候选仍按原编号递增。
        for (int j = lower_bound(right.begin(), right.end(), x0) - right.begin(); j < n; j++) {
            if (box[j][0] > x1) break;
            if (j == i) continue;
            if (box[j][1] < x0 ||
                y1 < box[j][2] || box[j][3] < y0) continue;
            i128 x = cross(v, p[j][0] - a);
            for (int t = 0, m = p[j].size(); t < m; t++) {
                P c = p[j][t], d = p[j][t + 1 == m ? 0 : t + 1];
                i128 y = cross(v, d - a);
                int sc = sgn(x), sd = sgn(y);
                if (sc != sd && min(sc, sd) < 0) {
                    add(cross(c - a, d - c), y - x, sc > sd ? weight[j] : -weight[j]);
                } else if (sc == 0 && sd == 0 && j < i && dot(v, d - c) > 0) {
                    endpoint(c, weight[j]); endpoint(d, -weight[j]);
                }
                x = y;
            }
        }
        sort(ev.begin(), ev.end(), [](Event a, Event b) { return (i128)a.t * b.d < (i128)b.t * a.d; });
        ll lastN = 0, lastD = 1;
        i128 lastInt = 0;
        ld lastFrac = 0;
        for (auto [t, d, delta] : ev) {
            if ((i128)lastN * d < (i128)t * lastD) {
                i128 z = base * t, curInt = z / d;
                ld curFrac = ld(z % d) / ld(d);
                whole[count] += curInt - lastInt;
                whole[count + weight[i]] -= curInt - lastInt;
                sums[count].add(curFrac - lastFrac);
                sums[count + weight[i]].add(lastFrac - curFrac);
                lastInt = curInt; lastFrac = curFrac;
            }
            count += delta;
            lastN = t; lastD = d;
        }
    }
    vector<ld> ans(total);
    i128 integer = 0;
    Sum fraction;
    for (int i = 0; i < total; i++) {
        integer += whole[i];
        fraction.add(sums[i].s); fraction.add(sums[i].c);
        ans[i] = (ld(integer) + fraction.value()) / 2;
    }
    return ans;
}

// ==================== [16] 多个圆的并面积与所有覆盖层 ====================
// 依赖：01、05、07 的补偿叉积

// ans[k-1] 为至少覆盖 k 次的面积，重复圆计重；最坏 O(n²log n)。
// 保留共享弦、稳定半角和分离原点；不是精确实数几何，复杂专题按需摘取。
vector<ld> circleCoverageAreas(vector<Circle> input) {
    int total = input.size();
    erase_if(input, [](Circle c) { return c.r == 0; }); // 尾部零层仍由 total 保留。
    P lo{}, hi{};
    if (!input.empty()) lo = hi = input[0].c;
    for (Circle c : input) {
        chmin(lo.x, c.c.x); chmin(lo.y, c.c.y);
        chmax(hi.x, c.c.x); chmax(hi.y, c.c.y);
    }
    bool byY = hi.y - lo.y > hi.x - lo.x;
    auto coord = [&](P p) { return byY ? p.y : p.x; };
    sort(input.begin(), input.end(), [&](Circle a, Circle b) {
        if (byY) return tie(a.c.y, a.c.x, a.r) < tie(b.c.y, b.c.x, b.r);
        return tie(a.c.x, a.c.y, a.r) < tie(b.c.x, b.c.y, b.r);
    });
    vector<int> weight;
    int m = 0;
    for (Circle a : input) {
        if (m && input[m - 1].c == a.c && input[m - 1].r == a.r) weight.back()++;
        else input[m++] = a, weight.pb(1);
    }
    input.resize(m);
    // 零半径已排除；只有一种圆时直接展开重数，尾部零层保留。
    if (m < 2) {
        vector<ld> ans(total);
        if (m) fill_n(ans.begin(), weight[0], PI * input[0].r * input[0].r);
        return ans;
    }
    // 同心圆按半径逆序直接展开：第 k 层就是第 k 大半径的圆盘。
    if (input.front().c == input.back().c) {
        vector<ld> ans(total);
        int k = 0;
        for (int i = m - 1; i >= 0; i--) {
            fill_n(ans.begin() + k, weight[i], circleArea(input[i]));
            k += weight[i];
        }
        return ans;
    }
    // 只剩两种圆时直接展开三段覆盖层，不建立圆弧事件。
    if (m == 2) {
        if (weight[0] > weight[1]) swap(weight[0], weight[1]), swap(input[0], input[1]);
        ld a = circleArea(input[0]), b = circleArea(input[1]);
        ld inter = circleIntersectionArea(input[0], input[1]);
        vector<ld> ans(total);
        fill(ans.begin(), ans.begin() + weight[0], a + b - inter);
        fill(ans.begin() + weight[0], ans.begin() + weight[1], b);
        fill(ans.begin() + weight[1], ans.begin() + weight[0] + weight[1], inter);
        return ans;
    }
    const auto& c = input; // 按值参数已是副本，直接压紧，不再复制圆表。
    // 同一对圆按固定顺序构造交点，弦叉积也固定顺序求值，保证公共弦互为相反项。
    struct Event { FP p, v; int add, other; ld root; };
    auto before = [](FP a, FP b) {
        auto half = [](FP p) { return p.y < 0 || (p.y == 0 && p.x < 0); };
        if (half(a) != half(b)) return half(a) < half(b);
        return crossSign(a, b) > 0;
    };
    // 按正面积相交分组；仅外切不合并，各组取自己的整数积分原点。
    vector<int> parent(c.size());
    iota(parent.begin(), parent.end(), 0);
    auto find = [&](int x) {
        while (parent[x] != x) x = parent[x] = parent[parent[x]];
        return x;
    };
    // 圆心已按选定坐标排序；轴向距离达到最大直径，不可能有正面积交。
    ll reach = 0;
    for (Circle a : c) chmax(reach, 2 * a.r);
    for (int i = 0, l = 0; i < m; i++) {
        while (coord(c[i].c) - coord(c[l].c) >= reach) l++;
        for (int j = l; j < i; j++) {
            ll r = c[i].r + c[j].r;
            if (dist2(c[i].c, c[j].c) < (i128)r * r)
                parent[find(i)] = find(j);
        }
    }
    for (int i = 0; i < m; i++) parent[i] = find(i);
    // 同组成员按原编号递增遍历，保持原事件生成顺序和积分原点。
    vector<int> head(m, -1), next(m);
    for (int i = m - 1; i >= 0; i--) {
        next[i] = head[parent[i]];
        head[parent[i]] = i;
    }
    vector<Sum> diff(total + 1);
    vector<Event> ev; // 每个圆复用事件缓冲，不改动数值公式。
    for (int i = 0; i < (int)c.size(); i++) {
        ev.clear();
        P origin = c[parent[i]].c;
        int count = 0;
        for (int j = head[parent[i]]; j != -1; j = next[j]) if (i != j) {
            ll ri = c[i].r, rj = c[j].r;
            i128 d = dist2(c[i].c, c[j].c);
            i128 sum = (i128)(ri + rj) * (ri + rj), dif = (i128)(ri - rj) * (ri - rj);
            if (rj >= ri && d <= dif) { count += weight[j]; continue; }
            if (d >= sum || d <= dif) continue;
            ld root = sqrtl((ld)(sum - d) * (ld)(d - dif));
            int lo = min(i, j), hi = max(i, j);
            P v = c[hi].c - c[lo].c, o = c[lo].c - origin;
            i128 t = d + (i128)c[lo].r * c[lo].r - (i128)c[hi].r * c[hi].r;
            FP mid = realPoint(2 * (i128)o.x * d + (i128)v.x * t,
                       2 * (i128)o.y * d + (i128)v.y * t, 2 * d);
            FP off = toFP(perp(v)) * (root / (2 * (ld)d));
            FP a = mid - off, b = mid + off;
            if (i == hi) swap(a, b);
            v = c[j].c - c[i].c;
            t = d + (i128)ri * ri - (i128)rj * rj;
            FP base = toFP(v) * ((ld)t / (2 * (ld)d));
            off = toFP(perp(v)) * (root / (2 * (ld)d));
            FP u = base - off, w = base + off;
            if (before(w, u)) count += weight[j];
            ev.pb({a, u, weight[j], j, root});
            ev.pb({b, w, -weight[j], j, root});
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
            if (a.other == b.other) {
                // 只有同一圆对的两个事件相邻时才需要该稳定半角。
                int j = a.other;
                i128 t = dist2(c[i].c, c[j].c) + (i128)c[i].r * c[i].r - (i128)c[j].r * c[j].r;
                half = atan2l(a.root, a.add > 0 ? (ld)t : -(ld)t);
            } else {
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

// ==================== [17] 圆反演（低频构造） ====================
// 依赖：01、02、05

// 0 无有限结果，1 圆，2 直线；构造坐标近似。
struct InversionResult { int type = 0; FCircle circle{}; FLine line{}; };

// 反演半径正；点在反演中心返回空。
optional<FP> invertPoint(Circle o, P p) {
    P v = p - o.c;
    i128 d = dot(v, v), r2 = (i128)o.r * o.r;
    if (d == 0) return nullopt;
    return realPoint((i128)o.c.x * d + (i128)v.x * r2,
             (i128)o.c.y * d + (i128)v.y * r2, d);
}

// 直线反演，经过中心仍为直线但排除中心点。
InversionResult invertLine(Circle o, Line l) {
    P v = l.dir(), n = perp(v);
    i128 h = cross(v, l.a - o.c), r2 = (i128)o.r * o.r;
    if (h == 0) return {2, {}, {toFP(l.a), toFP(v)}};
    FP c = realPoint(2 * (i128)o.c.x * h + (i128)n.x * r2,
             2 * (i128)o.c.y * h + (i128)n.y * r2, 2 * h);
    ld r = (ld)r2 * sqrtl((ld)dot(v, v)) / (2 * fabsl((ld)h));
    return {1, {c, r}, {}};
}

// 圆反演，经过中心变为直线。
InversionResult invertCircle(Circle o, Circle a) {
    P v = a.c - o.c;
    i128 d = dot(v, v), den = d - (i128)a.r * a.r, r2 = (i128)o.r * o.r;
    if (den == 0) {
        if (d == 0) return {};
        FP p = realPoint(2 * (i128)o.c.x * d + (i128)v.x * r2,
                 2 * (i128)o.c.y * d + (i128)v.y * r2, 2 * d);
        return {2, {}, {p, toFP(perp(v))}};
    }
    FP c = realPoint((i128)o.c.x * den + (i128)v.x * r2,
             (i128)o.c.y * den + (i128)v.y * r2, den);
    return {1, {c, (ld)r2 * a.r / fabsl((ld)den)}, {}};
}

} // namespace Geo
