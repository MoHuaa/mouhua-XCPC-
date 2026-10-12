std::vector<char> bipartiteGame(
    int n,
    const std::vector<std::pair<int, int>>& edges,
    const std::vector<int>& color
) {
    int S = 0;
    int T = n + 1;
    Flow<int> flow(n + 2);

    // 1. 建立源点、汇点与两侧顶点之间的边。
    for (int u = 1; u <= n; ++u) {
        if (color[u] == 0) {
            flow.addEdge(S, u, 1);
        } else {
            flow.addEdge(u, T, 1);
        }
    }

    // 2. 原图边统一按 左 -> 右 加入。
    for (auto [u, v] : edges) {
        if (color[u] == 1) {
            std::swap(u, v);
        }
        flow.addEdge(u, v, 1);
    }

    // 3. 求最大匹配。
    flow.maxFlow(S, T);

    // 此时 flow.h[u] != -1 表示 S 在残量网络中能到达 u。

    // 4. 从 T 反向搜索，找出残量网络中能到达 T 的点。
    std::vector<char> canReachT(n + 2, false);
    std::queue<int> q;

    canReachT[T] = true;
    q.push(T);

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (int id : flow.g[u]) {
            int v = flow.e[id].to;

            // 沿残量边的反方向搜索：
            // 只有 v -> u 具有正剩余容量，才能从 u 搜到 v。
            if (flow.e[id ^ 1].cap > 0 && !canReachT[v]) {
                canReachT[v] = true;
                q.push(v);
            }
        }
    }

    // 5. 判断所有原图顶点作为起点时的胜负。
    std::vector<char> win(n + 1, false);

    for (int u = 1; u <= n; ++u) {
        if (color[u] == 0) {
            // 左侧：S 可达则必败，不可达则必胜。
            win[u] = (flow.h[u] == -1);
        } else {
            // 右侧：能到 T 则必败，不能到 T 则必胜。
            win[u] = !canReachT[u];
        }
    }

    return win;
}