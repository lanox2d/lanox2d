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
 * @file        strlcpy.c
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
#ifdef LX_CONFIG_LIBC_HAVE_STRLCPY
lx_size_t lx_strlcpy(lx_char_t* s1, lx_char_t const* s2, lx_size_t n) {
    return strlcpy(s1, s2, n);
}
#else
lx_size_t lx_strlcpy_impl(lx_char_t* s1, lx_char_t const* s2, lx_size_t n) {
    lx_assert_and_check_return_val(s1 && s2, 0);

    lx_char_t*       d = s1;
    lx_char_t const* s = s2;
    lx_size_t        m = n;
    if (m != 0 && --m != 0) {
        do {
            if ((*d++ = *s++) == 0) {
                break;
            }
        } while (--m != 0);
    }
    if (m == 0) {
        if (n != 0) {
            *d = '\0';
        }
        while (*s++) ;
    }
    return (s - s2 - 1);
}
#endif
