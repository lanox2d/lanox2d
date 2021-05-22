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
 * @file        sh4.h
 *
 */
#ifndef LX_BASE_UTILS_BITS_SH4_H
#define LX_BASE_UTILS_BITS_SH4_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
// swap
#ifndef lx_bits_swap_u16
#   define lx_bits_swap_u16(x)              lx_bits_swap_u16_asm(x)
#endif
#ifndef lx_bits_swap_u32
#   define lx_bits_swap_u32(x)              lx_bits_swap_u32_asm(x)
#endif
/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

// swap
static lx_inline lx_uint16_t const lx_bits_swap_u16_asm(lx_uint16_t x) {
    lx_asm("swap.b %0,%0" : "+r"(x));
    return x;
}

static lx_inline lx_uint32_t const lx_bits_swap_u32_asm(lx_uint32_t x) {
    lx_asm( "swap.b %0,%0\n"
            "swap.w %0,%0\n"
            "swap.b %0,%0\n"
            : "+r"(x));
    return x;
}


#endif

