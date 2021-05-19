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
 * @file        utils.h
 *
 */
#ifndef LX_PREFIX_UTILS_H
#define LX_PREFIX_UTILS_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "cpu.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/// abs
#define lx_abs(x)                       ((x) > 0? (x) : -(x))

/// max
#define lx_max(x, y)                    (((x) > (y))? (x) : (y))

/// min
#define lx_min(x, y)                    (((x) < (y))? (x) : (y))

/// max3
#define lx_max3(x, y, z)                (((x) > (y))? (((x) > (z))? (x) : (z)) : (((y) > (z))? (y) : (z)))

/// min3
#define lx_min3(x, y, z)                (((x) < (y))? (((x) < (z))? (x) : (z)) : (((y) < (z))? (y) : (z)))

/// the number of entries in the array
#define lx_arrayn(x)                    ((sizeof((x)) / sizeof((x)[0])))

/// ispow2: 1, 2, 4, 8, 16, 32, ...
#define lx_ispow2(x)                    (!((x) & ((x) - 1)) && (x))

/// align2
#define lx_align2(x)                    (((x) + 1) >> 1 << 1)

/// align4
#define lx_align4(x)                    (((x) + 3) >> 2 << 2)

/// align8
#define lx_align8(x)                    (((x) + 7) >> 3 << 3)

/// align
#define lx_align(x, b)                  (((lx_size_t)(x) + ((lx_size_t)(b) - 1)) & ~((lx_size_t)(b) - 1))

/// align u32
#define lx_align_u32(x, b)              (((lx_uint32_t)(x) + ((lx_uint32_t)(b) - 1)) & ~((lx_uint32_t)(b) - 1))

/// align u64
#define lx_align_u64(x, b)              (((lx_uint64_t)(x) + ((lx_uint64_t)(b) - 1)) & ~((lx_uint64_t)(b) - 1))

/// align by pow2
#define lx_align_pow2(x)                (((x) > 1)? (lx_ispow2(x)? (x) : ((lx_size_t)1 << (32 - lx_bits_cl0_u32_be((lx_uint32_t)(x))))) : 1)

/*! @def lx_align_cpu
 *
 * align by cpu bytes
 */
#if LX_CPU_BIT64
#   define lx_align_cpu(x)              lx_align8(x)
#else
#   define lx_align_cpu(x)              lx_align4(x)
#endif

/// offsetof
#if defined(LX_COMPILER_IS_GCC) \
    &&  LX_COMPILER_VERSION_BE(4, 1)
#   define lx_offsetof(s, m)            (lx_size_t)__builtin_offsetof(s, m)
#else
#   define lx_offsetof(s, m)            (lx_size_t)&(((s const*)0)->m)
#endif

/// container of
#define lx_container_of(s, m, p)        ((s*)(((lx_byte_t*)(p)) - lx_offsetof(s, m)))

/// memsizeof
#define lx_memsizeof(s, m)              sizeof(((s const*)0)->m)

/// memtailof
#define lx_memtailof(s, m)              (lx_offsetof(s, m) + lx_memsizeof(s, m))

/// memdiffof: lm - rm
#define lx_memdiffof(s, lm, rm)         (lx_memtailof(s, lm) - lx_memtailof(s, rm))

/// check the offset and size of member for struct or union
#define lx_memberof_eq(ls, lm, rs, rm)  ((lx_offsetof(ls, lm) == lx_offsetof(rs, rm)) && (lx_memsizeof(ls, lm) == lx_memsizeof(rs, rm)))

/// pointer to bool
#define lx_p2b(x)                       ((lx_bool_t)(lx_size_t)(x))

/// pointer to u8
#define lx_p2u8(x)                      ((lx_uint8_t)(lx_size_t)(x))

/// pointer to u16
#define lx_p2u16(x)                     ((lx_uint16_t)(lx_size_t)(x))

/// pointer to u32
#define lx_p2u32(x)                     ((lx_uint32_t)(lx_size_t)(x))

/// pointer to u64
#define lx_p2u64(x)                     ((lx_uint64_t)(lx_size_t)(x))

/// pointer to s8
#define lx_p2s8(x)                      ((lx_sint8_t)(lx_long_t)(x))

/// pointer to s16
#define lx_p2s16(x)                     ((lx_sint16_t)(lx_long_t)(x))

/// pointer to s32
#define lx_p2s32(x)                     ((lx_sint32_t)(lx_long_t)(x))

/// pointer to s64
#define lx_p2s64(x)                     ((lx_sint64_t)(lx_long_t)(x))

/// bool to pointer
#define lx_b2p(x)                       ((lx_pointer_t)(lx_size_t)(x))

/// unsigned integer to pointer
#define lx_u2p(x)                       ((lx_pointer_t)(lx_size_t)(x))

/// integer to pointer
#define lx_i2p(x)                       ((lx_pointer_t)(lx_long_t)(x))

/// swap
#define lx_swap(t, l, r)                do { t p_ = (r); (r) = (l); (l) = p_; } while (0)


#endif


