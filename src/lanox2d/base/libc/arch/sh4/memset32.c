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
 * @file        memset32.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#if defined(LX_ASSEMBLER_IS_GAS)
#   define LX_LIBC_ARCH_MEMSET32
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

#ifdef LX_ASSEMBLER_IS_GAS
static lx_inline lx_void_t lx_memset32_opt_v1(lx_uint32_t* s, lx_uint32_t c, lx_size_t n) {
    lx_size_t l = n & 0x3;
    s += (n << 2);
    if (!l) {
        n >>= 2;
        lx_asm lx_volatile
        (
            "1:\n\t"
            "dt %0\n\t"         /* i--, i > 0? T = 0 : 1 */
            "mov.l %1,@-%2\n\t" /* *--s = c */
            "mov.l %1,@-%2\n\t" /* *--s = c */
            "mov.l %1,@-%2\n\t" /* *--s = c */
            "mov.l %1,@-%2\n\t" /* *--s = c */
            "bf 1b\n\t"         /* if T == 0 goto label 1: */
            :                   /* no output registers */
            : "r" (n), "r" (c), "r" (s) /* constraint: register */
        );
    } else {
        lx_asm lx_volatile
        (
            "1:\n\t"
            "dt %0\n\t"         /* i--, i > 0? T = 0 : 1 */
            "mov.l %1,@-%2\n\t" /* *--s = c */
            "bf 1b\n\t"         /* if T == 0 goto label 1: */
            :                   /* no output registers */
            : "r" (n), "r" (c), "r" (s) /* constraint: register */
        );
    }
}

static lx_inline lx_void_t lx_memset32_opt_v2(lx_uint32_t* s, lx_uint32_t c, lx_size_t n) {
    lx_size_t l = n & 0x3;
    s += (n << 2);
    if (!l) {
        n >>= 2;
        lx_asm lx_volatile
        (
            "1:\n\t"
            "dt %0\n\t"         /* i--, i > 0? T = 0 : 1 */
            "mov.l %1,@-%2\n\t" /* *--s = c */
            "mov.l %1,@-%2\n\t" /* *--s = c */
            "mov.l %1,@-%2\n\t" /* *--s = c */
            "mov.l %1,@-%2\n\t" /* *--s = c */
            "bf 1b\n\t"         /* if T == 0 goto label 1: */
            :                   /* no output registers */
            : "r" (n), "r" (c), "r" (s) /* constraint: register */
        );
    } else if (n >= 4) /* fixme */ {
        n >>= 2;
        lx_asm lx_volatile
        (
            "1:\n\t"            /* fill the left data */
            "dt %3\n\t"
            "mov.l %1,@-%2\n\t"
            "bf 1b\n\t"
            "2:\n\t"            /* fill aligned data by 4 */
            "dt %0\n\t"         /* i--, i > 0? T = 0 : 1 */
            "mov.l %1,@-%2\n\t" /* *--s = c */
            "mov.l %1,@-%2\n\t" /* *--s = c */
            "mov.l %1,@-%2\n\t" /* *--s = c */
            "mov.l %1,@-%2\n\t" /* *--s = c */
            "bf 2b\n\t"         /* if T == 0 goto label 1: */
            :                   /* no output registers */
            : "r" (n), "r" (c), "r" (s), "r" (l) /* constraint: register */
        );
    } else {
        lx_asm lx_volatile
        (
            "1:\n\t"
            "dt %0\n\t"         /* i--, i > 0? T = 0 : 1 */
            "mov.l %1,@-%2\n\t" /* *--s = c */
            "bf 1b\n\t"         /* if T == 0 goto label 1: */
            :                   /* no output registers */
            : "r" (n), "r" (c), "r" (s) /* constraint: register */
        );
    }
}

static lx_inline lx_void_t lx_memset32_opt_v3(lx_uint32_t* s, lx_uint32_t c, lx_size_t n) {
    s += n << 2;
    lx_asm lx_volatile
    (
        "1:\n\t"
        "dt %0\n\t"         /* i--, i > 0? T = 0 : 1 */
        "mov.l %1,@-%2\n\t" /* *--s = c */
        "bf 1b\n\t"         /* if T == 0 goto label 1: */
        :                   /* no output registers */
        : "r" (n), "r" (c), "r" (s) /* constraint: register */
    );
}
#endif

#ifdef LX_LIBC_ARCH_MEMSET32
lx_pointer_t lx_memset32(lx_pointer_t s, lx_uint32_t c, lx_size_t n) {
    lx_assert(!(((lx_size_t)s) & 0x3));
    if (!n) return s;
#   if defined(LX_ASSEMBLER_IS_GAS)
    lx_memset32_opt_v1(s, c, n);
#   else
#       error
#   endif
    return s;
}
#endif


