#include "real.h"

#include <stdio.h>
#include <stdlib.h>

#include "arithmetic.h"
#include "decimal.h"
#include "test.h"


int test_add() {
    int rtn = 0;

    struct Real* a;
    struct Real* b;
    struct Real* c;
    struct Real* sum;

    a = fill_real(POSITIVE, -1, 1,
                  (word) 0x1 << (sizeof(word)*8 - 1),
                  (word) 0x1 << (sizeof(word)*8 - 1));
    b = fill_real(POSITIVE, -1, 1,
                  (word) 0x1 << (sizeof(word)*8 - 1),
                  (word) 0x1 << (sizeof(word)*8 - 1));
    c = fill_real(POSITIVE, -1, 2,
                  0, 1, 1);
    sum = add(a, b);
    if (check_equal(c, sum) != 1) {
        FAIL("add positives with carry");
    }
    free_real(sum);

    negate(a);
    negate(b);
    negate(c);
    sum = add(a, b);
    if (check_equal(c, sum) != 1) {
        FAIL("add negatives with carry");
    }
    free_real(c);
    free_real(sum);

    negate(a);
    sum = add(a, b);
    if (is_zero(sum) != 1) {
        FAIL("add opposites to get 0");
    }
    free_real(a);
    free_real(b);
    free_real(sum);

    return rtn;
}

int test_mul() {
    int rtn = 0;

    struct Real* a = fill_real(POSITIVE, -2, 2,
                               0xee2e10f042398c45, 0xc3415b1b69b3eb9d,
                               0x94a9f4f79c458218, 0x16a75a2a5d8e8c80);
    struct Real* b = fill_real(POSITIVE, -2, 2,
                               0xdc49ff0d33620852, 0xd3b6a44e67526975,
                               0xf31ffbc79614c22e, 0xef148957b5fb53f6);
    struct Real* c = fill_real(POSITIVE, -4, 4,
                               0xfc59a81743b161a, 0xe5f0b9b8fc78b0b2,
                               0x2e3314cfaf0746aa, 0x50669630eb868b3d,
                               0xe47766ab83b98066, 0x2ffca16f0814c8e4,
                               0x91b86c837284a6a4, 0x15280e67eae24509);

    struct Real* prod = multiply(a, b);
    if (check_equal(c, prod) != 1) {
        FAIL("multiply");
    }
    free_real(c);
    free_real(prod);

    c = fill_real(POSITIVE, 0, 4,
                  0xe47766a8b36a5c9f, 0x2ffca16f0814c8e4,
                  0x91b86c837284a6a4, 0x15280e67eae24509);
    prod = mul_with_sig(a, b, 0);
    if (check_equal(c, prod) != 1) {
        FAIL("mul_with_sig");
    }
    free_real(prod);

    prod = mul_with_rel_sig(a, b, 4);
    if (check_equal(c, prod) != 1) {
        FAIL("mul_with_rel_sig");
    }

    free_real(a);
    free_real(b);
    free_real(c);
    free_real(prod);

    return rtn;
}

int test_div() {
    int rtn = 0;

    struct Real* a;
    struct Real* b;
    struct Real* quotient;

    a = fill_real(POSITIVE, 0, 1,
                  0xdeadbeefd00dcafe);
    b = fill_real(POSITIVE, -1, 1,
                  0xfe00000000000000,
                  0x00deadbeefd00dca);
    word divisor = 256;

    quotient = div_with_sig(a, divisor, -2);

    if (check_equal(b, quotient) != 1) {
        FAIL("div_with_sig");
    }
    free_real(quotient);

    quotient = div_with_rel_sig(a, divisor, 2);
    if (check_equal(b, quotient) != 1) {
        FAIL("div_with_rel_sig");
    }
    free_real(quotient);

    free_real(a);
    free_real(b);

    return rtn;
}

int dec_test() {
    struct Real* a = fill_real(POSITIVE, -1, 1,
                               0x243f6a8885a30000, 3);
    struct Real* b = fill_real(NEGATIVE, -2, 0,
                               0x243f6a8885a30000, 3);
    struct Real* c = fill_real(NEGATIVE, -1, 0,
                               0xcde32db45b8e18);

    free_real(a);
    free_real(b);
    free_real(c);

    return 0;
}


test_func_t tests[] = {
    test_add,
    test_mul,
    test_div,
    NULL};

char* test_names[] = {
    "add",
    "mul",
    "div",
    NULL};
