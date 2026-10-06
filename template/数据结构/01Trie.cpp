// 无符号多重集：Trie01<> 为 32 位，Trie01<ull> 为 64 位，不压缩数值。
// add/erase/count/kth 均 O(B)；erase 不存在的数返回 false。
// kth(k,x)：第 k 小的 (元素 ^ x)，k 从 1 开始，须 1 <= k <= size()。
// min_xor/max_xor 返回异或结果；要得到选中的元素，再异或 x。
// 空节点保留复用；历史上出现 D 种键，空间 O(D B)，每节点 3 个 int。
template<class T = unsigned>
struct Trie01 {
    static_assert(is_unsigned_v<T> && !is_same_v<T, bool>);
    static constexpr int B = numeric_limits<T>::digits;
    struct Node { int ch[2]{}, cnt = 0; };
    vector<Node> tr;

    Trie01() { init(); }
    void init() { tr.assign(2, {}); }
    int size() const { return tr[1].cnt; }

    int count(T x) const {
        int p = 1;
        for (int i = B - 1; i >= 0 && p; --i)
            p = tr[p].ch[x >> i & 1];
        return tr[p].cnt;
    }

    void add(T x) {
        int p = 1;
        ++tr[p].cnt;
        for (int i = B - 1; i >= 0; --i) {
            int b = x >> i & 1;
            if (!tr[p].ch[b]) {
                int q = tr.size();
                tr.emplace_back();
                tr[p].ch[b] = q;
            }
            p = tr[p].ch[b];
            ++tr[p].cnt;
        }
    }

    bool erase(T x) {
        if (!count(x)) return false;
        int p = 1;
        --tr[p].cnt;
        for (int i = B - 1; i >= 0; --i) {
            p = tr[p].ch[x >> i & 1];
            --tr[p].cnt;
        }
        return true;
    }

    T kth(int k, T x = 0) const {
        assert(1 <= k && k <= size());
        int p = 1;
        T ans = 0;
        for (int i = B - 1; i >= 0; --i) {
            int b = x >> i & 1, c = tr[tr[p].ch[b]].cnt;
            if (k > c) {
                k -= c;
                b ^= 1;
                ans |= T(1) << i;
            }
            p = tr[p].ch[b];
        }
        return ans;
    }

    T min_xor(T x) const { return kth(1, x); }
    T max_xor(T x) const { return kth(size(), x); }
};
