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
 * @file        strnlen.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "libc.h"
#include <string.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

#ifdef LX_CONFIG_LIBC_HAVE_STRNLEN
lx_size_t lx_strnlen(lx_char_t const* s, lx_size_t n) {
#   ifdef LX_CONFIG_OS_ANDROID
    /* fix the bug for android
     *
     * return -1 if n == (lx_uint32_t)-1
     */
    return strnlen(s, (lx_uint16_t)n);
#   else
    return strnlen(s, n);
#   endif
}
#else
lx_size_t lx_strnlen(lx_char_t const* s, lx_size_t n) {
    lx_assert_and_check_return_val(s, 0);
    if (!n) return 0;

    lx_register lx_char_t const* p = s;
#   ifdef LX_CONFIG_SMALL
    while (n-- && *p) ++p;
    return p - s;
#   else
    lx_size_t l = n & 0x3; n = (n - l) >> 2;
    while (n--) {
        if (!p[0]) return (p - s + 0);
        if (!p[1]) return (p - s + 1);
        if (!p[2]) return (p - s + 2);
        if (!p[3]) return (p - s + 3);
        p += 4;
    }
    while (l-- && *p) ++p;
    return p - s;
#   endif
}
#endif

