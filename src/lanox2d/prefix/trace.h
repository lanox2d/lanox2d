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
 * @file        trace.h
 *
 */
#ifndef LX_PREFIX_TRACE_H
#define LX_PREFIX_TRACE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"
#include "type.h"
#include "keyword.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#ifdef LX_DEBUG
#   define LX_TRACE_FILEPATH    lx_file
#else
#   define LX_TRACE_FILEPATH    lx_null
#endif

#if defined(LX_COMPILER_IS_GCC) || defined(LX_COMPILER_IS_TINYC)
#   define lx_trace_i(fmt, arg ...)           do { lx_trace(LX_TRACE_FILEPATH, fmt lx_newline, ## arg); } while (0)
#   define lx_trace_d(fmt, arg ...)           do { lx_trace(LX_TRACE_FILEPATH, fmt lx_newline, ## arg); } while (0)
#   define lx_trace_e(fmt, arg ...)           do { lx_trace(LX_TRACE_FILEPATH, "[error]: " fmt " at %s(): %d, %s" lx_newline, ##arg, lx_func, lx_line, lx_file); } while (0)
#   define lx_trace_a(fmt, arg ...)           do { lx_trace(LX_TRACE_FILEPATH, "[assert]: " fmt " at %s(): %d, %s" lx_newline, ##arg, lx_func, lx_line, lx_file); } while (0)
#   define lx_trace_w(fmt, arg ...)           do { lx_trace(LX_TRACE_FILEPATH, "[warning]: " fmt " at %s(): %d, %s" lx_newline, ##arg, lx_func, lx_line, lx_file); } while (0)
#elif defined(LX_COMPILER_IS_MSVC) && LX_COMPILER_VERSION_BE(13, 0)
#   define lx_trace_i(fmt, ...)               do { lx_trace(LX_TRACE_FILEPATH, fmt lx_newline, __VA_ARGS__); } while (0)
#   define lx_trace_d(fmt, ...)               do { lx_trace(LX_TRACE_FILEPATH, fmt lx_newline, __VA_ARGS__); } while (0)
#   define lx_trace_e(fmt, ...)               do { lx_trace(LX_TRACE_FILEPATH, "[error]: at %s(): %d, %s: " fmt lx_newline, lx_func, lx_line, lx_file, __VA_ARGS__); } while (0)
#   define lx_trace_a(fmt, ...)               do { lx_trace(LX_TRACE_FILEPATH, "[assert]: at %s(): %d, %s: " fmt lx_newline, lx_func, lx_line, lx_file, __VA_ARGS__); } while (0)
#   define lx_trace_w(fmt, ...)               do { lx_trace(LX_TRACE_FILEPATH, "[warning]: at %s(): %d, %s: " fmt lx_newline, lx_func, lx_line, lx_file, __VA_ARGS__); } while (0)
#else
#   define lx_trace_i
#   define lx_trace_d
#   define lx_trace_e
#   define lx_trace_a
#   define lx_trace_w
#endif

#if !defined(LX_DEBUG) || defined(LX_TRACE_DISABLED)
#   undef lx_trace_d
#   undef lx_trace_e
#   undef lx_trace_a
#   undef lx_trace_w
#   if defined(LX_COMPILER_IS_GCC) || defined(LX_COMPILER_IS_TINYC)
#       define lx_trace_d(fmt, arg ...)
#       define lx_trace_e(fmt, arg ...)
#       define lx_trace_a(fmt, arg ...)
#       define lx_trace_w(fmt, arg ...)
#   elif defined(LX_COMPILER_IS_MSVC) && LX_COMPILER_VERSION_BE(13, 0)
#       define lx_trace_d(fmt, ...)
#       define lx_trace_e(fmt, ...)
#       define lx_trace_a(fmt, ...)
#       define lx_trace_w(fmt, ...)
#   else
#       define lx_trace_d
#       define lx_trace_e
#       define lx_trace_a
#       define lx_trace_w
#   endif
#endif

// noimpl
#define lx_trace_noimpl()               lx_trace_w("noimpl")

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
lx_void_t lx_trace(lx_char_t const* filepath, lx_char_t const* fmt, ...);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


