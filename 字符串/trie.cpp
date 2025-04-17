constexpr int N = 1e6 + 7;
int tre[N][26];
int tot = 0;
int newNode() {
    tot++;
    fill(tre[tot], tre[tot] + 26, 0);
    return tot;
}
void insert(string & a) {
	int now = 1;
	for (auto q : a) {
		int &c = tre[now][q - 'a'];
		if (!c)c = newNode();
		now = c;
	}
}
