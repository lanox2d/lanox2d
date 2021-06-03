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
 * @file        line.h
 *
 */
#ifndef LX_CORE_PRIMITIVE_LINE_H
#define LX_CORE_PRIMITIVE_LINE_H

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

/*! make line
 *
 * @param line      the line
 * @param x0        the x0
 * @param y0        the y0
 * @param x1        the x1
 * @param y1        the y1
 */
lx_void_t           lx_line_make(lx_line_ref_t line, lx_float_t x0, lx_float_t y0, lx_float_t x1, lx_float_t y1);

/*! make line with the integer value
 *
 * @param line      the line
 * @param x0        the x0
 * @param y0        the y0
 * @param x1        the x1
 * @param y1        the y1
 */
lx_void_t           lx_line_imake(lx_line_ref_t line, lx_long_t x0, lx_long_t y0, lx_long_t x1, lx_long_t y1);

/*! apply matrix to line
 *
 * @param line      the line
 * @param matrix    the matrix
 */
lx_void_t           lx_line_apply(lx_line_ref_t line, lx_matrix_ref_t matrix);

/*! apply matrix to line
 *
 * @param line      the line
 * @param applied   the applied line
 * @param matrix    the matrix
 */
lx_void_t           lx_line_apply2(lx_line_ref_t line, lx_line_ref_t applied, lx_matrix_ref_t matrix);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


