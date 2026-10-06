// Hungarian 最小完整指派：n 行 m 列，n<=m，每行选不同的一列。
// a 使用 0 基完整有限费用矩阵；不支持禁边 INF。T 须容纳势能与中间差值。
// 返回 {最小费用, 每行所选列的0基下标}，n>m 返回 nullopt；O(n^2 m)。
template<class T>
optional<pair<T, vector<int>>> hungarian(const vector<vector<T>>& a) {
    int n = a.size(), m = n ? a[0].size() : 0;
    if (n > m) return nullopt;
    for (const auto& row : a) assert(int(row.size()) == m);
    vector<T> u(n + 1), v(m + 1);
    vector<int> p(m + 1), way(m + 1);
    for (int i = 1; i <= n; ++i) {
        p[0] = i; int j0 = 0;
        vector<T> best(m + 1);
        vector<char> seen(m + 1), used(m + 1);
        do {
            used[j0] = 1; int i0 = p[j0], j1 = 0; T delta{};
            for (int j = 1; j <= m; ++j) if (!used[j]) {
                T z = a[i0 - 1][j - 1] - u[i0] - v[j];
                if (!seen[j] || z < best[j]) best[j] = z, way[j] = j0, seen[j] = 1;
                if (!j1 || best[j] < delta) delta = best[j], j1 = j;
            }
            for (int j = 0; j <= m; ++j) {
                if (used[j]) u[p[j]] += delta, v[j] -= delta;
                else if (seen[j]) best[j] -= delta;
            }
            j0 = j1;
        } while (p[j0]);
        do { int j1 = way[j0]; p[j0] = p[j1]; j0 = j1; } while (j0);
    }
    vector<int> match(n); T cost{};
    for (int j = 1; j <= m; ++j) if (p[j]) match[p[j] - 1] = j - 1, cost += a[p[j] - 1][j - 1];
    return pair{cost, match};
}
