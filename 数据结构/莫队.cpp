#include<bits/stdc++.h>
using namespace std;
typedef long long ll;
const int maxn=2e5+7;
ll tmp=0;
int main(){
    int n,m;
    cin>>n>>m;
    vector<int>v(n+1);
    for(int i=1;i<=n;i++)cin>>v[i];
    vector<ll>ans(m);
    std::vector<std::array<int, 3>> que(m);
    for(int i=0;i<m;i++){
        int l,r;
        cin>>l>>r;
        que[i]={l,r,i};
    }
    const int B=400;
     vector<int>cnt(maxn,0);
    std::sort(que.begin(),que.end(),[&](array<int,3>a, array<int,3>b) {
        if (a[0] / B != b[0] / B) {
            return a[0]/B < b[0]/B;
        } else {
            return a[1] < b[1];
        }
    });
    
    ll res=0;
    auto add = [&](int x) {
        x=v[x];
        res += 1LL * cnt[x] * (cnt[x] - 1) / 2;
        cnt[x] += 1;
    };
    auto del = [&](int x) {
        x=v[x];
        cnt[x] -= 1;
        res -= 1LL * cnt[x] * (cnt[x] - 1) / 2;
    };
    int l = 1, r = 0;
    for(int i=0;i<m;i++){
        while(r<que[i][1])r++,add(r);
        while(l>que[i][0])l--,add(l);
        while(r>que[i][1])del(r),r--;
        while(l<que[i][0])del(l),l++;
        ans[que[i][2]]=res;
    }
    for(int i=0;i<m;i++)cout<<ans[i]<<"\n";
}