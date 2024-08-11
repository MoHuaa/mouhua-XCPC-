#include<bits/extc++.h>
using namespace __gnu_cxx;
using namespace __gnu_pbds;
using namespace std;
using ll = long long;
#define LNF 0x3f3f3f3f3f3f3f3f
#define INF 0x3f3f3f3f
#define IOS ios::sync_with_stdio(false);cin.tie(0);cout.tie(0);
#define pll pair<int,int>
#define fi first
#define se second
constexpr int N = 1e5 + 6;
std::vector<int> e[N];
int dfn[N], low[N], idx,cut[N],sz;
void dfs(int u, int f) {
	dfn[u] = low[u] = ++idx;
	int ch = 0;
	for (auto  v : e[u]) {
		if (!dfn[v]){
			dfs(v,u);
			ch++;
			low[u]=min(low[u],low[v]);
			if(low[v]>=dfn[u])cut[u]=1;
		}else if(v!=f){
			low[u]=min(low[u],dfn[v]);
		}
	}
	if(u==1 and ch<=1)cut[u]=0;
	sz+=cut[u];
}
void solve(void) {
	ll n, m;
	cin>>n>>m;
	for (int i = 0; i < m; i++) {
		int u, v;
		cin >> u >> v;
		e[u].push_back(v);
		e[v].push_back(u);
	}
	for(int i=1;i<=n;i++)if(!dfn[i])
	dfs(i, -1);
	cout<<sz<<"\n";
	for(int i=1;i<=n;i++)if(cut[i])cout<<i<<"\n";
}
int main() {

	IOS;
	int t = 1;
	// cin>>t;
	while (t--)
		solve();
	return 0;
}