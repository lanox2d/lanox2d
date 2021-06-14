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
#ifndef LX_CORE_DEVICE_OPENGL_MATRIX_H
#define LX_CORE_DEVICE_OPENGL_MATRIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the gl matrix type
typedef lx_GLfloat_t    lx_gl_matrix_t[16];

// the gl matrix ref type
typedef lx_GLfloat_t*   lx_gl_matrix_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline interfaces
 */

/* init matrix
 *
 * @param sx        the x-scale
 * @param kx        the x-skew
 * @param ky        the y-skew
 * @param sy        the y-scale
 * @param tx        the x-translate
 * @param ty        the y-translate
 */
static lx_inline lx_void_t lx_gl_matrix_init(lx_gl_matrix_ref_t matrix, lx_GLfloat_t sx, lx_GLfloat_t kx, lx_GLfloat_t ky, lx_GLfloat_t sy, lx_GLfloat_t tx, lx_GLfloat_t ty) {
    matrix[0]  = sx;
    matrix[1]  = ky;
    matrix[2]  = 0.0f;
    matrix[3]  = 0.0f;
    matrix[4]  = kx;
    matrix[5]  = sy;
    matrix[6]  = 0.0f;
    matrix[7]  = 0.0f;
    matrix[8]  = 0.0f;
    matrix[9]  = 0.0f;
    matrix[10] = 1.0f;
    matrix[11] = 0.0f;
    matrix[12] = tx;
    matrix[13] = ty;
    matrix[14] = 0.0f;
    matrix[15] = 1.0f;
}

/*! init matrix from the sin and cos value
 *
 * @param matrix    the matrix
 * @param sin       the sin value
 * @param cos       the cos value
 */
static lx_inline lx_void_t lx_gl_matrix_init_sincos(lx_gl_matrix_ref_t matrix, lx_GLfloat_t sin, lx_GLfloat_t cos) {
    lx_gl_matrix_init(matrix, cos, -sin, sin, cos, 0.0f, 0.0f);
}

/*! init matrix from the sin and cos value by the coordinate: (px, py)
 *
 * @param matrix    the matrix
 * @param sin       the sin value
 * @param cos       the cos value
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 */
static lx_inline lx_void_t lx_gl_matrix_init_sincosp(lx_gl_matrix_ref_t matrix, lx_GLfloat_t sin, lx_GLfloat_t cos, lx_GLfloat_t px, lx_GLfloat_t py) {
    lx_GLfloat_t const one_cos = 1.0f - cos;
    lx_gl_matrix_init(matrix, cos, -sin, sin, cos, sin * py + one_cos * px, -sin * px + one_cos * py);
}

/*! init matrix from the rotate degree
 *
 * @param matrix    the matrix
 * @param degrees   the rotate degrees
 */
static lx_inline lx_void_t lx_gl_matrix_init_rotate(lx_gl_matrix_ref_t matrix, lx_GLfloat_t degrees) {
    lx_GLfloat_t s;
    lx_GLfloat_t c;
    lx_sincosf((lx_GLfloat_t)((degrees * LX_PI) / 180.0f), &s, &c);
    lx_gl_matrix_init_sincos(matrix, s, c);
}

/*! init matrix from the rotate degree by the coordinate: (px, py)
 *
 * @param matrix    the matrix
 * @param degrees   the rotate degrees
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 */
static lx_inline lx_void_t lx_gl_matrix_init_rotatep(lx_gl_matrix_ref_t matrix, lx_GLfloat_t degrees, lx_GLfloat_t px, lx_GLfloat_t py) {
    lx_GLfloat_t s;
    lx_GLfloat_t c;
    lx_sincosf((lx_GLfloat_t)((degrees * LX_PI) / 180.0f), &s, &c);
    lx_gl_matrix_init_sincosp(matrix, s, c, px, py);
}

/*! init matrix from the skew value
 *
 * @param matrix    the matrix
 * @param kx        the x-skew
 * @param ky        the y-skew
 */
static lx_inline lx_void_t lx_gl_matrix_init_skew(lx_gl_matrix_ref_t matrix, lx_GLfloat_t kx, lx_GLfloat_t ky) {
    lx_gl_matrix_init(matrix, 1.0f, kx, ky, 1.0f, 0.0f, 0.0f);
}

/*! init matrix from the skew value by the coordinate: (px, py)
 *
 * @param matrix    the matrix
 * @param kx        the x-skew
 * @param ky        the y-skew
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 */
static lx_inline lx_void_t lx_gl_matrix_init_skewp(lx_gl_matrix_ref_t matrix, lx_GLfloat_t kx, lx_GLfloat_t ky, lx_GLfloat_t px, lx_GLfloat_t py) {
    lx_gl_matrix_init(matrix, 1.0f, kx, ky, 1.0f, -kx * py, -ky * px);
}

/*! init matrix from the scale value
 *
 * @param matrix    the matrix
 * @param sx        the x-scale
 * @param sy        the y-scale
 */
static lx_inline lx_void_t lx_gl_matrix_init_scale(lx_gl_matrix_ref_t matrix, lx_GLfloat_t sx, lx_GLfloat_t sy) {
    lx_gl_matrix_init(matrix, sx, 0.0f, 0.0f, sy, 0.0f, 0.0f);
}

/*! init matrix from the scale value by the coordinate: (px, py)
 *
 * @param matrix    the matrix
 * @param sx        the x-scale
 * @param sy        the y-scale
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 */
static lx_inline lx_void_t lx_gl_matrix_init_scalep(lx_gl_matrix_ref_t matrix, lx_GLfloat_t sx, lx_GLfloat_t sy, lx_GLfloat_t px, lx_GLfloat_t py) {
    lx_gl_matrix_init(matrix, sx, 0.0f, 0.0f, sy, px - sx * px, py - sy * py);
}

/*! init matrix from the translate value
 *
 * @param matrix    the matrix
 * @param tx        the x-translate
 * @param ty        the y-translate
 */
static lx_inline lx_void_t lx_gl_matrix_init_translate(lx_gl_matrix_ref_t matrix, lx_GLfloat_t tx, lx_GLfloat_t ty) {
    lx_gl_matrix_init(matrix, 1.0f, 0.0f, 0.0f, 1.0f, tx, ty);
}

/*! reset to the identity matrix
 *
 * @param matrix    the matrix
 */
static lx_inline lx_void_t lx_gl_matrix_clear(lx_gl_matrix_ref_t matrix) {
    lx_gl_matrix_init(matrix, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
}

/*! copy matrix
 *
 * @param matrix    the matrix
 * @param copied    the copied matrix
 */
static lx_inline lx_void_t lx_gl_matrix_copy(lx_gl_matrix_ref_t matrix, lx_gl_matrix_ref_t copied) {
    lx_memcpy(matrix, copied, sizeof(lx_gl_matrix_t));
}

/*! multiply matrix with the given translate factor
 *
 * matrix = matrix * factor
 *
 * @param matrix    the matrix
 * @param factor    the factor
 */
static lx_inline lx_void_t lx_gl_matrix_multiply(lx_gl_matrix_ref_t matrix, lx_gl_matrix_ref_t factor) {
    lx_GLfloat_t sx = matrix[0] * factor[0] + matrix[4] * factor[1];
    lx_GLfloat_t ky = matrix[1] * factor[0] + matrix[5] * factor[1];

    lx_GLfloat_t kx = matrix[0] * factor[4] + matrix[4] * factor[5];
    lx_GLfloat_t sy = matrix[1] * factor[4] + matrix[5] * factor[5];

    lx_GLfloat_t tx = matrix[0] * factor[12] + matrix[4] * factor[13] + matrix[12];
    lx_GLfloat_t ty = matrix[1] * factor[12] + matrix[5] * factor[13] + matrix[13];

    lx_gl_matrix_init(matrix, sx, kx, ky, sy, tx, ty);
}

/*! multiply matrix with the given translate factor
 *
 * matrix = factor * matrix
 *
 * @param matrix    the matrix
 * @param factor    the factor
 */
static lx_inline lx_void_t lx_gl_matrix_multiply_lhs(lx_gl_matrix_ref_t matrix, lx_gl_matrix_ref_t factor) {
    lx_GLfloat_t sx = factor[0] * matrix[0] + factor[4] * matrix[1];
    lx_GLfloat_t ky = factor[1] * matrix[0] + factor[5] * matrix[1];

    lx_GLfloat_t kx = factor[0] * matrix[4] + factor[4] * matrix[5];
    lx_GLfloat_t sy = factor[1] * matrix[4] + factor[5] * matrix[5];

    lx_GLfloat_t tx = factor[0] * matrix[12] + factor[4] * matrix[13] + factor[12];
    lx_GLfloat_t ty = factor[1] * matrix[12] + factor[5] * matrix[13] + factor[13];

    lx_gl_matrix_init(matrix, sx, kx, ky, sy, tx, ty);
}

/*! transform matrix with the given scale value
 *
 * matrix = matrix * factor
 *
 * @param matrix    the matrix
 * @param sx        the x-scale
 * @param sy        the y-scale
 */
static lx_inline lx_void_t lx_gl_matrix_scale(lx_gl_matrix_ref_t matrix, lx_GLfloat_t sx, lx_GLfloat_t sy) {
    matrix[0] *= sx;
    matrix[1] *= sx;
    matrix[4] *= sy;
    matrix[5] *= sy;
}

/*! transform matrix with the given rotate degrees
 *
 * matrix = matrix * factor
 *
 * @param matrix    the matrix
 * @param degrees   the rotate degrees
 */
static lx_inline lx_void_t lx_gl_matrix_rotate(lx_gl_matrix_ref_t matrix, lx_GLfloat_t degrees) {
    lx_gl_matrix_t factor;
    lx_gl_matrix_init_rotate(factor, degrees);
    lx_gl_matrix_multiply(matrix, factor);
}

/*! transform matrix with the given rotate degrees by the coordinate: (px, py)
 *
 * matrix = matrix * factor
 *
 * @param matrix    the matrix
 * @param degrees   the rotate degrees
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 */
static lx_inline lx_void_t lx_gl_matrix_rotatep(lx_gl_matrix_ref_t matrix, lx_GLfloat_t degrees, lx_GLfloat_t px, lx_GLfloat_t py) {
    lx_gl_matrix_t factor;
    lx_gl_matrix_init_rotatep(factor, degrees, px, py);
    lx_gl_matrix_multiply(matrix, factor);
}

/*! transform matrix with the given translate value
 *
 * matrix = matrix * factor
 *
 * @param matrix    the matrix
 * @param dx        the x-translate value
 * @param dy        the y-translate value
 */
static lx_inline lx_void_t lx_gl_matrix_translate(lx_gl_matrix_ref_t matrix, lx_GLfloat_t dx, lx_GLfloat_t dy) {
    matrix[12] += matrix[0] * dx + matrix[4] * dy;
    matrix[13] += matrix[1] * dx + matrix[5] * dy;
}

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
static lx_inline lx_GLfloat_t lx_gl_matrix_apply_x(lx_gl_matrix_ref_t matrix, lx_GLfloat_t x, lx_GLfloat_t y) {
    return (x * matrix[0]) + (y * matrix[4]) + matrix[12];
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
static lx_inline lx_GLfloat_t lx_gl_matrix_apply_y(lx_gl_matrix_ref_t matrix, lx_GLfloat_t x, lx_GLfloat_t y) {
    return (x * matrix[1]) + (y * matrix[5]) + matrix[13];
}

/* convert lx_matrix_t to the gl matrix
 *
 * @param matrix    the matrix
 * @param converted the converted matrix
 */
static lx_inline lx_void_t lx_gl_matrix_convert(lx_gl_matrix_ref_t matrix, lx_matrix_ref_t converted) {
    lx_gl_matrix_init(matrix, converted->sx, converted->kx, converted->ky, converted->sy, converted->tx, converted->ty);
}

/* multiply the current matrix by an orthographic matrix
 *
 * @param matrix    the matrix
 * @param left      the left position
 * @param right     the right position
 * @param bottom    the bottom position
 * @param top       the top position
 * @param nearp     the near position
 * @param farp      the far position
 */
static lx_inline lx_void_t lx_gl_matrix_orthof(lx_gl_matrix_ref_t matrix, lx_GLfloat_t left, lx_GLfloat_t right, lx_GLfloat_t bottom, lx_GLfloat_t top, lx_GLfloat_t nearp, lx_GLfloat_t farp) {
    lx_gl_matrix_clear(matrix);
    matrix[0]  = 2.0f / (right - left);
    matrix[5]  = 2.0f / (top - bottom);
    matrix[10] = -2.0f / (farp - nearp);
    matrix[12] = -(right + left) / (right - left);
    matrix[13] = -(top + bottom) / (top - bottom);
    matrix[14] = -(farp + nearp) / (farp - nearp);
}

#endif
