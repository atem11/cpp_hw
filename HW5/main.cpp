#include <random>

#include <iostream>
#include <vector>
#include <assert.h>
#include <algorithm>
#include <set>
#include <time.h>
#include "per_set.h"

int main() {
    const int TESTS = 100'000;

    per_set<int> a, b;
    a.insert(1);
    a.insert(3);
    a.insert(2);
    a.insert(4);
    b = a;

    b.erase(b.find(3));
    b.insert(8);
    a.erase(a.find(2));
    a.insert(-4);

    cout << "per-set a: ";
    for (int it : a) {
        cout << it << ' ';
    }

    cout << "\nper-set b: ";
    for (int it : b) {
        cout << it << ' ';
    }

    per_set<int> r;
    r.insert(5);
    r.insert(6);
    auto e = r.begin();
    ++e;
    std::cout << (*e) << '\n';
    e = r.end();
    --e;
    std::cout << (*e) << '\n';

    srand(static_cast<unsigned int>(time(nullptr)));
    std::vector <int> v;
    for (int i = 1; i <= 15; ++i) {
        v.push_back(i);
    }
    int tests = 0;
    for(int it = 0; it < TESTS; ++it) {
        ++tests;
        if (tests % 100000 == 0) {
            std::cout << "done : " << tests << '\n';
        }
        std::shuffle(v.begin(), v.end(), std::mt19937(std::random_device()()));
        per_set<int> s;
        for (int x : v) {
            s.insert(x);
        }

        for (int i = 1; i <= 15; ++i) {
            assert(s.find(i) != s.end());
        }

        int k = 1;
        for (int it1 : s) {
            assert(it1 == k);
            ++k;
        }
    }
    std::cout << "INSERT IS GOOD\n";

    tests = 0;
    for (int it = 0; it < TESTS; ++it) {
        ++tests;
        if (tests % 100000 == 0) {
            std::cout << "done : " << tests << '\n';
        }
        std::shuffle(v.begin(), v.end(), std::mt19937(std::random_device()()));
        std::vector<int> erase_order = v;
        std::shuffle(erase_order.begin(), erase_order.end(), std::mt19937(std::random_device()()));

        per_set<int> s;
        std::set<int> ss;

        for (int x : v) {
            s.insert(x);
            ss.insert(x);
        }

        for (int x : erase_order) {
            s.erase(s.find(x));
            ss.erase(ss.find(x));

            auto it1 = s.begin();
            auto it2 = ss.begin();
            for(; it2 != ss.end(); ++it2) {
                assert((*it1) == (*it2));
                ++it1;
            }
        }
    }
    std::cout << "ERASE IS GOOD\n";
    return 0;
}