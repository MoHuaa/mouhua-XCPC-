#include<bits/extc++.h>
using namespace __gnu_pbds;
using namespace std;
using ll=long long;
template <class T> constexpr auto NL(T) -> T {return std::numeric_limits<T>::max();}
template <class T> using Tree = tree<T, null_type, less<T>,rb_tree_tag, tree_order_statistics_node_update>;
#define pb push_back
constexpr int N=1e5+7;
array<int,5>a[N],tmp[N];
int n,ans[N];
template<typename T>
struct BIT
{
    int size;
    std::vector<T> c;
    BIT(int size = 0) {
        init(size);
    }
    void init(int size) {
        this->size = size;
        c.assign(size, T());
    }
    T lowbit(T x) {
        return x & (-x);
    }
    T ask(int n) {
        T sum = 0;
        for(int i = n;i;i -= i&-i){
            sum +=c[i];
        }
        return sum;
    }
    void add(int pos, T x) {
        for(int i = pos;i <= size;i += i&-i) {
            c[i]+=x;
        }
    }
    void modify(int l, int r, T x) {
        if(l>r)swap(l,r);
        add(l, x);
        add(r + 1, -x);
    }
    T getsum(T l,T r){
        if(l>r)swap(l,r);
        return ask(r)-ask(l-1);
    }
};
BIT<int>c(N);
void Solve(int l,int r){
    if(l==r)return ;
    int mid=(l+r)>>1;
    Solve(l,mid);
    Solve(mid+1,r);
    //solve 把所有的点按照（y,z)归并
    int p1=l,p2=mid+1;
    int p3=0;
    while(p1<=mid or p2<=r){
        if(p2>r || (p1<=mid and make_pair(a[p1][1],a[p1][2])<=make_pair(a[p2][1],a[p2][2]))){
            c.add(a[p1][2],a[p1][3]);
            tmp[p3++]=a[p1++];
        }else{
            a[p2][4]+=c.ask(a[p2][2]);
            tmp[p3++]=a[p2++];
        }
    }
    for(int i=l;i<=mid;i++)c.add(a[i][2],-a[i][3]);
    for(int i=0;i<p3;i++)a[l+i]=tmp[i];
}
void solve(void){
    ll n,k;
    cin>>n>>k;
    for(int i=0;i<n;i++){
        cin>>a[i][0]>>a[i][1]>>a[i][2];
        a[i][3]=1;
    }
    sort(a,a+n);
    int t=0;
    for(int i=0;i<n;i++){
        if(t!=0 and (a[i][0]==a[t-1][0] and a[i][1]==a[t-1][1] and a[i][2]==a[t-1][2])){
            a[t-1][3]+=1;
        }else{
            a[t++]==a[i];
        }
    }
    Solve(0,n-1);
    for(int i=0;i<t;i++){
        ans[a[i][3]+a[i][4]-1]+=a[i][3];
    }

}
int main() {
    ios::sync_with_stdio(false);cin.tie(nullptr);cout.tie(nullptr);
    int t=1;
    // cin>>t;
    while(t--)
        solve();
    return 0;
}