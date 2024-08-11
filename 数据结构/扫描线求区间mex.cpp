int sea
int main(){
    int q;
    cin>>q;
    vector<vector<int>>qu(n+1,vector<int>());
    for(int i=1;i<=q;i++){
        int l,r;
        cin>>l>>r;
        qu[r].push_back({l,i});
    }
    for(int r=1;r<=n;r++){
        change(1,0,n+1,a[r],r);
        for(auto que:qu[r]){
            ans[que.second]=search(1,0,n+1,que.first);
        }
    }
}
