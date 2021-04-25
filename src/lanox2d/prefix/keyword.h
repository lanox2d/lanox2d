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
#   define __lx_register__                      register
#else
#   define __lx_register__
#endif
#define __lx_volatile__                         volatile

#if defined(LX_COMPILER_IS_MSVC)

#   define __lx_func__                          __FUNCTION__
#   define __lx_file__                          __FILE__
#   define __lx_line__                          __LINE__
#   define __lx_date__                          __DATE__
#   define __lx_time__                          __TIME__
#   define __lx_asm__                           __asm
#   define __lx_inline__                        __inline
#   define __lx_inline_force__                  __forceinline
#   define __lx_cdecl__                         __cdecl
#   define __lx_stdcall__                       __stdcall
#   define __lx_fastcall__                      __fastcall
#   define __lx_thiscall__                      __thiscall
#   define __lx_packed__
#   define __lx_aligned__(a)                    __declspec(align(a))

#elif defined(LX_COMPILER_IS_GCC)

#   define __lx_func__                          __FUNCTION__
#   define __lx_file__                          __FILE__
#   define __lx_line__                          __LINE__
#   define __lx_date__                          __DATE__
#   define __lx_time__                          __TIME__
#   define __lx_asm__                           __asm__
#   define __lx_inline__                        __inline__
#   define __lx_inline_force__                  __inline__ __attribute__((always_inline))
#   define __lx_packed__                        __attribute__((packed, aligned(1)))
#   define __lx_aligned__(a)                    __attribute__((aligned(a)))
    // gcc will generate attribute ignored warning
#   if defined(__x86_64) \
    || defined(__amd64__) \
    || defined(__amd64) \
    || defined(_M_IA64) \
    || defined(_M_X64)
#       define __lx_cdecl__
#       define __lx_stdcall__
#       define __lx_fastcall__
#       define __lx_thiscall__
#   else
#       define __lx_cdecl__                     __attribute__((__cdecl__))
#       define __lx_stdcall__                   __attribute__((__stdcall__))
#       define __lx_fastcall__                  __attribute__((__fastcall__))
#       define __lx_thiscall__                  __attribute__((__thiscall__))
#   endif

#elif defined(LX_COMPILER_IS_TINYC)

#   define __lx_func__                          __func__
#   define __lx_file__                          __FILE__
#   define __lx_line__                          __LINE__
#   define __lx_date__                          __DATE__
#   define __lx_time__                          __TIME__
#   define __lx_asm__                           __asm__
#   define __lx_inline__                        __inline__
#   define __lx_inline_force__                  __inline__ __attribute__((always_inline))
#   define __lx_packed__                        __attribute__((packed, aligned(1)))
#   define __lx_aligned__(a)                    __attribute__((aligned(a)))
#   define __lx_cdecl__                         __attribute__((cdecl))
#   define __lx_stdcall__                       __attribute__((stdcall))
#   define __lx_fastcall__                      __attribute__((fastcall))
#   define __lx_thiscall__                      __attribute__((thiscall))

#else

#   define __lx_asm__
#   define __lx_inline__                        inline
#   define __lx_inline_force__                  inline
#   define __lx_func__                          ""
#   define __lx_file__                          ""
#   define __lx_date__                          ""
#   define __lx_time__                          ""
#   define __lx_line__                          (0)

#   define __lx_cdecl__
#   define __lx_stdcall__
#   define __lx_fastcall__
#   define __lx_thiscall__
#   define __lx_packed__
#   define __lx_aligned__(a)

#endif

/*! @def __lx_cpu_aligned__
 *
 * the cpu byte alignment
 */
#if (LX_CPU_BITBYTE == 8)
#   define __lx_cpu_aligned__                   __lx_aligned__(8)
#elif (LX_CPU_BITBYTE == 4)
#   define __lx_cpu_aligned__                   __lx_aligned__(4)
#elif (LX_CPU_BITBYTE == 2)
#   define __lx_cpu_aligned__                   __lx_aligned__(2)
#else
#   error unknown cpu bytes
#endif

// like
#if defined(LX_COMPILER_IS_GCC) && LX_COMPILER_VERSION_BT(2, 0)
#   define __lx_likely__(x)                     __builtin_expect((x), 1)
#   define __lx_unlikely__(x)                   __builtin_expect((x), 0)
#else
#   define __lx_likely__(x)                     (x)
#   define __lx_unlikely__(x)                   (x)
#endif

// debug
#ifdef __lx_debug__
#   define __lx_debug_decl__                    , LX_char_t const* func_, LX_size_t line_, LX_char_t const* file_
#   define __lx_debug_vals__                    , __lx_func__, __lx_line__, __lx_file__
#   define __lx_debug_args__                    , func_, line_, file_
#else
#   define __lx_debug_decl__
#   define __lx_debug_vals__
#   define __lx_debug_args__
#endif

// small
#undef __lx_small__
#ifdef LX_CONFIG_SMALL
#   define __lx_small__
#endif

// newline
#ifdef LX_CONFIG_OS_WINDOWS
#   define __lx_newline__                       "\r\n"
#else
#   define __lx_newline__                       "\n"
#endif

// the string only for the large mode
#ifdef __lx_small__
#   define __lx_lstring__(x)                    lx_null
#else
#   define __lx_lstring__(x)                    x
#endif

// the string only for the debug mode
#ifdef __lx_debug__
#   define __lx_dstring__(x)                    x
#else
#   define __lx_dstring__(x)                    lx_null
#endif

// extern c
#ifdef __cplusplus
#   define __lx_extern_c__                      extern "C"
#   define __lx_extern_c_enter__                extern "C" {
#   define __lx_extern_c_leave__                }
#else
#   define __lx_extern_c__
#   define __lx_extern_c_enter__
#   define __lx_extern_c_leave__
#endif

// export for the shared library
#if defined(LX_COMPILER_IS_MSVC)
#   define __lx_export__                        __declspec(dllexport)
#elif defined(LX_COMPILER_IS_GCC) && ((__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3))
#   define __lx_export__                        __attribute__((visibility("default")))
#else
#   define __lx_export__
#endif

#if defined(LX_COMPILER_IS_GCC) && LX_COMPILER_VERSION_BE(3, 0)
#   define __lx_deprecated__                    __attribute__((deprecated))
#elif defined(LX_COMPILER_IS_MSVC) && defined(_MSC_VER) && _MSC_VER >= 1300
#   define __lx_deprecated__                    __declspec(deprecated)
#else
#   define __lx_deprecated__
#endif

// has feature
#ifdef __has_feature
#   define __lx_has_feature__(x)                            __has_feature(x)
#else
#   define __lx_has_feature__(x)                            0
#endif

// has include
#ifdef __has_include
#   define __lx_has_include__(x)                            __has_include(x)
#else
#   define __lx_has_include__(x)                            0
#endif

// has builtin
#ifdef __has_builtin
#   define __lx_has_builtin__(x)                            __has_builtin(x)
#else
#   define __lx_has_builtin__(x)                            0
#endif

// no_sanitize_address
#if __lx_has_feature__(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
#   define __lx_no_sanitize_address__                       __attribute__((no_sanitize_address))
#else
#   define __lx_no_sanitize_address__
#endif

// thread local
#if __lx_has_feature__(c_thread_local)
#   define __lx_thread_local__                              _Thread_local
#elif defined(LX_CONFIG_KEYWORD_HAVE_Thread_local)
#   define __lx_thread_local__                              _Thread_local
#elif defined(LX_CONFIG_KEYWORD_HAVE__thread)
#   define __lx_thread_local__                              __thread
#elif defined(LX_COMPILER_IS_MSVC) || defined(LX_COMPILER_IS_BORLAND)
#   define __lx_thread_local__                              __declspec(thread)
#endif

/* c function overloadable
 *
 * @code
    static __lx_inline__ LX_void_t test(LX_int_t a) __lx_overloadable__
    {
        LX_trace_i("test1: %d", a);
    }
    static __lx_inline__ LX_void_t test(LX_int_t a, LX_int_t b) __lx_overloadable__
    {
        LX_trace_i("test2: %d %d", a, b);
    }
 * @endcode
 *
 * If the compiler does not support __lx_overloadable__, we can use the following code to implement function overload.
 *
 * @code
    #define test_n(a, b, ...) test_impl(a, b)
    #define test(a, args ...) test_n(a, ##args, 0, 0, 0)
    static __lx_inline__ LX_void_t test_impl(LX_int_t a, LX_int_t b)
    {
        LX_trace_i("test: %d %d", a, b);
    }

    test(1);
    test(1, 2);
 * @endcode
 */
#if defined(LX_COMPILER_IS_GCC) || defined(LX_COMPILER_IS_CLANG)
#   define __lx_overloadable__                              __attribute__((overloadable))
#else
#   define __lx_overloadable__
#endif

/*! the type reference keyword for defining LX_xxxx_ref_t
 *
 * typedef __lx_typeref__(xxxx);
 *
 *
 * suppress gcc 4.9 on c++ codes warning: '__lx_yyyy_t' has a field '__lx_yyyy_t::xxxx' whose type uses the anonymous namespace
 *
 * @code
 *
   typedef struct{}*    LX_xxxx_ref_t;

   typedef struct __lx_yyyy_t
   {
       LX_xxxx_ref_t    xxxx;

   }__lx_yyyy_t;

 *
 *
 * @endcode
 *
 */
#define __lx_typeref__(object)                              struct __lx_##object##_dummy_t{LX_int_t dummy;} const* LX_##object##_ref_t

// macros
#define __lx_mstring__(x)                                   #x
#define __lx_mstring_ex__(x)                                __lx_mstring__(x)

#define __lx_mconcat__(a, b)                                a##b
#define __lx_mconcat_ex__(a, b)                             __lx_mconcat__(a, b)

#define __lx_mconcat3__(a, b, c)                            a##b##c
#define __lx_mconcat3_ex__(a, b, c)                         __lx_mconcat3__(a, b, c)

#define __lx_mconcat4__(a, b, c, d)                         a##b##c##d
#define __lx_mconcat4_ex__(a, b, c, d)                      __lx_mconcat4__(a, b, c, d)

#define __lx_mconcat5__(a, b, c, d, e)                      a##b##c##d##e
#define __lx_mconcat5_ex__(a, b, c, d, e)                   __lx_mconcat5__(a, b, c, d, e)

#define __lx_mconcat6__(a, b, c, d, e, f)                   a##b##c##d##e##f
#define __lx_mconcat6_ex__(a, b, c, d, e, f)                __lx_mconcat6__(a, b, c, d, e, f)

#define __lx_mconcat7__(a, b, c, d, e, f, g)                a##b##c##d##e##f##g
#define __lx_mconcat7_ex__(a, b, c, d, e, f, g)             __lx_mconcat7__(a, b, c, d, e, f, g)

#define __lx_mconcat8__(a, b, c, d, e, f, g, h)             a##b##c##d##e##f##g##h
#define __lx_mconcat8_ex__(a, b, c, d, e, f, g, h)          __lx_mconcat8__(a, b, c, d, e, f, g, h)

#define __lx_mconcat9__(a, b, c, d, e, f, g, h, i)          a##b##c##d##e##f##g##h##i
#define __lx_mconcat9_ex__(a, b, c, d, e, f, g, h, i)       __lx_mconcat9__(a, b, c, d, e, f, g, h, i)

#define __lx_mstrcat__(a, b)                                a b
#define __lx_mstrcat3__(a, b, c)                            a b c
#define __lx_mstrcat4__(a, b, c, d)                         a b c d
#define __lx_mstrcat5__(a, b, c, d, e)                      a b c d e
#define __lx_mstrcat6__(a, b, c, d, e, f)                   a b c d e f
#define __lx_mstrcat7__(a, b, c, d, e, f, g)                a b c d e f g
#define __lx_mstrcat8__(a, b, c, d, e, f, g, h)             a b c d e f g h
#define __lx_mstrcat9__(a, b, c, d, e, f, g, h, i)          a b c d e f g h i


#endif


