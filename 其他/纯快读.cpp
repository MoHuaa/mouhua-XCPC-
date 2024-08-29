namespace IO {
#define getchar()(p1==p2&&(p2=(p1=buf)+fread(buf,1,1<<21,stdin),p1==p2)?EOF:*p1++)
    char buf[1 << 21], *p1 = buf, *p2 = buf;
    struct ifast {
        template<class T>
        ifast& operator>>(T &re) {
            re = 0;
            char ch = getchar();
            while (!isdigit(ch)) ch = getchar();
            while (isdigit(ch)) re = re*10 + ch - '0', ch = getchar();
            return *this;
        }
    };
    ifast cin;
}
using namespace IO;
#define cin IO::cin