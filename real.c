#include "real.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// A structure representing real numbers with arbitrary precision.
//
// A word index of 0 corresponds to the "ones" word.
// Positive word indices increase in significance, and negative
// word indices represent fractional words places.
// `max_word_idx` is actually the maximum present word index plus 1.
// `min_word_idx` is the actual minimum present word index.
// Thus there are `max_word_idx - min_word_idx` words present.
struct Real {
    enum sign_t sign;

    ssize_t min_word_idx;
    ssize_t max_word_idx;

    word* words;
};

// Functions for allocating and freeing structures.

struct Real* alloc_real(enum sign_t sign,
                         ssize_t min_word_idx,
                         ssize_t max_word_idx) {
    if (max_word_idx <= min_word_idx) {
        printf("Trying to alloc real with min = %ld, max = %ld\n",
               min_word_idx, max_word_idx);
        printf("Returning NULL\n");
        return NULL;
    }
    struct Real* r = malloc(sizeof(struct Real));
    set_sign(r, sign);
    set_max_word_idx(r, max_word_idx);
    set_min_word_idx(r, min_word_idx);
    allocate_words(r);
    return r;
}

struct Real* fill_real(enum sign_t sign,
                       ssize_t min_word_idx,
                       ssize_t max_word_idx,
                       ...) {
    struct Real* r = alloc_real(sign,
                                min_word_idx,
                                max_word_idx);

    va_list ap;
    va_start(ap, max_word_idx);

    ssize_t word_idx;
    for (word_idx = min_word_idx;
         word_idx < max_word_idx;
         word_idx++) {
        set_word(r, word_idx, va_arg(ap, word));
    }
    va_end(ap);

    return r;
}

void allocate_words(struct Real* r) {
    r->words = calloc(get_max_word_idx(r) - get_min_word_idx(r),
                      sizeof(word));
}

struct Real* copy_real(struct Real* r) {
    struct Real* rtn = alloc_real(get_sign(r),
                                   get_min_word_idx(r),
                                   get_max_word_idx(r));
    ssize_t word_idx;
    for (word_idx = get_min_word_idx(r);
         word_idx < get_max_word_idx(r);
         word_idx++) {
        set_word(rtn, word_idx, get_word(r, word_idx));
    }
    return rtn;
}

void free_real(struct Real* r) {
    free(r->words);
    free(r);
}


// Functions for getting and setting individual words.

word get_word(struct Real* r, ssize_t word_idx) {
    // Return the word of the real number corresponding
    // to the index `word_idx`. An index of 0 corresponds
    // to the units word, positive to higher integer words,
    // and negative to fractional words.
    // This does not take the sign of the real into account!
    word rtn;
    if (word_idx >= get_max_word_idx(r)) {
        rtn = 0;
    } else if (word_idx < get_min_word_idx(r)) {
        rtn = 0;
    } else {
        rtn = (r->words)[word_idx - get_min_word_idx(r)];
    }
    return rtn;
}

int set_word(struct Real* r, ssize_t word_idx, word l) {
    // Set the value of the `word_idx` word in `r`.
    // Returns 0 on success; -1 on error.
    int rtn;
    if (word_idx >= get_max_word_idx(r) ||
        word_idx < get_min_word_idx(r)) {
        puts("Tried to set word out of range!");
        rtn = -1;
    } else {
        rtn = 0;
        (r->words)[word_idx - get_min_word_idx(r)] = l;
    }
    return rtn;
}

hword get_half_word(struct Real* r, ssize_t hword_idx) {
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
        // We are going to change the more-significant half-word.
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


// Generic getters and setters.

ssize_t get_max_word_idx(struct Real* r) {
    return r->max_word_idx;
}

ssize_t get_min_word_idx(struct Real* r) {
    return r->min_word_idx;
}

enum sign_t get_sign(struct Real* r) {
    return r->sign;
}

void set_max_word_idx(struct Real* r, ssize_t idx) {
    r->max_word_idx = idx;
}

void set_min_word_idx(struct Real* r, ssize_t idx) {
    r->min_word_idx = idx;
}

void set_sign(struct Real* r, enum sign_t sign) {
    r->sign = sign;
}


// Miscellaneous functions.

int check_equal(struct Real* r1, struct Real* r2) {
    int rtn = 1;
    // After comparing all words, this flag will tell us whether both reals
    // are 0. If they are, then we shouldn't check the sign!
    int both_zero = 1;

    ssize_t word_idx;
    for (word_idx = MIN(get_min_word_idx(r1), get_min_word_idx(r2));
         word_idx < MAX(get_max_word_idx(r1), get_max_word_idx(r2));
         word_idx++) {
        if (get_word(r1, word_idx) != get_word(r2, word_idx)) {
            rtn = 0;
            break;
        }
        if (get_word(r1, word_idx) != 0) {
            both_zero = 0;
        }
    }

    if (rtn == 1 && both_zero == 0) {
        if (get_sign(r1) != get_sign(r2)) {
            rtn = 0;
        }
    }
    return rtn;
}

void trim_most_significant_zeros(struct Real* r) {
    // This function has to reference actual fields inside the struct Real
    // in order to reallocate the words buffer.
    ssize_t new_max_word_idx;
    ssize_t old_max_word_idx = get_max_word_idx(r);

    for (new_max_word_idx = old_max_word_idx;
         new_max_word_idx > get_min_word_idx(r);
         new_max_word_idx--) {
        if (get_word(r, new_max_word_idx - 1) != 0) {
            break;
        }
    }

    if (new_max_word_idx == get_min_word_idx(r)) {
        // `r` = 0. But it might already be as clean as possible.
        if (get_min_word_idx(r) != 0 || old_max_word_idx != 1) {
            free(r->words);
            set_min_word_idx(r, 0);
            set_max_word_idx(r, 1);
            allocate_words(r);
        }
    }
    else if (new_max_word_idx < old_max_word_idx) {
        // We can decrease the allocated space.
        word* old_words = r->words;
        set_max_word_idx(r, new_max_word_idx);
        allocate_words(r);
        ssize_t word_idx;
        for (word_idx = get_min_word_idx(r);
             word_idx < get_max_word_idx(r);
             word_idx++) {
            set_word(r, word_idx, old_words[word_idx - get_min_word_idx(r)]);
        }
        free(old_words);
    }
}

void trim_least_significant_zeros(struct Real* r) {
    // This function has to reference actual fields inside the struct Real
    // in order to reallocate the words buffer.
    ssize_t new_min_word_idx;
    ssize_t old_min_word_idx = get_min_word_idx(r);

    for (new_min_word_idx = old_min_word_idx;
         new_min_word_idx < get_max_word_idx(r);
         new_min_word_idx++) {
        if (get_word(r, new_min_word_idx) != 0) {
            break;
        }
    }

    if (new_min_word_idx == get_max_word_idx(r)) {
        // `r` = 0. But it might already be as clean as possible.
        if (old_min_word_idx != 0 || get_max_word_idx(r) != 1) {
            free(r->words);
            set_min_word_idx(r, 0);
            set_max_word_idx(r, 1);
            allocate_words(r);
        }
    }
    else if (new_min_word_idx > old_min_word_idx) {
        // We can decrease the allocated space.
        word* old_words = r->words;
        set_min_word_idx(r, new_min_word_idx);
        allocate_words(r);
        ssize_t word_idx;
        for (word_idx = get_min_word_idx(r);
             word_idx < get_max_word_idx(r);
             word_idx++) {
            set_word(r, word_idx, old_words[word_idx - old_min_word_idx]);
        }
        free(old_words);
    }
}

void trim_zeros(struct Real* r) {
    trim_most_significant_zeros(r);
    trim_least_significant_zeros(r);
}

void print_real(struct Real* r) {
    ssize_t word_idx;

    if (get_sign(r) == NEGATIVE) {
        fputs("-", stdout);
    }
    for (word_idx = MAX(0, get_max_word_idx(r) - 1);
         word_idx >= MIN(0, get_min_word_idx(r));
         word_idx--) {
        printf("%016lx", get_word(r, word_idx));
        if (word_idx == 0 &&
            get_min_word_idx(r) < 0) {
            fputs(".", stdout);
        }
    }
    fputs("\n", stdout);
}
