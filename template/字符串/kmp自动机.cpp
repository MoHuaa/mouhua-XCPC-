auto autokmp(string s) {
    ll m=s.size();
    std::vector<array<int, 26>> nxt(m + 1);
    vector<int>fail(m + 1);
    for (int i = 1; i <= m; i++) {
        int x = s[i] - 'a';
        for (int j = 0; j < 26; j++) {
            if (x == j) {
                fail[i] = nxt[fail[i - 1]][x];
                nxt[i - 1][x] = i;
            } else {
                nxt[i - 1][j] = nxt[fail[i - 1]][j];
            }
        }
    }
    return nxt;
}