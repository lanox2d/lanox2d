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
 * @file        libc.h
 *
 */
#ifndef LX_BASE_LIBC_H
#define LX_BASE_LIBC_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! printf
 *
 * @param format    the format string
 *
 * @return          the real size
 */
lx_int_t            lx_printf(lx_char_t const* format, ...);

/*! sprintf
 *
 * @param s         the string data
 * @param format    the format string
 *
 * @return          the real size
 */
lx_int_t            lx_sprintf(lx_char_t* s, lx_char_t const* format, ...);

/*! snprintf
 *
 * @param s         the string data
 * @param n         the string size
 * @param format    the format string
 *
 * @return          the real size
 */
lx_int_t            lx_snprintf(lx_char_t* s, lx_size_t n, lx_char_t const* format, ...);

/*! vsnprintf
 *
 * @param s         the string data
 * @param n         the string size
 * @param format    the format string
 * @param args      the arguments
 *
 * @return          the real size
 */
lx_int_t            lx_vsnprintf(lx_char_t* s, lx_size_t n, lx_char_t const* format, va_list args);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


