#include<bits/extc++.h>
using namespace __gnu_cxx;
using namespace __gnu_pbds;
using namespace std;
using ll=long long;
#define LNF 0x3f3f3f3f3f3f3f3f
#define INF 0x3f3f3f3f
#define IOS ios::sync_with_stdio(false);cin.tie(0);cout.tie(0);
#define pll pair<int,int>
#define fi first
#define se second
const int N=210;
const int B=60;
struct linear_basis{
    ll num[B+1];
    void init(){
        for(int i=B-1;i>=0;i--)num[i]=0;
    }
    bool insert(ll x){
        for(int i=B-1;i>=0;i--){
            if(x&(1ll<<i)){
                if(num[i]==0){num[i]=x;
                    return true;
                };
                x^=num[i];
            }
        }
        return false;
    }
    ll querymin(ll x){
        for(int i=B-1;i>=0;i--){
            x=min(x,x^num[i]);
        }
        return x;
    }
    ll querymax(ll x){
        for(int i=B-1;i>=0;i--){
            x=max(x,x^num[i]);
        }
        return x;
    }
};
void solve(void){
    
}
int main() {
    
    IOS;
    int t=1;
    // cin>>t;
    while(t--)
    solve();
    return 0;
}