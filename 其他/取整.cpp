auto ceils(auto n, auto m) {
    if (m < 0) {
        n *= -1;
        m *= -1;
    }
    if (n <= 0)return n / m;
    else return (n + m - 1) / m;
}
auto floors(auto n, auto m) {
    if (m < 0) {
        n *= -1;
        m *= -1;
    }
    if (n <= 0)return (n - m + 1) / m;
    else return n / m;
}