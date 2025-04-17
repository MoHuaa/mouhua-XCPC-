auto djikstra = [&](int s = 1) {
        vector<ll> dis(n + 1, 1E18);
        std::priority_queue<array<ll, 2>, vector<array<ll, 2>>, greater<array<ll, 2>>> Q;
        Q.push({0, s});
        dis[s] = 0;
        while (!Q.empty()) {
            auto [d, u] = Q.top();
            Q.pop();
            if (dis[u] != d)continue;
            for (auto [y, w] : E[u]) {
                if (dis[y] > dis[u] + w) {
                    dis[y] = dis[u] + w;
                    Q.push({dis[y], y});
                }
            }
        }
    };