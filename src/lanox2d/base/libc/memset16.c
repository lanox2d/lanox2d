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
#include "libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifndef LX_LIBC_MEMSET16
lx_pointer_t lx_memset16(lx_pointer_t s, lx_uint16_t c, lx_size_t n) {
    lx_check_return_val(n, s);
    lx_assert(!(((lx_size_t)s) & 0x1));

    lx_register lx_uint16_t* p = (lx_uint16_t*)s;
    lx_size_t l = n & 0x3; n = (n - l) >> 2;
    while (n--) {
        p[0] = c;
        p[1] = c;
        p[2] = c;
        p[3] = c;
        p += 4;
    }
    while (l--) *p++ = c;
    return s;
}
#endif
