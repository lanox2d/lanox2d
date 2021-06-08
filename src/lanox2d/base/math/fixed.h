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
 * @file        fixed.h
 *
 */
#ifndef LX_BASE_MATH_FIXED_H
#define LX_BASE_MATH_FIXED_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "fixed6.h"
#include "fixed16.h"
#include "fixed30.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define LX_FIXED_ONE                LX_FIXED16_ONE
#define LX_FIXED_HALF               LX_FIXED16_HALF
#define LX_FIXED_MAX                LX_FIXED16_MAX
#define LX_FIXED_MIN                LX_FIXED16_MIN
#define LX_FIXED_NAN                LX_FIXED16_NAN
#define LX_FIXED_INF                LX_FIXED16_INF
#define LX_FIXED_PI                 LX_FIXED16_PI
#define LX_FIXED_SQRT2              LX_FIXED16_SQRT2
#define LX_FIXED_NEAR0              LX_FIXED16_NEAR0

// conversion
#define lx_fixed_to_float(x)        lx_fixed16_to_float(x)
#define lx_float_to_fixed(x)        lx_float_to_fixed16(x)

#define lx_long_to_fixed(x)         lx_long_to_fixed16(x)
#define lx_fixed_to_long(x)         lx_fixed16_to_long(x)

#define lx_fixed6_to_fixed(x)       lx_fixed6_to_fixed16(x)
#define lx_fixed_to_fixed6(x)       lx_fixed16_to_fixed6(x)

#define lx_fixed30_to_fixed(x)      lx_fixed30_to_fixed16(x)
#define lx_fixed_to_fixed30(x)      lx_fixed16_to_fixed30(x)

// round
#define lx_fixed_round(x)           lx_fixed16_round(x)
#define lx_fixed_ceil(x)            lx_fixed16_ceil(x)
#define lx_fixed_floor(x)           lx_fixed16_floor(x)

// nearly equal?
#define lx_fixed_near_eq(x, y)      lx_fixed16_near_eq(x, y)

// operations
#define lx_fixed_abs(x)             lx_fixed16_abs(x)
#define lx_fixed_avg(x, y)          lx_fixed16_avg(x, y)
#define lx_fixed_lsh(x, y)          lx_fixed16_lsh(x, y)
#define lx_fixed_rsh(x, y)          lx_fixed16_rsh(x, y)
#define lx_fixed_mul(x, y)          lx_fixed16_mul(x, y)
#define lx_fixed_div(x, y)          lx_fixed16_div(x, y)
#define lx_fixed_imul(x, y)         lx_fixed16_imul(x, y)
#define lx_fixed_idiv(x, y)         lx_fixed16_idiv(x, y)
#define lx_fixed_imuldiv(x, y, z)   lx_fixed16_imuldiv(x, y, z)
#define lx_fixed_imulsub(x, y, z)   lx_fixed16_imulsub(x, y, z)
#define lx_fixed_invert(x)          lx_fixed16_invert(x)

#endif

