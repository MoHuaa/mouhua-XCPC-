template<class T>
struct Block {
    int n;
    int B;
    std::vector<T> a;
    std::vector<T> add;
    Block(int n_) : n{n_} {
        B = std::sqrt(n);
        a.resize(n);
        add.resize((n + B - 1) / B);
    }
    void rangeAdd(int l, int r, int v) {
        r++;
        if (l >= r) {
            return;
        }
        int lb = l / B;
        int rb = (r - 1) / B;
        if (lb == rb) {
            for (int i = l; i < r; i++) {
                a[i] += v;
            }
        } else {
            for (int i = l; i < (lb + 1) * B; i++) {
                a[i] += v;
            }
            for (int i = lb + 1; i < rb; i++) {
                add[i] += v;
            }
            for (int i = rb * B; i < r; i++) {
                a[i] += v;
            }
        }
    }
    T query(int x) {
        return a[x] + add[x / B];
    }
};