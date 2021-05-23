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
 * @file        memset24.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "libc.h"
#include "../utils/bits.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_pointer_t lx_memset24(lx_pointer_t s, lx_uint32_t c, lx_size_t n) {
    lx_check_return_val(n, s);

    lx_register lx_byte_t* p = (lx_byte_t*)s;
    lx_register lx_byte_t* e = p + (n * 3);
    lx_size_t l = n & 0x3; n -= l;
    while (p < e) {
        lx_bits_set_u24_ne(p + 0, c);
        lx_bits_set_u24_ne(p + 3, c);
        lx_bits_set_u24_ne(p + 6, c);
        lx_bits_set_u24_ne(p + 9, c);
        p += 12;
    }
    while (l--) {
        lx_bits_set_u24_ne(p, c);
        p += 3;
    }
    return s;
}
