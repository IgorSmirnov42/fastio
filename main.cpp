#include "fastio.h"
#include <bits/stdc++.h>

using namespace std;

int main() {
    assert(freopen("../test.txt", "r", stdin));
    {
        int n = read();
        assert(n == 1);
    }
    {
        auto n = read<long long>();
        assert(n == 42);
    }
    {
        auto n = read<__int128_t>();
        assert(n == 128);
    }
    {
        auto s = read<string>();
        assert(s == "aabs");
    }
    {
        char c = read<char>();
        assert(c == 'a');
    }
    {
        int n, k;
        read(n, k);
        assert(n == 239 && k == 179);
    }
    {
        int n;
        string s;
        read(n, s);
        assert(n == 11 && s == "aab");
    }
    {
        int n;
        char c;
        double x;
        string t;
        __int128_t d;
        read(n, c,  x,t, d);
        assert(n == 1 && c == '0' && t == "999" && fabs(x - 3.333) < 1e-4 && d == 7);
    }
    {
        auto vec = readVector<int>(5);
        assert(vec == std::vector<int>({1, 2, 3, 4, 5}));
    }
    {
        std::vector<int> vec(5);
        readVector(vec);
        assert(vec == std::vector<int>({5, 4, 3, 2, 1}));
    }
}
