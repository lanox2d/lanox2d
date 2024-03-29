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
 * @file        prefix.h
 *
 */
#ifndef LX_BASE_LIBM_PREFIX_H
#define LX_BASE_LIBM_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the ieee float type
typedef union lx_ieee_float_t_ {
    lx_float_t  f;
    lx_uint32_t i;
}lx_ieee_float_t;

// the ieee double type
#ifdef LX_FLOAT_BIGENDIAN
typedef union lx_ieee_double_t_ {
    lx_double_t d;
    struct {
        lx_uint32_t h;
        lx_uint32_t l;
    }i;
}lx_ieee_double_t;
#else
typedef union lx_ieee_double_t_ {
    lx_double_t d;
    struct {
        lx_uint32_t l;
        lx_uint32_t h;
    }i;
}lx_ieee_double_t;
#endif

#endif


