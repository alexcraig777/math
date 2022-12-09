#include "real.h"

#include <stdio.h>
#include <stdlib.h>

#define VERBOSE 0

int add_test() {
    struct Real* a;
    struct Real* b;
    struct Real* c;
    struct Real* sum;

    a = make_real(POSITIVE, -1, 1,
                  (word) 0x1 << (sizeof(word)*8 - 1),
                  (word) 0x1 << (sizeof(word)*8 - 1));
    b = make_real(POSITIVE, -1, 1,
                  (word) 0x1 << (sizeof(word)*8 - 1),
                  (word) 0x1 << (sizeof(word)*8 - 1));
    c = make_real(POSITIVE, -1, 2,
                  0, 1, 1);

    sum = add(a, b);

    if (VERBOSE) {
        print_real(a);
        print_real(b);
        print_real(c);
        print_real(sum);
    }

    int rtn = (test_equal(c, sum) == 1) ? 0 : -1;

    free_real(a);
    free_real(b);
    free_real(c);
    free_real(sum);

    return rtn;
}

int mul_test() {
    struct Real* a;
    struct Real* b;
    struct Real* c;
    struct Real* prod;

    a = make_real(POSITIVE, 0, 1,
                  0xdeadbeefd00dcafe);
    b = make_real(POSITIVE, 0, 1,
                  0xdeadbeefd00dcafe);
    c = make_real(POSITIVE, 0, 2,
                  0x7f7478defec1d404,
                  0xc1b1cd138b5ff82f);

    prod = multiply(a, b);

    if (VERBOSE) {
        print_real(a);
        print_real(b);
        print_real(c);
        print_real(prod);
    }

    int rtn = (test_equal(c, prod) == 1) ? 0 : -1;

    free_real(a);
    free_real(b);
    free_real(c);
    free_real(prod);

    return rtn;
}

int div_test() {
    struct Real* a;
    struct Real* b;
    struct Real* quotient;

    a = make_real(POSITIVE, 0, 1,
                  0xdeadbeefd00dcafe);
    b = make_real(POSITIVE, -1, 1,
                  0xfe00000000000000,
                  0x00deadbeefd00dca);

    word divisor = 256;

    quotient = divide(a, divisor, 2);

    if (VERBOSE) {
        print_real(a);
        printf("0x%lx\n", divisor);
        print_real(b);
        print_real(quotient);
    }

    int rtn = (test_equal(b, quotient) == 1) ? 0 : -1;

    free_real(a);
    free_real(b);
    free_real(quotient);

    return rtn;
}

int main() {
    int rtn = 0;
    int temp;

    temp = add_test();
    rtn |= temp;
    if (temp == 0) {
        puts("Addition test passed!\n");
    } else {
        puts("Addition test failed :(\n");
    }

    temp = mul_test();
    rtn |= temp;
    if (temp == 0) {
        puts("Multiplication test passed!\n");
    } else {
        puts("Multiplication test failed :(\n");
    }

    temp = div_test();
    rtn |= temp;
    if (temp == 0) {
        puts("Divison test passed!\n");
    } else {
        puts("Division test failed :(\n");
    }

    if (rtn == 0) {
        puts("\nAll tests passed!");
    } else {
        puts("\nAt least one test failed :(");
    }

    return rtn;
}
