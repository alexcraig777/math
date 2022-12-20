#ifndef REAL_H
#define REAL_H

#include <sys/types.h>
#include <stdint.h>
#include <stdarg.h>


enum sign_t {
    POSITIVE,
    NEGATIVE
};

struct Real;

typedef uint64_t word;
typedef uint32_t hword;


#define MAX(x, y) (x >= y ? x : y)
#define MIN(x, y) (x <= y ? x : y)


// Functions for allocating and freeing structures.

// Allocates space for a new struct Real.
//
// Assigns the simple fields and allocates the correct amount of space
// for the words, which are initialized to 0.
struct Real* alloc_real(enum sign_t sign,
                        ssize_t min_word_idx,
                        ssize_t max_word_idx);

// Creates a new struct Real, assigns all fields, allocates space for the
// words, and fills them in with the words supplied by the variable
// arguments.
//
// The number of varargs must be `max_word_idx - min_word_idx + 1`.
struct Real* fill_real(enum sign_t sign,
                       ssize_t min_word_idx,
                       ssize_t max_word_idx,
                       ...);

// Allocates the correct amount of space for `r`'s words based on the
// current values of `min_word_idx` and `max_word_idx`.
//
// The memory is initialized to 0.
void allocate_words(struct Real* r);

// Creates a copy of `r`.
struct Real* copy_real(struct Real* r);

// Frees all memory associated with `r`.
void free_real(struct Real* r);


// Functions for getting and setting individual words.

word get_word(struct Real* r, ssize_t word_idx);
int set_word(struct Real* r, ssize_t word_idx, word l);

hword get_half_word(struct Real* r, ssize_t hword_idx);
int set_half_word(struct Real* r, ssize_t hword_idx, hword h);


// Generic getters and setters.

ssize_t get_max_word_idx(struct Real* r);
ssize_t get_min_word_idx(struct Real* r);
enum sign_t get_sign(struct Real* r);

void set_max_word_idx(struct Real* r, ssize_t idx);
void set_min_word_idx(struct Real* r, ssize_t idx);
void set_sign(struct Real* r, enum sign_t sign);


// Miscellaneous functions.

// Checks if 2 real numbers are exactly arithmetically equal.
//
// It compares the signs and all words at all valid indices.
int check_equal(struct Real* r1, struct Real* r2);

// These functions trim the unneeded indices and space for `r`;
// they operate in-place.
void trim_most_significant_zeros(struct Real* r);
void trim_least_significant_zeros(struct Real* r);
void trim_zeros(struct Real* r);

void print_real(struct Real* real);

#endif
