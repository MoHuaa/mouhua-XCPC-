int v[maxn][maxn];
int a[maxn][maxn];
void floyd{
memset(v,INF,sizeof v)
for(int k=1;k<=n;k++){
    for(int i=1;i<=n;++i){
        for(int j=1;j<=n;j++){
            if(v[i][k]<1<<30&&v[k][j]<1<<30){
                v[i][j]=min(v[i][j],v[i][k]+v[k][j]);
            }
        }
    }
}
}
