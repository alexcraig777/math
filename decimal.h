#ifndef DECIMAL_H
#define DECIMAL_H

#include "real.h"

void print_decimal(struct Real* r);

char* real_to_decimal_str(struct Real* r);

struct Real* decimal_str_to_real(char* decimal_str);

#endif
