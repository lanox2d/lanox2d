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
 * @file        point.h
 *
 */
#ifndef LX_CORE_BASICTYPE_POINT_H
#define LX_CORE_BASICTYPE_POINT_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// points is equal?
#define lx_point_eq(a, b)       ((a)->x == (b)->x && (a)->y == (b)->y)

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! make point
 *
 * @param point     the point
 * @param x         the x
 * @param y         the y
 */
lx_void_t           lx_point_make(lx_point_ref_t point, lx_float_t x, lx_float_t y);

/*! make point with the integer value
 *
 * @param point     the point
 * @param x         the x
 * @param y         the y
 */
lx_void_t           lx_point_imake(lx_point_ref_t point, lx_long_t x, lx_long_t y);

/*! apply matrix to point
 *
 * @param point     the point
 * @param matrix    the matrix
 */
lx_void_t           lx_point_apply(lx_point_ref_t point, lx_matrix_ref_t matrix);

/*! apply matrix to point
 *
 * @param point     the point
 * @param applied   the applied point
 * @param matrix    the matrix
 */
lx_void_t           lx_point_apply2(lx_point_ref_t point, lx_point_ref_t applied, lx_matrix_ref_t matrix);

/*! the distance of the point to the other point
 *
 * @param point     the point
 * @param other     the other point
 */
lx_float_t          lx_point_distance(lx_point_ref_t point, lx_point_ref_t other);

/*! be equal to the other point?
 *
 * @param point     the point
 * @param other     the other point
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_point_near_eq(lx_point_ref_t point, lx_point_ref_t other);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


