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


