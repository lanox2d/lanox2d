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
 * @file        type.h
 *
 */
#ifndef LX_PREFIX_TYPE_H
#define LX_PREFIX_TYPE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"
#include "keyword.h"
#include "compiler.h"
#include "cpu.h"
#include "endian.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// bool values
#define lx_true                     ((lx_bool_t)1)
#define lx_false                    ((lx_bool_t)0)

// null
#ifdef __cplusplus
#   define lx_null                  (0)
#else
#   define lx_null                  ((lx_pointer_t)0)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// basic
typedef signed int                  lx_int_t;
typedef unsigned int                lx_uint_t;
typedef signed short                lx_short_t;
typedef unsigned short              lx_ushort_t;
typedef lx_int_t                    lx_bool_t;
typedef signed char                 lx_int8_t;
typedef lx_int8_t                   lx_sint8_t;
typedef unsigned char               lx_uint8_t;
typedef lx_short_t                  lx_int16_t;
typedef lx_int16_t                  lx_sint16_t;
typedef lx_ushort_t                 lx_uint16_t;
typedef lx_int_t                    lx_int32_t;
typedef lx_int32_t                  lx_sint32_t;
typedef lx_uint_t                   lx_uint32_t;
typedef char                        lx_char_t;
typedef unsigned char               lx_uchar_t;
typedef lx_uint8_t                  lx_byte_t;
typedef void                        lx_void_t;
typedef lx_void_t*                  lx_pointer_t;
typedef lx_void_t const*            lx_cpointer_t;
typedef lx_pointer_t                lx_handle_t;

// no argument
#ifdef __cplusplus
#   define lx_noarg_t
#else
#   define lx_noarg_t               lx_void_t
#endif

// wchar
#ifdef LX_CONFIG_TYPE_HAVE_WCHAR
    typedef wchar_t                 lx_wchar_t;
#elif defined(LX_CONFIG_OS_WINDOWS)
#   if defined(_WCHAR_T_DEFINED) || defined(_NATIVE_WCHAR_T_DEFINED)
    typedef wchar_t                 lx_wchar_t;
#   else
    typedef lx_uint16_t             lx_wchar_t;
#   endif
#elif defined(__WCHAR_TYPE__)
typedef __WCHAR_TYPE__              lx_wchar_t;
#elif defined(__WCHAR_WIDTH__) && (__WCHAR_WIDTH__ == 2)
typedef lx_int16_t                  lx_wchar_t;
#elif defined(__WCHAR_WIDTH__) && (__WCHAR_WIDTH__ == 4)
typedef lx_int32_t                  lx_wchar_t;
#else
typedef lx_int32_t                  lx_wchar_t;
#endif

// int64
#if defined(LX_COMPILER_IS_MSVC)
typedef __int64                     lx_int64_t;
typedef unsigned __int64            lx_uint64_t;
#elif (defined(__LONG_WIDTH__) && __LONG_WIDTH__ == 8) || \
        (defined(__SIZEOF_LONG__) && __SIZEOF_LONG__ == 8)
typedef signed long                 lx_int64_t;
typedef unsigned long               lx_uint64_t;
#else
typedef signed long long            lx_int64_t;
typedef unsigned long long          lx_uint64_t;
#endif
typedef lx_int64_t                  lx_sint64_t;

// hong and hize
typedef lx_sint64_t                 lx_hong_t;
typedef lx_uint64_t                 lx_hize_t;

// long and size
#if defined(LX_CONFIG_OS_WINDOWS) && LX_CPU_BIT64
typedef lx_int64_t                  lx_long_t;
typedef lx_uint64_t                 lx_ulong_t;
#else
typedef signed long                 lx_long_t;
typedef unsigned long               lx_ulong_t;
#endif
typedef lx_ulong_t                  lx_size_t;

// integer pointer
typedef lx_long_t                   lx_ptrdiff_t;
typedef lx_ulong_t                  lx_uintptr_t;

// float number
typedef float                       lx_float_t;
typedef double                      lx_double_t;

// fixed
typedef lx_int32_t                  lx_fixed6_t;
typedef lx_int32_t                  lx_fixed16_t;
typedef lx_int32_t                  lx_fixed30_t;
typedef lx_fixed16_t                lx_fixed_t;

/// the file ref type
typedef lx_typeref(file);

/// the value type
typedef union lx_value_t_ {
    lx_uint8_t                      u8;
    lx_sint8_t                      s8;
    lx_char_t                       c;
    lx_wchar_t                      wc;
    lx_bool_t                       b;
    lx_uint16_t                     u16;
    lx_sint16_t                     s16;
    lx_uint32_t                     u32;
    lx_sint32_t                     s32;
    lx_uint64_t                     u64;
    lx_sint64_t                     s64;
    lx_size_t                       ul;
    lx_long_t                       l;
    lx_pointer_t                    ptr;
    lx_cpointer_t                   cptr;
    lx_file_ref_t                   file;
    lx_handle_t                     handle;
    lx_char_t*                      str;
    lx_char_t const*                cstr;
    lx_wchar_t*                     wstr;
    lx_wchar_t const*               wcstr;
    lx_float_t                      f;
    lx_double_t                     d;
}lx_value_t, *lx_value_ref_t;

#endif


