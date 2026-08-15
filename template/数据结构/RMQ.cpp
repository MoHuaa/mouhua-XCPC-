#include<bits/extc++.h>
using namespace __gnu_pbds;
using namespace std;
using ll = long long;
using ull = unsigned long long;
using u32 = unsigned;
using u128 = unsigned __int128;
using i128 = __int128;
#define LNF 0x3f3f3f3f3f3f3f3f
#define W(...) println("{} = {}", #__VA_ARGS__, make_tuple(__VA_ARGS__))
template <class T> using Tree = tree<T, null_type, less<T>, rb_tree_tag, tree_order_statistics_node_update>;
void chmax(auto & a, const auto &... b) {((a = (b > a ? b : a)), ...);}
void chmin(auto & a, const auto &... b) {((a = (b < a ? b : a)), ...);}
#define pb push_back
void solve(void) {
    std::vector<vector<int>> matrix{{1}};
    int target=1;
     int n=matrix.size();
        int m=matrix[0].size();
        int l=-1,r=m*n;
        auto getid=[&](int id){
            int x=id/m;
            int y=id%m;
            return array<int,2>{x,y};
        };
        auto check=[&](int id){
            auto [x,y]=getid(id);
            return matrix[x][y]>=target;
        };
        while(l+1<r){
            int mid=(l+r)>>1;
            if(check(mid))r=mid;
            else l=mid;
        }
        auto [x,y]=getid(r);
        W(r,x,y);
        
}
int main() {
  ios::sync_with_stdio(false); cin.tie(nullptr);
  int t = 1;
  // cin >> t;
  while (t--)
    solve();
  return 0;
}