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
 * @file        matrix.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "matrix.h"
#include "../libc/libc.h"
#include "../libm/libm.h"
#include "../math/math.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_inline lx_float_t lx_matrix_mul_add(lx_float_t a, lx_float_t b, lx_float_t c, lx_float_t d) {
    return (lx_float_t)((lx_double_t)a * b + (lx_double_t)c * d);
}

static lx_inline lx_double_t lx_matrix_det(lx_float_t sx, lx_float_t sy, lx_float_t kx, lx_float_t ky) {
    lx_double_t det = (lx_double_t)sx * sy - (lx_double_t)kx * ky;
    if (lx_abs(det) <= LX_NEAR0 * LX_NEAR0 * LX_NEAR0) return 0.;
    return 1.0 / det;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_void_t lx_matrix_init(lx_matrix_ref_t matrix, lx_float_t sx, lx_float_t kx, lx_float_t ky, lx_float_t sy, lx_float_t tx, lx_float_t ty) {
    lx_assert(matrix);
    matrix->sx = sx;
    matrix->kx = kx;
    matrix->ky = ky;
    matrix->sy = sy;
    matrix->tx = tx;
    matrix->ty = ty;
}

lx_void_t lx_matrix_init_rotate(lx_matrix_ref_t matrix, lx_float_t degrees) {
    lx_float_t s;
    lx_float_t c;
    lx_sincosf(lx_degree_to_radian(degrees), &s, &c);
    lx_matrix_init_sincos(matrix, s, c);
}

lx_void_t lx_matrix_init_rotatep(lx_matrix_ref_t matrix, lx_float_t degrees, lx_float_t px, lx_float_t py) {
    lx_float_t s;
    lx_float_t c;
    lx_sincosf(lx_degree_to_radian(degrees), &s, &c);
    lx_matrix_init_sincosp(matrix, s, c, px, py);
}

lx_void_t lx_matrix_init_sincos(lx_matrix_ref_t matrix, lx_float_t sin, lx_float_t cos) {
    lx_matrix_init(matrix, cos, -sin, sin, cos, 0, 0);
}

lx_void_t lx_matrix_init_sincosp(lx_matrix_ref_t matrix, lx_float_t sin, lx_float_t cos, lx_float_t px, lx_float_t py) {
    lx_float_t const one_cos = 1.0f - cos;
    lx_matrix_init(matrix, cos, -sin, sin, cos, (sin * py) + (one_cos * px), (-sin * px) + (one_cos * py));
}

lx_void_t lx_matrix_init_skew(lx_matrix_ref_t matrix, lx_float_t kx, lx_float_t ky) {
    lx_matrix_init(matrix, 1.0f, kx, ky, 1.0f, 0, 0);
}

lx_void_t lx_matrix_init_skewp(lx_matrix_ref_t matrix, lx_float_t kx, lx_float_t ky, lx_float_t px, lx_float_t py) {
    lx_matrix_init(matrix, 1.0f, kx, ky, 1.0f, -kx * py, -ky * px);
}

lx_void_t lx_matrix_init_scale(lx_matrix_ref_t matrix, lx_float_t sx, lx_float_t sy) {
    lx_matrix_init(matrix, sx, 0, 0, sy, 0, 0);
}

lx_void_t lx_matrix_init_scalep(lx_matrix_ref_t matrix, lx_float_t sx, lx_float_t sy, lx_float_t px, lx_float_t py) {
    lx_matrix_init(matrix, sx, 0, 0, sy, px - (sx * px), py - (sy * py));
}

lx_void_t lx_matrix_init_translate(lx_matrix_ref_t matrix, lx_float_t tx, lx_float_t ty) {
    lx_matrix_init(matrix, 1.0f, 0, 0, 1.0f, tx, ty);
}

lx_void_t lx_matrix_clear(lx_matrix_ref_t matrix) {
    lx_matrix_init(matrix, 1.0f, 0, 0, 1.0f, 0, 0);
}

lx_void_t lx_matrix_copy(lx_matrix_ref_t matrix, lx_matrix_ref_t copied) {
    lx_assert(matrix);
    if (copied) {
        *matrix = *copied;
    } else {
        lx_matrix_clear(matrix);
    }
}

lx_bool_t lx_matrix_invert(lx_matrix_ref_t matrix) {

    // identity?
    if (lx_matrix_identity(matrix)) {
        return lx_true;
    }

    // no rotate?
    lx_matrix_t mx = *matrix;
    if (0 == matrix->kx && 0 == matrix->ky) {

        // invert it if sx != 1.0
        if (1.0f != matrix->sx) {

            // check
            lx_assert_and_check_return_val(!lx_near0(matrix->sx), lx_false);

            // compute sx and tx
            mx.sx = 1.0f / matrix->sx;
            mx.tx = -matrix->tx / matrix->sx;
        }

        // invert it if sy != 1.0
        if (1.0f != matrix->sy) {

            // check
            lx_assert_and_check_return_val(!lx_near0(matrix->sy), lx_false);

            // compute sy and ty
            mx.sy = 1.0f / matrix->sy;
            mx.ty = -matrix->ty / matrix->sy;
        }
    } else {
        /* |A|
         *
         * adj(A)
         *
         * (sx, kx)     (sy, -kx)
         *          =>
         * (ky, sy)     (-ky, sx)
         */
        lx_double_t det = lx_matrix_det(matrix->sx, matrix->sy, matrix->kx, matrix->ky);
        lx_assert_and_check_return_val(det, lx_false);

        mx.sx = (lx_float_t)(matrix->sy * det);
        mx.sy = (lx_float_t)(matrix->sx * det);
        mx.kx = (lx_float_t)(-matrix->kx * det);
        mx.ky = (lx_float_t)(-matrix->ky * det);
        mx.tx = (lx_float_t)(((lx_double_t)matrix->kx * matrix->ty - (lx_double_t)matrix->sy * matrix->tx) * det);
        mx.ty = (lx_float_t)(((lx_double_t)matrix->ky * matrix->tx - (lx_double_t)matrix->sx * matrix->ty) * det);
    }
    *matrix = mx;
    return lx_true;
}

lx_bool_t lx_matrix_identity(lx_matrix_ref_t matrix) {
    lx_assert(matrix);
    return (    1.0f == matrix->sx
            &&  1.0f == matrix->sy
            &&  0 == matrix->kx
            &&  0 == matrix->ky
            &&  0 == matrix->tx
            &&  0 == matrix->ty);
}

lx_bool_t lx_matrix_rotate(lx_matrix_ref_t matrix, lx_float_t degrees) {
    lx_check_return_val(degrees != 0, lx_true);
    lx_matrix_t mx;
    lx_matrix_init_rotate(&mx, degrees);
    return lx_matrix_multiply(matrix, &mx);
}

lx_bool_t lx_matrix_rotate_lhs(lx_matrix_ref_t matrix, lx_float_t degrees) {
    lx_check_return_val(degrees != 0, lx_true);
    lx_matrix_t mx;
    lx_matrix_init_rotate(&mx, degrees);
    return lx_matrix_multiply_lhs(matrix, &mx);
}

lx_bool_t lx_matrix_rotatep(lx_matrix_ref_t matrix, lx_float_t degrees, lx_float_t px, lx_float_t py) {
    lx_check_return_val(degrees != 0, lx_true);
    lx_matrix_t mx;
    lx_matrix_init_rotatep(&mx, degrees, px, py);
    return lx_matrix_multiply(matrix, &mx);
}

lx_bool_t lx_matrix_rotatep_lhs(lx_matrix_ref_t matrix, lx_float_t degrees, lx_float_t px, lx_float_t py) {
    lx_check_return_val(degrees != 0, lx_true);
    lx_matrix_t mx;
    lx_matrix_init_rotatep(&mx, degrees, px, py);
    return lx_matrix_multiply_lhs(matrix, &mx);
}

lx_bool_t lx_matrix_scale(lx_matrix_ref_t matrix, lx_float_t sx, lx_float_t sy) {
    lx_assert(matrix);
    lx_check_return_val(1.0f != sx || 1.0f != sy, lx_true);

#if 0
    lx_matrix_t mx;
    lx_matrix_init_scale(&mx, sx, sy);
    return lx_matrix_multiply(matrix, &mx);
#else
    matrix->sx = matrix->sx * sx;
    matrix->ky = matrix->ky * sx;
    matrix->kx = matrix->kx * sy;
    matrix->sy = matrix->sy * sy;
    return lx_true;
#endif
}

lx_bool_t lx_matrix_scale_lhs(lx_matrix_ref_t matrix, lx_float_t sx, lx_float_t sy) {
    lx_check_return_val(1.0f != sx || 1.0f != sy, lx_true);
    lx_matrix_t mx;
    lx_matrix_init_scale(&mx, sx, sy);
    return lx_matrix_multiply_lhs(matrix, &mx);
}

lx_bool_t lx_matrix_scalep(lx_matrix_ref_t matrix, lx_float_t sx, lx_float_t sy, lx_float_t px, lx_float_t py) {
    lx_check_return_val(1.0f != sx || 1.0f != sy, lx_true);
    lx_matrix_t mx;
    lx_matrix_init_scalep(&mx, sx, sy, px, py);
    return lx_matrix_multiply(matrix, &mx);
}

lx_bool_t lx_matrix_scalep_lhs(lx_matrix_ref_t matrix, lx_float_t sx, lx_float_t sy, lx_float_t px, lx_float_t py) {
    lx_check_return_val(1.0f != sx || 1.0f != sy, lx_true);
    lx_matrix_t mx;
    lx_matrix_init_scalep(&mx, sx, sy, px, py);
    return lx_matrix_multiply_lhs(matrix, &mx);
}

lx_bool_t lx_matrix_translate(lx_matrix_ref_t matrix, lx_float_t dx, lx_float_t dy) {
    lx_assert(matrix);
    lx_check_return_val(dx != 0 || dy != 0, lx_true);

#if 0
    lx_matrix_t mx;
    lx_matrix_init_translate(&mx, dx, dy);
    return lx_matrix_multiply(matrix, &mx);
#else
    matrix->tx = lx_matrix_mul_add(matrix->sx, dx, matrix->kx, dy) + matrix->tx;
    matrix->ty = lx_matrix_mul_add(matrix->ky, dx, matrix->sy, dy) + matrix->ty;
    return lx_true;
#endif
}

lx_bool_t lx_matrix_translate_lhs(lx_matrix_ref_t matrix, lx_float_t dx, lx_float_t dy) {
    lx_assert(matrix);
    lx_check_return_val(dx != 0 || dy != 0, lx_true);

#if 0
    lx_matrix_t mx;
    lx_matrix_init_translate(&mx, dx, dy);
    return lx_matrix_multiply_lhs(matrix, &mx);
#else
    matrix->tx += dx;
    matrix->ty += dy;
    return lx_true;
#endif
}

lx_bool_t lx_matrix_skew(lx_matrix_ref_t matrix, lx_float_t kx, lx_float_t ky) {
    lx_matrix_t mx;
    lx_matrix_init_skew(&mx, kx, ky);
    return lx_matrix_multiply(matrix, &mx);
}

lx_bool_t lx_matrix_skew_lhs(lx_matrix_ref_t matrix, lx_float_t kx, lx_float_t ky) {
    lx_matrix_t mx;
    lx_matrix_init_skew(&mx, kx, ky);
    return lx_matrix_multiply_lhs(matrix, &mx);
}

lx_bool_t lx_matrix_skewp(lx_matrix_ref_t matrix, lx_float_t kx, lx_float_t ky, lx_float_t px, lx_float_t py) {
    lx_matrix_t mx;
    lx_matrix_init_skewp(&mx, kx, ky, px, py);
    return lx_matrix_multiply(matrix, &mx);
}

lx_bool_t lx_matrix_skewp_lhs(lx_matrix_ref_t matrix, lx_float_t kx, lx_float_t ky, lx_float_t px, lx_float_t py) {
    lx_matrix_t mx;
    lx_matrix_init_skewp(&mx, kx, ky, px, py);
    return lx_matrix_multiply_lhs(matrix, &mx);
}

lx_bool_t lx_matrix_sincos(lx_matrix_ref_t matrix, lx_float_t sin, lx_float_t cos) {
    lx_matrix_t mx;
    lx_matrix_init_sincos(&mx, sin, cos);
    return lx_matrix_multiply(matrix, &mx);
}

lx_bool_t lx_matrix_sincos_lhs(lx_matrix_ref_t matrix, lx_float_t sin, lx_float_t cos) {
    lx_matrix_t mx;
    lx_matrix_init_sincos(&mx, sin, cos);
    return lx_matrix_multiply_lhs(matrix, &mx);
}

lx_bool_t lx_matrix_sincosp(lx_matrix_ref_t matrix, lx_float_t sin, lx_float_t cos, lx_float_t px, lx_float_t py) {
    lx_matrix_t mx;
    lx_matrix_init_sincosp(&mx, sin, cos, px, py);
    return lx_matrix_multiply(matrix, &mx);
}

lx_bool_t lx_matrix_sincosp_lhs(lx_matrix_ref_t matrix, lx_float_t sin, lx_float_t cos, lx_float_t px, lx_float_t py) {
    lx_matrix_t mx;
    lx_matrix_init_sincosp(&mx, sin, cos, px, py);
    return lx_matrix_multiply_lhs(matrix, &mx);
}

lx_bool_t lx_matrix_multiply(lx_matrix_ref_t matrix, lx_matrix_ref_t factor) {
    lx_assert(matrix && factor);
    if (lx_matrix_identity(factor)) {
        return lx_true;
    }

    /* multiply
     *
     * | lsx lkx ltx |   | rsx rkx rtx |
     * | lky lsy lty | * | rky rsy rty |
     * |   0   0   1 |   |   0   0   1 |
     *
     * =>
     *
     * | lsx * rsx + lkx * rky | lsx * rkx + lkx * rsy | lsx * rtx + lkx * rty + ltx |
     * | lky * rsx + lsy * rky | lky * rkx + lsy * rsy | lky * rtx + lsy * rty + lty |
     * |                     0 |                     0 |                           1 |
     *
     * @note path * (A * B * C) != ((path * A) * B) * C
     */
    lx_matrix_t mx;
    mx.sx = lx_matrix_mul_add(matrix->sx, factor->sx, matrix->kx, factor->ky);
    mx.ky = lx_matrix_mul_add(matrix->ky, factor->sx, matrix->sy, factor->ky);

    mx.kx = lx_matrix_mul_add(matrix->sx, factor->kx, matrix->kx, factor->sy);
    mx.sy = lx_matrix_mul_add(matrix->ky, factor->kx, matrix->sy, factor->sy);

    mx.tx = lx_matrix_mul_add(matrix->sx, factor->tx, matrix->kx, factor->ty) + matrix->tx;
    mx.ty = lx_matrix_mul_add(matrix->ky, factor->tx, matrix->sy, factor->ty) + matrix->ty;

    *matrix = mx;
    return lx_true;
}

lx_bool_t lx_matrix_multiply_lhs(lx_matrix_ref_t matrix, lx_matrix_ref_t factor) {
    if (lx_matrix_identity(factor)) return lx_true;
    lx_matrix_t mx = *factor;
    lx_bool_t ok = lx_matrix_multiply(&mx, matrix);
    *matrix = mx;
    return ok;
}

