#include <iostream>

static int test = 0;

int foo(int x) {
    return x * 2;
}

void bar(int y) {
    test += y;
}

int main(int argc, char **argv) {
    int x  = 3;

    int y = foo(x);

    bar(y);

    std::cout << test;

    return 0;
}
