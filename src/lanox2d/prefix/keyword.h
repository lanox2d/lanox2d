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
 * @file        keyword.h
 *
 */
#ifndef LX_PREFIX_KEYWORD_H
#define LX_PREFIX_KEYWORD_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"
#include "compiler.h"
#include "cpu.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the register keyword will be deprecated in C++
#ifndef __cplusplus
#   define lx_register                      register
#else
#   define lx_register
#endif
#define lx_volatile                         volatile

#if defined(LX_COMPILER_IS_MSVC)

#   define lx_func                          __FUNCTION__
#   define lx_file                          __FILE__
#   define lx_line                          __LINE__
#   define lx_date                          __DATE__
#   define lx_time                          __TIME__
#   define lx_asm                           __asm
#   define lx_inline                        __inline
#   define lx_inline_force                  __forceinline
#   define lx_cdecl                         __cdecl
#   define lx_stdcall                       __stdcall
#   define lx_fastcall                      __fastcall
#   define lx_thiscall                      __thiscall
#   define lx_packed
#   define lx_aligned(a)                    __declspec(align(a))

#elif defined(LX_COMPILER_IS_GCC)

#   define lx_func                          __FUNCTION__
#   define lx_file                          __FILE__
#   define lx_line                          __LINE__
#   define lx_date                          __DATE__
#   define lx_time                          __TIME__
#   define lx_asm                           __asm__
#   define lx_inline                        __inline__
#   define lx_inline_force                  __inline__ __attribute__((always_inline))
#   define lx_packed                        __attribute__((packed, aligned(1)))
#   define lx_aligned(a)                    __attribute__((aligned(a)))
    // gcc will generate attribute ignored warning
#   if defined(__x86_64) \
    || defined(__amd64__) \
    || defined(__amd64) \
    || defined(_M_IA64) \
    || defined(_M_X64)
#       define lx_cdecl
#       define lx_stdcall
#       define lx_fastcall
#       define lx_thiscall
#   else
#       define lx_cdecl                     __attribute__((__cdecl__))
#       define lx_stdcall                   __attribute__((__stdcall__))
#       define lx_fastcall                  __attribute__((__fastcall__))
#       define lx_thiscall                  __attribute__((__thiscall__))
#   endif

#elif defined(LX_COMPILER_IS_TINYC)

#   define lx_func                          __func__
#   define lx_file                          __FILE__
#   define lx_line                          __LINE__
#   define lx_date                          __DATE__
#   define lx_time                          __TIME__
#   define lx_asm                           __asm__
#   define lx_inline                        __inline__
#   define lx_inline_force                  __inline__ __attribute__((always_inline))
#   define lx_packed                        __attribute__((packed, aligned(1)))
#   define lx_aligned(a)                    __attribute__((aligned(a)))
#   define lx_cdecl                         __attribute__((cdecl))
#   define lx_stdcall                       __attribute__((stdcall))
#   define lx_fastcall                      __attribute__((fastcall))
#   define lx_thiscall                      __attribute__((thiscall))

#else

#   define lx_asm
#   define lx_inline                        inline
#   define lx_inline_force                  inline
#   define lx_func                          ""
#   define lx_file                          ""
#   define lx_date                          ""
#   define lx_time                          ""
#   define lx_line                          (0)

#   define lx_cdecl
#   define lx_stdcall
#   define lx_fastcall
#   define lx_thiscall
#   define lx_packed
#   define lx_aligned(a)

#endif

/*! @def lx_cpu_aligned
 *
 * the cpu byte alignment
 */
#if (LX_CPU_BITBYTE == 8)
#   define lx_cpu_aligned                   lx_aligned(8)
#elif (LX_CPU_BITBYTE == 4)
#   define lx_cpu_aligned                   lx_aligned(4)
#elif (LX_CPU_BITBYTE == 2)
#   define lx_cpu_aligned                   lx_aligned(2)
#else
#   error unknown cpu bytes
#endif

// like
#if defined(LX_COMPILER_IS_GCC) && LX_COMPILER_VERSION_BT(2, 0)
#   define lx_likely(x)                     __builtin_expect((x), 1)
#   define lx_unlikely(x)                   __builtin_expect((x), 0)
#else
#   define lx_likely(x)                     (x)
#   define lx_unlikely(x)                   (x)
#endif

// debug
#ifdef LX_DEBUG
#   define lx_debug_decl                    , lx_char_t const* func_, lx_size_t line_, lx_char_t const* file_
#   define lx_debug_vals                    , lx_func, lx_line, lx_file
#   define lx_debug_args                    , func_, line_, file_
#else
#   define lx_debug_decl
#   define lx_debug_vals
#   define lx_debug_args
#endif

// newline
#ifdef LX_CONFIG_OS_WINDOWS
#   define lx_newline                       "\r\n"
#else
#   define lx_newline                       "\n"
#endif

// the string only for the large mode
#ifdef LX_CONFIG_SMALL
#   define lx_lstring(x)                    lx_null
#else
#   define lx_lstring(x)                    x
#endif

// the string only for the debug mode
#ifdef LX_DEBUG
#   define lx_dstring(x)                    x
#else
#   define lx_dstring(x)                    lx_null
#endif

// extern c
#ifdef __cplusplus
#   define lx_extern_c                      extern "C"
#   define lx_extern_c_enter                extern "C" {
#   define lx_extern_c_leave                }
#else
#   define lx_extern_c
#   define lx_extern_c_enter
#   define lx_extern_c_leave
#endif

// export for the shared library
#if defined(LX_COMPILER_IS_MSVC)
#   define lx_export                        __declspec(dllexport)
#elif defined(LX_COMPILER_IS_GCC) && ((__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3))
#   define lx_export                        __attribute__((visibility("default")))
#else
#   define lx_export
#endif

#if defined(LX_COMPILER_IS_GCC) && LX_COMPILER_VERSION_BE(3, 0)
#   define lx_deprecated                    __attribute__((deprecated))
#elif defined(LX_COMPILER_IS_MSVC) && defined(_MSC_VER) && _MSC_VER >= 1300
#   define lx_deprecated                    __declspec(deprecated)
#else
#   define lx_deprecated
#endif

// has feature
#ifdef __has_feature
#   define lx_has_feature(x)                            __has_feature(x)
#else
#   define lx_has_feature(x)                            0
#endif

// has include
#ifdef __has_include
#   define lx_has_include(x)                            __has_include(x)
#else
#   define lx_has_include(x)                            0
#endif

// has builtin
#ifdef __has_builtin
#   define lx_has_builtin(x)                            __has_builtin(x)
#else
#   define lx_has_builtin(x)                            0
#endif

// no_sanitize_address
#if lx_has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
#   define lx_no_sanitize_address                       __attribute__((no_sanitize_address))
#else
#   define lx_no_sanitize_address
#endif

/*! the type reference keyword for defining LX_xxxx_ref_t
 *
 * typedef lx_typeref(xxxx);
 *
 *
 * suppress gcc 4.9 on c++ codes warning: 'lx_yyyy_t_' has a field 'lx_yyyy_t_::xxxx' whose type uses the anonymous namespace
 *
 * @code
 *
   typedef struct{}*    lx_xxxx_ref_t;

   typedef struct lx_yyyy_t_ {
       lx_xxxx_ref_t    xxxx;
   }lx_yyyy_t;

 *
 *
 * @endcode
 *
 */
#define lx_typeref(object)                              struct lx_##object##_dummy_t_{lx_int_t dummy;} const* lx_##object##_ref_t

// macros
#define lx_mstring(x)                                   #x
#define lx_mstring_ex(x)                                lx_mstring(x)

#define lx_mconcat(a, b)                                a##b
#define lx_mconcat_ex(a, b)                             lx_mconcat(a, b)

#define lx_mconcat3(a, b, c)                            a##b##c
#define lx_mconcat3_ex(a, b, c)                         lx_mconcat3(a, b, c)

#define lx_mconcat4(a, b, c, d)                         a##b##c##d
#define lx_mconcat4_ex(a, b, c, d)                      lx_mconcat4(a, b, c, d)

#define lx_mconcat5(a, b, c, d, e)                      a##b##c##d##e
#define lx_mconcat5_ex(a, b, c, d, e)                   lx_mconcat5(a, b, c, d, e)

#define lx_mconcat6(a, b, c, d, e, f)                   a##b##c##d##e##f
#define lx_mconcat6_ex(a, b, c, d, e, f)                lx_mconcat6(a, b, c, d, e, f)

#define lx_mconcat7(a, b, c, d, e, f, g)                a##b##c##d##e##f##g
#define lx_mconcat7_ex(a, b, c, d, e, f, g)             lx_mconcat7(a, b, c, d, e, f, g)

#define lx_mconcat8(a, b, c, d, e, f, g, h)             a##b##c##d##e##f##g##h
#define lx_mconcat8_ex(a, b, c, d, e, f, g, h)          lx_mconcat8(a, b, c, d, e, f, g, h)

#define lx_mconcat9(a, b, c, d, e, f, g, h, i)          a##b##c##d##e##f##g##h##i
#define lx_mconcat9_ex(a, b, c, d, e, f, g, h, i)       lx_mconcat9(a, b, c, d, e, f, g, h, i)

#define lx_mstrcat(a, b)                                a b
#define lx_mstrcat3(a, b, c)                            a b c
#define lx_mstrcat4(a, b, c, d)                         a b c d
#define lx_mstrcat5(a, b, c, d, e)                      a b c d e
#define lx_mstrcat6(a, b, c, d, e, f)                   a b c d e f
#define lx_mstrcat7(a, b, c, d, e, f, g)                a b c d e f g
#define lx_mstrcat8(a, b, c, d, e, f, g, h)             a b c d e f g h
#define lx_mstrcat9(a, b, c, d, e, f, g, h, i)          a b c d e f g h i


#endif


