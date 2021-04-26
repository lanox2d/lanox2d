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
 * @file        printf.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "libc.h"
#include <stdio.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// vsnprintf fmt
#define lx_vsnprintf_fmt(s, n, fmt, r) \
do \
{ \
    lx_int_t __lx_ret = 0; \
    va_list __varg_list; \
    va_start(__varg_list, fmt); \
    __lx_ret = lx_vsnprintf(s, (n), fmt, __varg_list); \
    va_end(__varg_list); \
    if (__lx_ret >= 0) s[__lx_ret] = '\0'; \
    *r = __lx_ret > 0? __lx_ret : 0; \
 \
} while (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

lx_int_t lx_puts(lx_char_t const* s) {
    return puts(s);
}

lx_int_t lx_printf(lx_char_t const* fmt, ...) {
    lx_int_t  ret = 0;
    lx_char_t line[8192] = {0};
    lx_vsnprintf_fmt(line, sizeof(line) - 1, fmt, &ret);
    lx_puts(line);
    return ret;
}

lx_int_t lx_sprintf(lx_char_t* s, lx_char_t const* fmt, ...) {
    lx_int_t ret = 0;
    lx_vsnprintf_fmt(s, LX_MAXU16, fmt, &ret);
    return ret;
}

lx_int_t lx_snprintf(lx_char_t* s, lx_size_t n, lx_char_t const* fmt, ...) {
    lx_int_t ret = 0;
    lx_vsnprintf_fmt(s, n, fmt, &ret);
    return ret;
}

lx_int_t lx_vsnprintf(lx_char_t* s, lx_size_t n, lx_char_t const* fmt, va_list args) {
    return vsnprintf(s, n, fmt, args);
}


