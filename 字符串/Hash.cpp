#include<bits/stdc++.h>
using namespace std;
typedef unsigned long long ULL;
using pll=pair<ULL,ULL>
const int maxn = 2e5+7;
const int sigma = maxn;
const int HASH_CNT = 2;
unsigned long long Prime_Pool[] = {1998585857ul,23333333333ul};
unsigned long long Seed_Pool[]={911,146527,19260817,91815541};
unsigned long long Mod_Pool[]={29123,998244353,1000000009,4294967291ull};
struct Hash{
    unsigned long long Seed,Mod;
    unsigned long long bas[maxn];
    Hash(ULL Seed, ULL Mod):Seed(Seed),  Mod(Mod){
        bas[0] = 1;
        for (int i = 1; i <=maxn; i++){
            bas[i] = bas[i - 1] * Seed % Mod;
        }
    }
    void init(int n){
        for (int i=1;i<=n;i++){
            sum[i] = (sum[i-1]*Seed%Mod+s[i])%Mod;
        }
    }
    ULL getHash(int l,int r){
        return (sum[r]-sum[l-1]*bas[r-l+1]%Mod+Mod)%Mod;
    }
};
struct HHash{
    Hash hasher[HASH_CNT] = {Hash(Seed_Pool[0], Mod_Pool[0]), Hash(Seed_Pool[1], Mod_Pool[1])};
    void init(int n){
        hasher[0].init(n);
        hasher[1].init(n);
    }
    pair<ULL,ULL>gethash(int l,int r){
        pair<ULL,ULL>now{hasher[0].getHash(l,r),hasher[1].getHash(l,r)};
        return now;
    };
}h;
int main(){
    return 0;
}