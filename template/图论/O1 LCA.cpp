struct Trh {
    std::vector<int> dep, parent, in;
    int cur, n;
    int logn;
    std::vector<std::vector<int>> e;
    vector<vector<int>>a;
    Trh(int _n) : n(_n),dep(_n), parent(_n, -1), e(_n), in(_n), cur(1) {
        logn = std::__lg(n);
        a.assign(logn + 1, std::vector<int>(n + 1));
    }
    void addEdge(int u, int v) {
        e[u].push_back(v);
        e[v].push_back(u);
    }
    void dfs (int x) {
        in[x] =cur++;
        if (cur > 1) {
            a[0][cur - 2] = parent[x];
        }
        for (auto y : e[x]) {
            if (y == parent[x]) {
                continue;
            }
            parent[y] = x;
            dep[y] = dep[x] + 1;
            dfs(y);
        }
    }
    void init(int s) {
        dfs(s);
        for (int j = 0; j < logn; j++) {
            for (int i = 1; i + (2 << j) <= n; i++) {
                a[j + 1][i] = dep[a[j][i]] < dep[a[j][i + (1 << j)]] ? a[j][i] : a[j][i + (1 << j)];
            }
        }
    }
    int  lca(int x, int y) {
        if (x == y) {
            return x;
        }
        if (in[x] > in[y]) {
            std::swap(x, y);
        }
        int k = std::__lg(in[y] - in[x]);
        int u = a[k][in[x]];
        int v = a[k][in[y] - (1 << k)];
        return dep[u] < dep[v] ? u : v;
    }
};