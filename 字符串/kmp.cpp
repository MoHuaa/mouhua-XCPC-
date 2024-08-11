#include<bits/stdc++.h>
using namespace std;
typedef long long ll;
int n,m;
const int N=1e6;
const int M=1e6;
int nxt[M+1],f[N+1];
char s[N+2],p[M+2];
void kmp(){
    n=strlen(s+1);
    m=strlen(p+1);
    int j=0;
    nxt[1]=0;
    for(int i=2;i<=m;i++){
        while(j&&p[j+1]!=p[i]){
            j=nxt[j];
        }
        if(p[j+1]==p[i])j++;
        nxt[i]=j;
    }

    j=0;
    for(int i=1;i<=n;i++){
        while((j==m)||(j>0&&p[j+1]!=s[i]))j=nxt[j];
        if(p[j+1]==s[i])j++;
        f[i]=j;
    }
}
int main(){
    scanf("%s",s+1);
    scanf("%s",p+1);
    kmp();
    for(int i=1;i<=n;i++){
        if(f[i]==m)cout<<i<<endl;
    }

    return 0;
}