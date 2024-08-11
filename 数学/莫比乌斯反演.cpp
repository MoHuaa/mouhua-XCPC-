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
const int maxn = 1e5 + 7;
bitset<maxn>p;
int pr[maxn];
int tot = 0;
ll phi[maxn];
int mu[maxn];
void  get(ll n) {
    p[1] = phi[1] = mu[1] = 1;
    for (int i = 2; i < n; ++i) {
        if (!p[i]) mu[i] = -1, pr[++tot] = i, phi[i] = i - 1;
        for (int j = 1; j <= tot && i * pr[j] < n; ++j) {
            p[i * pr[j]] = 1;
            if (i % pr[j]) {
                mu[i * pr[j]] = -mu[i];
                phi[i * pr[j]] = phi[i] * phi[pr[j]];
            } else {
                phi[pr[j]*i] = phi[i] * pr[j];
                break;
            }
        }
    }
}
void solve(void){
}
int main() {
    
    IOS;

    int t=1;
    cin>>t;
    while(t--)
    solve();
    return 0;
}