#include <iostream>
#include "deque.h"

int main() {
    deque<std::string> d;
    d.push_back("the");
    d.push_back("frogurt");
    d.push_back("is");
    d.push_back("also");
    d.push_back("cursed");
    std::cout << d.size() << '\n';
    deque<std::string> c = d;
    c.insert(c.begin() + 3, "hehmda");
    for (auto it = c.begin(); it < c.end(); it++) {
        std::cout << *it << ' ';
    }
    while (c.size() > 3) {
        c.erase(c.end() - 2);
    }
    std::cout << '\n';
    for (auto it = c.begin(); it < c.end(); it++) {
        std::cout << *it << ' ';
    }
    std::cout << '\n';

    std::cout << c[0] << '\n';

    deque<int> b;
    b.push_back(10);
    b.push_back(11);
    deque<int>::const_iterator it = b.begin();
    std::cout << *it << '\n';

    deque<int*> cd;
    for (size_t i = 0; i < 5; i++) {
        cd.push_front(new int(i));
    }
    std::cout << **(cd.end() - 1) << '\n';
    //std::cout << cd.size() << '\n';
    deque<int*>::const_iterator cit = cd.begin();
    std::cout << **(cd.erase(cit + 1)) << '\n';

    deque<int> onemore;
    for (int i = 0; i < 10; i++) {
        onemore.push_front(i);
    }
    std::cout << onemore.size() << '\n';
    for (size_t i = 0; i < onemore.size(); i++) {
        std::cout << onemore[i] << ' ';
    }
    std::cout << '\n';
    while (onemore.size() > 4) {
        std::cout << *onemore.erase(onemore.end() - 2) << ' ';
    }
    std::cout << '\n';

    b = onemore;
    std::cout << '\n';
    for (size_t i = 0; i < b.size(); i++) {
        std::cout << b[i] << ' ';
    }
    std::cout << '\n';

    for (size_t i = 0; i < onemore.size(); i++) {
        std::cout << onemore[i] << ' ';
    }
    std::cout << '\n';

    swap(c, d);
    for (auto it = c.begin(); it != c.end(); it++) {
        std::cout << *it << ' ';
    }
    std::cout << '\n';

    for (size_t i = 0; i < onemore.size(); i++) {
        std::cout << onemore[i] << ' ';
    }
    std::cout << '\n';

    while (onemore.size() < 10) {
        std::cout << *onemore.insert(onemore.end() - 2 + (onemore.size() % 2), onemore.size() + 10) << ' ';
    }
    std::cout << '\n';
    for (size_t i = 0; i < onemore.size(); i++) {
        std::cout << onemore[i] << ' ';
    }
    std::cout << '\n';

    onemore.clear();
    std::cout << "cleared\n";
    onemore.push_front(222);
    onemore.push_back(223);
    for (size_t i = 0; i < onemore.size(); i++) {
        std::cout << onemore[i] << ' ';
    }
    std::cout << '\n';

    deque<int> no_imaginary;
    for (int i = 1; i < 5; i++) {
        no_imaginary.push_back(i);
    }
    for (int i = 6; i <= 100; i++) {
        no_imaginary.push_front(i);
        no_imaginary.erase(no_imaginary.end() - 2);
    }

    for (size_t i = 0; i < no_imaginary.size(); i++) {
        std::cout << no_imaginary[i] << ' ';
    }
    std::cout << '\n';
    return 0;
}