#include "arithmetic.h"

#include <stdio.h>

#include "real.h"


int add_word_at_hword_idx(struct Real* r, ssize_t hword_idx, word w) {
    // Add a word at the given half-word index `hword_idx`.
    // Half-word indices work exactly as full-word indices, but
    // reference half-words instead of full-words.
    int rtn = 0;
    hword addend, sum;
    int carry = 0;
    ssize_t current_hword_idx = hword_idx;
    while (1) {
        // Determine what we need to add at this index.
        if (current_hword_idx == hword_idx) {
            // First iteration.
            addend = (hword) w;
        } else if (current_hword_idx == hword_idx + 1) {
            // Second iteration.
            addend = (hword) (w >> (sizeof(hword)*8));
        } else if (carry == 0) {
            break;
        } else {
            addend = 0;
        }

        // Stop if our current index is too big.
        if (current_hword_idx >= 2 * get_max_word_idx(r)) {
            if (addend || carry) {
                printf("idx = %ld\tmax_idx = %ld\n",
                       current_hword_idx, get_max_word_idx(r));
                printf("addend = %d\tcarry = %d\n", addend, carry);
                puts("Out of range, but still have to add more :(");
                rtn = -1;
                break;
            }
        }

        sum = addend + get_half_word(r, current_hword_idx) + carry;
        set_half_word(r, current_hword_idx, sum);
        if ((carry == 0 &&
             sum < addend) ||
            (carry == 1 &&
             sum <= addend)) {
            carry = 1;
        } else {
            carry = 0;
        }
        current_hword_idx++;
    }
    return rtn;
}

struct Real* mul_with_sig(struct Real* r1, struct Real* r2,
                          ssize_t min_sig_word_idx) {
    // Multiply 2 real numbers, ignore all words below `min_sig_word_idx`.
    struct Real* p;
    enum sign_t sign;
    ssize_t num_words;
    ssize_t min_word_idx, max_word_idx;

    if (get_sign(r1) == get_sign(r2)) {
        sign = POSITIVE;
    } else {
        sign = NEGATIVE;
    }

    max_word_idx = get_max_word_idx(r1) + get_max_word_idx(r2);
    min_word_idx = MAX(get_min_word_idx(r1) + get_min_word_idx(r2),
                       min_sig_word_idx);

    p = create_real(sign, min_word_idx, max_word_idx);

    ssize_t idx_1, idx_2;
    ssize_t idx_2_lower_bound;
    word h1, h2;
    for (idx_1 = 2*get_min_word_idx(r1);
         idx_1 < 2*get_max_word_idx(r1); idx_1++) {
        h1 = (word) get_half_word(r1, idx_1);

        idx_2_lower_bound = MAX(2 * get_min_word_idx(r2),
                                min_word_idx - idx_1);
        for (idx_2 = idx_2_lower_bound;
             idx_2 < 2*get_max_word_idx(r2); idx_2++) {
            h2 = (word) get_half_word(r2, idx_2);
            add_word_at_hword_idx(p, idx_1 + idx_2, h1*h2);
        }
    }
    return p;
}

struct Real* multiply(struct Real* r1, struct Real* r2) {
    struct Real* p = mul_with_sig(r1, r2,
                                  get_min_word_idx(r1)
                                  + get_min_word_idx(r2));
    return p;
}

char greater_abs(struct Real* r1, struct Real* r2) {
    // Returns 1 if abs(r1) > abs(r2).
    // Otherwise returns 0.
    char rtn = 0;
    ssize_t word_idx;

    word w1, w2;
    for (word_idx = MAX(get_max_word_idx(r1),
                        get_max_word_idx(r2)) - 1;
         word_idx >= MIN(get_min_word_idx(r1),
                         get_min_word_idx(r2));
         word_idx--) {

        w1 = get_word(r1, word_idx);
        w2 = get_word(r2, word_idx);
        if (w1 > w2) {
            // r1 has the bigger absolute value.
            rtn = 1;
            break;
        } else if (w1 < w2) {
            rtn = 0;
            break;
        }
        // Otherwise we need to check the lower words.
    }
    return rtn;
}

struct Real* add(struct Real* r1, struct Real* r2) {
    struct Real* s;
    struct Real* tmp;
    if (get_sign(r1) == NEGATIVE) {
        negate(r1);
        s = subtract(r2, r1);
        negate(r1);
    } else if (get_sign(r2) == NEGATIVE) {
        negate(r2);
        s = subtract(r1, r2);
        negate(r2);
    } else {
        s = create_real(POSITIVE,
                        MIN(get_min_word_idx(r1),
                            get_min_word_idx(r2)),
                        MAX(get_max_word_idx(r1),
                            get_max_word_idx(r2)));

        ssize_t word_idx;
        word w1, w2, sum_word;
        int carry = 0;
        for (word_idx = get_min_word_idx(s);
             word_idx < get_max_word_idx(s);
             word_idx++) {
            w1 = get_word(r1, word_idx);
            w2 = get_word(r2, word_idx);
            sum_word = w1 + w2 + carry;
            set_word(s, word_idx, sum_word);
            if ((carry == 0 &&
                 sum_word < w1) ||
                (carry == 1 &&
                 sum_word <= w1)) {
                carry = 1;
            } else {
                carry = 0;
            }
        }

        if (carry) {
            // There was a carry beyond the most-significant
            // allocated word.
            tmp = create_real(get_sign(s),
                              get_min_word_idx(s),
                              get_max_word_idx(s) + 1);
            for (word_idx = get_min_word_idx(s);
                 word_idx < get_max_word_idx(s);
                 word_idx++) {
                set_word(tmp, word_idx, get_word(s, word_idx));
            }
            set_word(tmp, get_max_word_idx(s), (word) carry);
            free_real(s);
            s = tmp;
        }
    }
    return s;
}

struct Real* subtract(struct Real* r1, struct Real* r2) {
    struct Real* s;

    if (greater_abs(r2, r1)) {
        // Always subtract the smaller absolute value from the
        // larger.
        s = subtract(r2, r1);
        negate(s);
    } else if (get_sign(r1) == NEGATIVE) {
        // If the first one is negative, compute the opposite
        // of the difference and return the opposite.
        negate(r1);
        negate(r2);
        s = subtract(r1, r2);
        negate(r1);
        negate(r2);
        negate(s);
    } else if (get_sign(r2) == NEGATIVE) {
        // Just add them.
        negate(r2);
        s = add(r1, r2);
        negate(r2);
    } else {
        // In this case, we know that `r1` has the greater absolute
        // value and is positive, and `r2` is negative.
        s = create_real(POSITIVE,
                        MIN(get_min_word_idx(r1),
                            get_min_word_idx(r2)),
                        MAX(get_max_word_idx(r1),
                            get_max_word_idx(r2)));

        int borrow = 0;
        word w1, w2, diff_word;
        ssize_t word_idx;
        for (word_idx = get_min_word_idx(s);
             word_idx < get_max_word_idx(s);
             word_idx++) {
            w1 = get_word(r1, word_idx);
            w2 = get_word(r2, word_idx);
            diff_word = w1 - w2 - borrow;
            set_word(s, word_idx, diff_word);
            if ((borrow == 0 &&
                 diff_word > w1) ||
                (borrow == 1 &&
                 diff_word >= w1)) {
                borrow = 1;
            } else {
                borrow = 0;
            }
        }
    }
    return s;
}

struct Real* div_with_sig(struct Real* r, word divisor,
                          ssize_t min_sig_word_idx) {
    word quotient, remainder;

    struct Real* q = create_real(get_sign(r),
                                 min_sig_word_idx,
                                 get_max_word_idx(r));

    ssize_t hword_idx;
    word h;
    remainder = 0;
    for (hword_idx = 2*get_max_word_idx(q) - 1;
         hword_idx >= 2*get_min_word_idx(q);
         hword_idx--) {
        h = (word) get_half_word(r, hword_idx);
        h += remainder << (sizeof(hword)*8);
        quotient = h / divisor;
        set_half_word(q, hword_idx, (hword) quotient);
        remainder = h % divisor;
    }
    return q;
}

void negate(struct Real* r) {
    if (get_sign(r) == POSITIVE) {
        set_sign(r, NEGATIVE);
    } else {
        set_sign(r, POSITIVE);
    }
}

char test_equal(struct Real* r1, struct Real* r2) {
    char rtn = 1;

    struct Real* t = subtract(r1, r2);

    ssize_t word_idx;
    for (word_idx = get_min_word_idx(t);
         word_idx < get_max_word_idx(t);
         word_idx++) {
        if (get_word(t, word_idx) != 0) {
            rtn = 0;
            break;
        }
    }
    free_real(t);
    return rtn;
}

struct Real* trim_leading_zeros(struct Real* r) {
    struct Real* rtn;
    ssize_t new_max_word_idx = get_max_word_idx(r);
    while (get_word(r, new_max_word_idx) == 0) {
        new_max_word_idx--;
        if (new_max_word_idx == get_min_word_idx(r) - 1) {
            break;
        }
    }
    if (new_max_word_idx == get_max_word_idx(r)) {
        // No change.
        rtn = r;
    } else if (new_max_word_idx == get_min_word_idx(r) - 1) {
        // The whole thing is 0.
        rtn = fill_real(POSITIVE, 0, 1, 0);
        free_real(r);
    } else {
        rtn = create_real(get_sign(r),
                          new_max_word_idx + 1,
                          get_min_word_idx(r));

        ssize_t word_idx;
        for (word_idx = get_min_word_idx(rtn);
             word_idx < get_max_word_idx(rtn);
             word_idx++) {
            set_word(rtn, word_idx,
                     get_word(r, word_idx));
        }
        free_real(r);
    }
    return rtn;
}


