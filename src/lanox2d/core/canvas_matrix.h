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
 * @file        canvas_matrix.h
 *
 */
#ifndef LX_CORE_CANVAS_MATRIX_H
#define LX_CORE_CANVAS_MATRIX_H

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

/*! get the matrix
 *
 * @param canvas    the canvas
 *
 * @return          the matrix
 */
lx_matrix_ref_t     lx_canvas_matrix(lx_canvas_ref_t canvas);

/*! save matrix
 *
 * @param canvas    the canvas
 *
 * @return          the current matrix
 */
lx_matrix_ref_t     lx_canvas_save_matrix(lx_canvas_ref_t canvas);

/*! load matrix
 *
 * @param canvas    the canvas
 */
lx_void_t           lx_canvas_load_matrix(lx_canvas_ref_t canvas);

/*! clear matrix
 *
 * @param canvas    the canvas
 */
lx_void_t           lx_canvas_clear_matrix(lx_canvas_ref_t canvas);

/*! transform matrix with the given rotate degrees
 *
 * matrix = matrix * factor
 *
 * @param canvas    the canvas
 * @param degrees   the rotate degrees
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_canvas_rotate(lx_canvas_ref_t canvas, lx_float_t degrees);

/*! transform matrix with the given rotate degrees
 *
 * matrix = factor * matrix
 *
 * @param canvas    the canvas
 * @param degrees   the rotate degrees
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_canvas_rotate_lhs(lx_canvas_ref_t canvas, lx_float_t degrees);

/*! transform matrix with the given rotate degrees by the coordinate: (px, py)
 *
 * matrix = matrix * factor
 *
 * @param canvas    the canvas
 * @param degrees   the rotate degrees
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_canvas_rotatep(lx_canvas_ref_t canvas, lx_float_t degrees, lx_float_t px, lx_float_t py);

/*! transform matrix with the given rotate degrees by the coordinate: (px, py)
 *
 * matrix = factor * matrix
 *
 * @param canvas    the canvas
 * @param degrees   the rotate degrees
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_canvas_rotatep_lhs(lx_canvas_ref_t canvas, lx_float_t degrees, lx_float_t px, lx_float_t py);

/*! transform matrix with the given scale value
 *
 * matrix = matrix * factor
 *
 * @param canvas    the canvas
 * @param sx        the x-scale
 * @param sy        the y-scale
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_canvas_scale(lx_canvas_ref_t canvas, lx_float_t sx, lx_float_t sy);

/*! transform matrix with the given scale value
 *
 * matrix = factor * matrix
 *
 * @param canvas    the canvas
 * @param sx        the x-scale
 * @param sy        the y-scale
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_canvas_scale_lhs(lx_canvas_ref_t canvas, lx_float_t sx, lx_float_t sy);

/*! transform matrix with the given scale value by the coordinate: (px, py)
 *
 * matrix = matrix * factor
 *
 * @param canvas    the canvas
 * @param sx        the x-scale
 * @param sy        the y-scale
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_canvas_scalep(lx_canvas_ref_t canvas, lx_float_t sx, lx_float_t sy, lx_float_t px, lx_float_t py);

/*! transform matrix with the given scale value by the coordinate: (px, py)
 *
 * matrix = factor * matrix
 *
 * @param canvas    the canvas
 * @param sy        the y-scale
 * @param sy        the y-scale
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_canvas_scalep_lhs(lx_canvas_ref_t canvas, lx_float_t sx, lx_float_t sy, lx_float_t px, lx_float_t py);

/*! transform matrix with the given skew value
 *
 * matrix = matrix * factor
 *
 * @param canvas    the canvas
 * @param sk        the x-skew
 * @param sk        the y-skew
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_canvas_skew(lx_canvas_ref_t canvas, lx_float_t kx, lx_float_t ky);

/*! transform matrix with the given skew value
 *
 * matrix = factor * matrix
 *
 * @param canvas    the canvas
 * @param sk        the x-skew
 * @param sk        the y-skew
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_canvas_skew_lhs(lx_canvas_ref_t canvas, lx_float_t kx, lx_float_t ky);

/*! transform matrix with the given skew value by the coordinate: (px, py)
 *
 * matrix = matrix * factor
 *
 * @param canvas    the canvas
 * @param sk        the x-skew
 * @param sk        the y-skew
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_canvas_skewp(lx_canvas_ref_t canvas, lx_float_t kx, lx_float_t ky, lx_float_t px, lx_float_t py);

/*! transform matrix with the given skew value by the coordinate: (px, py)
 *
 * matrix = factor * matrix
 *
 * @param canvas    the canvas
 * @param sk        the x-skew
 * @param sk        the y-skew
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_canvas_skewp_lhs(lx_canvas_ref_t canvas, lx_float_t kx, lx_float_t ky, lx_float_t px, lx_float_t py);

/*! transform matrix with the given sin and cos value
 *
 * matrix = matrix * factor
 *
 * @param canvas    the canvas
 * @param sin       the sin value
 * @param cos       the cos value
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_canvas_sincos(lx_canvas_ref_t canvas, lx_float_t sin, lx_float_t cos);

/*! transform matrix with the given sin and cos value
 *
 * matrix = factor * matrix
 *
 * @param canvas    the canvas
 * @param sin       the sin value
 * @param cos       the cos value
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_canvas_sincos_lhs(lx_canvas_ref_t canvas, lx_float_t sin, lx_float_t cos);

/*! transform matrix with the given sin and cos value by the coordinate: (px, py)
 *
 * matrix = matrix * factor
 *
 * @param canvas    the canvas
 * @param sin       the sin value
 * @param cos       the cos value
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_canvas_sincosp(lx_canvas_ref_t canvas, lx_float_t sin, lx_float_t cos, lx_float_t px, lx_float_t py);

/*! transform matrix with the given sin and cos value by the coordinate: (px, py)
 *
 * matrix = factor * matrix
 *
 * @param canvas    the canvas
 * @param sin       the sin value
 * @param cos       the cos value
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_canvas_sincosp_lhs(lx_canvas_ref_t canvas, lx_float_t sin, lx_float_t cos, lx_float_t px, lx_float_t py);

/*! transform matrix with the given translate value
 *
 * matrix = matrix * factor
 *
 * @param canvas    the canvas
 * @param dx        the x-translate value
 * @param dy        the y-translate value
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_canvas_translate(lx_canvas_ref_t canvas, lx_float_t dx, lx_float_t dy);

/*! transform matrix with the given translate value
 *
 * matrix = factor * matrix
 *
 * @param canvas    the canvas
 * @param dx        the x-translate value
 * @param dy        the y-translate value
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_canvas_translate_lhs(lx_canvas_ref_t canvas, lx_float_t dx, lx_float_t dy);

/*! multiply matrix with the given translate factor
 *
 * matrix = matrix * factor
 *
 * @param canvas    the canvas
 * @param factor    the factor
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_canvas_multiply(lx_canvas_ref_t canvas, lx_matrix_ref_t factor);

/*! multiply matrix with the given translate factor
 *
 * matrix = factor * matrix
 *
 * @param canvas    the canvas
 * @param factor    the factor
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_canvas_multiply_lhs(lx_canvas_ref_t canvas, lx_matrix_ref_t factor);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


