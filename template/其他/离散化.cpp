template <typename T> struct Compress {
    int n;
    vector<T> alls;
    Compress() {}
    void add(auto ...x) {
        ((alls.emplace_back(x)), ...);
    }
    void init() {
        add(numeric_limits<T>::min());
        add(numeric_limits<T>::max());
        sort(alls.begin(), alls.end());
        alls.erase(unique(alls.begin(), alls.end()), alls.end());
        this->n = alls.size() - 2;
    }
    int operator[](T x) {
        return upper_bound(alls.begin(), alls.end(), x) - alls.begin() - 1;
    }
    T operator()(int x) {
        return alls[x];
    }
};