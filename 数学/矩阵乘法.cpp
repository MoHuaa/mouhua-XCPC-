#include<bits/stdc++.h>
using namespace std;
using ll=long long;
#define INF 0x3f3f3f3f
#define IOS ios::sync_with_stdio(false);cin.tie(0);cout.tie(0);
#define pll pair<int,int>
#define fi first
#define se second
const int N=200,P=100000007;
ll a[N+1][N+1],fa[N+1];
void aa(){
	ll w=[N+1][N+1];
	memset(w,0,sizeof(w));
	for(int i=1;i<=n;i++){
		for(int j=1;j<=n;j++){
			for(int k=1;k<=n;k++)
				w[i][j]+=a[i][k]*a[k][j],w[i][j]%=P;

		}
	}
	memcpy(a,w,sizeof(a));
}
void fa(){
	ll w[N+1];
	memset(w,0,sizeof(w));
	for(int i=1;i<=n;i++){
		for(int j=1;j<=n;j++)w[i]+=f[j]*a[j][i],w[i]%=P;
	}
	memcpy(f,w,sizeof(f));

}
void matrixpow(int k){
	for(;k;k/2){
		if(k&1)fa();
		aa();
	}
}
int main(){
    
}