namespace IO {
#define getchar()(p1==p2&&(p2=(p1=buf)+fread(buf,1,1<<21,stdin),p1==p2)?EOF:*p1++)
char buf[1 << 21], *p1 = buf, *p2 = buf;
template<class T>
void read(T &re) {
	re = 0; char ch = getchar();
	while (ch > '9' || ch < '0') ch = getchar();
	while (ch >= '0' && ch <= '9') re = 10 * re + ch - '0', ch = getchar();
}
template<class T>
void write(T x, char div = '\n') {
	if (x == 0) {putchar('0'); putchar(div); return;}
	static int st[60], top; top = 0;
	while (x) st[++top] = x % 10, x /= 10;
	for (int i = top; i >= 1; i--) putchar('0' + st[i]);
	putchar(div);
}
} using namespace IO;