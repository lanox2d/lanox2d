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
 * @file        float.h
 *
 */
#ifndef LX_BASE_MATH_FLOAT_H
#define LX_BASE_MATH_FLOAT_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// constants
#define LX_PI                   (3.141592653f)
#define LX_NEAR0                (1.0f / (1 << 12))
#define LX_SQRT2                (1.414213562f)      //!< sqrt(2)
#define LX_ONEOVER_SQRT2        (0.707106781f)      //!< 1/sqrt(2)
#define LX_TAN_PIOVER8          (0.414213562f)      //!< tan(pi/8)
#define LX_SQRT2_OVER2          (0.707106781f)      //!< sqrt(2)/2
#define LX_PIOVER180            (0.017453293f)      //<! pi/180
#define LX_180OVERPI            (57.29577951f)      //!< 180/pi

/// some helper functions
#define lx_avg(x, y)            (((x) + (y)) * 0.5f)
#define lx_half(x)              ((x) * 0.5f)
#define lx_invert(x)            (1.0f / (x))

/// nearly zero?
#define lx_near0(x)             (lx_abs(x) <= LX_NEAR0)

/// nearly equal?
#define lx_near_eq(x, y)        (lx_abs((x) - (y)) <= LX_NEAR0)

/// degree/radian
#define lx_degree_to_radian(x)  ((x) * LX_PIOVER180)
#define lx_radian_to_degree(x)  ((x) * LX_180OVERPI)

// interpolation
#define lx_interp(x, y, f)      ((x) + ((y) - (x)) * (f))

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* compute numer / denom and check it
 *
 * @param numer     the numerator
 * @param denom     the denominator
 * @param result    the result, must be in range: (0, 1)
 *
 * @return          the result is in range(0, 1): 1, otherwise: 0
 */
lx_int_t            lx_unitdiv(lx_float_t numer, lx_float_t denom, lx_float_t* result);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


