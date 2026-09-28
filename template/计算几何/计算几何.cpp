// 二维几何 R4 常用层整理：0-base；完整索引保留 R3 全功能，单题摘专题。
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