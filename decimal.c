#include "decimal.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "real.h"
#include "arithmetic.h"


// Flexible string stuff to make dynamically-handling the decimal string
// easier.
struct String {
    char* buf;
    size_t size;
};

void sprintf_append(struct String* s, char* fmt, ...)
    __attribute__ ((format (printf, 2, 3)));

struct String* create_string(char* initial_string) {
    struct String* s = malloc(sizeof(struct String));
    s->size = strlen(initial_string) + 1;
    s->buf = strdup(initial_string);
    return s;
}

char* free_string(struct String* s) {
    char* rtn = s->buf;
    free(s);
    return rtn;
}

void sprintf_append(struct String* s, char* fmt, ...) {
    va_list ap1, ap2;
    va_start(ap1, fmt);
    va_copy(ap2, ap1);

    // This returns the number of characters that will be written.
    int chars_to_write = vsnprintf(NULL, 0, fmt, ap1);

    if (strlen(s->buf) + chars_to_write + 1 > s->size) {
        s->size = strlen(s->buf) + chars_to_write + 1;
        s->buf = realloc(s->buf, s->size);
        vsnprintf(s->buf + strlen(s->buf), chars_to_write + 1, fmt, ap2);
    }

    va_end(ap1);
    va_end(ap2);
}

// Real to decimal string.

void print_decimal(struct Real* r) {
    char* decimal_str = real_to_decimal_str(r);
    printf("%s\n", decimal_str);
}

char* get_positive_integer_decimal_digits(struct Real* r_int) {
    struct String* rev_s = create_string("");

    r_int = copy_real(r_int);
    struct Real* next_int;
    struct Real* ten = fill_real(POSITIVE, 0, 1, 10);
    struct Real* to_subtract;
    struct Real* digit;

    while (1) {
        next_int = div_with_sig(r_int, 10, 0);
        to_subtract = multiply(next_int, ten);
        digit = subtract(r_int, to_subtract);

        sprintf_append(rev_s, "%ld", get_word(digit, 0));

        free_real(to_subtract);
        free_real(digit);
        free_real(r_int);
        r_int = next_int;

        if (is_zero(r_int) == 1) {
            break;
        }
    }
    free_real(r_int);
    free_real(ten);

    char* rev_chars = free_string(rev_s);

    struct String* rtn_s = create_string("");

    int c_idx;
    for (c_idx = strlen(rev_chars) - 1; c_idx >= 0; c_idx--) {
        sprintf_append(rtn_s, "%c", rev_chars[c_idx]);
    }
    free(rev_chars);

    return free_string(rtn_s);
}

char* get_positive_fractional_decimal_digits(struct Real* r) {
    struct String* s = create_string("");

    r = copy_real(r);
    struct Real* temp;
    struct Real* ten = fill_real(POSITIVE, 0, 1, 10);

    while (1) {
        trim_zeros(r);
        if (is_zero(r)) {
            // We're done.
            break;
        }

        temp = multiply(r, ten);
        free_real(r);
        r = temp;
        sprintf_append(s, "%ld", get_word(r, 0));
        if (get_word(r, 0) != 0) {
            set_word(r, 0, 0);
        }
    }

    free_real(r);
    free_real(ten);

    return free_string(s);
}

char* real_to_decimal_str(struct Real* r) {
    struct String* s = create_string("");

    // First, print out the negative sign, if there is one.
    if (get_sign(r) == NEGATIVE) {
        sprintf_append(s, "-");
    }

    // Get the integer digits.
    struct Real* integer = div_with_sig(r, 1, 0);
    set_sign(integer, POSITIVE);
    char* int_digits = get_positive_integer_decimal_digits(integer);
    sprintf_append(s, "%s", int_digits);

    // Get the fractional digits.
    struct Real* fraction = subtract(r, integer);
    set_sign(fraction, POSITIVE);
    char* frac_digits = get_positive_fractional_decimal_digits(fraction);
    if (strlen(frac_digits) > 0) {
        sprintf_append(s, ".%s", frac_digits);
    }

    free(int_digits);
    free(frac_digits);

    free_real(integer);
    free_real(fraction);

    return free_string(s);
}


// Decimal string to real.

struct Real* decimal_str_to_real(char* decimal_str) {
    enum sign_t sign = POSITIVE;

    struct Real* r = fill_real(POSITIVE, 0, 1, 0);
    //struct Real* temp;
    //struct Real* place_value = fill_real(POSITIVE, 0, 1, 1);
    //struct Real* to_add;

    int fractional = 0;

    int idx;
    for (idx = 0; decimal_str[idx] != 0; idx++) {
        if (idx == 0 && decimal_str[idx] == '-') {
            sign = NEGATIVE;
        } else if (decimal_str[idx] == '.') {
            fractional = 1;
        } else if ('0' <= decimal_str[idx] && decimal_str[idx] <= '9') {
            if (fractional) {
                continue;
            }
        }
    }

    if (sign == NEGATIVE) {
        negate(r);
    }

    return r;
}
