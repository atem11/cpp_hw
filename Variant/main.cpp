#include <iostream>
#include <vector>
#include <string>
#include "variant.h"
using std::vector;
using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::in_place_index_t;
using std::in_place_type_t;
using std::move;

int main() {
    string a = "absss";
    variant<int, double, string> t(a);
    variant<int, double, string> tt(3.22);
    variant<long, int> testttt(100l);

    cout << (t == tt) << endl;

    cout << get<0>(testttt) << endl;

    cout << holds_alternative<long>(testttt) << endl;
    cout << holds_alternative<int>(testttt) << endl;

    cout << (t < tt) << endl;

    visit([](auto&& a, auto&& b, auto c) { cout << a << " " << b << " " << c << endl; }, t, tt, testttt);

    auto w = visit([](auto&& arg)->variant<int, double, string> { return arg + arg; }, t);
    auto ww = visit([](auto&& arg)->variant<int, double, string> { return arg + arg; }, tt);

    visit([](auto&& a, auto&& b) { cout << a << " " << b << endl; }, w, ww);

    cout << (tt < ww) << endl;

    variant<int, double, string> q;
    q = t;
    get<string>(t) = "kekek";

    tt = "lololo";

    swap(t, tt);
    cout << get<string>(t) << endl;
    return 0;
}