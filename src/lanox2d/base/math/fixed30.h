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
 * @file        fixed30.h
 *
 */
#ifndef LX_BASE_MATH_FIXED30_H
#define LX_BASE_MATH_FIXED30_H

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
#define LX_FIXED30_ONE                      (1 << 30)
#define LX_FIXED30_HALF                     (1 << 29)
#define LX_FIXED30_MAX                      (LX_MAXS32)
#define LX_FIXED30_MIN                      (-LX_FIXED30_MAX)
#define LX_FIXED30_NAN                      ((lx_int_t)0x80000000)
#define LX_FIXED30_INF                      (LX_MAXS32)
#define LX_FIXED30_SQRT2                    (0x5a827999)

// conversion
#ifndef lx_fixed30_to_float
#    define lx_fixed30_to_float(x)          (((x) * 0.00000000093132257f))
#endif
#ifndef lx_float_to_fixed30
#    ifdef LX_DEBUG
#        define lx_float_to_fixed30(x)      lx_float_to_fixed30_check(x)
#    else
#        define lx_float_to_fixed30(x)      ((lx_fixed30_t)((x) * LX_FIXED30_ONE))
#    endif
#endif

#ifdef LX_DEBUG
#   define lx_fixed16_to_fixed30(x)         lx_fixed16_to_fixed30_check(x)
#else
#   define lx_fixed16_to_fixed30(x)         ((x) << 14)
#endif
#define lx_fixed30_to_fixed16(x)            ((x) >> 14)

// abs
#define lx_fixed30_abs(x)                   lx_abs(x)

// avg
#define lx_fixed30_avg(x, y)                (((x) + (y)) >> 1)

// mul
#ifndef lx_fixed30_mul
#   define lx_fixed30_mul(x, y)             lx_fixed30_mul_int64(x, y)
#endif

// div
#ifndef lx_fixed30_div
#   define lx_fixed30_div(x, y)             lx_fixed30_div_int64(x, y)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */

#ifdef LX_DEBUG
static lx_inline lx_fixed30_t lx_float_to_fixed30_check(lx_float_t x) {
    // check overflow, [-2., 2.]
    lx_assert(x >= -2. && x <= 2.);
    return ((lx_fixed30_t)((x) * LX_FIXED30_ONE));
}
#endif

static lx_inline lx_fixed30_t lx_fixed30_mul_int64(lx_fixed30_t x, lx_fixed30_t y) {
    return (lx_fixed30_t)((lx_hong_t)x * y >> 30);
}

static lx_inline lx_fixed30_t lx_fixed30_div_int64(lx_fixed30_t x, lx_fixed30_t y) {
    lx_assert(y);
    return (lx_fixed30_t)((((lx_hong_t)x) << 30) / y);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave


#endif

