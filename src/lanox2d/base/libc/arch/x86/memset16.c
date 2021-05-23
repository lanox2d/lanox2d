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
#ifdef LX_ARCH_SSE2
#   include <emmintrin.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#if (defined(LX_ASSEMBLER_IS_GAS) && LX_CPU_BIT32) || defined(LX_ARCH_SSE2)
#   define LX_LIBC_ARCH_MEMSET16
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if defined(LX_ASSEMBLER_IS_GAS) && LX_CPU_BIT32
static lx_inline lx_void_t lx_memset16_opt_v1(lx_uint16_t* s, lx_uint16_t c, lx_size_t n) {

    // align by 4-bytes
    if (((lx_size_t)s) & 0x3) {
        *s++ = c;
        --n;
    }

    lx_asm lx_volatile
    (
        "cld\n\t"                           // clear the direction bit, s++, not s--
        "rep stosw"                         // *s++ = ax
        :                                   // no output registers
        : "c" (n), "a" (c), "D" (s)         // ecx = n, eax = c, edi = s
    );
}
#endif

#ifdef LX_ARCH_SSE2
static lx_inline lx_void_t lx_memset16_opt_v2(lx_uint16_t* s, lx_uint16_t c, lx_size_t n) {
    if (n >= 32) {
        // aligned by 16-bytes
        for (; ((lx_size_t)s) & 0x0f; --n) *s++ = c;

        // l = n % 32
        lx_size_t l = n & 0x1f; n = (n - l) >> 5;

        // fill 4 x 16 bytes
        __m128i* d = (__m128i*)(s);
        __m128i  v = _mm_set1_epi16(c);
        while (n) {
            _mm_store_si128(d++, v);
            _mm_store_si128(d++, v);
            _mm_store_si128(d++, v);
            _mm_store_si128(d++, v);
            --n;
        }
        s = (lx_uint16_t*)(d);
        n = l;
    }
    while (n--) *s++ = c;
}
#endif

#ifdef LX_LIBC_ARCH_MEMSET16
lx_pointer_t lx_memset16(lx_pointer_t s, lx_uint16_t c, lx_size_t n) {
    lx_assert(!(((lx_size_t)s) & 0x1));
    if (!n) return s;
#   if (defined(LX_ASSEMBLER_IS_GAS) && LX_CPU_BIT32) && defined(LX_ARCH_SSE2)
    if (n < 2049) lx_memset16_opt_v2(s, c, n);
    else lx_memset16_opt_v1(s, c, n);
#   elif defined(LX_ASSEMBLER_IS_GAS) && LX_CPU_BIT32
    lx_memset16_opt_v1(s, c, n);
#   elif defined(LX_ARCH_SSE2)
    lx_memset16_opt_v2(s, c, n);
#   else
#       error
#   endif
    return s;
}
#endif

