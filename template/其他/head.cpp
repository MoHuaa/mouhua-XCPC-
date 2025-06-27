#include<bits/extc++.h>
using namespace __gnu_pbds;
using namespace std;
using ll = long long;
using ull=unsigned long long;
using u32 = unsigned;
using u128 = unsigned __int128;
using i128 = __int128;
#define LNF 0x3f3f3f3f3f3f3f3f
#define W(...) debug(string(#__VA_ARGS__)+",", __VA_ARGS__)
void debug(string names, auto... value) {int now = 0, j; ((j = names.find(',', now), (cerr << names.substr(now, j - now) << "=" << value << ","), (now = j + 1), value), ...); cerr << endl;}
template <class T> using Tree = tree<T, null_type, less<T>, rb_tree_tag, tree_order_statistics_node_update>;
void chmax(auto & a, const auto &... b) {((a = (b > a ? b : a)), ...);}
void chmin(auto & a, const auto &... b) {((a = (b < a ? b : a)), ...);}
#define pb push_back
void solve(void) {
    
}
int main() {
    ios::sync_with_stdio(false); cin.tie(nullptr);
    int t = 1;
    // cin >> t;
    while (t--)
        solve();
    return 0;
}