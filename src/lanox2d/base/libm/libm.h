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
 * @file        libm.h
 *
 */
#ifndef LX_BASE_LIBM_H
#define LX_BASE_LIBM_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#define lx_ceil(x)              ((x) > 0? (lx_int32_t)((x) + 0.9999999999) : (lx_int32_t)(x))
#define lx_floor(x)             ((x) > 0? (lx_int32_t)(x) : (lx_int32_t)((x) - 0.9999999999))

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/// double value is infinite?
lx_int_t        lx_isinf(lx_double_t x);

/// float value is infinite?
lx_int_t        lx_isinff(lx_float_t x);

/// double value is finite?
lx_int_t        lx_isfin(lx_double_t x);

/// float value is finite?
lx_int_t        lx_isfinf(lx_float_t x);

/// double value is nan?
lx_int_t        lx_isnan(lx_double_t x);

/// float value is nan?
lx_int_t        lx_isnanf(lx_float_t x);

/// compute sqrt of double value
lx_double_t     lx_sqrt(lx_double_t x);

/// compute sqrt of float value
lx_float_t      lx_sqrtf(lx_float_t x);

/// compute sin of double value
lx_double_t     lx_sin(lx_double_t x);

/// compute sin of float value
lx_float_t      lx_sinf(lx_float_t x);

/// compute cos of double value
lx_double_t     lx_cos(lx_double_t x);

/// compute cos of float value
lx_float_t      lx_cosf(lx_float_t x);

/// compute atan of double value
lx_double_t     lx_atan(lx_double_t x);

/// compute atan of float value
lx_float_t      lx_atanf(lx_float_t x);

/// compute sin and cos of double value
lx_void_t       lx_sincos(lx_double_t x, lx_double_t* s, lx_double_t* c);

/// compute sin and cos of float value
lx_void_t       lx_sincosf(lx_float_t x, lx_float_t* s, lx_float_t* c);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


