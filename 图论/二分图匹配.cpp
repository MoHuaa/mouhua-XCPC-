vector<int>edge[maxn];
int n,m,n1,n2,v[maxn];
bool b[maxn];
bool find(int x){
    b[x]=true;
    for(auto y:edge[x]){
        if(!v[y]||(!b[v[y]])&&find[v[y]]){
            v[y]=x;
            return true;
        }
    }
    return false;
}
int match(){
    int ans=0;
    memset(v,0,sizeof v);
    for(int i=1;i<=n1;i++){
        memset(b,0,sizeof b);
        if(find(i))++ans;
    }
    return ans;
}