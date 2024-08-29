namespace IO {
#define getchar()(p1==p2&&(p2=(p1=buf)+fread(buf,1,1<<21,stdin),p1==p2)?EOF:*p1++)
#define putchar(x)(*(p2=buf)+fwrite(buf,1,p2-buf,stdout))
    char buf[1 << 21], *p1 = buf, *p2 = buf;

    struct FastInput {
        template<class T>
        FastInput& operator>>(T &re) {
            re = 0;
            char ch = getchar();
            while (ch > '9' || ch < '0') ch = getchar();
            while (ch >= '0' && ch <= '9') re = 10 * re + ch - '0', ch = getchar();
            return *this;
        }
    };

    struct FastOutput {
        template<class T>
        FastOutput& operator<<(T x) {
            if (x == 0) {
                putchar('0');
                return *this;
            }
            static int st[60], top;
            top = 0;
            while (x) st[++top] = x % 10, x /= 10;
            for (int i = top; i >= 1; i--) putchar('0' + st[i]);
            return *this;
        }

        FastOutput& operator<<(char div) {
            putchar(div);
            return *this;
        }
    };

    FastInput cin;
    FastOutput cout;
}

using namespace IO;
#define cin IO::cin
#define cout IO::cout