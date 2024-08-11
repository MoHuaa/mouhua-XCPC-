struct SqrtDecomposition {
    const int block_size, n;
    std::vector<int> ls, rs; // 每个块i的段位记为[ls[i], rs[i]), 右侧是开区间
    std::vector<bool> to_be_eval; // 该block的懒标记是否全部被清空

    explicit SqrtDecomposition(const int n_)
            : block_size(std::round(std::sqrt(n_))),
              n((n_ + block_size - 1) / block_size) {
        ls.resize(n); // n是block的数量 这里上取整了
        rs.resize(n);
        to_be_eval.assign(n, false);
        for (int i = 0; i < n; ++i) {
            // 每个块i的段位记为[ls[i], rs[i]), 右侧是开区间
            // 最后一个block里的元素的rs不一定填满了最后一个block，因此最后一个blocki的rs会等于数组长度。
            ls[i] = block_size * i;
            rs[i] = (i + 1 == n ? n_ : block_size * (i + 1));
        }
    }

    template <typename T>
    void partial_update(const int idx, const T val); // 这是我们需要实现的

    template <typename T>
    void total_update(const int idx, const T val); // 这是我们需要实现的

    template <typename T>
    void update(const int l, const int r, const T val) {
        if (r <= l) return;
        // 这里如果想debug好用点可以改成 assert(l < r);
        const int b_l = l / block_size, b_r = (r - 1) / block_size;
        // b_l是左端点在的block，g_r是右端点的block， -1是因为外部调用也是开区间。
        if (b_l < b_r) { // 不在一个区间的话，必定要分开讨论
            if (l == ls[b_l]) { // 如果左端点恰好是区间的开始
                total_update(b_l, val); // 只要更新这个区间就好了
            } else {
                for (int i = l; i < rs[b_l]; ++i) {
                    partial_update(i, val); // 否则我们要对左端点到下一个block之间进行暴力更新
                }
            }
            for (int i = b_l + 1; i < b_r; ++i) {
                total_update(i, val); // 我们对中间的block进行更新
            }
            if (r == rs[b_r]) {
                total_update(b_r, val); // 同理讨论右侧端点冒出来的点，不再赘述
            } else {
                for (int i = ls[b_r]; i < r; ++i) {
                    partial_update(i, val);
                }
            }
        } else {
            for (int i = l; i < r; ++i) {
                partial_update(i, val); // 如果 l r本来就在一个block，只需要直接暴力更新即可。
            }
        }
    }

    template <typename T>
    void partial_query(const int idx, T* val);

    template <typename T>
    void total_query(const int idx, T* val);

    template <typename T>
    T query(const int l, const int r, const T id) {
        const int b_l = l / block_size, b_r = (r - 1) / block_size;
        // b_l是左端点在的block，g_r是右端点的block， -1是因为外部调用也是开区间。
        T res = id;
        // 这里是我们的初始值，实现的时候会具体解释，简单举例，如果是求和这里就是0，然后累加
        // 之后的逻辑和update一模一样，不再赘述
        if (b_l < b_r) {
            if (l == ls[b_l]) {
                total_query(b_l, &res);
            } else {
                for (int i = l; i < rs[b_l]; ++i) {
                    partial_query(i, &res);
                }
            }
            for (int i = b_l + 1; i < b_r; ++i) {
                total_query(i, &res);
            }
            if (r == rs[b_r]) {
                total_query(b_r, &res);
            } else {
                for (int i = ls[b_r]; i < r; ++i) {
                    partial_query(i, &res);
                }
            }
        } else {
            for (int i = l; i < r; ++i) {
                partial_query(i, &res);
            }
        }
        return res;
    }
};