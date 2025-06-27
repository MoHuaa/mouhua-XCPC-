SuffixArray sa(s);

RMQ rmq(sa.lc);

auto lcp = [&](int i, int j) {
    if (i == j) {
        return n - i;
    }
    i = sa.rk[i];
    j = sa.rk[j];
    if (i > j) {
        std::swap(i, j);
    }
    return rmq(i, j);
};