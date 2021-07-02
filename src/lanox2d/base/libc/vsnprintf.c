/*!A lightweight and fast 2D vector graphics engine
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2021-present, Lanox2D Open Source Group.
 *
 * @author      ruki
 * @file        vsnprintf.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "libc.h"
#include "../libm/libm.h"
#include <stdio.h>
#include <stdarg.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the printf type
typedef enum lx_printf_type_t_ {
    LX_PRINTF_TYPE_NONE             = 0
,   LX_PRINTF_TYPE_INT              = 1
,   LX_PRINTF_TYPE_CHAR             = 2
,   LX_PRINTF_TYPE_CHAR_PERCENT     = 3
,   LX_PRINTF_TYPE_FLOAT            = 4
,   LX_PRINTF_TYPE_DOUBLE           = 5
,   LX_PRINTF_TYPE_STRING           = 6
,   LX_PRINTF_TYPE_WIDTH            = 7
,   LX_PRINTF_TYPE_PRECISION        = 8
,   LX_PRINTF_TYPE_OBJECT           = 9
,   LX_PRINTF_TYPE_INVALID          = 10
}lx_printf_type_t;

// the printf extra info
typedef enum lx_printf_extra_t_ {
    LX_PRINTF_EXTRA_NONE            = 0
,   LX_PRINTF_EXTRA_SIGNED          = 1     // signed integer for %d %i
,   LX_PRINTF_EXTRA_UPPER           = 2     // upper case for %X %B
,   LX_PRINTF_EXTRA_PERCENT         = 4     // percent char: %
,   LX_PRINTF_EXTRA_EXP             = 8     // exponent form: [-]d.ddd e[+/-]ddd
}lx_printf_extra_t;

// printf length qualifier
typedef enum lx_printf_qual_t_ {
    LX_PRINTF_QUAL_NONE             = 0
,   LX_PRINTF_QUAL_H                = 1
,   LX_PRINTF_QUAL_L                = 2
,   LX_PRINTF_QUAL_LL               = 3
,   LX_PRINTF_QUAL_I8               = 4
,   LX_PRINTF_QUAL_I16              = 5
,   LX_PRINTF_QUAL_I32              = 6
,   LX_PRINTF_QUAL_I64              = 7
}lx_printf_qual_t;

// printf flag type
typedef enum lx_printf_flag_t {
    LX_PRINTF_FLAG_NONE             = 0
,   LX_PRINTF_FLAG_PLUS             = 1     // +: denote the sign '+' or '-' of a number
,   LX_PRINTF_FLAG_LEFT             = 2     // -: left-justified
,   LX_PRINTF_FLAG_ZERO             = 4     // 0: fill 0 instead of spaces
,   LX_PRINTF_FLAG_PFIX             = 8     // #: add prefix
}lx_printf_flag_t;

// printf entry
typedef struct lx_printf_entry_t_ {

    // format type
    lx_uint8_t          type;

    // extra info
    lx_uint8_t          extra;

    // flags
    lx_uint8_t          flags;

    // qualifier
    lx_uint8_t          qual;

    // field width
    lx_int_t            width;

    // precision
    lx_int_t            precision;

    // base: 2 8 10 16
    lx_int_t            base;

    // the object name
    lx_char_t           object[32];

}lx_printf_entry_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
lx_extern_c_enter
lx_int_t lx_vsnprintf_object(lx_char_t* s, lx_size_t n, lx_char_t const* name, lx_cpointer_t object);
lx_extern_c_leave

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static lx_int_t lx_skip_atoi(lx_char_t const** s) {
    lx_int_t i = 0;
    while (lx_isdigit(**s)) {
        i = i * 10 + *((*s)++) - '0';
    }
    return i;
}

static lx_char_t* lx_printf_object(lx_char_t* pb, lx_char_t* pe, lx_printf_entry_t e, lx_cpointer_t object) {
    lx_int_t size = lx_vsnprintf_object(pb, pe - pb, e.object, object);
    if (size >= 0) {
        pb += size;
    } else {
        if (pb < pe) *pb++ = 'i';
        if (pb < pe) *pb++ = 'n';
        if (pb < pe) *pb++ = 'v';
        if (pb < pe) *pb++ = 'a';
        if (pb < pe) *pb++ = 'l';
        if (pb < pe) *pb++ = 'i';
        if (pb < pe) *pb++ = 'd';
    }
    return pb;
}

static lx_char_t* lx_printf_string(lx_char_t* pb, lx_char_t* pe, lx_printf_entry_t e, lx_char_t const* s) {
    if (s) {

        // fill space at left side, e.g. "   abcd"
        lx_int_t n = (lx_int_t)lx_strnlen(s, e.precision);
        if (!(e.flags & LX_PRINTF_FLAG_LEFT)) {
            while (n < e.width--) {
                if (pb < pe) *pb++ = ' ';
            }
        }

        // copy string
        lx_int_t i = 0;
        for (i = 0; i < n; ++i) {
            if (pb < pe) *pb++ = *s++;
        }

        // fill space at right side, e.g. "abcd    "
        while (n < e.width--) {
            if (pb < pe) *pb++ = ' ';
        }
    } else {
        if (pb < pe) *pb++ = 'n';
        if (pb < pe) *pb++ = 'u';
        if (pb < pe) *pb++ = 'l';
        if (pb < pe) *pb++ = 'l';
    }
    return pb;
}

static lx_char_t* lx_printf_int64(lx_char_t* pb, lx_char_t* pe, lx_printf_entry_t e, lx_uint64_t num) {
    static lx_char_t const* digits_table = "0123456789ABCDEF";

    // max: 64-bit binary decimal
    lx_char_t   digits[64] = {0};
    lx_int_t    digit_i = 0;

    // lowercase mask, e.g. 'F' | 0x20 => 'f'
    lx_int_t lomask = (e.extra & LX_PRINTF_EXTRA_UPPER)? 0x0 : 0x20;

    // sign: + -
    lx_char_t sign = 0;
    if (e.extra & LX_PRINTF_EXTRA_SIGNED) {
        if ((lx_int64_t)num < 0) {
            sign = '-';
            --e.width;
            num = (lx_uint64_t)(-(lx_int64_t)num);
        } else if (e.flags & LX_PRINTF_FLAG_PLUS) {
            sign = '+';
            --e.width;
        }
    }

    // convert num => digits string in reverse order
    if (num == 0) {
        digits[digit_i++] = '0';
    } else {
#ifdef LX_CONFIG_SMALL
        do
        {
            digits[digit_i++] = digits_table[num % e.base] | lomask;
            num /= e.base;
        }
        while (num);
#else
        if (e.base != 10) {
            lx_int_t shift_bits = 4;
            if (e.base == 8) shift_bits--;
            else if (e.base == 2) shift_bits -= 3;
            do {
                digits[digit_i++] = digits_table[(lx_uint8_t)num & (e.base - 1)] | lomask;
                num >>= shift_bits;
            }
            while (num);
        } else {
            do {
                digits[digit_i++] = digits_table[num % e.base] | lomask;
                num /= e.base;
            }
            while (num);
        }
#endif
    }

    // adjust precision
    if (digit_i > e.precision) {
        e.precision = digit_i;
    }

    // fill spaces at left side, e.g. "   0x0"
    e.width -= e.precision;
    if (!(e.flags & (LX_PRINTF_FLAG_LEFT + LX_PRINTF_FLAG_ZERO))) {
        while (--e.width >= 0) {
            if (pb < pe) *pb++ = ' ';
        }
    }

    // append sign: + / -
    if (sign && (pb < pe)) *pb++ = sign;

    // append prefix: 0x..., 0X..., 0b..., 0B...
    if (e.flags & LX_PRINTF_FLAG_PFIX) {
        switch (e.base) {
        case 16: {
            if (pb + 1 < pe) {
                *pb++ = '0';
                *pb++ = 'X' | lomask;
                e.width -= 2;
            }
            break;
        }
        case 8: {
            if (pb < pe) {
                *pb++ = '0';
                --e.width;
            }
            break;
        }
        case 2: {
            if (pb + 1 < pe) {
                *pb++ = '0';
                *pb++ = 'B' | lomask;
                e.width -= 2;
            }
            break;
        }
        default:
            break;
        }
    }

    // fill 0 or spaces, e.g. "0x   ff"
    if (!(e.flags & LX_PRINTF_FLAG_LEFT)) {
        lx_char_t c = (e.flags & LX_PRINTF_FLAG_ZERO)? '0' : ' ';
        while (--e.width >= 0) {
            if (pb < pe) *pb++ = c;
        }
    }

    // fill 0 if precision is larger, e.g. "0x000ff"
    while (digit_i <= --e.precision) {
        if (pb < pe) *pb++ = '0';
    }

    // append digits
    while (--digit_i >= 0) {
        if (pb < pe) *pb++ = digits[digit_i];
    }

    // trailing space padding for left-justified flags, e.g. "0xff   "
    while (--e.width >= 0) {
        if (pb < pe) *pb++ = ' ';
    }
    return pb;
}

static lx_char_t* lx_printf_int32(lx_char_t* pb, lx_char_t* pe, lx_printf_entry_t e, lx_uint32_t num) {
    static lx_char_t const* digits_table = "0123456789ABCDEF";

    // max: 64-bit binary decimal
    lx_char_t   digits[64] = {0};
    lx_int_t    digit_i = 0;

    // lowercase mask, e.g. 'F' | 0x20 => 'f'
    lx_int_t lomask = (e.extra & LX_PRINTF_EXTRA_UPPER)? 0x0 : 0x20;

    // sign: + -
    lx_char_t sign = 0;
    if (e.extra & LX_PRINTF_EXTRA_SIGNED) {
        if ((lx_int32_t)num < 0) {
            sign = '-';
            --e.width;
            num = (lx_uint32_t)(-(lx_int32_t)num);
        } else if (e.flags & LX_PRINTF_FLAG_PLUS) {
            sign = '+';
            --e.width;
        }
    }

    // convert num => digits string in reverse order
    if (num == 0) {
        digits[digit_i++] = '0';
    } else {
#ifdef LX_CONFIG_SMALL
        do {
            digits[digit_i++] = digits_table[num % e.base] | lomask;
            num /= e.base;
        }
        while (num);
#else
        if (e.base != 10) {
            lx_int_t shift_bits = 4;
            if (e.base == 8) shift_bits--;
            else if (e.base == 2) shift_bits -= 3;
            do {
                digits[digit_i++] = digits_table[(lx_uint8_t)num & (e.base - 1)] | lomask;
                num >>= shift_bits;
            }
            while (num);
        } else {
            do {
                digits[digit_i++] = digits_table[num % e.base] | lomask;
                num /= e.base;
            }
            while (num);
        }
#endif
    }

    // adjust precision
    if (digit_i > e.precision) {
        e.precision = digit_i;
    }

    // fill spaces at left side, e.g. "   0x0"
    e.width -= e.precision;
    if (!(e.flags & (LX_PRINTF_FLAG_LEFT + LX_PRINTF_FLAG_ZERO))) {
        while (--e.width >= 0) {
            if (pb < pe) *pb++ = ' ';
        }
    }

    // append sign: + / -
    if (sign && (pb < pe)) *pb++ = sign;

    // append prefix: 0x..., 0X..., 0b..., 0B...
    if (e.flags & LX_PRINTF_FLAG_PFIX) {
        switch (e.base) {
        case 16: {
            if (pb + 1 < pe) {
                *pb++ = '0';
                *pb++ = 'X' | lomask;
                e.width -= 2;
            }
            break;
        }
        case 8: {
            if (pb < pe) {
                *pb++ = '0';
                --e.width;
            }
            break;
        }
        case 2: {
            if (pb + 1 < pe) {
                *pb++ = '0';
                *pb++ = 'B' | lomask;
                e.width -= 2;
            }
            break;
        }
        default:
            break;
        }
    }

    // fill 0 or spaces, e.g. "0x   ff"
    if (!(e.flags & LX_PRINTF_FLAG_LEFT)) {
        lx_char_t c = (e.flags & LX_PRINTF_FLAG_ZERO)? '0' : ' ';
        while (--e.width >= 0) {
            if (pb < pe) *pb++ = c;
        }
    }

    // fill 0 if precision is larger, e.g. "0x000ff"
    while (digit_i <= --e.precision) {
        if (pb < pe) *pb++ = '0';
    }

    // append digits
    while (--digit_i >= 0) {
        if (pb < pe) *pb++ = digits[digit_i];
    }

    // trailing space padding for left-justified flags, e.g. "0xff   "
    while (--e.width >= 0) {
        if (pb < pe) *pb++ = ' ';
    }
    return pb;
}

static lx_char_t* lx_printf_float(lx_char_t* pb, lx_char_t* pe, lx_printf_entry_t e, lx_float_t num) {
    lx_char_t   ints[32] = {0};
    lx_char_t   decs[32] = {0};
    lx_int_t    ints_i = 0, decs_i = 0;

    // for inf nan
    if (lx_isinff(num)) {
        if (pb < pe && num < 0) *pb++ = '-';
        if (pb < pe) *pb++ = (e.extra & LX_PRINTF_EXTRA_UPPER)? 'I' : 'i';
        if (pb < pe) *pb++ = (e.extra & LX_PRINTF_EXTRA_UPPER)? 'N' : 'n';
        if (pb < pe) *pb++ = (e.extra & LX_PRINTF_EXTRA_UPPER)? 'F' : 'f';
        return pb;
    } else if (lx_isnanf(num)) {
        if (pb < pe) *pb++ = (e.extra & LX_PRINTF_EXTRA_UPPER)? 'N' : 'n';
        if (pb < pe) *pb++ = (e.extra & LX_PRINTF_EXTRA_UPPER)? 'A' : 'a';
        if (pb < pe) *pb++ = (e.extra & LX_PRINTF_EXTRA_UPPER)? 'N' : 'n';
        return pb;
    }

    // sign: + -
    lx_char_t sign = 0;
    if (e.extra & LX_PRINTF_EXTRA_SIGNED) {
        if (num < 0) {
            sign = '-';
            --e.width;
        } else if (e.flags & LX_PRINTF_FLAG_PLUS) {
            sign = '+';
            --e.width;
        }
    }

    // adjust sign
    if (num < 0) num = -num;

    // default precision: 6
    if (e.precision <= 0) e.precision = 6;

    // round? i.dddddddd5 => i.ddddddde
    lx_uint32_t p = 1;
    lx_size_t   n = e.precision;
    while (n--) p *= 10;
    if (((lx_uint32_t)(num * p * 10) % 10) > 4) {
        num += 1.0f / (lx_float_t)p;
    }

    // get integer & decimal
    lx_int32_t integer = (lx_int32_t)num;
    lx_float_t decimal = num - integer;

    // convert integer => digits string in reverse order
    if (integer == 0) {
        ints[ints_i++] = '0';
    } else {
        if (integer < 0) integer = -integer;
        do {
            ints[ints_i++] = (integer % 10) + '0';
            integer /= 10;
        }
        while (integer);
    }

    // convert decimal => digits string in positive order
    if (decimal == 0) {
        decs[decs_i++] = '0';
    } else {
        lx_long_t d = (lx_long_t)(decimal * 10);
        do {
            decs[decs_i++] = (lx_char_t)(d + (lx_long_t)'0');
            decimal = decimal * 10 - d;
            d = (lx_long_t)(decimal * 10);
        }
        while (decs_i < e.precision);
    }

    // adjust precision
    if (decs_i > e.precision) {
        decs_i = e.precision;
    }

    // fill spaces at left side, e.g. "   0.31415926"
    e.width -= (lx_int_t)(ints_i + 1 + e.precision);
    if (!(e.flags & (LX_PRINTF_FLAG_LEFT + LX_PRINTF_FLAG_ZERO))) {
        while (--e.width >= 0) {
            if (pb < pe) *pb++ = ' ';
        }
    }

    // append sign: + / -
    if (sign && (pb < pe)) *pb++ = sign;

    // fill 0 or spaces, e.g. "00003.1415926"
    if (!(e.flags & LX_PRINTF_FLAG_LEFT)) {
        lx_char_t c = (e.flags & LX_PRINTF_FLAG_ZERO)? '0' : ' ';
        while (--e.width >= 0) {
            if (pb < pe) *pb++ = c;
        }
    }

    // append integer
    while (--ints_i >= 0) {
        if (pb < pe) *pb++ = ints[ints_i];
    }

    // append .
    if (pb < pe) *pb++ = '.';

    // append decimal
    lx_int_t decs_n = decs_i;
    while (--decs_i >= 0) {
        if (pb < pe) *pb++ = decs[decs_n - decs_i - 1];
    }

    // fill 0 if precision is larger, e.g. "0.3140000"
    while (decs_n <= --e.precision) {
        if (pb < pe) *pb++ = '0';
    }

    // trailing space padding for left-justified flags, e.g. "0.31415926   "
    while (--e.width >= 0) {
        if (pb < pe) *pb++ = ' ';
    }
    return pb;
}

static lx_char_t* lx_printf_double(lx_char_t* pb, lx_char_t* pe, lx_printf_entry_t e, lx_double_t num) {
    lx_char_t   ints[64] = {0};
    lx_char_t   decs[64] = {0};
    lx_int_t    ints_i = 0, decs_i = 0;

    // for inf nan
    if (lx_isinf(num)) {
        if (pb < pe && num < 0) *pb++ = '-';
        if (pb < pe) *pb++ = (e.extra & LX_PRINTF_EXTRA_UPPER)? 'I' : 'i';
        if (pb < pe) *pb++ = (e.extra & LX_PRINTF_EXTRA_UPPER)? 'N' : 'n';
        if (pb < pe) *pb++ = (e.extra & LX_PRINTF_EXTRA_UPPER)? 'F' : 'f';
        return pb;
    } else if (lx_isnan(num)) {
        if (pb < pe) *pb++ = (e.extra & LX_PRINTF_EXTRA_UPPER)? 'N' : 'n';
        if (pb < pe) *pb++ = (e.extra & LX_PRINTF_EXTRA_UPPER)? 'A' : 'a';
        if (pb < pe) *pb++ = (e.extra & LX_PRINTF_EXTRA_UPPER)? 'N' : 'n';
        return pb;
    }

    // sign: + -
    lx_char_t sign = 0;
    if (e.extra & LX_PRINTF_EXTRA_SIGNED) {
        if (num < 0) {
            sign = '-';
            --e.width;
        } else if (e.flags & LX_PRINTF_FLAG_PLUS) {
            sign = '+';
            --e.width;
        }
    }

    // adjust sign
    if (num < 0) num = -num;

    // default precision: 6
    if (e.precision <= 0) e.precision = 6;

    // round? i.dddddddd5 => i.ddddddde
    lx_uint64_t p = 1;
    lx_size_t   n = e.precision;
    while (n--) p *= 10;
    if (((lx_uint64_t)(num * p * 10) % 10) > 4) {
        num += 1. / (lx_double_t)p;
    }

    // get integer & decimal
    lx_int64_t integer = (lx_int64_t)num;
    lx_double_t decimal = num - integer;

    // convert integer => digits string in reverse order
    if (integer == 0) {
        ints[ints_i++] = '0';
    } else {
        if (integer < 0) integer = -integer;
        do {
            ints[ints_i++] = (integer % 10) + '0';
            integer /= 10;
        }
        while (integer);
    }

    // convert decimal => digits string in positive order
    if (decimal == 0) {
        decs[decs_i++] = '0';
    } else {
        lx_long_t d = (lx_long_t)(decimal * 10);
        do {
            decs[decs_i++] = (lx_char_t)(d + (lx_long_t)'0');
            decimal = decimal * 10 - d;
            d = (lx_long_t)(decimal * 10);
        }
        while (decs_i < e.precision);
    }

    // adjust precision
    if (decs_i > e.precision) {
        decs_i = e.precision;
    }

    // fill spaces at left side, e.g. "   0.31415926"
    e.width -= (lx_int_t)(ints_i + 1 + e.precision);
    if (!(e.flags & (LX_PRINTF_FLAG_LEFT + LX_PRINTF_FLAG_ZERO))) {
        while (--e.width >= 0) {
            if (pb < pe) *pb++ = ' ';
        }
    }

    // append sign: + / -
    if (sign && (pb < pe)) *pb++ = sign;

    // fill 0 or spaces, e.g. "00003.1415926"
    if (!(e.flags & LX_PRINTF_FLAG_LEFT)) {
        lx_char_t c = (e.flags & LX_PRINTF_FLAG_ZERO)? '0' : ' ';
        while (--e.width >= 0) {
            if (pb < pe) *pb++ = c;
        }
    }

    // append integer
    while (--ints_i >= 0) {
        if (pb < pe) *pb++ = ints[ints_i];
    }

    // append .
    if (pb < pe) *pb++ = '.';

    // append decimal
    lx_int_t decs_n = decs_i;
    while (--decs_i >= 0) {
        if (pb < pe) *pb++ = decs[decs_n - decs_i - 1];
    }

    // fill 0 if precision is larger, e.g. "0.3140000"
    while (decs_n <= --e.precision) {
        if (pb < pe) *pb++ = '0';
    }

    // trailing space padding for left-justified flags, e.g. "0.31415926   "
    while (--e.width >= 0) {
        if (pb < pe) *pb++ = ' ';
    }
    return pb;
}

// get a printf format entry
static lx_int_t lx_printf_entry(lx_char_t const* fmt, lx_printf_entry_t* e) {
    // get field width for *
    lx_char_t const* p = fmt;
    if (e->type == LX_PRINTF_TYPE_WIDTH) {
        if (e->width < 0) {
            e->width = -e->width;
            e->flags |= LX_PRINTF_FLAG_LEFT;
        }
        e->type = LX_PRINTF_TYPE_NONE;
        goto get_precision;
    }

    // get precision for *
    if (e->type == LX_PRINTF_TYPE_PRECISION) {
        if (e->precision < 0) e->precision = 0;
        e->type = LX_PRINTF_TYPE_NONE;
        goto get_qualifier;
    }

    // default type
    e->type = LX_PRINTF_TYPE_NONE;

    // goto %
    for (; *p; ++p) {
        if (*p == '%') break;
    }

    // return non-format string
    if (p != fmt || !*p) {
        return (lx_int_t)(p - fmt);
    }

    // skip %
    ++p;

    // get flags
    e->flags = LX_PRINTF_FLAG_NONE;
    while (1) {
        lx_bool_t is_found = lx_true;
        switch (*p) {
        case '+': e->flags |= LX_PRINTF_FLAG_PLUS; break;
        case '-': e->flags |= LX_PRINTF_FLAG_LEFT; break;
        case '0': e->flags |= LX_PRINTF_FLAG_ZERO; break;
        case '#': e->flags |= LX_PRINTF_FLAG_PFIX; break;
        default: is_found = lx_false; break;
        }
        if (is_found == lx_false) break;
        else ++p;
    }

    // get field width
    e->width = -1;
    if (lx_isdigit(*p)) {
        e->width = lx_skip_atoi(&p);
    } else if (*p == '*') {
        // it's the next argument
        e->type = LX_PRINTF_TYPE_WIDTH;
        return (lx_int_t)(++p - fmt);
    }

get_precision:
    // get precision
    e->precision = -1;
    if (*p == '.') {
        ++p;
        if (lx_isdigit(*p)) {
            e->precision = lx_skip_atoi(&p);
            if (e->precision < 0) e->precision = 0;
        } else if (*p == '*') {
            // it's the next argument
            e->type = LX_PRINTF_TYPE_PRECISION;
            return (lx_int_t)(++p - fmt);
        }
    }

get_qualifier:
    // get length qualifier
    e->qual = LX_PRINTF_QUAL_NONE;
    switch (*p) {
        // short & long => int
    case 'h':
        e->qual = LX_PRINTF_QUAL_H;
        ++p;
        break;
    case 'l':
        e->qual = LX_PRINTF_QUAL_L;
        ++p;
        if (*p == 'l') {
            e->qual = LX_PRINTF_QUAL_LL;
            ++p;
        }
        break;
    case 'I': {
        ++p;
        lx_int_t n = lx_skip_atoi(&p);
        switch (n) {
        case 8: e->qual = LX_PRINTF_QUAL_I8; break;
        case 16: e->qual = LX_PRINTF_QUAL_I16; break;
        case 32: e->qual = LX_PRINTF_QUAL_I32; break;
        case 64: e->qual = LX_PRINTF_QUAL_I64; break;
        default: break;
        }
        break;
    }
    case 'z': {
        switch (sizeof(lx_size_t)) {
        case 1: e->qual = LX_PRINTF_QUAL_I8; break;
        case 2: e->qual = LX_PRINTF_QUAL_I16; break;
        case 4: e->qual = LX_PRINTF_QUAL_I32; break;
        case 8: e->qual = LX_PRINTF_QUAL_I64; break;
        default: break;
        }
        ++p;
        break;
    }
    default:
        e->qual = LX_PRINTF_QUAL_NONE;
        break;
    }

    // get base & type
    e->base = -1;
    e->type = LX_PRINTF_TYPE_INVALID;
    e->extra = LX_PRINTF_EXTRA_NONE;
    switch (*p) {
    case 's':
        e->type = LX_PRINTF_TYPE_STRING;
        return (lx_int_t)(++p - fmt);
    case '%':
        e->extra |= LX_PRINTF_EXTRA_PERCENT;
    case 'c':
        e->type = LX_PRINTF_TYPE_CHAR;
        return (lx_int_t)(++p - fmt);
    case 'd':
    case 'i':
        e->extra |= LX_PRINTF_EXTRA_SIGNED;
    case 'u':
        e->base = 10;
        e->type = LX_PRINTF_TYPE_INT;
        break;
    case 'X':
        e->extra |= LX_PRINTF_EXTRA_UPPER;
    case 'x':
        e->base = 16;
        e->type = LX_PRINTF_TYPE_INT;
        break;
    case 'P':
        e->extra |= LX_PRINTF_EXTRA_UPPER;
    case 'p':
        e->base = 16;
        e->type = LX_PRINTF_TYPE_INT;
        e->flags |= LX_PRINTF_FLAG_PFIX;
#if LX_CPU_BITSIZE == 64
        e->qual = LX_PRINTF_QUAL_I64;
#endif
        break;
    case 'o':
        e->base = 8;
        e->type = LX_PRINTF_TYPE_INT;
        break;
    case 'B':
        e->extra |= LX_PRINTF_EXTRA_UPPER;
    case 'b':
        e->base = 2;
        e->type = LX_PRINTF_TYPE_INT;
        break;
    case 'F':
        e->extra |= LX_PRINTF_EXTRA_UPPER;
    case 'f':
        e->type = LX_PRINTF_TYPE_FLOAT;
        e->extra |= LX_PRINTF_EXTRA_SIGNED;
        break;
    case 'E':
        e->extra |= LX_PRINTF_EXTRA_UPPER;
    case 'e':
        e->type = LX_PRINTF_TYPE_FLOAT;
        e->extra |= LX_PRINTF_EXTRA_SIGNED;
        e->extra |= LX_PRINTF_EXTRA_EXP;
        break;
    case '{': {
        // get the object name
        ++p;
        lx_size_t indx = 0;
        lx_size_t maxn = lx_arrayn(e->object);
        while (*p && *p != '}' && indx < maxn - 1) e->object[indx++] = *p++;
        e->object[indx] = '\0';

        // save the object type
        e->type = *p == '}'? LX_PRINTF_TYPE_OBJECT : LX_PRINTF_TYPE_INVALID;
        break;
    }
    default:
        e->type = LX_PRINTF_TYPE_INVALID;
        return (lx_int_t)(p - fmt);
    }
    return (lx_int_t)(++p - fmt);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_int_t lx_vsnprintf(lx_char_t* s, lx_size_t n, lx_char_t const* fmt, va_list args) {
    if (!n || !s || !fmt) return 0;

    // init start and end pointer
    lx_char_t* pb = s;
    lx_char_t* pe = s + n - 1;

    // parse format
    lx_printf_entry_t e = {0};
    lx_int_t en = 0;
    while (*fmt) {
        lx_char_t const* ofmt = fmt;

        // get an entry
        en = lx_printf_entry(fmt, &e);
        fmt += en;

        switch (e.type) {
            // copy it if none type
        case LX_PRINTF_TYPE_NONE: {
                lx_int_t copy_n = en;
                if (pb < pe)  {
                    if (copy_n > pe - pb) copy_n = (lx_int_t)(pe - pb);
                    lx_memcpy(pb, ofmt, copy_n);
                    pb += copy_n;
                }
                break;
            }
            // get a character for %c
        case LX_PRINTF_TYPE_CHAR: {
                // char: %
                if (e.extra & LX_PRINTF_EXTRA_PERCENT) {
                    if (pb < pe) *pb++ = '%';
                } else { // char: %c
                    // fill space at left side, e.g. "   a"
                    if (!(e.flags & LX_PRINTF_FLAG_LEFT)) {
                        while (--e.width > 0) {
                            if (pb < pe) *pb++ = ' ';
                        }
                    }

                    if (pb < pe) *pb++ = (lx_char_t)va_arg(args, lx_int_t);

                    // fill space at right side, e.g. "a   "
                    while (--e.width > 0) {
                        if (pb < pe) *pb++ = ' ';
                    }
                }
                break;
            }
            // get field width for *
        case LX_PRINTF_TYPE_WIDTH:
            e.width = va_arg(args, lx_int_t);
            break;
            // get precision for *
        case LX_PRINTF_TYPE_PRECISION:
            e.precision = va_arg(args, lx_int_t);
            break;
            // get string for %s
        case LX_PRINTF_TYPE_STRING: {
                pb = lx_printf_string(pb, pe, e, va_arg(args, lx_char_t const*));
                break;
            }
            // get an integer for %d %u %x ...
        case LX_PRINTF_TYPE_INT: {
                if (    e.qual == LX_PRINTF_QUAL_I64
#if LX_CPU_BIT64
                    ||  e.qual == LX_PRINTF_QUAL_L
#endif
                    ||  e.qual == LX_PRINTF_QUAL_LL) {
                    pb = lx_printf_int64(pb, pe, e, va_arg(args, lx_uint64_t));
                } else {
                    lx_uint32_t num = 0;
                    if (e.extra & LX_PRINTF_EXTRA_SIGNED) {
                        switch (e.qual) {
                        case LX_PRINTF_QUAL_I8:     num = (lx_int8_t)va_arg(args, lx_int_t); break;
                        case LX_PRINTF_QUAL_I16:    num = (lx_int16_t)va_arg(args, lx_int_t); break;
                        case LX_PRINTF_QUAL_I32:    num = (lx_int32_t)va_arg(args, lx_int32_t); break;
                        default:                    num = va_arg(args, lx_int_t); break;
                        }
                    } else {
                        switch (e.qual) {
                        case LX_PRINTF_QUAL_I8:     num = (lx_uint8_t)va_arg(args, lx_uint_t); break;
                        case LX_PRINTF_QUAL_I16:    num = (lx_uint16_t)va_arg(args, lx_uint_t); break;
                        case LX_PRINTF_QUAL_I32:    num = va_arg(args, lx_uint32_t); break;
                        default:                    num = va_arg(args, lx_uint_t); break;
                        }
                    }
                    pb = lx_printf_int32(pb, pe, e, num);
                }
                break;
            }
        case LX_PRINTF_TYPE_FLOAT: {
                // double?
                if (e.qual == LX_PRINTF_QUAL_L) {
                    lx_double_t num = va_arg(args, lx_double_t);
                    pb = lx_printf_double(pb, pe, e, num);
                } else { // float?
                    lx_float_t num = (lx_float_t)va_arg(args, lx_double_t);
                    pb = lx_printf_float(pb, pe, e, num);
                }
                break;
            }
            // get object for %{object_name}
        case LX_PRINTF_TYPE_OBJECT: {
                pb = lx_printf_object(pb, pe, e, va_arg(args, lx_cpointer_t));
                break;
            }
        case LX_PRINTF_TYPE_INVALID: {
                if (pb < pe) *pb++ = '%';
                break;
            }
        default:
            break;
        }
    }

    // end
    if (pb < s + n) *pb = '\0';

    // the trailing null byte doesn't count towards the total
    return (lx_int_t)(pb - s);
}

