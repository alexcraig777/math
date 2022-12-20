#ifndef TEST_H
#define TEST_H

#define FAIL(msg) printf("%s failed: %s\n", __func__, msg); \
    rtn = -1;

typedef int (*test_func_t)(void);

// Runs all tests and prints out success/failure for each.
//
// `tests` and `test_names` are both null-terminated arrays of the same
// length.
void run_tests(test_func_t* tests, char** test_names);

#endif
