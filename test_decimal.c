#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "real.h"
#include "decimal.h"
#include "test.h"


int test_real_to_decimal() {
    int rtn = 0;

    struct Real* r = fill_real(POSITIVE, -2, 2,
                               17162673854044802117ul, 14069626884177783709ul,
                               10712362532993335832ul, 1632372527949843584ul);

    char* dec_str = real_to_decimal_str(r);
    char* correct = "30111958256045056550262256846967767576.76271600169430058847757619092581866378082250145855637206374712502560758825364072187248838663453653907708940096199512481689453125";

    if (strcmp(dec_str, correct) != 0) {
        FAIL("real_to_decimal");
        printf("correct: %s\n", correct);
        printf("ours:    %s\n", dec_str);
    }

    free(dec_str);
    free_real(r);

    return rtn;
}

test_func_t tests[] = {
    test_real_to_decimal,
    NULL};
char* test_names[] = {
    "real_to_decimal",
    NULL};

