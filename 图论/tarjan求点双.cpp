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
