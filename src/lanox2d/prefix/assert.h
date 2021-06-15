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
 * @file        assert.h
 *
 */
#ifndef LX_PREFIX_ASSERT_H
#define LX_PREFIX_ASSERT_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"
#include "trace.h"
#include "abort.h"
#include "check.h"
#include "keyword.h"
#include "compiler.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#ifdef LX_DEBUG
#   define lx_assert(x)                                     lx_assert_impl(!!(x), #x lx_debug_vals)
#   define lx_assert_and_check_abort(x)                     lx_assert(x)
#   define lx_assert_and_check_return(x)                    lx_assert(x)
#   define lx_assert_and_check_return_val(x, v)             lx_assert(x)
#   define lx_assert_and_check_goto(x, b)                   lx_assert(x)
#   define lx_assert_and_check_break(x)                     lx_assert(x)
#   define lx_assert_and_check_continue(x)                  lx_assert(x)
#   define lx_assert_and_check_break_state(x, s, v)         lx_assert(x)
#else
#   define lx_assert(x)                                     lx_assert_empty_impl()
#   define lx_assert_and_check_abort(x)                     lx_check_abort(x)
#   define lx_assert_and_check_return(x)                    lx_check_return(x)
#   define lx_assert_and_check_return_val(x, v)             lx_check_return_val(x, v)
#   define lx_assert_and_check_goto(x, b)                   lx_check_goto(x, b)
#   define lx_assert_and_check_break(x)                     lx_check_break(x)
#   define lx_assert_and_check_continue(x)                  lx_check_continue(x)
#   define lx_assert_and_check_break_state(x, s, v)         lx_check_break_state(x, s, v)
#endif

#ifdef LX_DEBUG
#   if defined(LX_COMPILER_IS_GCC) || defined(LX_COMPILER_IS_TINYC)
#       define lx_assertf(x, fmt, arg...)                                   do { if (!(x)) {lx_trace_a("expr[%s]: " fmt, #x, ##arg); lx_abort(); } } while(0)
#       define lx_assertf_and_check_abort(x, fmt, arg...)                   lx_assertf(x, fmt, ##arg)
#       define lx_assertf_and_check_return(x, fmt, arg...)                  lx_assertf(x, fmt, ##arg)
#       define lx_assertf_and_check_return_val(x, v, fmt, arg...)           lx_assertf(x, fmt, ##arg)
#       define lx_assertf_and_check_goto(x, b, fmt, arg...)                 lx_assertf(x, fmt, ##arg)
#       define lx_assertf_and_check_break(x, fmt, arg...)                   lx_assertf(x, fmt, ##arg)
#       define lx_assertf_and_check_continue(x, fmt, arg...)                lx_assertf(x, fmt, ##arg)
#   elif defined(LX_COMPILER_IS_MSVC) && LX_COMPILER_VERSION_BE(13, 0)
#       define lx_assertf(x, fmt, ...)                                      do { if (!(x)) {lx_trace_a("expr[%s]: " fmt, #x, __VA_ARGS__); lx_abort(); } } while(0)
#       define lx_assertf_and_check_abort(x, fmt, ...)                      lx_assertf(x, fmt, __VA_ARGS__)
#       define lx_assertf_and_check_return(x, fmt, ...)                     lx_assertf(x, fmt, __VA_ARGS__)
#       define lx_assertf_and_check_return_val(x, v, fmt, ...)              lx_assertf(x, fmt, __VA_ARGS__)
#       define lx_assertf_and_check_goto(x, b, fmt, ...)                    lx_assertf(x, fmt, __VA_ARGS__)
#       define lx_assertf_and_check_break(x, fmt, ...)                      lx_assertf(x, fmt, __VA_ARGS__)
#       define lx_assertf_and_check_continue(x, fmt, ...)                   lx_assertf(x, fmt, __VA_ARGS__)
#   else
#       define lx_assertf
#       define lx_assertf_and_check_abort
#       define lx_assertf_and_check_return
#       define lx_assertf_and_check_return_val
#       define lx_assertf_and_check_goto
#       define lx_assertf_and_check_break
#       define lx_assertf_and_check_continue
#   endif
#else
#   if defined(LX_COMPILER_IS_GCC) || defined(LX_COMPILER_IS_TINYC)
#       define lx_assertf(x, fmt, arg...)
#       define lx_assertf_and_check_abort(x, fmt, arg...)                   lx_check_abort(x)
#       define lx_assertf_and_check_return(x, fmt, arg...)                  lx_check_return(x)
#       define lx_assertf_and_check_return_val(x, v, fmt, arg...)           lx_check_return_val(x, v)
#       define lx_assertf_and_check_goto(x, b, fmt, arg...)                 lx_check_goto(x, b)
#       define lx_assertf_and_check_break(x, fmt, arg...)                   lx_check_break(x)
#       define lx_assertf_and_check_continue(x, fmt, arg...)                lx_check_continue(x)
#   elif defined(LX_COMPILER_IS_MSVC) && LX_COMPILER_VERSION_BE(13, 0)
#       define lx_assertf(x, fmt, ...)
#       define lx_assertf_and_check_abort(x, fmt, ...)                      lx_check_abort(x)
#       define lx_assertf_and_check_return(x, fmt, ...)                     lx_check_return(x)
#       define lx_assertf_and_check_return_val(x, v, fmt, ...)              lx_check_return_val(x, v)
#       define lx_assertf_and_check_goto(x, b, fmt, ...)                    lx_check_goto(x, b)
#       define lx_assertf_and_check_break(x, fmt, ...)                      lx_check_break(x)
#       define lx_assertf_and_check_continue(x, fmt, ...)                   lx_check_continue(x)
#   else
#       define lx_assertf
#       define lx_assertf_and_check_abort
#       define lx_assertf_and_check_return
#       define lx_assertf_and_check_return_val
#       define lx_assertf_and_check_goto
#       define lx_assertf_and_check_break
#       define lx_assertf_and_check_continue
#   endif
#endif

/*! the static assert
 *
 * @code
 *
 * lx_assert_static(sizeof(lx_uint32_t) == 4);
 *
 * @endcode
 */
#if lx_has_feature(c_static_assert)
#   define lx_assert_static(x)          _Static_assert(x, "")
#   define lx_assert_static_msg(x, m)   _Static_assert(x, m)
#elif defined(__cpp_static_assert)
#   define lx_assert_static(x)          static_assert(x, "")
#   define lx_assert_static_msg(x, m)   static_assert(x, m)
#else
#   define lx_assert_static(x)          do { typedef int lx_static_assert__[(x)? 1 : -1]; lx_volatile lx_static_assert__ a__; (lx_void_t)a__; } while(0)
#   define lx_assert_static_msg(x, m)   lx_assert_static(x)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */

#ifdef LX_DEBUG
static lx_inline lx_bool_t  lx_assert_impl(lx_bool_t x, lx_char_t const* expr lx_debug_decl)
{
    if (!x)
    {
        lx_trace_a("expr[%s] at %s(): %d, %s", expr lx_debug_args);
        lx_abort();
    }
    return x;
}
#else
static lx_inline lx_bool_t  lx_assert_empty_impl(lx_noarg_t)
{
    return lx_false;
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


