//
// Created by 墨华 on 2024/4/8.
//
constexpr int N=2e5+7;
int l[N],r[N],id[N],sz[N],hs[N],tot;
int dep1[N];
ll dep2[N];
int cnt[N];
int maxncnt[N];
ll sumcnt,ans[N];
void dfs_init(int u,int f){
    l[u]=++tot;
    id[tot]=u;
    sz[u]=1;
    hs[u]=-1;
    for(auto [v,w]:e[u])if(v!=f){
        dep1[v]=dep1[u]+1;
        dep2[v]=dep2[u]+w;
        dfs_init(v,u);
        sz[u]+=sz[v];
        if(hs[u]==-1 or sz[v]>sz[hs[u]])hs[u]=v;
    }
    r[u]=tot;
}
void dfs_solve(int u,int f,bool keep){
    for(auto v:e[u]){
        if(v!=f and v!=hs[u]){
            dfs_solve(v,u, false);
        }
    }
    if(hs[u]!=-1){
        dfs_solve(hs[u],u,true);
    }
    auto add=[&](int x){
        x=cnt[x];
        cnt[x]++;
        if(cnt[x]>maxcnt)maxcnt=cnt[x],sumcnt=0;
        if(cnt[x]==maxcnt)sumcnt+=x;
    };
    auto del=[&](int x){
        x=c[x];
        cnt[x]--;
    };
    for(auto v:e[u]){
        if(v!=f and v!=hs[u]){
            for(int x=l[v];x<=r[v];x++){
                add(id[x]);
            }
        }
    }
    add(u);
    ans[u]=sumcnt;
    if(!keep){

    }
}