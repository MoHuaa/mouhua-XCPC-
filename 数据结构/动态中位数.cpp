struct DynamicMedian {
    std::priority_queue<ll> down;
    std::priority_queue<ll, vector<ll>, greater<ll>> up;
    DynamicMedian() {}
    void insert(ll x) {
        if (down.empty() || x <= down.top()) {
            down.push(x);
        } else {
            up.push(x);
        }
        if (down.size() > 1 + up.size()) {
            up.push(down.top());
            down.pop();
        }
        if (up.size() > down.size()) {
            down.push(up.top());
            up.pop();
        }
    };
    double Ans() {
        if (up.size() + down.size() & 1) {
            return down.top();
        } else {
            return (down.top() + up.top()) / 2.0;
        }
    };
};
