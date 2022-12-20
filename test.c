#include "test.h"

#include <stdio.h>

void run_tests(test_func_t* tests, char** test_names) {
    int test_idx;
    int rtn;
    int successes = 0;

    for (test_idx = 0; tests[test_idx] != NULL; test_idx++) {
        rtn = tests[test_idx]();
        if (rtn == 0) {
            successes += 1;
        }

        printf("\n--------------------------------------------------\n");
        printf("(%d) %s %s\n", test_idx + 1, test_names[test_idx],
               rtn == 0 ? "passed!" : "failed :(");
        printf("--------------------------------------------------\n");
    }

    printf("\n%d/%d tests passed!\n", successes, test_idx);

    if (successes == test_idx) {
        printf("\nAll tests passed!\n");
    } else {
        printf("\nAt least one test failed :(\n");
    }
}
