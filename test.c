#include "test.h"

#include <stdio.h>

extern test_func_t tests[];
extern char* test_names[];

void run_tests(test_func_t* tests, char** test_names) {
    int test_idx;
    int rtn;
    int successes = 0;

    for (test_idx = 0; tests[test_idx] != NULL; test_idx++) {
        rtn = tests[test_idx]();
        if (rtn == 0) {
            successes += 1;
        }

        printf("--> (%d) %s %s\n", test_idx + 1, test_names[test_idx],
               rtn == 0 ? "passed!" : "failed :(");
    }

    printf("\n-----------------------------------------\n");
    printf("--> %d/%d tests passed!\n", successes, test_idx);
    if (successes == test_idx) {
        printf("--> All tests passed!\n");
    } else {
        printf("--> At least one test failed :(\n");
    }
    printf("-----------------------------------------\n");
}

int main(void) {
    run_tests(tests, test_names);

    return 0;
}

