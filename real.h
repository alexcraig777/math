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

struct Real* create_real(enum sign_t sign,
                         ssize_t min_word_idx,
                         ssize_t max_word_idx);
struct Real* fill_real(enum sign_t sign,
                       ssize_t min_word_idx,
                       ssize_t max_word_idx,
                       ...);
void allocate_words(struct Real* r);
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

void print_real(struct Real* real);

#endif
