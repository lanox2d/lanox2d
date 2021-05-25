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
 * @file        isinf.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "libm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_int_t lx_isinf(lx_double_t x) {
    lx_ieee_double_t e; e.d = x;
    lx_int32_t      t = e.i.l | ((e.i.h & 0x7fffffff) ^ 0x7ff00000);
    t |= -t;
    return (lx_long_t)(~(t >> 31) & (e.i.h >> 30));
}

lx_int_t lx_isinff(lx_float_t x) {
    /* 0 * finite => 0
     * 0 * infinity => NaN
     * 0 * NaN => NaN
     */
    lx_float_t p = x * 0;

    // p will either be NaN or 0 now, so we can return (prod == prod) or (0 == prod).
    return p == p;
}
