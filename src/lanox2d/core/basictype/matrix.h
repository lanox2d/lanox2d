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
 * @file        matrix.h
 *
 */
#ifndef LX_CORE_BASICTYPE_MATRIX_H
#define LX_CORE_BASICTYPE_MATRIX_H

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

/*! init matrix
 *
 * @param matrix    the matrix
 * @param sx        the x-scale
 * @param kx        the x-skew
 * @param ky        the y-skew
 * @param sy        the y-scale
 * @param tx        the x-translate
 * @param ty        the y-translate
 */
lx_void_t           lx_matrix_init(lx_matrix_ref_t matrix, lx_float_t sx, lx_float_t kx, lx_float_t ky, lx_float_t sy, lx_float_t tx, lx_float_t ty);

/*! init matrix from the rotate degree
 *
 * @param matrix    the matrix
 * @param degrees   the rotate degrees
 */
lx_void_t           lx_matrix_init_rotate(lx_matrix_ref_t matrix, lx_float_t degrees);

/*! init matrix from the rotate degree by the coordinate: (px, py)
 *
 * @param matrix    the matrix
 * @param degrees   the rotate degrees
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 */
lx_void_t           lx_matrix_init_rotatep(lx_matrix_ref_t matrix, lx_float_t degrees, lx_float_t px, lx_float_t py);

/*! init matrix from the sin and cos value
 *
 * @param matrix    the matrix
 * @param sin       the sin value
 * @param cos       the cos value
 */
lx_void_t           lx_matrix_init_sincos(lx_matrix_ref_t matrix, lx_float_t sin, lx_float_t cos);

/*! init matrix from the sin and cos value by the coordinate: (px, py)
 *
 * @param matrix    the matrix
 * @param sin       the sin value
 * @param cos       the cos value
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 */
lx_void_t           lx_matrix_init_sincosp(lx_matrix_ref_t matrix,  lx_float_t sin, lx_float_t cos, lx_float_t px, lx_float_t py);

/*! init matrix from the skew value
 *
 * @param matrix    the matrix
 * @param kx        the x-skew
 * @param ky        the y-skew
 */
lx_void_t           lx_matrix_init_skew(lx_matrix_ref_t matrix, lx_float_t kx, lx_float_t ky);

/*! init matrix from the skew value by the coordinate: (px, py)
 *
 * @param matrix    the matrix
 * @param kx        the x-skew
 * @param ky        the y-skew
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 */
lx_void_t           lx_matrix_init_skewp(lx_matrix_ref_t matrix, lx_float_t kx, lx_float_t ky, lx_float_t px, lx_float_t py);

/*! init matrix from the scale value
 *
 * @param matrix    the matrix
 * @param sx        the x-scale
 * @param sy        the y-scale
 */
lx_void_t           lx_matrix_init_scale(lx_matrix_ref_t matrix, lx_float_t sx, lx_float_t sy);

/*! init matrix from the scale value by the coordinate: (px, py)
 *
 * @param matrix    the matrix
 * @param sx        the x-scale
 * @param sy        the y-scale
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 */
lx_void_t           lx_matrix_init_scalep(lx_matrix_ref_t matrix, lx_float_t sx, lx_float_t sy, lx_float_t px, lx_float_t py);

/*! init matrix from the translate value
 *
 * @param matrix    the matrix
 * @param tx        the x-translate
 * @param ty        the y-translate
 */
lx_void_t           lx_matrix_init_translate(lx_matrix_ref_t matrix, lx_float_t tx, lx_float_t ty);

/*! reset to the identity matrix
 *
 * @param matrix    the matrix
 */
lx_void_t           lx_matrix_clear(lx_matrix_ref_t matrix);

/*! invert matrix
 *
 * @param matrix    the matrix
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_matrix_invert(lx_matrix_ref_t matrix);

/*! copy matrix
 *
 * @param matrix    the matrix
 * @param copied    the copied matrix
 */
lx_void_t           lx_matrix_copy(lx_matrix_ref_t matrix, lx_matrix_ref_t copied);

/*! is the identity matrix?
 *
 * @param matrix    the matrix
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_matrix_identity(lx_matrix_ref_t matrix);

/*! transform matrix with the given rotate degrees
 *
 * matrix = matrix * factor
 *
 * @param matrix    the matrix
 * @param degrees   the rotate degrees
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_matrix_rotate(lx_matrix_ref_t matrix, lx_float_t degrees);

/*! transform matrix with the given rotate degrees
 *
 * matrix = factor * matrix
 *
 * @param matrix    the matrix
 * @param degrees   the rotate degrees
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_matrix_rotate_lhs(lx_matrix_ref_t matrix, lx_float_t degrees);

/*! transform matrix with the given rotate degrees by the coordinate: (px, py)
 *
 * matrix = matrix * factor
 *
 * @param matrix    the matrix
 * @param degrees   the rotate degrees
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_matrix_rotatep(lx_matrix_ref_t matrix, lx_float_t degrees, lx_float_t px, lx_float_t py);

/*! transform matrix with the given rotate degrees by the coordinate: (px, py)
 *
 * matrix = factor * matrix
 *
 * @param matrix    the matrix
 * @param degrees   the rotate degrees
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_matrix_rotatep_lhs(lx_matrix_ref_t matrix, lx_float_t degrees, lx_float_t px, lx_float_t py);

/*! transform matrix with the given scale value
 *
 * matrix = matrix * factor
 *
 * @param matrix    the matrix
 * @param sx        the x-scale
 * @param sy        the y-scale
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_matrix_scale(lx_matrix_ref_t matrix, lx_float_t sx, lx_float_t sy);

/*! transform matrix with the given scale value
 *
 * matrix = factor * matrix
 *
 * @param matrix    the matrix
 * @param sx        the x-scale
 * @param sy        the y-scale
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_matrix_scale_lhs(lx_matrix_ref_t matrix, lx_float_t sx, lx_float_t sy);

/*! transform matrix with the given scale value by the coordinate: (px, py)
 *
 * matrix = matrix * factor
 *
 * @param matrix    the matrix
 * @param sx        the x-scale
 * @param sy        the y-scale
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_matrix_scalep(lx_matrix_ref_t matrix, lx_float_t sx, lx_float_t sy, lx_float_t px, lx_float_t py);

/*! transform matrix with the given scale value by the coordinate: (px, py)
 *
 * matrix = factor * matrix
 *
 * @param matrix    the matrix
 * @param sy        the y-scale
 * @param sy        the y-scale
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_matrix_scalep_lhs(lx_matrix_ref_t matrix, lx_float_t sx, lx_float_t sy, lx_float_t px, lx_float_t py);

/*! transform matrix with the given skew value
 *
 * matrix = matrix * factor
 *
 * @param matrix    the matrix
 * @param sk        the x-skew
 * @param sk        the y-skew
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_matrix_skew(lx_matrix_ref_t matrix, lx_float_t kx, lx_float_t ky);

/*! transform matrix with the given skew value
 *
 * matrix = factor * matrix
 *
 * @param matrix    the matrix
 * @param sk        the x-skew
 * @param sk        the y-skew
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_matrix_skew_lhs(lx_matrix_ref_t matrix, lx_float_t kx, lx_float_t ky);

/*! transform matrix with the given skew value by the coordinate: (px, py)
 *
 * matrix = matrix * factor
 *
 * @param matrix    the matrix
 * @param sk        the x-skew
 * @param sk        the y-skew
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_matrix_skewp(lx_matrix_ref_t matrix, lx_float_t kx, lx_float_t ky, lx_float_t px, lx_float_t py);

/*! transform matrix with the given skew value by the coordinate: (px, py)
 *
 * matrix = factor * matrix
 *
 * @param matrix    the matrix
 * @param sk        the x-skew
 * @param sk        the y-skew
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_matrix_skewp_lhs(lx_matrix_ref_t matrix, lx_float_t kx, lx_float_t ky, lx_float_t px, lx_float_t py);

/*! transform matrix with the given sin and cos value
 *
 * matrix = matrix * factor
 *
 * @param matrix    the matrix
 * @param sin       the sin value
 * @param cos       the cos value
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_matrix_sincos(lx_matrix_ref_t matrix, lx_float_t sin, lx_float_t cos);

/*! transform matrix with the given sin and cos value
 *
 * matrix = factor * matrix
 *
 * @param matrix    the matrix
 * @param sin       the sin value
 * @param cos       the cos value
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_matrix_sincos_lhs(lx_matrix_ref_t matrix, lx_float_t sin, lx_float_t cos);

/*! transform matrix with the given sin and cos value by the coordinate: (px, py)
 *
 * matrix = matrix * factor
 *
 * @param matrix    the matrix
 * @param sin       the sin value
 * @param cos       the cos value
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_matrix_sincosp(lx_matrix_ref_t matrix, lx_float_t sin, lx_float_t cos, lx_float_t px, lx_float_t py);

/*! transform matrix with the given sin and cos value by the coordinate: (px, py)
 *
 * matrix = factor * matrix
 *
 * @param matrix    the matrix
 * @param sin       the sin value
 * @param cos       the cos value
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_matrix_sincosp_lhs(lx_matrix_ref_t matrix, lx_float_t sin, lx_float_t cos, lx_float_t px, lx_float_t py);

/*! transform matrix with the given translate value
 *
 * matrix = matrix * factor
 *
 * @param matrix    the matrix
 * @param dx        the x-translate value
 * @param dy        the y-translate value
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_matrix_translate(lx_matrix_ref_t matrix, lx_float_t dx, lx_float_t dy);

/*! transform matrix with the given translate value
 *
 * matrix = factor * matrix
 *
 * @param matrix    the matrix
 * @param dx        the x-translate value
 * @param dy        the y-translate value
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_matrix_translate_lhs(lx_matrix_ref_t matrix, lx_float_t dx, lx_float_t dy);

/*! multiply matrix with the given translate factor
 *
 * matrix = matrix * factor
 *
 * @param matrix    the matrix
 * @param factor    the factor
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_matrix_multiply(lx_matrix_ref_t matrix, lx_matrix_ref_t factor);

/*! multiply matrix with the given translate factor
 *
 * matrix = factor * matrix
 *
 * @param matrix    the matrix
 * @param factor    the factor
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_matrix_multiply_lhs(lx_matrix_ref_t matrix, lx_matrix_ref_t factor);

/*! apply matrix to the points
 *
 * @param matrix    the matrix
 * @param points    the points
 * @param count     the count
 */
lx_void_t           lx_matrix_apply_points(lx_matrix_ref_t matrix, lx_point_ref_t points, lx_size_t count);

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */

/*! apply matrix to the x-coordinate
 *
 * x' = x * sx + y * kx + tx
 *
 * @param matrix    the matrix
 * @param x         the x value
 * @param y         the y value
 *
 * @return          the new x value
 */
static lx_inline lx_float_t lx_matrix_apply_x(lx_matrix_ref_t matrix, lx_float_t x, lx_float_t y) {
    lx_assert(matrix);
    return (x * matrix->sx) + (y * matrix->kx) + matrix->tx;
}

/*! apply matrix to the y-coordinate
 *
 * y' = x * ky + y * sy + ty
 *
 * @param matrix    the matrix
 * @param x         the x value
 * @param y         the y value
 *
 * @return          the new y value
 */
static lx_inline lx_float_t lx_matrix_apply_y(lx_matrix_ref_t matrix, lx_float_t x, lx_float_t y) {
    lx_assert(matrix);
    return (x * matrix->ky) + (y * matrix->sy) + matrix->ty;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


