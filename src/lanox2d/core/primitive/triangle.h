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
 * @file        triangle.h
 *
 */
#ifndef LX_CORE_PRIMITIVE_TRIANGLE_H
#define LX_CORE_PRIMITIVE_TRIANGLE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "point.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline interfaces
 */

/*! make triangle
 *
 * @param triangle  the triangle
 * @param x0        the x0
 * @param y0        the y0
 * @param x1        the x1
 * @param y1        the y1
 * @param x2        the x2
 * @param y2        the y2
 */
static lx_inline lx_void_t lx_triangle_make(lx_triangle_ref_t triangle, lx_float_t x0, lx_float_t y0, lx_float_t x1, lx_float_t y1, lx_float_t x2, lx_float_t y2) {
    triangle->p0.x = x0;
    triangle->p0.y = y0;
    triangle->p1.x = x1;
    triangle->p1.y = y1;
    triangle->p2.x = x2;
    triangle->p2.y = y2;
}

/*! make triangle with the integer value
 *
 * @param triangle  the triangle
 * @param x0        the x0
 * @param y0        the y0
 * @param x1        the x1
 * @param y1        the y1
 * @param x2        the x2
 * @param y2        the y2
 */
static lx_inline lx_void_t lx_triangle_imake(lx_triangle_ref_t triangle, lx_long_t x0, lx_long_t y0, lx_long_t x1, lx_long_t y1, lx_long_t x2, lx_long_t y2) {
    lx_triangle_make(triangle, (lx_float_t)x0, (lx_float_t)y0, (lx_float_t)x1, (lx_float_t)y1, (lx_float_t)x2, (lx_float_t)y2);
}

/*! apply matrix to triangle
 *
 * @param triangle  the triangle
 * @param matrix    the matrix
 */
static lx_inline lx_void_t lx_triangle_apply(lx_triangle_ref_t triangle, lx_matrix_ref_t matrix) {
    lx_point_apply(&triangle->p0, matrix);
    lx_point_apply(&triangle->p1, matrix);
    lx_point_apply(&triangle->p2, matrix);
}

/*! apply matrix to triangle
 *
 * @param triangle  the triangle
 * @param applied   the applied triangle
 * @param matrix    the matrix
 */
static lx_inline lx_void_t lx_triangle_apply2(lx_triangle_ref_t triangle, lx_triangle_ref_t applied, lx_matrix_ref_t matrix) {
    lx_point_apply2(&triangle->p0, &applied->p0, matrix);
    lx_point_apply2(&triangle->p1, &applied->p1, matrix);
    lx_point_apply2(&triangle->p2, &applied->p2, matrix);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


