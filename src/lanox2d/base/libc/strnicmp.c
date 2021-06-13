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
 * @file        strnicmp.c
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
#ifdef LX_CONFIG_LIBC_HAVE_STRNCASECMP
lx_int_t lx_strnicmp(lx_char_t const* s1, lx_char_t const* s2, lx_size_t n) {
    lx_assert_and_check_return_val(s1 && s2, 0);
#   ifdef LX_COMPILER_IS_MSVC
    return _strnicmp(s1, s2, n);
#   else
    return strncasecmp(s1, s2, n);
#   endif
}
#else
lx_int_t lx_strnicmp(lx_char_t const* s1, lx_char_t const* s2, lx_size_t n) {
    lx_assert_and_check_return_val(s1 && s2, 0);
    if (s1 == s2 || !n) return 0;

    lx_int_t r = 0;
    while (n && ((s1 == s2) || !(r = ((lx_long_t)(lx_tolower(*((lx_byte_t*)s1)))) - lx_tolower(*((lx_byte_t*)s2)))) && (--n, ++s2, *s1++));
    return r;
}
#endif
