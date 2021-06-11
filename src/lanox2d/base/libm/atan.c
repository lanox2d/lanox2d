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
 * @file        atan.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "libm.h"
#include <math.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_double_t lx_atan(lx_double_t x) {
#ifdef LX_CONFIG_LIBM_HAVE_ATAN
    return atan(x);
#else
    lx_assert(0);
    return 0;
#endif
}

lx_float_t lx_atanf(lx_float_t x) {
#ifdef LX_CONFIG_LIBM_HAVE_ATANF
    return atanf(x);
#else
    return (lx_float_t)lx_atan(x);
#endif
}
