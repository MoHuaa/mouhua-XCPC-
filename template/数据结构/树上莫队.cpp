int LOG = 1;
    while ((1LL << LOG) <= n) ++LOG;

    vector<vector<int>> up(LOG, vector<int>(n + 1));
    vector<int> depth(n + 1);
    vector<int> tin(n + 1), tout(n + 1);
    vector<int> euler(2 * n + 1);

    // 1. 显式栈 DFS，生成长度为 2n 的入出序。
    vector<int> stk{1};
    vector<int> nextEdge(n + 1);

    int timer = 0;
    tin[1] = ++timer;
    euler[timer] = 1;

    while (!stk.empty()) {
        int u = stk.back();

        // 所有邻接边均已处理，退出 u。
        if (nextEdge[u] == (int)E[u].size()) {
            tout[u] = ++timer;
            euler[timer] = u;
            stk.pop_back();
            continue;
        }

        int v = E[u][nextEdge[u]++];
        if (v == up[0][u]) continue;

        // 进入孩子 v，同时预处理倍增祖先。
        depth[v] = depth[u] + 1;
        up[0][v] = u;

        for (int j = 1; j < LOG; ++j) {
            up[j][v] = up[j - 1][up[j - 1][v]];
        }

        tin[v] = ++timer;
        euler[timer] = v;
        stk.push_back(v);
    }

    // 2. 倍增求 LCA。
    auto lca = [&](int u, int v) {
        if (depth[u] < depth[v]) swap(u, v);

        int diff = depth[u] - depth[v];
        for (int j = 0; j < LOG; ++j) {
            if ((diff >> j) & 1) u = up[j][u];
        }

        if (u == v) return u;

        for (int j = LOG - 1; j >= 0; --j) {
            if (up[j][u] != up[j][v]) {
                u = up[j][u];
                v = up[j][v];
            }
        }

        return up[0][u];
    };

    struct Query {
        int l, r, id, extra;
    };
    vector<Query> queries;
    queries.reserve(q);
    // 3. 把路径转成区间。
    for (int id = 0; id < q; ++id) {
        auto [u, v,x,y] = paths[id];

        if (tin[u] > tin[v]) swap(u, v);

        int w = lca(u, v);

        if (w == u) {
            queries.push_back({tin[u], tin[v], id, 0,x,y});
        } else {
            queries.push_back({tout[u], tin[v], id, w,x,y});
        }
    }

    // 4. 莫队排序。
    int blockSize = max(1, (int)sqrt(2 * n));
    sort(queries.begin(), queries.end(),
         [&](const Query& a, const Query& b) {
        int ba = (a.l - 1) / blockSize;
        int bb = (b.l - 1) / blockSize;

        if (ba != bb) return ba < bb;
        return (ba & 1) ? a.r > b.r : a.r < b.r;
    });

    // 5. 维护当前有效节点集合的颜色频次。
    vector<int> answer(q);
    vector<char> active(n + 1, false);
    auto toggle = [&](int u) {
        int c=col[u];
        if (active[u]) {
            //删这个点
        } else {
            //加这个点
        }

        active[u] ^= 1;
    };

    // 初始为空区间。
    int L = 1, R = 0;

    for (const auto& query : queries) {
        while (L > query.l) toggle(euler[--L]);
        while (R < query.r) toggle(euler[++R]);
        while (L < query.l) toggle(euler[L++]);
        while (R > query.r) toggle(euler[R--]);

        if (query.extra) toggle(query.extra);

        answer[query.id] = sum.query(query.x,query.y);

        if (query.extra) toggle(query.extra);
    }