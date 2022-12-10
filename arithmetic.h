#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include "real.h"

struct Real* add(struct Real* r1, struct Real* r2);
struct Real* subtract(struct Real* r1, struct Real* r2);
struct Real* multiply(struct Real* r1, struct Real* r2);
struct Real* divide(struct Real* r, word divisor,
                    ssize_t num_extra_words);

void negate(struct Real* r);

char test_equal(struct Real* r1, struct Real* r2);

struct Real* trim_leading_zeros(struct Real* r);

#endif
