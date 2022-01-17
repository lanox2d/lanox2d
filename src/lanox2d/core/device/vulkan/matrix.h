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
#ifndef LX_CORE_DEVICE_VULKAN_MATRIX_H
#define LX_CORE_DEVICE_VULKAN_MATRIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

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
static lx_inline lx_void_t lx_vk_matrix_init(lx_vk_matrix_ref_t matrix, lx_float_t sx, lx_float_t kx, lx_float_t ky, lx_float_t sy, lx_float_t tx, lx_float_t ty) {
    lx_float_t* mx = (lx_float_t*)matrix;
    mx[0]  = sx;
    mx[1]  = ky;
    mx[2]  = 0.0f;
    mx[3]  = 0.0f;
    mx[4]  = kx;
    mx[5]  = sy;
    mx[6]  = 0.0f;
    mx[7]  = 0.0f;
    mx[8]  = 0.0f;
    mx[9]  = 0.0f;
    mx[10] = 1.0f;
    mx[11] = 0.0f;
    mx[12] = tx;
    mx[13] = ty;
    mx[14] = 0.0f;
    mx[15] = 1.0f;
}

/*! init matrix from the sin and cos value
 *
 * @param matrix    the matrix
 * @param sin       the sin value
 * @param cos       the cos value
 */
static lx_inline lx_void_t lx_vk_matrix_init_sincos(lx_vk_matrix_ref_t matrix, lx_float_t sin, lx_float_t cos) {
    lx_vk_matrix_init(matrix, cos, -sin, sin, cos, 0.0f, 0.0f);
}

/*! init matrix from the sin and cos value by the coordinate: (px, py)
 *
 * @param matrix    the matrix
 * @param sin       the sin value
 * @param cos       the cos value
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 */
static lx_inline lx_void_t lx_vk_matrix_init_sincosp(lx_vk_matrix_ref_t matrix, lx_float_t sin, lx_float_t cos, lx_float_t px, lx_float_t py) {
    lx_float_t const one_cos = 1.0f - cos;
    lx_vk_matrix_init(matrix, cos, -sin, sin, cos, sin * py + one_cos * px, -sin * px + one_cos * py);
}

/*! init matrix from the rotate degree
 *
 * @param matrix    the matrix
 * @param degrees   the rotate degrees
 */
static lx_inline lx_void_t lx_vk_matrix_init_rotate(lx_vk_matrix_ref_t matrix, lx_float_t degrees) {
    lx_float_t s;
    lx_float_t c;
    lx_sincosf((lx_float_t)((degrees * LX_PI) / 180.0f), &s, &c);
    lx_vk_matrix_init_sincos(matrix, s, c);
}

/*! init matrix from the rotate degree by the coordinate: (px, py)
 *
 * @param matrix    the matrix
 * @param degrees   the rotate degrees
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 */
static lx_inline lx_void_t lx_vk_matrix_init_rotatep(lx_vk_matrix_ref_t matrix, lx_float_t degrees, lx_float_t px, lx_float_t py) {
    lx_float_t s;
    lx_float_t c;
    lx_sincosf((lx_float_t)((degrees * LX_PI) / 180.0f), &s, &c);
    lx_vk_matrix_init_sincosp(matrix, s, c, px, py);
}

/*! init matrix from the skew value
 *
 * @param matrix    the matrix
 * @param kx        the x-skew
 * @param ky        the y-skew
 */
static lx_inline lx_void_t lx_vk_matrix_init_skew(lx_vk_matrix_ref_t matrix, lx_float_t kx, lx_float_t ky) {
    lx_vk_matrix_init(matrix, 1.0f, kx, ky, 1.0f, 0.0f, 0.0f);
}

/*! init matrix from the skew value by the coordinate: (px, py)
 *
 * @param matrix    the matrix
 * @param kx        the x-skew
 * @param ky        the y-skew
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 */
static lx_inline lx_void_t lx_vk_matrix_init_skewp(lx_vk_matrix_ref_t matrix, lx_float_t kx, lx_float_t ky, lx_float_t px, lx_float_t py) {
    lx_vk_matrix_init(matrix, 1.0f, kx, ky, 1.0f, -kx * py, -ky * px);
}

/*! init matrix from the scale value
 *
 * @param matrix    the matrix
 * @param sx        the x-scale
 * @param sy        the y-scale
 */
static lx_inline lx_void_t lx_vk_matrix_init_scale(lx_vk_matrix_ref_t matrix, lx_float_t sx, lx_float_t sy) {
    lx_vk_matrix_init(matrix, sx, 0.0f, 0.0f, sy, 0.0f, 0.0f);
}

/*! init matrix from the scale value by the coordinate: (px, py)
 *
 * @param matrix    the matrix
 * @param sx        the x-scale
 * @param sy        the y-scale
 * @param px        the x-coordinate
 * @param py        the y-coordinate
 */
static lx_inline lx_void_t lx_vk_matrix_init_scalep(lx_vk_matrix_ref_t matrix, lx_float_t sx, lx_float_t sy, lx_float_t px, lx_float_t py) {
    lx_vk_matrix_init(matrix, sx, 0.0f, 0.0f, sy, px - sx * px, py - sy * py);
}

/*! init matrix from the translate value
 *
 * @param matrix    the matrix
 * @param tx        the x-translate
 * @param ty        the y-translate
 */
static lx_inline lx_void_t lx_vk_matrix_init_translate(lx_vk_matrix_ref_t matrix, lx_float_t tx, lx_float_t ty) {
    lx_vk_matrix_init(matrix, 1.0f, 0.0f, 0.0f, 1.0f, tx, ty);
}

/*! reset to the identity matrix
 *
 * @param matrix    the matrix
 */
static lx_inline lx_void_t lx_vk_matrix_clear(lx_vk_matrix_ref_t matrix) {
    lx_vk_matrix_init(matrix, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
}

/*! copy matrix
 *
 * @param matrix    the matrix
 * @param copied    the copied matrix
 */
static lx_inline lx_void_t lx_vk_matrix_copy(lx_vk_matrix_ref_t matrix, lx_vk_matrix_ref_t copied) {
    lx_memcpy(matrix, copied, sizeof(lx_vk_matrix_t));
}

/*! multiply matrix with the given translate factor
 *
 * matrix = matrix * factor
 *
 * @param matrix    the matrix
 * @param factor    the factor
 */
static lx_inline lx_void_t lx_vk_matrix_multiply(lx_vk_matrix_ref_t matrix, lx_vk_matrix_ref_t factor) {
    lx_float_t* mx = (lx_float_t*)matrix;
    lx_float_t* ft = (lx_float_t*)factor;
    lx_float_t sx = mx[0] * ft[0] + mx[4] * ft[1];
    lx_float_t ky = mx[1] * ft[0] + mx[5] * ft[1];

    lx_float_t kx = mx[0] * ft[4] + mx[4] * ft[5];
    lx_float_t sy = mx[1] * ft[4] + mx[5] * ft[5];

    lx_float_t tx = mx[0] * ft[12] + mx[4] * ft[13] + mx[12];
    lx_float_t ty = mx[1] * ft[12] + mx[5] * ft[13] + mx[13];

    lx_vk_matrix_init(matrix, sx, kx, ky, sy, tx, ty);
}

/*! multiply matrix with the given translate factor
 *
 * matrix = factor * matrix
 *
 * @param matrix    the matrix
 * @param factor    the factor
 */
static lx_inline lx_void_t lx_vk_matrix_multiply_lhs(lx_vk_matrix_ref_t matrix, lx_vk_matrix_ref_t factor) {
    lx_float_t* mx = (lx_float_t*)matrix;
    lx_float_t* ft = (lx_float_t*)factor;
    lx_float_t sx = ft[0] * mx[0] + ft[4] * mx[1];
    lx_float_t ky = ft[1] * mx[0] + ft[5] * mx[1];

    lx_float_t kx = ft[0] * mx[4] + ft[4] * mx[5];
    lx_float_t sy = ft[1] * mx[4] + ft[5] * mx[5];

    lx_float_t tx = ft[0] * mx[12] + ft[4] * mx[13] + ft[12];
    lx_float_t ty = ft[1] * mx[12] + ft[5] * mx[13] + ft[13];

    lx_vk_matrix_init(matrix, sx, kx, ky, sy, tx, ty);
}

/*! transform matrix with the given scale value
 *
 * matrix = matrix * factor
 *
 * @param matrix    the matrix
 * @param sx        the x-scale
 * @param sy        the y-scale
 */
static lx_inline lx_void_t lx_vk_matrix_scale(lx_vk_matrix_ref_t matrix, lx_float_t sx, lx_float_t sy) {
    lx_float_t* mx = (lx_float_t*)matrix;
    mx[0] *= sx;
    mx[1] *= sx;
    mx[4] *= sy;
    mx[5] *= sy;
}

/*! transform matrix with the given rotate degrees
 *
 * matrix = matrix * factor
 *
 * @param matrix    the matrix
 * @param degrees   the rotate degrees
 */
static lx_inline lx_void_t lx_vk_matrix_rotate(lx_vk_matrix_ref_t matrix, lx_float_t degrees) {
    lx_vk_matrix_t factor;
    lx_vk_matrix_init_rotate(&factor, degrees);
    lx_vk_matrix_multiply(matrix, &factor);
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
static lx_inline lx_void_t lx_vk_matrix_rotatep(lx_vk_matrix_ref_t matrix, lx_float_t degrees, lx_float_t px, lx_float_t py) {
    lx_vk_matrix_t factor;
    lx_vk_matrix_init_rotatep(&factor, degrees, px, py);
    lx_vk_matrix_multiply(matrix, &factor);
}

/*! transform matrix with the given translate value
 *
 * matrix = matrix * factor
 *
 * @param matrix    the matrix
 * @param dx        the x-translate value
 * @param dy        the y-translate value
 */
static lx_inline lx_void_t lx_vk_matrix_translate(lx_vk_matrix_ref_t matrix, lx_float_t dx, lx_float_t dy) {
    lx_float_t* mx = (lx_float_t*)matrix;
    mx[12] += mx[0] * dx + mx[4] * dy;
    mx[13] += mx[1] * dx + mx[5] * dy;
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
static lx_inline lx_float_t lx_vk_matrix_apply_x(lx_vk_matrix_ref_t matrix, lx_float_t x, lx_float_t y) {
    lx_float_t* mx = (lx_float_t*)matrix;
    return (x * mx[0]) + (y * mx[4]) + mx[12];
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
static lx_inline lx_float_t lx_vk_matrix_apply_y(lx_vk_matrix_ref_t matrix, lx_float_t x, lx_float_t y) {
    lx_float_t* mx = (lx_float_t*)matrix;
    return (x * mx[1]) + (y * mx[5]) + mx[13];
}

/* convert lx_matrix_t to the gl matrix
 *
 * @param matrix    the matrix
 * @param converted the converted matrix
 */
static lx_inline lx_void_t lx_vk_matrix_convert(lx_vk_matrix_ref_t matrix, lx_matrix_ref_t converted) {
    lx_vk_matrix_init(matrix, converted->sx, converted->kx, converted->ky, converted->sy, converted->tx, converted->ty);
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
static lx_inline lx_void_t lx_vk_matrix_orthof(lx_vk_matrix_ref_t matrix, lx_float_t left, lx_float_t right, lx_float_t bottom, lx_float_t top, lx_float_t nearp, lx_float_t farp) {
    lx_float_t* mx = (lx_float_t*)matrix;
    lx_vk_matrix_clear(matrix);
    mx[0]  = 2.0f / (right - left);
    mx[5]  = 2.0f / (top - bottom);
    mx[10] = -2.0f / (farp - nearp);
    mx[12] = -(right + left) / (right - left);
    mx[13] = -(top + bottom) / (top - bottom);
    mx[14] = -(farp + nearp) / (farp - nearp);
}


#endif
