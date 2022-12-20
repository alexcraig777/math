#include <stdio.h>

#include "real.h"
#include "test.h"


int test_fill_get_set() {
    struct Real* r;
    int rtn = 0;

    r = alloc_real(POSITIVE, 1, 0);
    if (r != NULL) {
        FAIL("alloc_real: we should get a NULL real when min > max\n");
    }

    r = fill_real(POSITIVE, -2, 2, 3, 4, 5, 6);
    if (get_word(r, -3) != 0) {
        FAIL("alloc_word/get_word");
    }
    if (get_word(r, -2) != 3) {
        FAIL("alloc_word/get_word");
    }
    if (get_word(r, -1) != 4) {
        FAIL("alloc_word/get_word");
    }
    if (get_word(r, 0) != 5) {
        FAIL("alloc_word/get_word");
    }
    if (get_word(r, 1) != 6) {
        FAIL("alloc_word/get_word");
    }
    if (get_word(r, 2) != 0) {
        FAIL("alloc_word/get_word");
    }

    if (set_word(r, -3, 42) != -1) {
        FAIL("set_word with out of range index should be -1!");
    }
    if (set_word(r, 2, 42) != -1) {
        FAIL("set_word with out of range index should be -1!");
    }
    if (set_word(r, 0, 42) != 0) {
        FAIL("set_word with good index should return 0!");
    }
    if (get_word(r, 0) != 42) {
        FAIL("get_word/set_word");
    }

    free_real(r);

    return rtn;
}

int test_copy() {
    int rtn = 0;

    struct Real* r = fill_real(POSITIVE, 2, 4, 24, 42);
    struct Real* s = copy_real(r);

    if (get_sign(r) != get_sign(s)) {
        FAIL("copy_real/get_sign");
    }
    int i;
    for (i = -1; i < 6; i++) {
        if (get_word(r, i) != get_word(s, i)) {
            FAIL("copy_real/get_word");
        }
    }

    free_real(r);
    free_real(s);

    return rtn;
}

int test_equal() {
    int rtn = 0;

    struct Real* r = fill_real(POSITIVE, -2, 2, 2, 3, 4, 0);
    struct Real* s = fill_real(POSITIVE, -3, 1, 0, 2, 3, 4);
    struct Real* t = fill_real(POSITIVE, -3, 1, 0, 2, 5, 4);
    struct Real* u = fill_real(POSITIVE, -1, 2, 0, 0, 0);
    struct Real* v = fill_real(NEGATIVE, 0, 1, 0);

    if (check_equal(r, s) != 1) {
        FAIL("check_equal false negative on nonzero values");
    }
    if (check_equal(s, t) != 0) {
        FAIL("check_equal: false positive with unequal words");
    }
    set_sign(s, NEGATIVE);
    if (check_equal(r, s) != 0) {
        FAIL("check_equal: false positive with different signs");
    }

    if (check_equal(u, v) != 1) {
        FAIL("check_equal: false negative with signed 0");
    }

    free_real(r);
    free_real(s);
    free_real(t);
    free_real(u);
    free_real(v);

    return rtn;
}

int test_trim() {
    int rtn = 0;

    struct Real* r = fill_real(POSITIVE, -2, 3, 0, 0, 1, 0, 0);
    struct Real* s = copy_real(r);

    trim_most_significant_zeros(r);
    if (get_min_word_idx(r) != -2 || get_max_word_idx(r) != 1) {
        FAIL("trim_most_significant_zeros: min/max word index");
    }
    if (check_equal(r, s) != 1) {
        FAIL("trim_most_significant_zeros: equality");
    }

    trim_least_significant_zeros(r);
    if (get_min_word_idx(r) != 0 || get_max_word_idx(r) != 1) {
        FAIL("trim_least_significant_zeros: min/max word index");
    }
    if (check_equal(r, s) != 1) {
        print_real(r);
        print_real(s);
        FAIL("trim_least_significant_zeros: equality");
    }

    trim_zeros(r);
    if (get_min_word_idx(r) != 0 || get_max_word_idx(r) != 1) {
        FAIL("trim_zeros: min/max word index on clean nonzero");
    }
    if (check_equal(r, s) != 1) {
        print_real(r);
        print_real(s);
        FAIL("trim_zeros: equality on clean nonzero");
    }

    free_real(r);
    free_real(s);

    struct Real* z = fill_real(POSITIVE, 2, 4, 0, 0);
    trim_most_significant_zeros(z);
    if (get_min_word_idx(z) != 0 || get_max_word_idx(z) != 1) {
        FAIL("trim_most_significant_zeros: min/max word index on 0");
    }
    if (get_word(z, 0) != 0) {
        FAIL("trim_most_significant_zeros: equality on 0");
    }
    free_real(z);

    z = fill_real(POSITIVE, 2, 4, 0, 0);
    trim_least_significant_zeros(z);
    if (get_min_word_idx(z) != 0 || get_max_word_idx(z) != 1) {
        FAIL("trim_least_significant_zeros: min/max word index on 0");
    }
    if (get_word(z, 0) != 0) {
        FAIL("trim_least_significant_zeros: equality on 0");
    }
    free_real(z);
    return rtn;
}

int main(void) {
    test_func_t tests[] = {
        test_fill_get_set,
        test_copy,
        test_equal,
        test_trim, NULL};
    char* test_names[] = {
        "fill_get_set",
        "test_copy",
        "test_equal",
        "test_trim", NULL};

    run_tests(tests, test_names);

    return 0;
}
