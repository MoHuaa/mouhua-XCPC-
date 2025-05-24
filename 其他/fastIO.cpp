namespace IO {
#define gc()(p1==p2&&(p2=(p1=buf)+fread(buf,1,1<<21,stdin),p1==p2)?EOF:*p1++)
char buf[1 << 21], *p1 = buf, *p2 = buf;
struct ifast {
    template<class T>
    ifast& operator>>(T &re) {
        re = 0;
        char ch = gc();
        while (!isdigit(ch)) ch = gc();
        while (isdigit(ch)) re = re * 10 + ch - '0', ch = gc();
        return *this;
    }
};
ifast cin;
#define pc(x)(*(p2=buf)+fwrite(buf,1,p2-buf,stdout))
struct ofast {
    template<class T>
    ofast& operator<<(T x) {
        if (x == 0) {
            pc('0');
            return *this;
        }
        static int st[60], top;
        top = 0;
        while (x) st[++top] = x % 10, x /= 10;
        for (int i = top; i >= 1; i--) pc('0' + st[i]);
        return *this;
    }

    ofast& operator<<(char div) {
        pc(div);
        return *this;
    }
};
ofast cout;
}

using namespace IO;
#define cin IO::cin
#define cout IO::cout