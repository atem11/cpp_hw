#include <iostream>
#include <string>
#include <iostream>
#include <utility>
#include <tuple>
#include "bind.h"
using std::string;
using std::cin;
using std::cout;

void f(string a, string b, string c, string& d) {
    cout << a << " " << b << " " << c << " " << d << "\n";
    //d = "AZAZAA";
}

void ff(int a, int b, int c, int d) {
    cout << a << " " << b << " " << c << " " << d << "\n";
    //d = "AZAZAA";
}

string concat(string x, string y) {
    return x + y;
}

int add(int x, int y) {
    return x + y;
}

struct  nonc
{
    nonc(const nonc&) = delete;
    nonc(nonc&&) = default;
    nonc() {}
};

int main()
{
    string s = "lol";
    string a = "kek";
    string b = "wow";
    auto w = bind(f, "YOU", placeholder<2>(), bind(concat, string("TROLL+"), _3), _3);
    w(std::move(a), std::move(b), s);
    w("a", "b", std::move(s));
    cout << a << "!" << b << "\n";

    auto ww = bind([](nonc const&, nonc const&) {}, nonc{}, _1);
    ww(nonc{});
//    auto w = bind(add, _1, _2);
//    auto ww = bind(add, w, _1);
    return 0;
}