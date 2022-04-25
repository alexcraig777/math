#ifndef REAL_H
#define REAL_H

#include <sys/types.h>
#include <stdint.h>
#include <stdarg.h>

typedef uint64_t word;
typedef uint32_t hword;

#define MAX_WORD ((word) (long) -1)
#define MAX_HWORD ((hword) (long) -1)

enum sign_t {POSITIVE, NEGATIVE};

struct Real {
    enum sign_t sign;
    
    ssize_t min_word_idx;
    ssize_t max_word_idx;

    word* words;
};

struct Real* make_real(enum sign_t sign,
                       ssize_t min_word_idx,
                       ssize_t max_word_idx,
                       ...);
void free_real(struct Real* r);

void print_real(struct Real* real);

struct Real* trim_leading_zeros(struct Real* r);

struct Real* add(struct Real* r1, struct Real* r2);
struct Real* subtract(struct Real* r1, struct Real* r2);
struct Real* multiply(struct Real* r1, struct Real* r2);
struct Real* divide(struct Real* r, word divisor,
		    ssize_t num_extra_words);

void negate(struct Real* r);

char test_equal(struct Real* r1, struct Real* r2);

#endif
