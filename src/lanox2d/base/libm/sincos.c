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
 * @file        sincos.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "libm.h"
#include <math.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
#ifdef LX_CONFIG_LIBM_HAVE_SINCOS
extern lx_void_t sincos(lx_double_t x, lx_double_t* s, lx_double_t* c);
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_void_t lx_sincos(lx_double_t x, lx_double_t* s, lx_double_t* c) {
#ifdef LX_CONFIG_LIBM_HAVE_SINCOS
    sincos(x, s, c);
#else
    if (s) *s = lx_sin(x);
    if (c) *c = lx_cos(x);
#endif
}

lx_void_t lx_sincosf(lx_float_t x, lx_float_t* s, lx_float_t* c) {
#ifdef LX_CONFIG_LIBM_HAVE_SINCOSF
    sincosf(x, s, c);
#else
    if (s) *s = lx_sinf(x);
    if (c) *c = lx_cosf(x);
#endif
}
