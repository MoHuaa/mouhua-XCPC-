struct Chairman_Tree {
    struct Node {int L, R, val;} tree[maxn * 500];
    void init() {
        memset(root, 0, sizeof root);
        cnt = 0;
    }
    /* 建T0空树 */
    int build(int l, int r) {
        int k = cnt++;
        tree[k].val = 0;
        if (l == r) return k;
        int mid = l + r >> 1;
        tree[k].L = build(l, mid); tree[k].R = build(mid + 1, r);
        return k;
    }
    /* 上一个版本节点P，【ppos】+=del 返回新版本节点*/
    int update (int P, int l, int r, int ppos, int del) {
        int k = cnt++;
        tree[k].val = tree[P].val + del;
        if (l == r) return k;
        int mid = l + r >> 1;
        if (ppos <= mid) {
            tree[k].L = update(tree[P].L, l, mid, ppos, del);
            tree[k].R = tree[P].R;
        } else {
            tree[k].L = tree[P].L;
            tree[k].R = update(tree[P].R, mid + 1, r, ppos, del);
        }
        return k;
    }
    int query_kth(int lt, int rt, int l, int r, int k) {
        if (l == r) return a[rk[l]];
        int mid = l + r >> 1;
        if (tree[tree[rt].L].val - tree[tree[lt].L].val >= k) return query_kth(tree[lt].L, tree[rt].L, l, mid, k);
        else return query_kth(tree[lt].R, tree[rt].R, mid + 1, r, k + tree[tree[lt].L].val - tree[tree[rt].L].val);
    }
} tree;