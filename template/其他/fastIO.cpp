struct FastInput {
    static constexpr int S = 1 << 20;
    static constexpr int P2 = 100, P4 = P2 * P2, P8 = P4 * P4;
    unsigned char buf[S + 8]{}, *p = buf, *end = buf;
    signed char tab[1 << 16];
    bool ok = true;
    FastInput() {
        std::memset(tab, -1, sizeof tab);
        for (int a = 0; a < 10; ++a)
            for (int b = 0; b < 10; ++b)
                tab[(a + '0') | ((b + '0') << 8)] = a * 10 + b;
    }
    int two(const unsigned char *s) {
        return tab[s[0] | (s[1] << 8)];
    }
    bool refill(unsigned char *&s) {
        end = buf + std::fread(buf, 1, S, stdin);
        std::memset(end, 0, 8);
        s = buf;
        return s != end;
    }
    explicit operator bool() const { return ok; }
    template<class T> [[gnu::always_inline]]
    FastInput& operator>>(T &out) {
        if (!ok) return *this;
        auto s = p;
        while (*s <= ' ') {
            if (s != end) ++s;
            else if (!refill(s)) { p = s; ok = false; return *this; }
        }
        bool neg = *s == '-';
        s += neg || *s == '+';
        T x = 0;
        for (;;) {
            int a = two(s), b = two(s + 2);
            if ((a | b) >= 0) {
                int c = two(s + 4), d = two(s + 6);
                if ((c | d) >= 0) {
                    x = x * P8 - ((a * P2 + b) * P4 + c * P2 + d);
                    s += 8; continue;
                }
                x = x * P4 - (a * P2 + b); s += 4; a = c;
            }
            if (a >= 0) x = x * P2 - a, s += 2;
            if (*s >= '0') x = x * 10 - (*s++ - '0');
            if (s != end || !refill(s)) break;
        }
        p = s + (s != end);
        out = neg ? x : -x;
        return *this;
    }
} in;