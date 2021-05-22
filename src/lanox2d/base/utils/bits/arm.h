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
 * @file        arm.h
 *
 */
#ifndef LX_BASE_UTILS_BITS_ARM_H
#define LX_BASE_UTILS_BITS_ARM_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#if defined(LX_ASSEMBLER_IS_GAS) && !defined(LX_ARCH_ARM64)

// swap
#if LX_ARCH_ARM_VERSION >= 6
#   ifndef lx_bits_swap_u16
#       define lx_bits_swap_u16(x)          lx_bits_swap_u16_asm(x)
#   endif
#endif
#ifndef lx_bits_swap_u32
#   define lx_bits_swap_u32(x)              lx_bits_swap_u32_asm(x)
#endif

// FIXME: for ios
//#define lx_bits_get_ubits32_impl(p, b, n)     lx_bits_get_ubits32_impl_asm(p, b, n)

#endif /* LX_ASSEMBLER_IS_GAS */

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

#if defined(LX_ASSEMBLER_IS_GAS) && !defined(LX_ARCH_ARM64)

// swap
#if (LX_ARCH_ARM_VERSION >= 6)
static lx_inline lx_uint16_t const lx_bits_swap_u16_asm(lx_uint16_t x) {
    lx_asm lx_volatile("rev16 %0, %0" : "+r"(x));
    return x;
}
#endif

static lx_inline lx_uint32_t const lx_bits_swap_u32_asm(lx_uint32_t x) {
#if (LX_ARCH_ARM_VERSION >= 6)
    lx_asm("rev %0, %0" : "+r"(x));
#else
    lx_register lx_uint32_t t;
    lx_asm lx_volatile
    (
        "eor %1, %0, %0, ror #16 \n"
        "bic %1, %1, #0xff0000   \n"
        "mov %0, %0, ror #8      \n"
        "eor %0, %0, %1, lsr #8  \n"
        : "+r"(x), "=&r"(t)
    );
#endif
    return x;
}

#if 0
static lx_inline lx_uint32_t lx_bits_get_ubits32_impl_asm(lx_byte_t const* p, lx_size_t b, lx_size_t n) {
#ifdef __tb_small__
    lx_register lx_uint32_t x;
    lx_asm lx_volatile
    (
        "ldrb r6, [%1], #1\n"
        "ldrb r7, [%1], #1\n"
        "ldrb r8, [%1], #1\n"
        "ldrb r9, [%1], #1\n"
        "ldrb %1, [%1]\n"
        "orr %0, r6, lsl %2\n"
        "sub %2, %2, #8\n"
        "cmp %3, #8\n"
        "orrhi %0, r7, lsl %2\n"
        "sub %2, %2, #8\n"
        "cmp %3, #16\n"
        "orrhi %0, r8, lsl %2\n"
        "sub %2, %2, #8\n"
        "cmp %3, #24\n"
        "orrhi %0, r9, lsl %2\n"
        "rsb %2, %2, #8\n"
        "cmp %3, #32\n"
        "orrhi %0, %1, lsr %2\n"
        "rsb %4, %4, #32\n"
        "mov %0, %0, lsr %4\n"


        : "=&r"(x)
        : "r"(p), "r"(b + 24), "r"(b + n), "r"(n), "0"(0)
        : "r6", "r7", "r8", "r9"
    );

    return x;
#else
    lx_register lx_uint32_t x;
    lx_asm lx_volatile
    (
        "   cmp %3, #32\n"
        "   bhi 1f\n"
        "   cmp %3, #24\n"
        "   bhi 2f\n"
        "   cmp %3, #16\n"
        "   bhi 3f\n"
        "   cmp %3, #8\n"
        "   bhi 4f\n"
        "   ldrb %1, [%1]\n"
        "   orr %0, %1, lsl %2\n"
        "   b   5f\n"
        "1:\n"
        "   ldrb r6, [%1], #1\n"
        "   ldrb r7, [%1], #1\n"
        "   ldrb r8, [%1], #1\n"
        "   ldrb r9, [%1], #1\n"
        "   ldrb %1, [%1]\n"
        "   orr %0, r6, lsl %2\n"
        "   sub %2, %2, #8\n"
        "   orr %0, r7, lsl %2\n"
        "   sub %2, %2, #8\n"
        "   orr %0, r8, lsl %2\n"
        "   sub %2, %2, #8\n"
        "   orr %0, r9, lsl %2\n"
        "   rsb %2, %2, #8\n"
        "   orr %0, %1, lsr %2\n"
        "   b   5f\n"
        "2:\n"
        "   ldrb r6, [%1], #1\n"
        "   ldrb r7, [%1], #1\n"
        "   ldrb r8, [%1], #1\n"
        "   ldrb r9, [%1], #1\n"
        "   orr %0, r6, lsl %2\n"
        "   sub %2, %2, #8\n"
        "   orr %0, r7, lsl %2\n"
        "   sub %2, %2, #8\n"
        "   orr %0, r8, lsl %2\n"
        "   sub %2, %2, #8\n"
        "   orr %0, r9, lsl %2\n"
        "   b   5f\n"
        "3:\n"
        "   ldrb r6, [%1], #1\n"
        "   ldrb r7, [%1], #1\n"
        "   ldrb r8, [%1], #1\n"
        "   orr %0, r6, lsl %2\n"
        "   sub %2, %2, #8\n"
        "   orr %0, r7, lsl %2\n"
        "   sub %2, %2, #8\n"
        "   orr %0, r8, lsl %2\n"
        "   b   5f\n"
        "4:\n"
        "   ldrb r6, [%1], #1\n"
        "   ldrb r7, [%1], #1\n"
        "   orr %0, r6, lsl %2\n"
        "   sub %2, %2, #8\n"
        "   orr %0, r7, lsl %2\n"
        "5:\n"
        "   rsb %4, %4, #32\n"
        "   mov %0, %0, lsr %4\n"

        : "=&r"(x)
        : "r"(p), "r"(b + 24), "r"(b + n), "r"(n), "0"(0)
        : "r6", "r7", "r8", "r9"
    );

    return x;
#endif
}
#endif

#endif /* LX_ASSEMBLER_IS_GAS */


#endif

