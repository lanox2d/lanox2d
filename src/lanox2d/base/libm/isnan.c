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
 * @file        isnan.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "libm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_int_t lx_isnan(lx_double_t x) {
#if 0
    lx_ieee_double_t e; e.d = x;
    lx_int32_t      t = e.i.h & 0x7fffffff;
    t |= (lx_uint32_t)(e.i.l | (lx_uint32_t)(-(lx_int32_t)e.i.l)) >> 31;
    t = 0x7ff00000 - t;
    return (lx_long_t)(((lx_uint32_t)t) >> 31);
#else
    // optimization
    return x != x;
#endif
}

lx_int_t lx_isnanf(lx_float_t x) {
#if 0
    lx_ieee_float_t e; e.f = x;
    lx_int32_t      t = e.i & 0x7fffffff;
    t = 0x7f800000 - t;
    return (lx_long_t)(((lx_uint32_t)(t)) >> 31);
#else
    // optimization
    return x != x;
#endif
}
