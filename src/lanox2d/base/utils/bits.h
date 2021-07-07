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
 * @file        bits.h
 *
 */
#ifndef LX_BASE_UTILS_BITS_H
#define LX_BASE_UTILS_BITS_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#if defined(LX_COMPILER_IS_GCC)
#   include "bits/gcc.h"
#endif
#if defined(LX_ARCH_x86) || defined(LX_ARCH_x64)
#   include "bits/x86.h"
#elif defined(LX_ARCH_ARM)
#   include "bits/arm.h"
#elif defined(LX_ARCH_SH4)
#   include "bits/sh4.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/* TODO
 *
 * check LX_UNALIGNED_ACCESS_ENABLE for compiler
 */
//#define LX_UNALIGNED_ACCESS_ENABLE

// 1-bits
#define lx_bits_get_u1(p)                   (((*(p)) >> 7) & 1)
#define lx_bits_set_u1(p, x)                do { *(p) &= 0x7f; *(p) |= (((x) & 0x1) << 7); } while (0)

// 8-bits
#define lx_bits_get_u8(p)                   (*(p))
#define lx_bits_get_s8(p)                   (*(p))

#define lx_bits_set_u8(p, x)                do { *(p) = (lx_uint8_t)(x); } while (0)
#define lx_bits_set_s8(p, x)                do { *(p) = (lx_sint8_t)(x); } while (0)

// 16-bits
#define lx_bits_get_u16_le_impl(p)          ((lx_uint16_t)((lx_uint16_t)*((p) + 1) << 8 | (lx_uint16_t)*(p)))
#define lx_bits_get_s16_le_impl(p)          lx_bits_get_u16_le_impl(p)
#define lx_bits_get_u16_be_impl(p)          ((lx_uint16_t)(*((p)) << 8 | (lx_uint16_t)*((p) + 1)))
#define lx_bits_get_s16_be_impl(p)          lx_bits_get_u16_be_impl(p)
#define lx_bits_get_u16_ne_impl(p)          (*((lx_uint16_t*)(p)))
#define lx_bits_get_s16_ne_impl(p)          lx_bits_get_u16_ne_impl(p)

#define lx_bits_set_u16_le_impl(p, x)       lx_bits_set_u16_le_inline(p, x)
#define lx_bits_set_s16_le_impl(p, x)       lx_bits_set_u16_le_inline(p, x)
#define lx_bits_set_u16_be_impl(p, x)       lx_bits_set_u16_be_inline(p, x)
#define lx_bits_set_s16_be_impl(p, x)       lx_bits_set_u16_be_inline(p, x)
#define lx_bits_set_u16_ne_impl(p, x)       do { *((lx_uint16_t*)(p)) = (lx_uint16_t)(x); } while (0)
#define lx_bits_set_s16_ne_impl(p, x)       lx_bits_set_u16_ne_impl(p, x)

// 24-bits
#define lx_bits_get_u24_le_impl(p)          ((lx_uint32_t)(*((p) + 2) << 16 | *((p) + 1) << 8 | *(p)))
#define lx_bits_get_s24_le_impl(p)          ((lx_bits_get_u24_le_impl(p) + 0xff800000) ^ 0xff800000)
#define lx_bits_get_u24_be_impl(p)          ((lx_uint32_t)(*(p) << 16 | *((p) + 1) << 8 | *((p) + 2)))
#define lx_bits_get_s24_be_impl(p)          ((lx_bits_get_u24_be_impl(p) + 0xff800000) ^ 0xff800000)
#define lx_bits_get_u24_ne_impl(p)          ((lx_uint32_t)(*((lx_uint32_t*)(p)) & 0x00ffffff))
#define lx_bits_get_s24_ne_impl(p)          ((lx_bits_get_u24_ne_impl(p) + 0xff800000) ^ 0xff800000)

#define lx_bits_set_u24_le_impl(p, x)       lx_bits_set_u24_le_inline(p, x)
#define lx_bits_set_s24_le_impl(p, x)       lx_bits_set_u24_le_inline(p, x)
#define lx_bits_set_u24_be_impl(p, x)       lx_bits_set_u24_be_inline(p, x)
#define lx_bits_set_s24_be_impl(p, x)       lx_bits_set_u24_be_inline(p, x)
#define lx_bits_set_u24_ne_impl(p, x)       do { *((lx_uint32_t*)(p)) = (lx_uint32_t)(x) & 0x00ffffff; } while (0)
#define lx_bits_set_s24_ne_impl(p, x)       lx_bits_set_u24_ne_impl(p, x)

// 32-bits
#define lx_bits_get_u32_le_impl(p)          ((lx_uint32_t)(*((p) + 3) << 24 | *((p) + 2) << 16 | *((p) + 1) << 8 | *(p)))
#define lx_bits_get_s32_le_impl(p)          lx_bits_get_u32_le_impl(p)
#define lx_bits_get_u32_be_impl(p)          ((lx_uint32_t)(*(p) << 24 | *((p) + 1) << 16 | *((p) + 2) << 8 | *((p) + 3)))
#define lx_bits_get_s32_be_impl(p)          lx_bits_get_u32_be_impl(p)
#define lx_bits_get_u32_ne_impl(p)          (*((lx_uint32_t*)(p)))
#define lx_bits_get_s32_ne_impl(p)          lx_bits_get_u32_ne_impl(p)

#define lx_bits_set_u32_le_impl(p, x)       lx_bits_set_u32_le_inline(p, x)
#define lx_bits_set_s32_le_impl(p, x)       lx_bits_set_u32_le_inline(p, x)
#define lx_bits_set_u32_be_impl(p, x)       lx_bits_set_u32_be_inline(p, x)
#define lx_bits_set_s32_be_impl(p, x)       lx_bits_set_u32_be_inline(p, x)
#define lx_bits_set_u32_ne_impl(p, x)       do { *((lx_uint32_t*)(p)) = (lx_uint32_t)(x); } while (0)
#define lx_bits_set_s32_ne_impl(p, x)       lx_bits_set_u32_ne_impl(p, x)

// 64-bits
#define lx_bits_get_u64_le_impl(p)          ((lx_uint64_t)*((p) + 7) << 56 | (lx_uint64_t)*((p) + 6) << 48 | (lx_uint64_t)*((p) + 5) << 40 | (lx_uint64_t)*((p) + 4) << 32 | (lx_uint64_t)*((p) + 3) << 24 | (lx_uint64_t)*((p) + 2) << 16 | (lx_uint64_t)*((p) + 1) << 8 | (lx_uint64_t)*(p))
#define lx_bits_get_s64_le_impl(p)          lx_bits_get_u64_le_impl(p)
#define lx_bits_get_u64_be_impl(p)          ((lx_uint64_t)*(p) << 56 | (lx_uint64_t)*((p) + 1) << 48 | (lx_uint64_t)*((p) + 2) << 40 | (lx_uint64_t)*((p) + 3) << 32 | (lx_uint64_t)*((p) + 4) << 24 | (lx_uint64_t)*((p) + 5) << 16 | (lx_uint64_t)*((p) + 6) << 8 | (lx_uint64_t)*((p) + 7))
#define lx_bits_get_s64_be_impl(p)          lx_bits_get_u64_be_impl(p)
#define lx_bits_get_u64_ne_impl(p)          (*((lx_uint64_t*)(p)))
#define lx_bits_get_s64_ne_impl(p)          lx_bits_get_u64_ne_impl(p)

#define lx_bits_set_u64_le_impl(p, x)       lx_bits_set_u64_le_inline(p, x)
#define lx_bits_set_s64_le_impl(p, x)       lx_bits_set_u64_le_inline(p, x)
#define lx_bits_set_u64_be_impl(p, x)       lx_bits_set_u64_be_inline(p, x)
#define lx_bits_set_s64_be_impl(p, x)       lx_bits_set_u64_be_inline(p, x)
#define lx_bits_set_u64_ne_impl(p, x)       do { *((lx_uint64_t*)(p)) = (lx_uint64_t)(x); } while (0)
#define lx_bits_set_s64_ne_impl(p, x)       lx_bits_set_u64_ne_impl(p, x)

// float
#ifdef LX_CONFIG_TYPE_HAVE_FLOAT

#   define lx_bits_get_float_le(p)              lx_bits_get_float_le_inline(p)
#   define lx_bits_get_float_be(p)              lx_bits_get_float_be_inline(p)

#   define lx_bits_set_float_le(p, x)           lx_bits_set_float_le_inline(p, x)
#   define lx_bits_set_float_be(p, x)           lx_bits_set_float_be_inline(p, x)

#   define lx_bits_get_double_ble(p)            lx_bits_get_double_ble_inline(p)
#   define lx_bits_get_double_bbe(p)            lx_bits_get_double_bbe_inline(p)

#   define lx_bits_get_double_lle(p)            lx_bits_get_double_lle_inline(p)
#   define lx_bits_get_double_lbe(p)            lx_bits_get_double_lbe_inline(p)

#   define lx_bits_set_double_ble(p, x)         lx_bits_set_double_ble_inline(p, x)
#   define lx_bits_set_double_bbe(p, x)         lx_bits_set_double_bbe_inline(p, x)

#   define lx_bits_set_double_lle(p, x)         lx_bits_set_double_lle_inline(p, x)
#   define lx_bits_set_double_lbe(p, x)         lx_bits_set_double_lbe_inline(p, x)

#   ifdef LX_FLOAT_BIGENDIAN
#       define lx_bits_get_double_nbe(p)        lx_bits_get_double_bbe(p)
#       define lx_bits_get_double_nle(p)        lx_bits_get_double_ble(p)

#       define lx_bits_set_double_nbe(p, x)     lx_bits_set_double_bbe(p, x)
#       define lx_bits_set_double_nle(p, x)     lx_bits_set_double_ble(p, x)
#   else
#       define lx_bits_get_double_nbe(p)        lx_bits_get_double_lbe(p)
#       define lx_bits_get_double_nle(p)        lx_bits_get_double_lle(p)

#       define lx_bits_set_double_nbe(p, x)     lx_bits_set_double_lbe(p, x)
#       define lx_bits_set_double_nle(p, x)     lx_bits_set_double_lle(p, x)
#   endif
#   ifdef LX_WORDS_BIGENDIAN
#       define lx_bits_get_float_ne(p)          lx_bits_get_float_be(p)
#       define lx_bits_set_float_ne(p, x)       lx_bits_set_float_be(p, x)

#       define lx_bits_get_double_nne(p)        lx_bits_get_double_nbe(p)
#       define lx_bits_get_double_bne(p)        lx_bits_get_double_bbe(p)
#       define lx_bits_get_double_lne(p)        lx_bits_get_double_lbe(p)

#       define lx_bits_set_double_nne(p, x)     lx_bits_set_double_nbe(p, x)
#       define lx_bits_set_double_bne(p, x)     lx_bits_set_double_bbe(p, x)
#       define lx_bits_set_double_lne(p, x)     lx_bits_set_double_lbe(p, x)
#   else
#       define lx_bits_get_float_ne(p)          lx_bits_get_float_le(p)
#       define lx_bits_set_float_ne(p, x)       lx_bits_set_float_le(p, x)

#       define lx_bits_get_double_nne(p)        lx_bits_get_double_nle(p)
#       define lx_bits_get_double_bne(p)        lx_bits_get_double_ble(p)
#       define lx_bits_get_double_lne(p)        lx_bits_get_double_lle(p)

#       define lx_bits_set_double_nne(p, x)     lx_bits_set_double_nle(p, x)
#       define lx_bits_set_double_bne(p, x)     lx_bits_set_double_ble(p, x)
#       define lx_bits_set_double_lne(p, x)     lx_bits_set_double_lle(p, x)
#   endif
#endif

#ifdef LX_UNALIGNED_ACCESS_ENABLE

#   ifdef LX_WORDS_BIGENDIAN
// 16-bits
#   define lx_bits_get_u16_le(p)        lx_bits_get_u16_le_impl((lx_byte_t*)(p))
#   define lx_bits_get_s16_le(p)        lx_bits_get_s16_le_impl((lx_byte_t*)(p))
#   define lx_bits_get_u16_be(p)        lx_bits_get_u16_ne_impl((lx_byte_t*)(p))
#   define lx_bits_get_s16_be(p)        lx_bits_get_s16_ne_impl((lx_byte_t*)(p))

#   define lx_bits_set_u16_le(p, x)     lx_bits_set_u16_le_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_s16_le(p, x)     lx_bits_set_s16_le_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_u16_be(p, x)     lx_bits_set_u16_ne_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_s16_be(p, x)     lx_bits_set_s16_ne_impl((lx_byte_t*)(p), x)

// 24-bits
#   define lx_bits_get_u24_le(p)        lx_bits_get_u24_le_impl((lx_byte_t*)(p))
#   define lx_bits_get_s24_le(p)        lx_bits_get_s24_le_impl((lx_byte_t*)(p))
#   define lx_bits_get_u24_be(p)        lx_bits_get_u24_ne_impl((lx_byte_t*)(p))
#   define lx_bits_get_s24_be(p)        lx_bits_get_s24_ne_impl((lx_byte_t*)(p))

#   define lx_bits_set_u24_le(p, x)     lx_bits_set_u24_le_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_s24_le(p, x)     lx_bits_set_s24_le_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_u24_be(p, x)     lx_bits_set_u24_ne_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_s24_be(p, x)     lx_bits_set_s24_ne_impl((lx_byte_t*)(p), x)

// 32-bits
#   define lx_bits_get_u32_le(p)        lx_bits_get_u32_le_impl((lx_byte_t*)(p))
#   define lx_bits_get_s32_le(p)        lx_bits_get_s32_le_impl((lx_byte_t*)(p))
#   define lx_bits_get_u32_be(p)        lx_bits_get_u32_ne_impl((lx_byte_t*)(p))
#   define lx_bits_get_s32_be(p)        lx_bits_get_s32_ne_impl((lx_byte_t*)(p))

#   define lx_bits_set_u32_le(p, x)     lx_bits_set_u32_le_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_s32_le(p, x)     lx_bits_set_u32_le_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_u32_be(p, x)     lx_bits_set_u32_ne_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_s32_be(p, x)     lx_bits_set_s32_ne_impl((lx_byte_t*)(p), x)

// 64-bits
#   define lx_bits_get_u64_le(p)        lx_bits_get_u64_le_impl((lx_byte_t*)(p))
#   define lx_bits_get_s64_le(p)        lx_bits_get_s64_le_impl((lx_byte_t*)(p))
#   define lx_bits_get_u64_be(p)        lx_bits_get_u64_ne_impl((lx_byte_t*)(p))
#   define lx_bits_get_s64_be(p)        lx_bits_get_s64_ne_impl((lx_byte_t*)(p))

#   define lx_bits_set_u64_le(p, x)     lx_bits_set_u64_le_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_s64_le(p, x)     lx_bits_set_u64_le_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_u64_be(p, x)     lx_bits_set_u64_ne_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_s64_be(p, x)     lx_bits_set_s64_ne_impl((lx_byte_t*)(p), x)

#   else

// 16-bits
#   define lx_bits_get_u16_le(p)        lx_bits_get_u16_ne_impl((lx_byte_t*)(p))
#   define lx_bits_get_s16_le(p)        lx_bits_get_s16_ne_impl((lx_byte_t*)(p))
#   define lx_bits_get_u16_be(p)        lx_bits_get_u16_be_impl((lx_byte_t*)(p))
#   define lx_bits_get_s16_be(p)        lx_bits_get_s16_be_impl((lx_byte_t*)(p))

#   define lx_bits_set_u16_le(p, x)     lx_bits_set_u16_ne_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_s16_le(p, x)     lx_bits_set_s16_ne_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_u16_be(p, x)     lx_bits_set_u16_be_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_s16_be(p, x)     lx_bits_set_s16_be_impl((lx_byte_t*)(p), x)

// 24-bits
#   define lx_bits_get_u24_le(p)        lx_bits_get_u24_ne_impl((lx_byte_t*)(p))
#   define lx_bits_get_s24_le(p)        lx_bits_get_s24_ne_impl((lx_byte_t*)(p))
#   define lx_bits_get_u24_be(p)        lx_bits_get_u24_be_impl((lx_byte_t*)(p))
#   define lx_bits_get_s24_be(p)        lx_bits_get_s24_be_impl((lx_byte_t*)(p))

#   define lx_bits_set_u24_le(p, x)     lx_bits_set_u24_ne_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_s24_le(p, x)     lx_bits_set_s24_ne_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_u24_be(p, x)     lx_bits_set_u24_be_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_s24_be(p, x)     lx_bits_set_s24_be_impl((lx_byte_t*)(p), x)

// 32-bits
#   define lx_bits_get_u32_le(p)        lx_bits_get_u32_ne_impl((lx_byte_t*)(p))
#   define lx_bits_get_s32_le(p)        lx_bits_get_s32_ne_impl((lx_byte_t*)(p))
#   define lx_bits_get_u32_be(p)        lx_bits_get_u32_be_impl((lx_byte_t*)(p))
#   define lx_bits_get_s32_be(p)        lx_bits_get_s32_be_impl((lx_byte_t*)(p))

#   define lx_bits_set_u32_le(p, x)     lx_bits_set_u32_le_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_s32_le(p, x)     lx_bits_set_u32_le_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_u32_be(p, x)     lx_bits_set_u32_be_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_s32_be(p, x)     lx_bits_set_s32_be_impl((lx_byte_t*)(p), x)

// 64-bits
#   define lx_bits_get_u64_le(p)        lx_bits_get_u64_ne_impl((lx_byte_t*)(p))
#   define lx_bits_get_s64_le(p)        lx_bits_get_s64_ne_impl((lx_byte_t*)(p))
#   define lx_bits_get_u64_be(p)        lx_bits_get_u64_be_impl((lx_byte_t*)(p))
#   define lx_bits_get_s64_be(p)        lx_bits_get_s64_be_impl((lx_byte_t*)(p))

#   define lx_bits_set_u64_le(p, x)     lx_bits_set_u64_le_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_s64_le(p, x)     lx_bits_set_u64_le_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_u64_be(p, x)     lx_bits_set_u64_be_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_s64_be(p, x)     lx_bits_set_s64_be_impl((lx_byte_t*)(p), x)

#   endif /* LX_WORDS_BIGENDIAN */

#else
// 16-bits
#   define lx_bits_get_u16_le(p)        lx_bits_get_u16_le_impl((lx_byte_t*)(p))
#   define lx_bits_get_s16_le(p)        lx_bits_get_s16_le_impl((lx_byte_t*)(p))
#   define lx_bits_get_u16_be(p)        lx_bits_get_u16_be_impl((lx_byte_t*)(p))
#   define lx_bits_get_s16_be(p)        lx_bits_get_s16_be_impl((lx_byte_t*)(p))

#   define lx_bits_set_u16_le(p, x)     lx_bits_set_u16_le_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_s16_le(p, x)     lx_bits_set_s16_le_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_u16_be(p, x)     lx_bits_set_u16_be_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_s16_be(p, x)     lx_bits_set_s16_be_impl((lx_byte_t*)(p), x)

// 24-bits
#   define lx_bits_get_u24_le(p)        lx_bits_get_u24_le_impl((lx_byte_t*)(p))
#   define lx_bits_get_s24_le(p)        lx_bits_get_s24_le_impl((lx_byte_t*)(p))
#   define lx_bits_get_u24_be(p)        lx_bits_get_u24_be_impl((lx_byte_t*)(p))
#   define lx_bits_get_s24_be(p)        lx_bits_get_s24_be_impl((lx_byte_t*)(p))

#   define lx_bits_set_u24_le(p, x)     lx_bits_set_u24_le_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_s24_le(p, x)     lx_bits_set_s24_le_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_u24_be(p, x)     lx_bits_set_u24_be_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_s24_be(p, x)     lx_bits_set_s24_be_impl((lx_byte_t*)(p), x)

// 32-bits
#   define lx_bits_get_u32_le(p)        lx_bits_get_u32_le_impl((lx_byte_t*)(p))
#   define lx_bits_get_s32_le(p)        lx_bits_get_s32_le_impl((lx_byte_t*)(p))
#   define lx_bits_get_u32_be(p)        lx_bits_get_u32_be_impl((lx_byte_t*)(p))
#   define lx_bits_get_s32_be(p)        lx_bits_get_s32_be_impl((lx_byte_t*)(p))

#   define lx_bits_set_u32_le(p, x)     lx_bits_set_u32_le_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_s32_le(p, x)     lx_bits_set_u32_le_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_u32_be(p, x)     lx_bits_set_u32_be_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_s32_be(p, x)     lx_bits_set_s32_be_impl((lx_byte_t*)(p), x)

// 64-bits
#   define lx_bits_get_u64_le(p)        lx_bits_get_u64_le_impl((lx_byte_t*)(p))
#   define lx_bits_get_s64_le(p)        lx_bits_get_s64_le_impl((lx_byte_t*)(p))
#   define lx_bits_get_u64_be(p)        lx_bits_get_u64_be_impl((lx_byte_t*)(p))
#   define lx_bits_get_s64_be(p)        lx_bits_get_s64_be_impl((lx_byte_t*)(p))

#   define lx_bits_set_u64_le(p, x)     lx_bits_set_u64_le_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_s64_le(p, x)     lx_bits_set_u64_le_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_u64_be(p, x)     lx_bits_set_u64_be_impl((lx_byte_t*)(p), x)
#   define lx_bits_set_s64_be(p, x)     lx_bits_set_s64_be_impl((lx_byte_t*)(p), x)

#endif /* LX_UNALIGNED_ACCESS_ENABLE */

#ifdef LX_WORDS_BIGENDIAN
#   define lx_bits_get_u16_ne(p)        lx_bits_get_u16_be(p)
#   define lx_bits_get_s16_ne(p)        lx_bits_get_s16_be(p)
#   define lx_bits_get_u24_ne(p)        lx_bits_get_u24_be(p)
#   define lx_bits_get_s24_ne(p)        lx_bits_get_s24_be(p)
#   define lx_bits_get_u32_ne(p)        lx_bits_get_u32_be(p)
#   define lx_bits_get_s32_ne(p)        lx_bits_get_s32_be(p)
#   define lx_bits_get_u64_ne(p)        lx_bits_get_u64_be(p)
#   define lx_bits_get_s64_ne(p)        lx_bits_get_s64_be(p)

#   define lx_bits_set_u16_ne(p, x)     lx_bits_set_u16_be(p, x)
#   define lx_bits_set_s16_ne(p, x)     lx_bits_set_s16_be(p, x)
#   define lx_bits_set_u24_ne(p, x)     lx_bits_set_u24_be(p, x)
#   define lx_bits_set_s24_ne(p, x)     lx_bits_set_s24_be(p, x)
#   define lx_bits_set_u32_ne(p, x)     lx_bits_set_u32_be(p, x)
#   define lx_bits_set_s32_ne(p, x)     lx_bits_set_s32_be(p, x)
#   define lx_bits_set_u64_ne(p, x)     lx_bits_set_u64_be(p, x)
#   define lx_bits_set_s64_ne(p, x)     lx_bits_set_s64_be(p, x)

#else
#   define lx_bits_get_u16_ne(p)        lx_bits_get_u16_le(p)
#   define lx_bits_get_s16_ne(p)        lx_bits_get_s16_le(p)
#   define lx_bits_get_u24_ne(p)        lx_bits_get_u24_le(p)
#   define lx_bits_get_s24_ne(p)        lx_bits_get_s24_le(p)
#   define lx_bits_get_u32_ne(p)        lx_bits_get_u32_le(p)
#   define lx_bits_get_s32_ne(p)        lx_bits_get_s32_le(p)
#   define lx_bits_get_u64_ne(p)        lx_bits_get_u64_le(p)
#   define lx_bits_get_s64_ne(p)        lx_bits_get_s64_le(p)

#   define lx_bits_set_u16_ne(p, x)     lx_bits_set_u16_le(p, x)
#   define lx_bits_set_s16_ne(p, x)     lx_bits_set_s16_le(p, x)
#   define lx_bits_set_u24_ne(p, x)     lx_bits_set_u24_le(p, x)
#   define lx_bits_set_s24_ne(p, x)     lx_bits_set_s24_le(p, x)
#   define lx_bits_set_u32_ne(p, x)     lx_bits_set_u32_le(p, x)
#   define lx_bits_set_s32_ne(p, x)     lx_bits_set_s32_le(p, x)
#   define lx_bits_set_u64_ne(p, x)     lx_bits_set_u64_le(p, x)
#   define lx_bits_set_s64_ne(p, x)     lx_bits_set_s64_le(p, x)

#endif /* LX_WORDS_BIGENDIAN */

// swap
#ifndef lx_bits_swap_u16
#   define lx_bits_swap_u16(x)          lx_bits_swap_u16_inline((lx_uint16_t)(x))
#endif

#ifndef lx_bits_swap_u24
#   define lx_bits_swap_u24(x)          lx_bits_swap_u24_inline((lx_uint32_t)(x))
#endif

#ifndef lx_bits_swap_u32
#   define lx_bits_swap_u32(x)          lx_bits_swap_u32_inline((lx_uint32_t)(x))
#endif

#ifndef lx_bits_swap_u64
#   define lx_bits_swap_u64(x)          lx_bits_swap_u64_inline((lx_uint64_t)(x))
#endif

#ifdef LX_WORDS_BIGENDIAN
#   define lx_bits_be_to_ne_u16(x)      ((lx_uint16_t)(x))
#   define lx_bits_le_to_ne_u16(x)      lx_bits_swap_u16(x)
#   define lx_bits_be_to_ne_u24(x)      ((lx_uint32_t)(x) & 0x00ffffff)
#   define lx_bits_le_to_ne_u24(x)      lx_bits_swap_u24(x)
#   define lx_bits_be_to_ne_u32(x)      ((lx_uint32_t)(x))
#   define lx_bits_le_to_ne_u32(x)      lx_bits_swap_u32(x)
#   define lx_bits_be_to_ne_u64(x)      ((lx_uint64_t)(x))
#   define lx_bits_le_to_ne_u64(x)      lx_bits_swap_u64(x)
#else
#   define lx_bits_be_to_ne_u16(x)      lx_bits_swap_u16(x)
#   define lx_bits_le_to_ne_u16(x)      ((lx_uint16_t)(x))
#   define lx_bits_be_to_ne_u24(x)      lx_bits_swap_u24(x)
#   define lx_bits_le_to_ne_u24(x)      ((lx_uint32_t)(x) & 0x00ffffff)
#   define lx_bits_be_to_ne_u32(x)      lx_bits_swap_u32(x)
#   define lx_bits_le_to_ne_u32(x)      ((lx_uint32_t)(x))
#   define lx_bits_be_to_ne_u64(x)      lx_bits_swap_u64(x)
#   define lx_bits_le_to_ne_u64(x)      ((lx_uint64_t)(x))
#endif

#define lx_bits_ne_to_be_u16(x)         lx_bits_be_to_ne_u16(x)
#define lx_bits_ne_to_le_u16(x)         lx_bits_le_to_ne_u16(x)
#define lx_bits_ne_to_be_u24(x)         lx_bits_be_to_ne_u24(x)
#define lx_bits_ne_to_le_u24(x)         lx_bits_le_to_ne_u24(x)
#define lx_bits_ne_to_be_u32(x)         lx_bits_be_to_ne_u32(x)
#define lx_bits_ne_to_le_u32(x)         lx_bits_le_to_ne_u32(x)
#define lx_bits_ne_to_be_u64(x)         lx_bits_be_to_ne_u64(x)
#define lx_bits_ne_to_le_u64(x)         lx_bits_le_to_ne_u64(x)

// cl0, count leading bit 0
#ifndef lx_bits_cl0_u32_be
#   define lx_bits_cl0_u32_be(x)        lx_bits_cl0_u32_be_inline(x)
#endif
#ifndef lx_bits_cl0_u32_le
#   define lx_bits_cl0_u32_le(x)        lx_bits_cl0_u32_le_inline(x)
#endif
#ifndef lx_bits_cl0_u64_be
#   define lx_bits_cl0_u64_be(x)        lx_bits_cl0_u64_be_inline(x)
#endif
#ifndef lx_bits_cl0_u64_le
#   define lx_bits_cl0_u64_le(x)        lx_bits_cl0_u64_le_inline(x)
#endif

// cl1, count leading bit 1
#ifndef lx_bits_cl1_u32_be
#   define lx_bits_cl1_u32_be(x)        lx_bits_cl0_u32_be(~(lx_uint32_t)(x))
#endif
#ifndef lx_bits_cl1_u32_le
#   define lx_bits_cl1_u32_le(x)        lx_bits_cl0_u32_le(~(lx_uint32_t)(x))
#endif
#ifndef lx_bits_cl1_u64_be
#   define lx_bits_cl1_u64_be(x)        lx_bits_cl0_u64_be(~(lx_uint64_t)(x))
#endif
#ifndef lx_bits_cl1_u64_le
#   define lx_bits_cl1_u64_le(x)        lx_bits_cl0_u64_le(~(lx_uint64_t)(x))
#endif

// cb1, count bit 1
#ifndef lx_bits_cb1_u32
#   define lx_bits_cb1_u32(x)           lx_bits_cb1_u32_inline(x)
#endif
#ifndef lx_bits_cb1_u64
#   define lx_bits_cb1_u64(x)           lx_bits_cb1_u64_inline(x)
#endif

// cb0, count bit 0
#ifndef lx_bits_cb0_u32
#   define lx_bits_cb0_u32(x)           ((x)? (lx_size_t)lx_bits_cb1_u32(~(lx_uint32_t)(x)) : 32)
#endif
#ifndef lx_bits_cb0_u64
#   define lx_bits_cb0_u64(x)           ((x)? (lx_size_t)lx_bits_cb1_u64(~(lx_uint64_t)(x)) : 64)
#endif

/* fb0, find the first bit 0
 *
 * find bit zero by little endian, fb0(...11101101) == 1
 * find bit zero by big endian, fb0(...11101101) == 27
 */
#ifndef lx_bits_fb0_u32_be
#   define lx_bits_fb0_u32_be(x)        ((x)? lx_bits_cl0_u32_be(~(lx_uint32_t)(x)) : 0)
#endif
#ifndef lx_bits_fb0_u32_le
#   define lx_bits_fb0_u32_le(x)        ((x)? lx_bits_cl0_u32_le(~(lx_uint32_t)(x)) : 0)
#endif
#ifndef lx_bits_fb0_u64_be
#   define lx_bits_fb0_u64_be(x)        ((x)? lx_bits_cl0_u64_be(~(lx_uint64_t)(x)) : 0)
#endif
#ifndef lx_bits_fb0_u64_le
#   define lx_bits_fb0_u64_le(x)        ((x)? lx_bits_cl0_u64_le(~(lx_uint64_t)(x)) : 0)
#endif

// fb1, find the first bit 1
#ifndef lx_bits_fb1_u32_be
#   define lx_bits_fb1_u32_be(x)        ((x)? lx_bits_cl0_u32_be(x) : 32)
#endif
#ifndef lx_bits_fb1_u32_le
#   define lx_bits_fb1_u32_le(x)        ((x)? lx_bits_cl0_u32_le(x) : 32)
#endif
#ifndef lx_bits_fb1_u64_be
#   define lx_bits_fb1_u64_be(x)        ((x)? lx_bits_cl0_u64_be(x) : 64)
#endif
#ifndef lx_bits_fb1_u64_le
#   define lx_bits_fb1_u64_le(x)        ((x)? lx_bits_cl0_u64_le(x) : 64)
#endif

// only for lx_size_t
#if LX_CPU_BIT64

#   define lx_bits_swap(x)              lx_bits_swap_u64(x)

#   define lx_bits_cl0_be(x)            lx_bits_cl0_u64_be(x)
#   define lx_bits_cl0_le(x)            lx_bits_cl0_u64_le(x)
#   define lx_bits_cl1_be(x)            lx_bits_cl1_u64_be(x)
#   define lx_bits_cl1_le(x)            lx_bits_cl1_u64_le(x)

#   define lx_bits_fb0_be(x)            lx_bits_fb0_u64_be(x)
#   define lx_bits_fb0_le(x)            lx_bits_fb0_u64_le(x)
#   define lx_bits_fb1_be(x)            lx_bits_fb1_u64_be(x)
#   define lx_bits_fb1_le(x)            lx_bits_fb1_u64_le(x)

#   define lx_bits_cb0(x)               lx_bits_cb0_u64(x)
#   define lx_bits_cb1(x)               lx_bits_cb1_u64(x)

#elif LX_CPU_BIT32

#   define lx_bits_swap(x)              lx_bits_swap_u32(x)

#   define lx_bits_cl0_be(x)            lx_bits_cl0_u32_be(x)
#   define lx_bits_cl0_le(x)            lx_bits_cl0_u32_le(x)
#   define lx_bits_cl1_be(x)            lx_bits_cl1_u32_be(x)
#   define lx_bits_cl1_le(x)            lx_bits_cl1_u32_le(x)

#   define lx_bits_fb0_be(x)            lx_bits_fb0_u32_be(x)
#   define lx_bits_fb0_le(x)            lx_bits_fb0_u32_le(x)
#   define lx_bits_fb1_be(x)            lx_bits_fb1_u32_be(x)
#   define lx_bits_fb1_le(x)            lx_bits_fb1_u32_le(x)

#   define lx_bits_cb0(x)               lx_bits_cb0_u32(x)
#   define lx_bits_cb1(x)               lx_bits_cb1_u32(x)

#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! get ubits32 integer
 *
 * @param p     the data pointer
 * @param b     the start bits
 * @param n     the bits count
 *
 * @return      the ubits32 integer
 */
lx_uint32_t     lx_bits_get_ubits32(lx_byte_t const* p, lx_size_t b, lx_size_t n);

/*! get sbits32 integer
 *
 * @param p     the data pointer
 * @param b     the start bits
 * @param n     the bits count
 *
 * @return      the ubits32 integer
 */
lx_sint32_t     lx_bits_get_sbits32(lx_byte_t const* p, lx_size_t b, lx_size_t n);

/* //////////////////////////////////////////////////////////////////////////////////////
 * bits
 */

static lx_inline lx_void_t lx_bits_set_u16_le_inline(lx_byte_t* p, lx_uint16_t x) {
    p[0] = (lx_byte_t)x;
    p[1] = (lx_byte_t)(x >> 8);
}

static lx_inline lx_void_t lx_bits_set_u16_be_inline(lx_byte_t* p, lx_uint16_t x) {
    p[0] = (lx_byte_t)(x >> 8);
    p[1] = (lx_byte_t)x;
}

static lx_inline lx_void_t lx_bits_set_u24_le_inline(lx_byte_t* p, lx_uint32_t x) {
    p[0] = (lx_byte_t)x;
    p[1] = (lx_byte_t)(x >> 8);
    p[2] = (lx_byte_t)(x >> 16);
}

static lx_inline lx_void_t lx_bits_set_u24_be_inline(lx_byte_t* p, lx_uint32_t x) {
    p[0] = (lx_byte_t)(x >> 16);
    p[1] = (lx_byte_t)(x >> 8);
    p[2] = (lx_byte_t)x;
}

static lx_inline lx_void_t lx_bits_set_u32_le_inline(lx_byte_t* p, lx_uint32_t x) {
    p[0] = (lx_byte_t)x;
    p[1] = (lx_byte_t)(x >> 8);
    p[2] = (lx_byte_t)(x >> 16);
    p[3] = (lx_byte_t)(x >> 24);
}

static lx_inline lx_void_t lx_bits_set_u32_be_inline(lx_byte_t* p, lx_uint32_t x) {
    p[0] = (lx_byte_t)(x >> 24);
    p[1] = (lx_byte_t)(x >> 16);
    p[2] = (lx_byte_t)(x >> 8);
    p[3] = (lx_byte_t)x;
}

static lx_inline lx_void_t lx_bits_set_u64_le_inline(lx_byte_t* p, lx_uint64_t x) {
    p[0] = (lx_byte_t)x;
    p[1] = (lx_byte_t)(x >> 8);
    p[2] = (lx_byte_t)(x >> 16);
    p[3] = (lx_byte_t)(x >> 24);
    p[4] = (lx_byte_t)(x >> 32);
    p[5] = (lx_byte_t)(x >> 40);
    p[6] = (lx_byte_t)(x >> 48);
    p[7] = (lx_byte_t)(x >> 56);
}

static lx_inline lx_void_t lx_bits_set_u64_be_inline(lx_byte_t* p, lx_uint64_t x) {
    p[0] = (lx_byte_t)(x >> 56);
    p[1] = (lx_byte_t)(x >> 48);
    p[2] = (lx_byte_t)(x >> 40);
    p[3] = (lx_byte_t)(x >> 32);
    p[4] = (lx_byte_t)(x >> 24);
    p[5] = (lx_byte_t)(x >> 16);
    p[6] = (lx_byte_t)(x >> 8);
    p[7] = (lx_byte_t)x;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * swap
 */

// swap
static lx_inline lx_uint16_t lx_bits_swap_u16_inline(lx_uint16_t x) {
    x = (x >> 8) | (x << 8);
    return x;
}

static lx_inline lx_uint32_t lx_bits_swap_u24_inline(lx_uint32_t x) {
    return (x >> 16) | (x & 0x0000ff00) | (x << 16);
}

static lx_inline lx_uint32_t lx_bits_swap_u32_inline(lx_uint32_t x) {
    x = ((x << 8) & 0xff00ff00) | ((x >> 8) & 0x00ff00ff);
    x = (x >> 16) | (x << 16);
    return x;
}

static lx_inline lx_hize_t lx_bits_swap_u64_inline(lx_hize_t x) {
    union {
        lx_hize_t u64;
        lx_uint32_t u32[2];
    } w, r;
    w.u64 = x;
    r.u32[0] = lx_bits_swap_u32(w.u32[1]);
    r.u32[1] = lx_bits_swap_u32(w.u32[0]);
    return r.u64;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * cl0
 */
static lx_inline lx_size_t lx_bits_cl0_u32_be_inline(lx_uint32_t x) {
    lx_check_return_val(x, 32);

    lx_size_t n = 31;
    if (x & 0xffff0000) { n -= 16;  x >>= 16;   }
    if (x & 0xff00)     { n -= 8;   x >>= 8;    }
    if (x & 0xf0)       { n -= 4;   x >>= 4;    }
    if (x & 0xc)        { n -= 2;   x >>= 2;    }
    if (x & 0x2)        { n--;                  }
    return n;
}

static lx_inline lx_size_t lx_bits_cl0_u32_le_inline(lx_uint32_t x) {
    lx_check_return_val(x, 32);
    lx_size_t n = 31;
    if (x & 0x0000ffff) { n -= 16;  } else x >>= 16;
    if (x & 0x00ff)     { n -= 8;   } else x >>= 8;
    if (x & 0x0f)       { n -= 4;   } else x >>= 4;
    if (x & 0x3)        { n -= 2;   } else x >>= 2;
    if (x & 0x1)        { n--;      }
    return n;
}

static lx_inline lx_size_t lx_bits_cl0_u64_be_inline(lx_uint64_t x) {
    lx_check_return_val(x, 64);
    lx_size_t n = lx_bits_cl0_u32_be((lx_uint32_t)(x >> 32));
    if (n == 32) {
        n += lx_bits_cl0_u32_be((lx_uint32_t)x);
    }
    return n;
}

static lx_inline lx_size_t lx_bits_cl0_u64_le_inline(lx_uint64_t x) {
    lx_check_return_val(x, 64);
    lx_size_t n = lx_bits_cl0_u32_le((lx_uint32_t)x);
    if (n == 32) {
        n += lx_bits_cl0_u32_le((lx_uint32_t)(x >> 32));
    }
    return n;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * cb1
 */
static lx_inline lx_size_t lx_bits_cb1_u32_inline(lx_uint32_t x) {
    lx_check_return_val(x, 0);
#if 0
    /*
     * 0x55555555 = 01010101010101010101010101010101
     * 0x33333333 = 00110011001100110011001100110011
     * 0x0f0f0f0f = 00001111000011110000111100001111
     * 0x00ff00ff = 00000000111111110000000011111111
     * 0x0000ffff = 00000000000000001111111111111111
     */

    x = (x & 0x55555555) + ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x & 0x0f0f0f0f) + ((x >> 4) & 0x0f0f0f0f);
    x = (x & 0x00ff00ff) + ((x >> 8) & 0x00ff00ff);
    x = (x & 0x0000ffff) + ((x >> 16) & 0x0000ffff);
#elif 0
    // mit hackmem count
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0f0f0f0f;
    x = x + (x >> 8);
    x = x + (x >> 16);
    x &= 0x7f;
#elif 0
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4) & 0x0f0f0f0f);
    x = (x * 0x01010101) >> 24;
#elif 0
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4) & 0x0f0f0f0f) % 255;
#else
    x = x - ((x >> 1) & 0x77777777) - ((x >> 2) & 0x33333333) - ((x >> 3) & 0x11111111);
    x = (x + (x >> 4)) & 0x0f0f0f0f;
    x = (x * 0x01010101) >> 24;
#endif
    return x;
}

static lx_inline lx_size_t lx_bits_cb1_u64_inline(lx_uint64_t x) {
    lx_check_return_val(x, 0);
#if 0
    x = x - ((x >> 1) & 0x5555555555555555L);
    x = (x & 0x3333333333333333L) + ((x >> 2) & 0x3333333333333333L);
    x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0fL;
    x = x + (x >> 8);
    x = x + (x >> 16);
    x = x + (x >> 32);
    x &= 0x7f;
#else
    x = x - ((x >> 1) & 0x7777777777777777ULL) - ((x >> 2) & 0x3333333333333333ULL) - ((x >> 3) & 0x1111111111111111ULL);
    x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0fULL;
    x = (x * 0x0101010101010101ULL) >> 56;
#endif
    return (lx_size_t)x;
}

#ifdef LX_CONFIG_TYPE_HAVE_FLOAT
/* //////////////////////////////////////////////////////////////////////////////////////
 * float
 */
static lx_inline lx_float_t lx_bits_get_float_le_inline(lx_byte_t const* p) {
#if defined(LX_UNALIGNED_ACCESS_ENABLE) \
    && !defined(LX_WORDS_BIGENDIAN)
    return *((lx_float_t*)p);
#else
    lx_ieee_float_t conv;
    conv.i = lx_bits_get_u32_le(p);
    return conv.f;
#endif
}

static lx_inline lx_float_t lx_bits_get_float_be_inline(lx_byte_t const* p) {
#if defined(LX_UNALIGNED_ACCESS_ENABLE) \
    && defined(LX_WORDS_BIGENDIAN)
    return *((lx_float_t*)p);
#else
    lx_ieee_float_t conv;
    conv.i = lx_bits_get_u32_be(p);
    return conv.f;
#endif
}

static lx_inline lx_void_t lx_bits_set_float_le_inline(lx_byte_t* p, lx_float_t x) {
#if defined(LX_UNALIGNED_ACCESS_ENABLE) \
    && !defined(LX_WORDS_BIGENDIAN)
    *((lx_float_t*)p) = x;
#else
    lx_ieee_float_t conv;
    conv.f = x;
    lx_bits_set_u32_le(p, conv.i);
#endif
}

static lx_inline lx_void_t lx_bits_set_float_be_inline(lx_byte_t* p, lx_float_t x) {
#if defined(LX_UNALIGNED_ACCESS_ENABLE) \
    && defined(LX_WORDS_BIGENDIAN)
    *((lx_float_t*)p) = x;
#else
    lx_ieee_float_t conv;
    conv.f = x;
    lx_bits_set_u32_be(p, conv.i);
#endif
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * double
 */
static lx_inline lx_double_t lx_bits_get_double_bbe_inline(lx_byte_t const* p) {
#if defined(LX_UNALIGNED_ACCESS_ENABLE) \
        && defined(LX_FLOAT_BIGENDIAN) \
            && defined(LX_WORDS_BIGENDIAN)
    return *((lx_double_t*)p);
#else
    union {
        lx_uint32_t i[2];
        lx_double_t d;
    } conv;
    conv.i[1] = lx_bits_get_u32_be(p);
    conv.i[0] = lx_bits_get_u32_be(p + 4);
    return conv.d;
#endif
}

static lx_inline lx_double_t lx_bits_get_double_ble_inline(lx_byte_t const* p) {
#if defined(LX_UNALIGNED_ACCESS_ENABLE) \
        && defined(LX_FLOAT_BIGENDIAN) \
            && !defined(LX_WORDS_BIGENDIAN)
    return *((lx_double_t*)p);
#else
    union {
        lx_uint32_t i[2];
        lx_double_t d;
    } conv;
    conv.i[1] = lx_bits_get_u32_le(p);
    conv.i[0] = lx_bits_get_u32_le(p + 4);
    return conv.d;
#endif
}

static lx_inline lx_double_t lx_bits_get_double_lbe_inline(lx_byte_t const* p) {
#if defined(LX_UNALIGNED_ACCESS_ENABLE) \
        && !defined(LX_FLOAT_BIGENDIAN) \
            && defined(LX_WORDS_BIGENDIAN)
    return *((lx_double_t*)p);
#else
    union {
        lx_uint32_t i[2];
        lx_double_t d;
    } conv;
    conv.i[0] = lx_bits_get_u32_be(p);
    conv.i[1] = lx_bits_get_u32_be(p + 4);
    return conv.d;
#endif
}

static lx_inline lx_double_t lx_bits_get_double_lle_inline(lx_byte_t const* p)
{
#if defined(LX_UNALIGNED_ACCESS_ENABLE) \
        && !defined(LX_FLOAT_BIGENDIAN) \
            && !defined(LX_WORDS_BIGENDIAN)
    return *((lx_double_t*)p);
#else
    union {
        lx_uint32_t i[2];
        lx_double_t d;
    } conv;
    conv.i[0] = lx_bits_get_u32_le(p);
    conv.i[1] = lx_bits_get_u32_le(p + 4);
    return conv.d;
#endif
}

// big double endian & big words endian
// 7 6 5 4 3 2 1 0
static lx_inline lx_void_t lx_bits_set_double_bbe_inline(lx_byte_t* p, lx_double_t x) {
#if defined(LX_UNALIGNED_ACCESS_ENABLE) \
        && defined(LX_FLOAT_BIGENDIAN) \
            && defined(LX_WORDS_BIGENDIAN)
    *((lx_double_t*)p) = x;
#else
    union {
        lx_uint32_t     i[2];
        lx_double_t     d;
    } conv;
    conv.d = x;
    lx_bits_set_u32_be(p,       conv.i[1]);
    lx_bits_set_u32_be(p + 4,   conv.i[0]);
#endif
}

// big double endian & litte words endian
// 4 5 6 7 0 1 2 3
static lx_inline lx_void_t lx_bits_set_double_ble_inline(lx_byte_t* p, lx_double_t x) {
#if defined(LX_UNALIGNED_ACCESS_ENABLE) \
        && defined(LX_FLOAT_BIGENDIAN) \
            && !defined(LX_WORDS_BIGENDIAN)
    *((lx_double_t*)p) = x;
#else
    union {
        lx_uint32_t     i[2];
        lx_double_t     d;
    } conv;
    conv.d = x;
    lx_bits_set_u32_le(p,       conv.i[1]);
    lx_bits_set_u32_le(p + 4,   conv.i[0]);
#endif
}

// litte double endian & big words endian
// 3 2 1 0 7 6 5 4
static lx_inline lx_void_t lx_bits_set_double_lbe_inline(lx_byte_t* p, lx_double_t x) {
#if defined(LX_UNALIGNED_ACCESS_ENABLE) \
        && !defined(LX_FLOAT_BIGENDIAN) \
            && defined(LX_WORDS_BIGENDIAN)
    *((lx_double_t*)p) = x;
#else
    union {
        lx_uint32_t     i[2];
        lx_double_t     d;
    } conv;
    conv.d = x;
    lx_bits_set_u32_be(p,       conv.i[0]);
    lx_bits_set_u32_be(p + 4,   conv.i[1]);
#endif
}

// litte double endian & litte words endian
// 0 1 2 3 4 5 6 7
static lx_inline lx_void_t lx_bits_set_double_lle_inline(lx_byte_t* p, lx_double_t x) {
#if defined(LX_UNALIGNED_ACCESS_ENABLE) \
        && !defined(LX_FLOAT_BIGENDIAN) \
            && !defined(LX_WORDS_BIGENDIAN)
    *((lx_double_t*)p) = x;
#else
    union {
        lx_uint32_t     i[2];
        lx_double_t     d;
    } conv;
    conv.d = x;
    lx_bits_set_u32_le(p,       conv.i[0]);
    lx_bits_set_u32_le(p + 4,   conv.i[1]);
#endif
}

#endif /* LX_CONFIG_TYPE_HAVE_FLOAT */

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif

