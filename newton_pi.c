#include <stdio.h>

#include "arithmetic.h"
#include "decimal.h"

struct Real* get_threshold(struct Real* cos_est) {
    // Compute a rough estimate for the threshold beneath which we can
    // ignore cosine Taylor terms.
    struct Real* est_squared = mul_with_rel_sig(cos_est, cos_est, 3);
    struct Real* est_cubed = mul_with_rel_sig(cos_est, est_squared, 3);
    struct Real* result = div_with_rel_sig(est_cubed, 12, 3);

    /*
    printf("  * est_squared = ");
    print_decimal(est_squared);
    printf("  * est_cubed   = ");
    print_decimal(est_cubed);*/

    free_real(est_squared);
    free_real(est_cubed);
    return result;
}

struct Real* get_next_term(struct Real* current_term,
                           struct Real* theta_squared,
                           word term_idx, ssize_t min_sig_word_idx) {
    struct Real* temp1 = mul_with_sig(current_term, theta_squared,
                                      min_sig_word_idx);
    struct Real* temp2 = div_with_sig(temp1, 2*term_idx - 1,
                                      min_sig_word_idx);
    free_real(temp1);

    struct Real* rtn = div_with_sig(temp2, 2*term_idx, min_sig_word_idx);
    free_real(temp2);

    negate(rtn);

    return rtn;
}

struct Real* my_cos(struct Real* theta, ssize_t min_sig_word_idx) {
    // Computes cos(theta) using the Taylor series, keeping only words at or
    // above `min_sig_word_idx`.
    // This is tailored to computations of pi, so we will stop
    // computing when the terms drops below a pretty specific
    // threshold.

    // `cos_est` will hold our accumulated result. We've already
    // accounted for the first term!
    struct Real* cos_est = fill_real(POSITIVE, 0, 1, 1);

    // `next_term` will be operated on to compute the next Taylor
    // series term and then added to `cos_est` each iteration.
    struct Real* next_term = fill_real(POSITIVE, 0, 1, 1);

    // Since the Taylor series uses only even terms, we actually
    // really need only `theta` squared.
    struct Real* theta_squared = mul_with_sig(theta, theta,
                                              min_sig_word_idx);

    struct Real* temp;
    struct Real* threshold;
    int below_threshold;

    printf("computing cosine with min_sig_word_idx = %ld\n", min_sig_word_idx);
    /*
    printf("Computing cosine for theta = ");
    print_decimal(theta);
    printf("min_sig_word_idx = %ld\n", min_sig_word_idx);
    */

    word term_idx = 1;
    while (1) {
        /*
        printf("\nBeginning Taylor series term %ld\n", term_idx);
        printf("- current estimate = ");
        print_decimal(cos_est);
        */

        // Compute the next Taylor term.
        temp = get_next_term(next_term, theta_squared, term_idx,
                             min_sig_word_idx);
        free_real(next_term);
        next_term = temp;

        /*
        printf("- Taylor term = ");
        print_decimal(next_term);
        */

        // Compute a threshold estimate.
        threshold = get_threshold(cos_est);

        /*
        printf("- current threshold estimate = ");
        print_decimal(threshold);
        */

        // We keep track of the sign in `next_term`, so we can just add.
        temp = add(cos_est, next_term);
        free_real(cos_est);
        cos_est = temp;

        // If the next term is below the threshold, stop.
        below_threshold = greater_abs(threshold, next_term);
        free_real(threshold);
        if (below_threshold) {
            break;
        }

        term_idx++;
    }
    free_real(theta_squared);
    free_real(next_term);

    printf("computed cosine using %ld terms\n", term_idx);

    return cos_est;
}

void pi_step(struct Real** x, struct Real** d) {
    struct Real* new_x;

    trim_most_significant_zeros(*d);
    ssize_t min_sig_word_idx = get_max_word_idx(*d) * 9 - 5;

    free_real(*d);

    *d = my_cos(*x, min_sig_word_idx);
    new_x = add(*x, *d);

    free_real(*x);

    *x = new_x;
}

int main() {
    // Initial guess: 1.5
    struct Real* x = fill_real(POSITIVE, -1, 1,
                               (word) 1 << (sizeof(word)*8 - 1),
                               1);

    struct Real* two = fill_real(POSITIVE, 0, 1, 2);
    struct Real* pi;

    // Initial error: ~ 0.1
    struct Real* d = fill_real(POSITIVE, -1, 0,
                               0x1999999999999999ul);

    int newton_steps = 0;
    while (newton_steps < 10) {
        printf("\n============= %d Newton steps ==================\n",
               newton_steps);

        pi = multiply(x, two);

        printf("x_%d = ", newton_steps);
        print_decimal(x);

        printf("p_%d = ", newton_steps);
        print_decimal(pi);
        free_real(pi);

        printf("eps_%d ~ ", newton_steps - 1);
        print_decimal(d);

        printf("==================================================\n\n");

        pi_step(&x, &d);

        newton_steps++;
    }

    free_real(two);

    free_real(x);
    free_real(d);

    return 0;
}
