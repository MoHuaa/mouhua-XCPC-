template<class T>
struct CaT{
    vector<T>l,r;
    CaT() {}
    CaT(std::vector<T>&v,int n) {
        l.assign(n+1,-1);
        r.assign(n+1,-1);
        build(v,n);
    }
    void build(std::vector<T>&v,int n) {
        vector<int> st;
        int root = 0;
        for (int i = 1; i <= n; i++) {
            int last = -1;
            while (!st.empty() && v[st.back()] > v[i]) {
                last = st.back();
                st.pop_back();
            }
            if (!st.empty())r[st.back()] = i;
            else root = i;
            l[i] = last;
            st.push_back(i);
        }
    }
};