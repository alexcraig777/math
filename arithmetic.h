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

struct Real* mul_with_rel_sig(struct Real* r1, struct Real* r2,
                              int num_sig_words);
struct Real* div_with_rel_sig(struct Real* r, word divisor,
                              int num_sig_words);

void negate(struct Real* r);

int greater_abs(struct Real* r1, struct Real* r2);

int is_zero(struct Real* r);

#endif
