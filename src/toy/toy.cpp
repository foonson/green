// 2024.12.07

#include <cstdlib>

struct X {
    virtual ~X() = default;
};

struct Y: public X {
    char a[1000];
    virtual ~Y() = default;
};




int main() {

    X* x = new Y;
    //int* y = (int*) x;
    delete x;
    //x->a[0]='a';
}