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
 * @file        fixed6.h
 *
 */
#ifndef LX_BASE_MATH_FIXED6_H
#define LX_BASE_MATH_FIXED6_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "fixed16.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// constant
#define LX_FIXED6_ONE                   (64)
#define LX_FIXED6_HALF                  (32)
#define LX_FIXED6_MAX                   (LX_MAXS32)
#define LX_FIXED6_MIN                   (LX_MINS32)
#define LX_FIXED6_NAN                   ((lx_int_t)0x80000000)
#define LX_FIXED6_INF                   (LX_MAXS32)
#define LX_FIXED6_PI                    (0xc9)
#define LX_FIXED6_SQRT2                 (0x5a)
#define LX_FIXED6_NEAR0                 (0)

// conversion
#ifndef lx_fixed6_to_float
#    define lx_fixed6_to_float(x)       (((x) * 0.015625))
#endif
#ifndef lx_float_to_fixed6
#    define lx_float_to_fixed6(x)       ((lx_fixed6_t)((x) * LX_FIXED6_ONE))
#endif

#ifdef LX_DEBUG
#   define lx_int_to_fixed6(x)          lx_long_to_fixed6_check(x)
#   define lx_fixed6_to_int(x)          lx_fixed6_to_long_check(x)

#   define lx_long_to_fixed6(x)         lx_long_to_fixed6_check(x)
#   define lx_fixed6_to_long(x)         lx_fixed6_to_long_check(x)
#else
#   define lx_int_to_fixed6(x)          (lx_fixed6_t)((x) << 6)
#   define lx_fixed6_to_int(x)          (lx_int_t)((x) >> 6)

#   define lx_long_to_fixed6(x)         (lx_fixed6_t)((x) << 6)
#   define lx_fixed6_to_long(x)         (lx_long_t)((x) >> 6)
#endif

#define lx_fixed6_to_fixed16(x)         ((x) << 10)
#define lx_fixed16_to_fixed6(x)         ((x) >> 10)

// round
#define lx_fixed6_round(x)              (((x) + LX_FIXED6_HALF) >> 6)

// ceil
#define lx_fixed6_ceil(x)               (((x) + LX_FIXED6_ONE - 1) >> 6)

// floor
#define lx_fixed6_floor(x)              ((x) >> 6)

// abs
#define lx_fixed6_abs(x)                lx_abs(x)

// avg
#define lx_fixed6_avg(x, y)             (((x) + (y)) >> 1)

// nearly equal?
#define lx_fixed6_near_eq(x, y)         (lx_fixed6_abs((x) - (y)) <= LX_FIXED6_NEAR0)

// mul
#ifndef lx_fixed6_mul
#   define lx_fixed6_mul(x, y)          lx_fixed6_mul_inline(x, y)
#endif

// div
#ifndef lx_fixed6_div
#   define lx_fixed6_div(x, y)          lx_fixed6_div_inline(x, y)
#endif

// imul
#ifndef lx_fixed6_imul
#   define lx_fixed6_imul(x, y)         lx_fixed16_imul(x, y)
#endif

// idiv
#ifndef lx_fixed6_idiv
#   define lx_fixed6_idiv(x, y)         lx_fixed16_idiv(x, y)
#endif

// imuldiv
#ifndef lx_fixed6_imuldiv
#   define lx_fixed6_imuldiv(x, y, z)   lx_fixed16_imuldiv(x, y, z)
#endif

// imulsub
#ifndef lx_fixed6_imulsub
#   define lx_fixed6_imulsub(x, y, z)   lx_fixed16_imulsub(x, y, z)
#endif

// lsh
#ifndef lx_fixed6_lsh
#   define lx_fixed6_lsh(x, y)          lx_fixed16_lsh(x, y)
#endif

// rsh
#ifndef lx_fixed6_rsh
#   define lx_fixed6_rsh(x, y)          lx_fixed16_rsh(x, y)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */

#ifdef LX_DEBUG
static lx_inline lx_fixed6_t lx_long_to_fixed6_check(lx_long_t x) {
    lx_assert(x >= (LX_MINS32 >> 10) && x <= (LX_MAXS32 >> 10));
    return (lx_fixed6_t)(x << 6);
}

static lx_inline lx_long_t lx_fixed6_to_int_check(lx_fixed6_t x) {
    lx_assert(x >= LX_FIXED6_MIN && x <= LX_FIXED6_MAX);
    return (lx_fixed6_t)(x >> 6);
}
#endif
static lx_inline lx_fixed6_t lx_fixed6_mul_inline(lx_fixed6_t x, lx_fixed6_t y) {
    lx_hong_t v = (((lx_hong_t)x * y) >> 6);
    lx_assert(v == (lx_int32_t)v);
    return (lx_fixed16_t)v;
}

static lx_inline lx_fixed16_t lx_fixed6_div_inline(lx_fixed6_t x, lx_fixed6_t y) {
    lx_assert(y);
    // no overflow? compute it fastly
    if (x == (lx_int16_t)x) {
        return (x << 16) / y;
    }
    return lx_fixed16_div(x, y);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif

