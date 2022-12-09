#include "trig.h"

struct Real* my_cos(struct Real* theta, word num_terms) {
    struct Real* rtn = make_real(POSITIVE, 0, 1, 1);
    struct Real* addend = make_real(POSITIVE, 0, 1, 1);
    struct Real* partial;

    struct Real* square = multiply(theta, theta);

    word term_idx;
    for (term_idx = 1; term_idx < num_terms; term_idx++) {
        partial = divide(addend, 2*term_idx - 1, 2);
        free_real(addend);
        addend = divide(partial, 2*term_idx, 2);
        free_real(partial);
        partial = multiply(addend, square);
        free_real(addend);
        addend = partial;

        if (term_idx & 1) {
            partial = subtract(rtn, addend);
        } else {
            partial = add(rtn, addend);
        }
        free_real(rtn);
        rtn = partial;
    }
    free_real(square);
    free_real(addend);
    return rtn;
}

struct Real* my_pi(word num_steps, word num_terms) {
    struct Real* p = make_real(POSITIVE, -1, 1,
                               (word) 1 << (sizeof(word)*8 - 1),
                               1);
    struct Real* partial_1;
    struct Real* partial_2;

    word step_idx;
    for (step_idx = 0; step_idx < num_steps; step_idx++) {
        partial_1 = my_cos(p, num_terms);
        partial_2 = add(p, partial_1);
        free_real(p);
        free_real(partial_1);

        p = partial_2;

        p = trim_leading_zeros(p);
    }

    return p;
}

int main() {
    struct Real* half_pi = my_pi(2, 10);

    struct Real* two = make_real(POSITIVE, 0, 1, 2);

    struct Real* pi = multiply(half_pi, two);

    free_real(half_pi);
    free_real(two);

    print_real(pi);

    free_real(pi);

    return 0;
}
