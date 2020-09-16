#include <iostream>
#include <memory>

#include "LeakDetector.hpp"

// Test hook leak
class Err {
public:
    Err(int n) {
        if (n == 0) throw 1000;
        data = new int[n];
    }
    ~Err() { delete[] data; }
private:
    int *data;
};

// Test class
class A;
class B;
class A {
public:
    std::shared_ptr<B> p;
};
class B {
public:
    std::shared_ptr<A> p;
};

// g++ main.cpp LeakDetector.cpp -std=c++11 -Wno-write-strings

int main() {

    // Forget to release pointer b which will cause memory leaking
    int *a = new int;
    int *b = new int;
    delete a;

    // 0 passed as a parameter to the constructor will cause 
    // an exception, resulting in a memory leak of the 
    // exception branch
    try {
        Err *e = new Err(0);
        delete e;
    } catch (int &ex) {
        std::cout << "Exception catch: " << ex << std::endl;
    };


    // Memory leak caused by smart pointer circular references
    auto smartA = std::make_shared<A>();
    auto smartB = std::make_shared<B>();
    smartA->p = smartB;
    smartB->p = smartA;

    return 0;
}