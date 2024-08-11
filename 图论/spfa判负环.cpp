vector<pair<int, double> > e[N];
int f[N], cnt[N];
double dist[N];

bool spfa()
{
	queue<int> que;
	for(int i=1;i<=n;i++) que.push(i), f[i] = 1, dist[i] = 1, cnt[i] = 0;
	
	while(que.size())
	{
		int x = que.front(); que.pop();
		f[x] = 0;
		
		for(auto it : e[x])
		{
			int tx = it.fi; double bi = it.se;
			if(dist[x] + + bi > dist[tx])
			{
				dist[tx] = dist[x] + bi;
				cnt[tx] = cnt[x] + 1;
				if(cnt[tx] >= n) return 0;
				if(!f[tx]) f[tx] = 1, que.push(tx);
			}
		}
	}
	return 1;
}