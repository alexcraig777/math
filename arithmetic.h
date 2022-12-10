#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include "real.h"

struct Real* add(struct Real* r1, struct Real* r2);
struct Real* subtract(struct Real* r1, struct Real* r2);
struct Real* multiply(struct Real* r1, struct Real* r2);

struct Real* mul_with_sig(struct Real* r1, struct Real* r2,
                          ssize_t min_sig_word_idx);
struct Real* div_with_sig(struct Real* r, word divisor,
                          ssize_t min_sig_word_idx);

void negate(struct Real* r);

char test_equal(struct Real* r1, struct Real* r2);

struct Real* trim_leading_zeros(struct Real* r);

#endif
