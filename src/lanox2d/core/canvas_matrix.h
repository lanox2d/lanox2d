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
lx_matrix_ref_t     lx_canvas_matrix_save(lx_canvas_ref_t canvas);

/*! load matrix
 *
 * @param canvas    the canvas
 */
lx_void_t           lx_canvas_matrix_load(lx_canvas_ref_t canvas);

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline interfaces
 */
static lx_inline lx_void_t lx_canvas_matrix_clear(lx_canvas_ref_t canvas) {
    lx_matrix_clear(lx_canvas_matrix(canvas));
}

static lx_inline lx_bool_t lx_canvas_rotate(lx_canvas_ref_t canvas, lx_float_t degrees) {
    return lx_matrix_rotate(lx_canvas_matrix(canvas), degrees);
}

static lx_inline lx_bool_t lx_canvas_rotate_lhs(lx_canvas_ref_t canvas, lx_float_t degrees) {
    return lx_matrix_rotate_lhs(lx_canvas_matrix(canvas), degrees);
}

static lx_inline lx_bool_t lx_canvas_rotatep(lx_canvas_ref_t canvas, lx_float_t degrees, lx_float_t px, lx_float_t py) {
    return lx_matrix_rotatep(lx_canvas_matrix(canvas), degrees, px, py);
}

static lx_inline lx_bool_t lx_canvas_rotatep_lhs(lx_canvas_ref_t canvas, lx_float_t degrees, lx_float_t px, lx_float_t py) {
    return lx_matrix_rotatep_lhs(lx_canvas_matrix(canvas), degrees, px, py);
}

static lx_inline lx_bool_t lx_canvas_scale(lx_canvas_ref_t canvas, lx_float_t sx, lx_float_t sy) {
    return lx_matrix_scale(lx_canvas_matrix(canvas), sx, sy);
}

static lx_inline lx_bool_t lx_canvas_scale_lhs(lx_canvas_ref_t canvas, lx_float_t sx, lx_float_t sy) {
    return lx_matrix_scale_lhs(lx_canvas_matrix(canvas), sx, sy);
}

static lx_inline lx_bool_t lx_canvas_scalep(lx_canvas_ref_t canvas, lx_float_t sx, lx_float_t sy, lx_float_t px, lx_float_t py) {
    return lx_matrix_scalep(lx_canvas_matrix(canvas), sx, sy, px, py);
}

static lx_inline lx_bool_t lx_canvas_scalep_lhs(lx_canvas_ref_t canvas, lx_float_t sx, lx_float_t sy, lx_float_t px, lx_float_t py) {
    return lx_matrix_scalep_lhs(lx_canvas_matrix(canvas), sx, sy, px, py);
}

static lx_inline lx_bool_t lx_canvas_skew(lx_canvas_ref_t canvas, lx_float_t kx, lx_float_t ky) {
    return lx_matrix_skew(lx_canvas_matrix(canvas), kx, ky);
}

static lx_inline lx_bool_t lx_canvas_skew_lhs(lx_canvas_ref_t canvas, lx_float_t kx, lx_float_t ky) {
    return lx_matrix_skew_lhs(lx_canvas_matrix(canvas), kx, ky);
}

static lx_inline lx_bool_t lx_canvas_skewp(lx_canvas_ref_t canvas, lx_float_t kx, lx_float_t ky, lx_float_t px, lx_float_t py) {
    return lx_matrix_skewp(lx_canvas_matrix(canvas), kx, ky, px, py);
}

static lx_inline lx_bool_t lx_canvas_skewp_lhs(lx_canvas_ref_t canvas, lx_float_t kx, lx_float_t ky, lx_float_t px, lx_float_t py) {
    return lx_matrix_skewp_lhs(lx_canvas_matrix(canvas), kx, ky, px, py);
}

static lx_inline lx_bool_t lx_canvas_sincos(lx_canvas_ref_t canvas, lx_float_t sin, lx_float_t cos) {
    return lx_matrix_sincos(lx_canvas_matrix(canvas), sin, cos);
}

static lx_inline lx_bool_t lx_canvas_sincos_lhs(lx_canvas_ref_t canvas, lx_float_t sin, lx_float_t cos) {
    return lx_matrix_sincos_lhs(lx_canvas_matrix(canvas), sin, cos);
}

static lx_inline lx_bool_t lx_canvas_sincosp(lx_canvas_ref_t canvas, lx_float_t sin, lx_float_t cos, lx_float_t px, lx_float_t py) {
    return lx_matrix_sincosp(lx_canvas_matrix(canvas), sin, cos, px, py);
}

static lx_inline lx_bool_t lx_canvas_sincosp_lhs(lx_canvas_ref_t canvas, lx_float_t sin, lx_float_t cos, lx_float_t px, lx_float_t py) {
    return lx_matrix_sincosp_lhs(lx_canvas_matrix(canvas), sin, cos, px, py);
}

static lx_inline lx_bool_t lx_canvas_translate(lx_canvas_ref_t canvas, lx_float_t dx, lx_float_t dy) {
    return lx_matrix_translate(lx_canvas_matrix(canvas), dx, dy);
}

static lx_inline lx_bool_t lx_canvas_translate_lhs(lx_canvas_ref_t canvas, lx_float_t dx, lx_float_t dy) {
    return lx_matrix_translate_lhs(lx_canvas_matrix(canvas), dx, dy);
}

static lx_inline lx_bool_t lx_canvas_multiply(lx_canvas_ref_t canvas, lx_matrix_ref_t factor) {
    return lx_matrix_multiply(lx_canvas_matrix(canvas), factor);
}

static lx_inline lx_bool_t lx_canvas_multiply_lhs(lx_canvas_ref_t canvas, lx_matrix_ref_t factor) {
    return lx_matrix_multiply_lhs(lx_canvas_matrix(canvas), factor);
}


/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


