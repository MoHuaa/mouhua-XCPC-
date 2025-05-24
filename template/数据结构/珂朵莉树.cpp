struct ODT {
    struct odt {
        int l, r;
        mutable int x;
        bool operator < (const odt &a) const {
            return l < a.l;
        }
    };
    set<odt> tr;
    typedef set <odt> :: iterator IT;
    ODT(int l, int r, int x) {
        tr.insert({l, r, x});
    }
    IT split(int pos) { //将pos-1和pos之间切开，返回pos所在区间指针
        auto it = tr.lower_bound({pos, 0, 0});
        if (it != tr.end() && it->l == pos) return it;
        it--;
        int l = it->l, r = it->r, x = it->x;
        tr.erase(it);
        tr.insert({l, pos - 1, x});
        return tr.insert({pos, r, x}).first;
    }
    void assign(int l, int r, int x) {
        auto R = split(r + 1);
        auto L = split(l);
        tr.erase(L, R);
        tr.insert({l, r, x});
    }
    void modify(int l, int r) {
        auto R = split(r + 1);
        auto L = split(l);
        for (auto it = L; it != R; it++) {
            // 对it->x暴力修改
        }
    }
    int query() {
        int ans = 0;
        for (auto it = tr.begin(); it != tr.end(); it++) {

        }
        return ans;
    }
};