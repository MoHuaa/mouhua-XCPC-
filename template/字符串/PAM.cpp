template<int S = 26, int BASE = 'a'>
struct PAM {
    struct Node {
        int len = 0, link = 0;
        array<int, S> next{};
    };
    vector<Node> t;
    string s;
    int last = 0;

    PAM(string_view text = "") {
        init();
        for (unsigned char c : text) add(c);
    }

    void init() {
        t.assign(2, {});
        t[0].link = 1;
        t[1].len = -1;
        s.clear();
        last = 0;
    }

    int size() const { return int(t.size()); }
    int length() const { return int(s.size()); }

    // i is a 0-based text position; find a suffix that can be surrounded by s[i].
    int getLink(int u, int i) const {
        while (i <= t[u].len || s[i - t[u].len - 1] != s[i])
            u = t[u].link;
        return u;
    }

    // Returns the longest palindromic suffix after appending ch.
    int add(unsigned char ch) {
        int c = int(ch) - BASE;
        s.push_back(char(ch));
        int i = length() - 1, u = getLink(last, i);

        if (!t[u].next[c]) {
            int v = size();
            t.emplace_back();
            t[v].len = t[u].len + 2;
            // Compute link BEFORE installing u --c--> v (length-one case).
            t[v].link = t[getLink(t[u].link, i)].next[c];
            t[u].next[c] = v;
        }
        return last = t[u].next[c];
    }

    // ===== Optional: distinct palindromes / suffix counts =====
    int distinct() const { return size() - 2; }

    vector<int> suffixCount() const {
        vector<int> a(size());
        for (int u = 2; u < size(); ++u)
            a[u] = a[t[u].link] + 1;
        return a;
    }

    // ===== Optional: aggregate endpoint weights upwards =====
    // T{} is identity; + is associative and commutative.
    template<class T>
    void pull(vector<T>& a) const {
        for (int u = size() - 1; u >= 2; --u)
            a[t[u].link] = a[t[u].link] + a[u];
    }

    // Occurrences in the CURRENT stored text. Does not mutate the tree.
    // Root entries are not empty-palindrome occurrence counts.
    vector<long long> count() const {
        vector<long long> a(size());
        int u = 0;
        for (int i = 0; i < length(); ++i) {
            u = t[getLink(u, i)].next[int((unsigned char)s[i]) - BASE];
            ++a[u];
        }
        pull(a);
        return a;
    }

    // ===== Optional: start another independent text, keep all nodes =====
    // Existing states and links are unchanged. count() then counts only this text.
    void newString() {
        s.clear();
        last = 0;
    }
};