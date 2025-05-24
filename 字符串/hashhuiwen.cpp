struct HHash{
    Hash h1,h2;
    int n;
    HHash(string s): h1(s), h2(string(s.rbegin(), s.rend())), n(s.length()){}
    bool check(int i, int j) {
        if (h1.getHash(i, j) == h2.getHash(n - j + 1, n - i + 1)) {
            return true;
        }
        return false;
    }
};