#include "real.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX(x, y) (x >= y ? x : y)
#define MIN(x, y) (x <= y ? x : y)


word get_word(struct Real* r, ssize_t word_idx) {
    // Return the word of the real number corresponding
    // to the index `word_idx`. An index of 0 corresponds
    // to the units word, positive to higher integer words,
    // and negative to fractional words.
    // This does not take the sign of the real into account!
    word rtn;
    if (word_idx >= r->max_word_idx) {
        rtn = 0;
    } else if (word_idx < r->min_word_idx) {
        rtn = 0;
    } else {
        rtn = (r->words)[word_idx - r->min_word_idx];
    }
    return rtn;
}

int set_word(struct Real* r, ssize_t word_idx, word l) {
    // Set the given word, following the indexing scheme
    // described above.
    int rtn;
    if (word_idx >= r->max_word_idx ||
        word_idx < r->min_word_idx) {
        puts("Tried to set word out of range!");
        rtn = -1;
    } else {
        (r->words)[word_idx - r->min_word_idx] = l;
        rtn = 0;
    }
    return rtn;
}

word get_half_word(struct Real* r, ssize_t hword_idx) {
    // Return the half-word of the real number corresponding
    // to the index `word_idx`. Half-word indices work the
    // same way as full-word indices, but where each chunk
    // is half as big.
    hword rtn;
    if (hword_idx & 1) {
        // This is an odd index, so it refers to the high
        // half-word of a word.
        rtn = (hword) (get_word(r, (hword_idx-1)/2)
                       >> sizeof(hword) * 8);
    } else {
        // This is an even index, so it refers to the low
        // half-word of a word.
        rtn = (hword) get_word(r, hword_idx/2);
    }
    return rtn;
}

int set_half_word(struct Real* r, ssize_t hword_idx, hword h) {
    int rtn;
    ssize_t word_idx;
    word w;
    if (hword_idx & 1) {
        word_idx = (hword_idx-1) / 2;
        // We are going to change the most-significant half-word.
        w = get_word(r, word_idx);
        // Zero out the most-significant half-word.
        w = w & (word) ((hword) -1);
        w = w | (((word) h) << (sizeof(hword)*8));
        rtn = set_word(r, word_idx, w);
    } else {
        // This is an even index, so it refers to the low
        // half-word of a word.
        word_idx = hword_idx/2;
        w = get_word(r, word_idx);
        w = (w >> (sizeof(hword)*8)) << (sizeof(hword)*8);
        w = w | h;
        rtn = set_word(r, word_idx, w);
    }
    return rtn;
}

int add_word_at_idx(struct Real* r, ssize_t hword_idx, word w) {
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
        if (current_hword_idx >= 2 * r->max_word_idx) {
            if (addend || carry) {
                printf("idx = %ld\tmax_idx = %ld\n",
                       current_hword_idx, r->max_word_idx);
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

struct Real* multiply(struct Real* r1, struct Real* r2) {
    struct Real* p = malloc(sizeof(struct Real));
    if (r1->sign == r2->sign) {
        p->sign = POSITIVE;
    } else {
        p->sign = NEGATIVE;
    }

    size_t num_words = (r1->max_word_idx
                        + r2->max_word_idx
                        - r1->min_word_idx
                        - r2->min_word_idx);

    p->min_word_idx = r1->min_word_idx + r2->min_word_idx;
    p->max_word_idx = p->min_word_idx + num_words;

    p->words = calloc(p->max_word_idx - p->min_word_idx,
                      sizeof(word));

    ssize_t idx_1, idx_2;
    word h1, h2;
    for (idx_1 = 2*r1->min_word_idx;
         idx_1 < 2*r1->max_word_idx; idx_1++) {
        h1 = (word) get_half_word(r1, idx_1);
        for (idx_2 = 2*r2->min_word_idx;
             idx_2 < 2*r2->max_word_idx; idx_2++) {
            h2 = (word) get_half_word(r2, idx_2);
            add_word_at_idx(p, idx_1 + idx_2, h1*h2);
        }
    }
    return p;
}

char greater_abs(struct Real* r1, struct Real* r2) {
    // Returns 1 if abs(r1) > abs(r2).
    // Otherwise returns 0.
    char rtn = 0;
    ssize_t word_idx;

    word w1, w2;
    for (word_idx = MAX(r1->max_word_idx,
                        r2->max_word_idx) - 1;
         word_idx >= MIN(r1->min_word_idx,
                         r2->min_word_idx);
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
    if (r1->sign == NEGATIVE) {
        negate(r1);
        s = subtract(r2, r1);
        negate(r1);
    } else if (r2->sign == NEGATIVE) {
        negate(r2);
        s = subtract(r1, r2);
        negate(r2);
    } else {
        s = malloc(sizeof(struct Real));
        s->sign = POSITIVE;
        s->max_word_idx = MAX(r1->max_word_idx,
                              r2->max_word_idx);
        s->min_word_idx = MIN(r1->min_word_idx,
                              r2->min_word_idx);

        s->words = calloc(s->max_word_idx - s->min_word_idx,
                          sizeof(word));

        ssize_t word_idx;
        word w1, w2, sum_word;
        int carry = 0;
        for (word_idx = s->min_word_idx;
             word_idx < s->max_word_idx;
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
            s->max_word_idx++;
            s->words = realloc(s->words,
                               (s->max_word_idx - s->min_word_idx) *
                               sizeof(word));
            set_word(s, s->max_word_idx - 1, (word) carry);
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
    } else if (r1->sign == NEGATIVE) {
        // If the first one is negative, compute the opposite
        // of the difference and return the opposite.
        negate(r1);
        negate(r2);
        s = subtract(r1, r2);
        negate(r1);
        negate(r2);
        negate(s);
    } else if (r2->sign == NEGATIVE) {
        // Just add them.
        negate(r2);
        s = add(r1, r2);
        negate(r2);
    } else {
        s = malloc(sizeof(struct Real));
        s->sign = POSITIVE;
        s->max_word_idx = MAX(r1->max_word_idx,
                              r2->max_word_idx);
        s->min_word_idx = MIN(r1->min_word_idx,
                              r2->min_word_idx);

        s->words = calloc(s->max_word_idx - s->min_word_idx,
                          sizeof(word));

        int borrow = 0;
        word w1, w2, diff_word;
        ssize_t word_idx;
        for (word_idx = s->min_word_idx;
             word_idx < s->max_word_idx;
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

struct Real* divide(struct Real* r, uint64_t divisor,
                    ssize_t num_extra_words) {
    word quotient, remainder;

    struct Real* q = malloc(sizeof(struct Real));
    q->sign = r->sign;
    q->max_word_idx = r->max_word_idx;
    q->min_word_idx = r->min_word_idx - num_extra_words;
    q->words = calloc(q->max_word_idx - q->min_word_idx, sizeof(word));

    ssize_t hword_idx;
    word h;
    remainder = 0;
    for (hword_idx = 2*q->max_word_idx - 1;
         hword_idx >= 2*q->min_word_idx;
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
    if (r->sign == POSITIVE) {
        r->sign = NEGATIVE;
    } else {
        r->sign = POSITIVE;
    }
}

char test_equal(struct Real* r1, struct Real* r2) {
    char rtn = 1;

    struct Real* t = subtract(r1, r2);

    ssize_t word_idx;
    for (word_idx = t->min_word_idx;
         word_idx < t->max_word_idx;
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
    ssize_t new_max_word_idx = r->max_word_idx;
    while (get_word(r, new_max_word_idx) == 0) {
        new_max_word_idx--;
        if (new_max_word_idx == r->min_word_idx - 1) {
            break;
        }
    }
    if (new_max_word_idx == r->max_word_idx) {
        // No change.
        rtn = r;
    } else if (new_max_word_idx == r->min_word_idx - 1) {
        // The whole thing is 0.
        rtn = make_real(POSITIVE, 0, 1, 0);
        free_real(r);
    } else {
        rtn = malloc(sizeof(struct Real));
        rtn->sign = r->sign;
        rtn->max_word_idx = new_max_word_idx + 1;
        rtn->min_word_idx = r->min_word_idx;
        rtn->words = calloc(rtn->max_word_idx - rtn->min_word_idx,
                            sizeof(word));

        ssize_t word_idx;
        for (word_idx = rtn->min_word_idx;
             word_idx < rtn->max_word_idx;
             word_idx++) {
            set_word(rtn, word_idx,
                     get_word(r, word_idx));
        }
        free_real(r);
    }
    return rtn;
}

void print_real(struct Real* r) {
    ssize_t word_idx;

    if (r->sign == NEGATIVE) {
        fputs("-", stdout);
    }
    for (word_idx = MAX(0, r->max_word_idx - 1);
         word_idx >= MIN(0, r->min_word_idx);
         word_idx--) {
        printf("%016lx", get_word(r, word_idx));
        if (word_idx == 0 &&
            r->min_word_idx < 0) {
            fputs(".", stdout);
        }
    }
    fputs("\n", stdout);
}

struct Real* make_real(enum sign_t sign,
                       ssize_t min_word_idx,
                       ssize_t max_word_idx,
                       ...) {
    struct Real* r = malloc(sizeof(struct Real));
    r->sign = sign;
    r->max_word_idx = max_word_idx;
    r->min_word_idx = min_word_idx;

    r->words = calloc(max_word_idx - min_word_idx, sizeof(word));

    va_list ap;
    va_start(ap, max_word_idx);

    ssize_t word_idx;
    word next_word;
    for (word_idx = min_word_idx;
         word_idx < max_word_idx;
         word_idx++) {
        next_word = va_arg(ap, word);
        set_word(r, word_idx, next_word);
    }

    return r;
}

void free_real(struct Real* r) {
    free(r->words);
    free(r);
}

int old_main() {
    struct Real* r;
    struct Real* t;
    struct Real* s;

    r = malloc(sizeof(struct Real));
    r->sign = POSITIVE;
    r->max_word_idx = 1;
    r->min_word_idx = -1;
    r->words = calloc(r->max_word_idx - r->min_word_idx, sizeof(word));

    set_word(r, 0, 1);

    t = malloc(sizeof(struct Real));
    t->sign = POSITIVE;
    t->max_word_idx = 1;
    t->min_word_idx = -1;
    t->words = calloc(t->max_word_idx - t->min_word_idx, sizeof(word));

    set_word(t, -1, 1);

    print_real(r);
    print_real(t);

    s = add(r, t);
    print_real(s);
    free_real(s);

    s = subtract(r, t);
    print_real(s);
    free_real(s);

    s = multiply(r, t);
    print_real(s);
    free_real(s);

    free_real(r);
    free_real(t);

    return 0;
}
