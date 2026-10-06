// T 为域类型（如素数模 mint），要求 +,-,*,/,==；普通整数除法不适用。
// 点 1..n，addEdge(u,v,w) 加权边，work(root) 返回生成树边权积之和；O(n^3)。
// directed=true：所有点沿有向边走向 root；要出树，加入时反向。
// 支持重边；自环不影响结果；n=1 空生成树权积为 1。
template<class T>
struct MatrixTree {
    int n; bool directed;
    vector<vector<T>> a;
    MatrixTree(int n, bool directed = false) : n(n), directed(directed), a(n, vector<T>(n)) {}
    void addEdge(int u, int v, T w = T{1}) {
        --u; --v;
        if (u == v) return;
        a[u][u] = a[u][u] + w; a[u][v] = a[u][v] - w;
        if (!directed) a[v][v] = a[v][v] + w, a[v][u] = a[v][u] - w;
    }
    T work(int root = 1) const {
        assert(1 <= root && root <= n); --root;
        vector<vector<T>> b(n - 1, vector<T>(n - 1));
        for (int i = 0, x = 0; i < n; ++i) if (i != root) {
            for (int j = 0, y = 0; j < n; ++j) if (j != root) b[x][y++] = a[i][j];
            ++x;
        }
        T ans{1};
        for (int i = 0; i < n - 1; ++i) {
            int k = i;
            while (k < n - 1 && b[k][i] == T{}) ++k;
            if (k == n - 1) return T{};
            if (k != i) swap(b[k], b[i]), ans = T{} - ans;
            ans = ans * b[i][i]; T inv = T{1} / b[i][i];
            for (int j = i + 1; j < n - 1; ++j) {
                T z = b[j][i] * inv;
                for (int k = i; k < n - 1; ++k) b[j][k] = b[j][k] - z * b[i][k];
            }
        }
        return ans;
    }
};
