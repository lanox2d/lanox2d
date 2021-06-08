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
 * @file        fixed16.h
 *
 */
#ifndef LX_BASE_MATH_FIXED16_H
#define LX_BASE_MATH_FIXED16_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../libm/libm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// constant
#define LX_FIXED16_ONE                      (1 << 16)
#define LX_FIXED16_HALF                     (1 << 15)
#define LX_FIXED16_MAX                      (LX_MAXS32)
#define LX_FIXED16_MIN                      (LX_MINS32)
#define LX_FIXED16_NAN                      ((lx_int_t)0x80000000)
#define LX_FIXED16_INF                      (LX_MAXS32)
#define LX_FIXED16_PI                       (0x3243f)
#define LX_FIXED16_SQRT2                    (92682)
#define LX_FIXED16_NEAR0                    (1 << 4)

// conversion
#ifndef lx_fixed16_to_float
#    define lx_fixed16_to_float(x)          ((lx_float_t)((x) * 1.5258789e-5))
#endif
#ifndef lx_float_to_fixed16
#    define lx_float_to_fixed16(x)          ((lx_fixed16_t)((x) * LX_FIXED16_ONE))
#endif

#ifdef LX_DEBUG
#   define lx_long_to_fixed16(x)            lx_long_to_fixed16_check(x)
#   define lx_fixed16_to_long(x)            lx_fixed16_to_long_check(x)
#else
#   define lx_long_to_fixed16(x)            (lx_fixed16_t)((x) << 16)
#   define lx_fixed16_to_long(x)            (lx_long_t)((x) >> 16)
#endif

// round
#define lx_fixed16_round(x)                 (((x) + LX_FIXED16_HALF) >> 16)

// ceil
#define lx_fixed16_ceil(x)                  (((x) + LX_FIXED16_ONE - 1) >> 16)

// floor
#define lx_fixed16_floor(x)                 ((x) >> 16)

// abs
#define lx_fixed16_abs(x)                   lx_abs(x)

// avg
#define lx_fixed16_avg(x, y)                (((x) + (y)) >> 1)

// nearly equal?
#define lx_fixed16_near_eq(x, y)            (lx_fixed16_abs((x) - (y)) <= LX_FIXED16_NEAR0)

// mul
#ifndef lx_fixed16_mul
#   ifdef LX_DEBUG
#       define lx_fixed16_mul(x, y)         lx_fixed16_mul_check(x, y)
#   else
#       define lx_fixed16_mul(x, y)         ((lx_fixed16_t)(((lx_hong_t)(x) * (y)) >> 16))
#   endif
#endif

// div
#ifndef lx_fixed16_div
#   ifdef LX_DEBUG
#       define lx_fixed16_div(x, y)         lx_fixed16_div_check(x, y)
#   else
#       define lx_fixed16_div(x, y)         ((lx_fixed16_t)((((lx_hong_t)(x)) << 16) / (y)))
#   endif
#endif

// imul
#ifndef lx_fixed16_imul
#   ifdef LX_DEBUG
#       define lx_fixed16_imul(x, y)        lx_fixed16_imul_check(x, y)
#   else
#       define lx_fixed16_imul(x, y)        ((lx_fixed16_t)((x) * (y)))
#   endif
#endif

// idiv
#ifndef lx_fixed16_idiv
#   ifdef LX_DEBUG
#       define lx_fixed16_idiv(x, y)        lx_fixed16_idiv_check(x, y)
#   else
#       define lx_fixed16_idiv(x, y)        ((lx_fixed16_t)((x) / (y)))
#   endif
#endif

// imuldiv
#ifndef lx_fixed16_imuldiv
#   ifdef LX_DEBUG
#       define lx_fixed16_imuldiv(x, y, z)  lx_fixed16_imuldiv_check(x, y, z)
#   else
#       define lx_fixed16_imuldiv(x, y, z)  ((lx_fixed16_t)(((lx_hong_t)(x) * (y)) / (z)))
#   endif
#endif

// imulsub
#ifndef lx_fixed16_imulsub
#   ifdef LX_DEBUG
#       define lx_fixed16_imulsub(x, y, z)  lx_fixed16_imulsub_check(x, y, z)
#   else
#       define lx_fixed16_imulsub(x, y, z)  ((lx_fixed16_t)(((lx_hong_t)(x) * (y)) - (z)))
#   endif
#endif

// lsh
#ifndef lx_fixed16_lsh
#   define lx_fixed16_lsh(x, y)             ((x) << (y))
#endif

// rsh
#ifndef lx_fixed16_rsh
#   define lx_fixed16_rsh(x, y)             ((x) >> (y))
#endif

// invert: 1 / x
#ifndef lx_fixed16_invert
#   define lx_fixed16_invert(x)             lx_fixed16_div(LX_FIXED16_ONE, x)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline interfaces
 */

static lx_inline lx_fixed16_t lx_long_to_fixed16_check(lx_long_t x) {
    lx_assert(x == (lx_int16_t)x);
    return (lx_fixed16_t)(x << 16);
}

static lx_inline lx_long_t lx_fixed16_to_long_check(lx_fixed16_t x) {
    lx_assert(x >= LX_FIXED16_MIN && x <= LX_FIXED16_MAX);
    return (x >> 16);
}

static lx_inline lx_fixed16_t lx_fixed16_mul_check(lx_fixed16_t x, lx_fixed16_t y) {
    lx_hong_t v = (((lx_hong_t)x * y) >> 16);
    lx_assert(v == (lx_int32_t)v);
    return (lx_fixed16_t)v;
}

static lx_inline lx_fixed16_t lx_fixed16_div_check(lx_fixed16_t x, lx_fixed16_t y) {
    lx_assert(y);
    lx_hong_t v = ((((lx_hong_t)x) << 16) / y);
    lx_assert(v == (lx_int32_t)v);
    return (lx_fixed16_t)v;
}

static lx_inline lx_fixed16_t lx_fixed16_sqre_check(lx_fixed16_t x) {
    lx_hong_t v = (((lx_hong_t)x * x) >> 16);
    lx_assert(v == (lx_int32_t)v);
    return (lx_fixed16_t)v;
}

static lx_inline lx_fixed16_t lx_fixed16_imul_check(lx_fixed16_t x, lx_long_t y) {
    lx_hong_t v = ((lx_hong_t)x * y);
    lx_assert(v == (lx_int32_t)v);
    return (lx_fixed16_t)v;
}

static lx_inline lx_fixed16_t lx_fixed16_idiv_check(lx_fixed16_t x, lx_long_t y) {
    lx_assert(y);
    return (lx_fixed16_t)(x / y);
}

static lx_inline lx_fixed16_t lx_fixed16_imuldiv_check(lx_fixed16_t x, lx_long_t y, lx_long_t z) {
    lx_hong_t v = ((lx_hong_t)x * y) / z;
    lx_assert(v == (lx_int32_t)v);
    return (lx_fixed16_t)v;
}

static lx_inline lx_fixed16_t lx_fixed16_imulsub_check(lx_fixed16_t x, lx_long_t y, lx_long_t z) {
    lx_hong_t v = ((lx_hong_t)x * y) - z;
    lx_assert(v == (lx_int32_t)v);
    return (lx_fixed16_t)v;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif

