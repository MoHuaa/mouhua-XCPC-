struct SegGraphR {
    int n, tot;
    SegGraphR(int n) : n(n), tot(n + 2 * max(0, n - 1)) { assert(n >= 0); }
    int node(int k = 1) {
        assert(k >= 0);
        int p = tot + 1; tot += k;
        return p;
    }
    int id(int l, int r, bool rev) const {
        return l == r ? l : n + rev * (n - 1) + (l + r) / 2;
    }
    void build(auto& g, auto... w) const {
        auto dfs = [&](auto&& self, int l, int r) -> void {
            if (l == r) return;
            int m = (l + r) / 2;
            for (auto [a, b] : {pair{l, m}, pair{m + 1, r}}) {
                g.addEdge(id(l, r, 0), id(a, b, 0), w...);
                g.addEdge(id(a, b, 1), id(l, r, 1), w...);
                self(self, a, b);
            }
        };
        if (n) dfs(dfs, 1, n);
    }
    void link(auto& g, int u, int L, int R, bool rev, auto... w) const {
        if (!n || L > R) return;
        assert(1 <= L && R <= n);
        auto add = [&](int v) {
            if (rev) g.addEdge(v, u, w...);
            else g.addEdge(u, v, w...);
        };
        auto dfs = [&](auto&& self, int l, int r) -> void {
            if (L <= l && r <= R) return add(id(l, r, rev));
            int m = (l + r) / 2;
            if (L <= m) self(self, l, m);
            if (R > m) self(self, m + 1, r);
        };
        dfs(dfs, 1, n);
    }
    void to(auto& g, int u, int l, int r, auto... w) const { link(g, u, l, r, 0, w...); }
    void from(auto& g, int u, int l, int r, auto... w) const { link(g, u, l, r, 1, w...); }
};
// 递归用 SegGraphR；非递归用 SegGraphI，以下接口完全相同。
// 公共头：#include<bits/stdc++.h>、using namespace std;，C++20。
// n 是原点数；seg.tot 是含两棵树和登记的业务点的总点数/最大点号。
// 本体只存 n、tot；不保存边，不更改 SCC、Dinic 的接口或工作数组。

// 1. SCC：只需要区间边，不增加业务点。
SegGraphI seg(n);
SCC g(seg.tot);                   // 原 SCC 构造函数；自动用精确点数初始化
seg.build(g);                    // 直接把两棵树的骨架加入 g，只调用一次

g.addEdge(u, v);                  // u -> v
seg.to(g, u, l, r);               // u -> [l,r]
seg.from(g, u, l, r);             // [l,r] -> u

g.work();
bool same = g.bel[x] == g.bel[y];  // 原点一直是 1..n，无需转换编号

// 2. SCC：区间 -> 区间。一条关系用一个独占中转点。
SegGraphR seg(n);
int p = seg.node();               // 只登记编号，必须在构造 g 前完成
SCC g(seg.tot);
seg.build(g);
seg.from(g, p, l1, r1);
seg.to(g, p, l2, r2);             // [l1,r1] -> [l2,r2]

// 多条关系可先 int first = seg.node(m)，第 i 条用 first+i，i 从 0 开始。

// 3. 网络流：整条区间规则共享容量 C，B 取本模型可发送总流量的上界。
SegGraphI seg(n);
int s = seg.node(), t = seg.node();
int p = seg.node(), q = seg.node();
Dinic<ll> f(seg.tot + 1);         // 原 Dinic 参数是数组大小，0 号留空
seg.build(f, B);                  // 骨架容量 B
f.addEdge(s, u, supply);
f.addEdge(v, t, demand);
seg.from(f, p, l1, r1, B);
f.addEdge(p, q, C);               // 容量门：整条规则最多通过 C
seg.to(f, q, l2, r2, B);
ll ans = f.flow(s, t);

// 4. 费用流：仍传原对象，addEdge(u,v,cap,cost) 的参数原样转发。
// 同样先在 seg 中登记全部业务点，再构造 MCF<ll,ll> f(seg.tot+1)。
seg.build(f, B, 0LL);             // 骨架容量 B、费用 0
seg.to(f, u, l, r, B, cost);      // u 到区间，一次转移只计一次 cost
seg.from(f, u, l, r, B, cost);    // 区间到 u

// 重要：node() 只更新 seg.tot，绝不会扩容已存在的 g/f。
// 所有 node() 都在创建算法对象之前完成；创建之后只 build、加边、求解。
// 两版默认建两个方向；n>=1 时基底点数是 3n-2，此公式由构造函数计算。
// 数据范围需保证点号、总点数和内层 int 运算不溢出。