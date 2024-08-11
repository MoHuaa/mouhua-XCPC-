constexpr int maxn=1e5+7;
int dval[maxn], dfn[maxn], tot,Dfn[maxn];
namespace Rmq {
    int st[20][maxn / 32];
    int pre[maxn], p[maxn], w[maxn];
    inline int getmin(int x, int y) { return dfn[x] < dfn[y] ? x : y; }
    inline void down(int & x, int y) { if (dfn[x] > dfn[y]) x = y; }
    inline int qry(int l, int r) {
        const int lg = std::__lg(r - l);
        return l >= r ? 0 : getmin(st[lg][l], st[lg][r - (1 << lg)]);
    }
    inline int rmq(int l, int r) {
        if (l >> 5 == r >> 5) return p[l + __builtin_ctz(w[r] >> l)];
        else return getmin(qry((l >> 5) + 1, r >> 5), getmin(dval[l], pre[r]));
    }
    inline void build(int n) {
        ++ (n |= 31);
        memcpy(p, dval, n << 2);
        for (int i = 0; i < n; i += 32) {
            static int st[33];
            pre[i] = dval[i];
            int * top = st + 1, s = 1; w[*top = i] = s;
            for (int j = i + 1; j < i + 32; ++j) {
                for (; top != st && dfn[dval[j]] < dfn[dval[*top]]; --top) s ^= 1 << *top;
                w[j] = s |= 1 << j; *++top = j; pre[j] = dval[st[1]];
            }
            for (int j = i + 30; j >= i; --j) down(dval[j], dval[j + 1]);
            Rmq::st[0][i >> 5] = dval[i];
        }
        for (int i = 1; i < 15; ++i)
            for (int j = 0; j + (1 << i) - 1 <= n / 32; ++j)
                st[i][j] = getmin(st[i - 1][j], st[i - 1][j + (1 << i - 1)]);
    }
}
struct T { int to, nxt; } way[maxn << 1];
int h[maxn], num;
inline void addEdge(int x, int y) {
    way[++num] = {y, h[x]}, h[x] = num;
    way[++num] = {x, h[y]}, h[y] = num;
}
inline void dfs(int x, int f) {
    dval[tot] = f; dfn[x] = ++tot;
    for (int i = h[x]; i; i = way[i].nxt) if (way[i].to != f)
            dfs(way[i].to, x);
    Dfn[x]=tot;
}
inline int lca(int x, int y) {
    if (dfn[x] > dfn[y]) std::swap(x, y);
    return x == y ? x : Rmq::rmq(dfn[x], dfn[y] - 1);
}
void init() {
    for (int i = 0; i <= num; i++) {
        way[i].to = 0;
        way[i].nxt = 0;
    }
    for (int i = 0; i <= tot; i++) {
        dval[i] = 0;
        h[i] = 0;
    }
    tot = 0;
    num = 0;
}
void Getfa(int s,int n) {
    dfs(s, 0); *dfn = 1e9; Rmq::build(n - 1);
}