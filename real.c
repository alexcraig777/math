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

struct Real* create_real(enum sign_t sign,
                         ssize_t min_word_idx,
                         ssize_t max_word_idx) {
    // Allocates space for a new struct Real.
    // Assigns the simple fields and allocates the correct amount
    // of space for the words.
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
    // Creates a new struct Real, assigns all fields, allocates
    // space for the words, and fills them in with the words supplied
    // by the variable arguments.
    // The number of varargs must be `max_word_idx - min_word_idx + 1`.
    struct Real* r = create_real(sign,
                                 min_word_idx,
                                 max_word_idx);

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

void allocate_words(struct Real* r) {
    // Allocates space for the appropriate number of words.
    r->words = calloc(get_max_word_idx(r) - get_min_word_idx(r),
                      sizeof(word));
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
    // Set the given word, following the indexing scheme
    // described above.
    int rtn;
    if (word_idx >= get_max_word_idx(r) ||
        word_idx < get_min_word_idx(r)) {
        puts("Tried to set word out of range!");
        rtn = -1;
    } else {
        (r->words)[word_idx - get_min_word_idx(r)] = l;
        rtn = 0;
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
