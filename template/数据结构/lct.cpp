template<class T = ll>
struct LCT {
    struct Node {
        int ch[2]{}, p = 0;
        bool rev = false;
        T val{}, sum{};
    };
    vector<Node> t;
    vector<int> stk;
    LCT(int n) : t(n + 1), stk(n + 1) {}
    bool isRoot(int x) {
        int p = t[x].p;
        return t[p].ch[0] != x && t[p].ch[1] != x;
    }
    void pull(int x) {
        t[x].sum = t[t[x].ch[0]].sum + t[x].val + t[t[x].ch[1]].sum;
    }
    void reverse(int x) {
        if (!x) return;
        swap(t[x].ch[0], t[x].ch[1]);
        t[x].rev ^= 1;
    }
    void push(int x) {
        if (t[x].rev) {
            reverse(t[x].ch[0]); reverse(t[x].ch[1]);
            t[x].rev = false;
        }
    }
    void rotate(int x) {
        int y = t[x].p, z = t[y].p, k = t[y].ch[1] == x;
        int w = t[x].ch[k ^ 1];
        if (!isRoot(y)) t[z].ch[t[z].ch[1] == y] = x;
        t[x].p = z;
        t[y].ch[k] = w;
        if (w) t[w].p = y;
        t[x].ch[k ^ 1] = y; t[y].p = x;
        pull(y);
    }
    void splay(int x) {
        int top = 0, y = x;
        stk[top++] = y;
        while (!isRoot(y)) stk[top++] = y = t[y].p;
        while (top) push(stk[--top]);
        while (!isRoot(x)) {
            int y = t[x].p, z = t[y].p;
            if (!isRoot(y))
                rotate((t[y].ch[1] == x) == (t[z].ch[1] == y) ? y : x);
            rotate(x);
        }
        pull(x);
    }
    int access(int x) {
        int y = 0;
        for (int u = x; u; u = t[u].p) {
            splay(u);
            t[u].ch[1] = y;
            pull(u);
            y = u;
        }
        splay(x);
        return y;
    }
    void makeRoot(int x) { access(x); reverse(x); }
    int findRoot(int x) {
        access(x);
        while (true) {
            push(x);
            if (!t[x].ch[0]) break;
            x = t[x].ch[0];
        }
        splay(x);
        return x;
    }
    bool same(int x, int y) {
        if (x == y) return true;
        access(x); access(y);
        return t[x].p != 0;
    }
    void split(int x, int y) { makeRoot(x); access(y); }
    void link(int x, int y) { makeRoot(x); t[x].p = y; }
    void cut(int x, int y) {
        split(x, y);
        t[y].ch[0] = t[x].p = 0;
        pull(y);
    }
    void set(int x, T v) { access(x); t[x].val = v; pull(x); }
    T query(int x, int y) { split(x, y); return t[y].sum; }
    int lca(int r, int x, int y) {
        makeRoot(r); access(x);
        return access(y);
    }
};