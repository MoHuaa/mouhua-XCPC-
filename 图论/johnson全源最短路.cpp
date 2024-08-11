#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const int inf=0x3f3f3f3f;
const ll Inf=0x3f3f3f3f3f3f3f3f;
ll n,m;
vector<pair<ll,ll>>v[3009];
vector<pair<ll,ll>>v2[3009];
ll dis0[3009];
ll vis0[3009];
//ll vis2[3009][3009];
ll vis2[3009];
ll cnt[3009];
ll dis[3009][3009];
struct node{
    ll order;
    ll d;
    bool operator <(const node& b)const{
        return d>b.d;
    }
};
ll spfa(){
    int i,j;
    for(i=1;i<=n;i++){
        v[0].emplace_back(i,0);
    }
    dis0[0]=0;
    queue<ll>q;
    vis0[0]=1;
    q.push(0);
    while(!q.empty()){
        ll cur=q.front();
        q.pop();
        vis0[cur]=0;
        cnt[cur]++;
        if(cnt[cur]>n){  //n+1个点，这里是n
            return -1;
        }
        for(i=0;i<(int)v[cur].size();i++){
            if(dis0[v[cur][i].first]>dis0[cur]+v[cur][i].second){
                dis0[v[cur][i].first]=dis0[cur]+v[cur][i].second;
                if(vis0[v[cur][i].first]==0){
                    q.push(v[cur][i].first);
                    vis0[v[cur][i].first]=1;
                }
            }
        }
    }
    return 1;
}
void dijkstra(ll s){  //这里不再处理新增的那个点
    memset(vis2,0,sizeof(vis2));
    priority_queue<node>q2;
    q2.push(node{s,0});
    dis[s][s]=0;
    ll i;
    while(!q2.empty()){
        node cur=q2.top();
        q2.pop();
        //if(vis2[s][cur.order]==1)continue;
        //vis2[s][cur.order]=1;
        if(vis2[cur.order]==1)continue;
        vis2[cur.order]=1;
        for(i=0;i<(ll)v2[cur.order].size();i++){
            if(dis[s][v2[cur.order][i].first]>dis[s][cur.order]+v2[cur.order][i].second){
                dis[s][v2[cur.order][i].first]=dis[s][cur.order]+v2[cur.order][i].second;
                q2.push(node{v2[cur.order][i].first,dis[s][cur.order]+v2[cur.order][i].second});
            }
        }
    }
    return ;
}
int main (){
    ios::sync_with_stdio(false);
    cin.tie(0);
    cin>>n>>m;
    ll i,j;
    for(i=1;i<=m;i++){
        ll x,y,w;
        cin>>x>>y>>w;
        if(x==y)continue;
        v[x].emplace_back(y,w);
    }
    memset(dis0,0x3f,sizeof(dis0));
    if(spfa()==-1){
        cout<<-1<<endl;
        return 0;
    }
    for(i=1;i<=n;i++){
        for(j=0;j<(int)v[i].size();j++){
            v2[i].emplace_back(v[i][j].first,v[i][j].second+dis0[i]-dis0[v[i][j].first]);
        }
    }
    memset(dis,0x3f,sizeof(dis));
    for(i=1;i<=n;i++){
        dijkstra(i);
    }
    for(i=1;i<=n;i++){
        ll res=0;
        for(j=1;j<=n;j++){
            if(i==j)continue;
            if(dis[i][j]==Inf)res+=j*1e9;
            else res+=j*(dis[i][j]-dis0[i]+dis0[j]);  //记得再处理回来
        }
        cout<<res<<"\n";
    }
    return 0;
}