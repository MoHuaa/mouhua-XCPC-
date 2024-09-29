struct DynamicMedian {
    multiset<ll>down,up;
    DynamicMedian() {}
    void change() {
        while (down.size() > 1 + up.size()) {
            up.insert(*down.rbegin());
            down.extract(*down.rbegin());
        }
        while (up.size() > down.size()) {
            down.insert(*up.begin());
            up.extract(*up.begin());
        }
    }
    void insert(ll x) {
        if (down.empty() || x <= *down.rbegin()) {
            down.insert(x);
        } else {
            up.insert(x);
        }
        change();
    };
    void erase(ll x){
        if(down.find(x)!=down.end()){
            down.extract(x);
        }else up.extract(x);
        change();
    }
    ll Ans() {
        return *down.rbegin();
    };
};