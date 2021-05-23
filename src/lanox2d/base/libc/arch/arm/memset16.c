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
 * @file        memset16.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#if defined(LX_ASSEMBLER_IS_GAS) && !defined(LX_ARCH_ARM_THUMB) && !defined(LX_ARCH_ARM64)
#   define LX_LIBC_ARCH_MEMSET16
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef LX_LIBC_ARCH_MEMSET16
static lx_inline lx_void_t lx_memset16_opt_v1(lx_uint16_t* s, lx_uint16_t c, lx_size_t n) {
    // cache line: 16-bytes
    lx_asm lx_volatile
    (
        "   tst     %2, #3\n"                   //!<  align by 4-bytes, if (s & 0x3) *((lx_uint16_t*)s) = c, s += 2, n--
        "   strneh  %1, [%2], #2\n"
        "   subne   %0, %0, #1\n"
        "   orr     %1, %1, %1, lsl #16\n"      //!<  c |= c << 16, expand to 32-bits
        "   mov     r3, %1\n"                   //!<  for storing data by 4x32bits
        "   mov     r4, %1\n"
        "   mov     r5, %1\n"
        "1:\n"                                  //!<  fill data by cache line n
        "   subs    %0, %0, #8\n"               //!<  n -= 4x2[x16bits] and update cpsr, assume 16-bytes cache lines
        "   stmhsia %2!, {%1, r3, r4, r5}\n"    //!<  storing data by 4x32bits = 8[x16bits], cond: hs (if >= 0), ia: s++
        "   bhs     1b\n"                       //!<  goto 1b if hs (>= 0)
        "   add     %0, %0, #8\n"               //!<  fill the left data, n = left n (< 8[x16bits])
        "   movs    %0, %0, lsl #30\n"          //!<  1, 11000000000000000000000000000000
        "   stmcsia %2!, {r4, r5}\n"            //!<  store 2x32bits, cond: cs (if carry bit == 1, >= 4[x16bits])
        "   strmi   r3, [%2], #4\n"             //!<  store 32bits, cond: mi (if negative bit == 1, >=2[x16bits])
        "   movs    %0, %0, lsl #2\n"           //!<  1, 00000000000000000000000000000000
        "   strcsh  %1, [%2]\n"                 //!<  store 16bits, cond: cs (if carry bit == 1, >= [x16bits])

        : : "r" (n), "r" (c), "r" (s)
        : "r3", "r4", "r5"
    );
}

static lx_inline lx_void_t lx_memset16_opt_v2(lx_uint16_t* s, lx_uint16_t c, lx_size_t n) {
    // cache line: 32-bytes
    lx_asm lx_volatile
    (
        "   tst     %2, #3\n"                   //!<  align by 4-bytes, if (s & 0x3) *((lx_uint16_t*)s) = c, s += 2, n--
        "   strneh  %1, [%2], #2\n"
        "   subne   %0, %0, #1\n"
        "   orr     %1, %1, %1, lsl #16\n"      //!<  c |= c << 16, expand to 32-bits
        "   mov     r3, %1\n"                   //!<  for storing data by 8x32bits
        "   mov     r4, %1\n"
        "   mov     r5, %1\n"
        "1:\n"                                  //!<  fill data by cache line n
        "   subs    %0, %0, #16\n"              //!<  n -= 8x2[x16bits] and update cpsr, assume 32-bytes cache lines
        "   stmhsia %2!, {%1, r3, r4, r5}\n"    //!<  storing data by 8x32bits = 16[x16bits], cond: hs (if >= 0), ia: s++
        "   stmhsia %2!, {%1, r3, r4, r5}\n"
        "   bhs     1b\n"                       //!<  goto 1b if hs (>= 0)
        "   add     %0, %0, #16\n"              //!<  fill the left data, n = left n (< 16[x16bits])
        "   movs    %0, %0, lsl #29\n"          //!<  1, 11100000000000000000000000000000
        "   stmcsia %2!, {%1, r3, r4, r5}\n"    //!<  store 4x32bits, cond: cs (if carry bit == 1, >= 8[x16bits])
        "   stmmiia %2!, {r4, r5}\n"            //!<  store 2x32bits, cond: mi (if negative bit == 1, >=4[x16bits])
        "   movs    %0, %0, lsl #2\n"           //!<  1, 10000000000000000000000000000000
        "   strcs   %1, [%2], #4\n"             //!<  store 32bits, cond: cs (if carry bit == 1, >= 2[x16bits])
        "   strmih  r3, [%2]\n"                 //!<  store 16bits, cond: cs (if negative bit == 1, >= [x16bits])

        : : "r" (n), "r" (c), "r" (s)
        : "r3", "r4", "r5"
    );
}
#endif

#ifdef LX_LIBC_ARCH_MEMSET16
lx_pointer_t lx_memset16(lx_pointer_t s, lx_uint16_t c, lx_size_t n) {
    lx_assert(!(((lx_size_t)s) & 0x1));
    if (!n) return s;
    if (n > 8) lx_memset16_opt_v1(s, c, n);
    else {
        lx_register lx_uint16_t*    p = s;
        lx_register lx_uint16_t     b = c;
        while (n--) *p++ = b;
    }
    return s;
}
#endif

