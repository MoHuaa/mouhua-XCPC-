template<typename T> struct Tre
{
    int n, m = 0;
    vector<vector<int>> e;
    vector<int> to;
    vector<T> wt;
    int maxk;
    vector<vector<int>> fa;
    vector<int> dep;
    void add_arc(const int u, const int v, const T w = 0) {e[u].push_back(m++); to.push_back(v); wt.push_back(w);}
    void add_edge(const int u, const int v, const T w = 0) {add_arc(u, v, w); add_arc(v, u, w);}
    Tre(const int n, const int maxk = 25): n(n), e(n), maxk(maxk), fa(n, vector<int>(maxk + 1, -1)), dep(n) {}
    void dfs(const int u, const int f)
    {
        fa[u][0] = f;
        dep[u] = f == -1 ? 0 : dep[f] + 1;
        for (int i = 1; i <= maxk; i++) fa[u][i] = fa[u][i - 1] == -1 ? -1 : fa[fa[u][i - 1]][i - 1];
        for (const int i : e[u])
        {
            const int v = to[i];
            if (v == f) continue;
            dfs(v, u);
        }
    }

    int lca(int u, int v) const
    {
        if (dep[u] < dep[v]) swap(u, v);
        for (int i = maxk; i >= 0; i--)
        {
            if (fa[u][i] != -1 && dep[fa[u][i]] >= dep[v]) u = fa[u][i];
        }
        if (u == v) return u;
        for (int i = maxk; i >= 0; i--)
        {
            if (fa[u][i] != fa[v][i]) u = fa[u][i], v = fa[v][i];
        }
        return fa[u][0];
    }
};