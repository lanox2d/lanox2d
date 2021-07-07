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
 * @file        bits.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "bits.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_uint32_t lx_bits_get_ubits32(lx_byte_t const* p, lx_size_t b, lx_size_t n) {
    lx_assert(p && n <= 32);
    lx_check_return_val(n, 0);

    p += b >> 3; b &= 0x07;
    if (!b && n == 1) {
        return lx_bits_get_u1(p);
    } else if (!b && n == 8) {
        return lx_bits_get_u8(p);
    } else if (!b && n == 16) {
        return lx_bits_get_u16_be(p);
    } else if (!b && n == 24) {
        return lx_bits_get_u24_be(p);
    } else if (!b && n == 32) {
        return lx_bits_get_u32_be(p);
    } else {
#ifdef LX_CONFIG_SMALL
        lx_uint32_t x = 0;
        lx_size_t   i = b;
        lx_int_t    j = 24;
        b += n;
        while (b > 7) {
            x |= *p++ << (i + j);
            j -= 8;
            b -= 8;
        }
        if (b > 0) {
            x |= j < 0? (*p >> (8 - i)) : *p << (i + j);
        }
        return (n < 32)? (x >> (32 - n)) : x;
#else
        lx_uint32_t x = 0;
        lx_size_t   i = b; b += n;
        if (b > 32) {
            x |= p[0] << (i + 24);
            x |= p[1] << (i + 16);
            x |= p[2] << (i + 8);
            x |= p[3] << (i + 0);
            x |= p[4] >> (8 - i);
        } else if (b > 24) {
            x |= p[0] << (i + 24);
            x |= p[1] << (i + 16);
            x |= p[2] << (i + 8);
            x |= p[3] << (i + 0);
        } else if (b > 16) {
            x |= p[0] << (i + 24);
            x |= p[1] << (i + 16);
            x |= p[2] << (i + 8);
        } else if (b > 8) {
            x |= p[0] << (i + 24);
            x |= p[1] << (i + 16);
        } else {
            x |= p[0] << (i + 24);
        }
        return (n < 32)? (x >> (32 - n)) : x;
#endif
    }
}

lx_sint32_t lx_bits_get_sbits32(lx_byte_t const* p, lx_size_t b, lx_size_t n) {
    lx_assert(p && n <= 32);
    lx_check_return_val(n, 0);

    p += b >> 3; b &= 0x07;
    if (n < 32) {
        lx_sint32_t s = -(lx_sint32_t)lx_bits_get_ubits32(p, b, 1);
        return ((s << (n - 1)) | lx_bits_get_ubits32(p, b + 1, n - 1));
    }
    else {
        return lx_bits_get_ubits32(p, b, n);
    }
}

