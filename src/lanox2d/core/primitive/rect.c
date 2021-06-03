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
 * @file        rect.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "rect.h"
#include "point.h"
#include "utils.h"
#include "../../base/base.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_void_t lx_rect_make(lx_rect_ref_t rect, lx_float_t x, lx_float_t y, lx_float_t w, lx_float_t h) {
    rect->x = x;
    rect->y = y;
    rect->w = w;
    rect->h = h;
}

lx_void_t lx_rect_imake(lx_rect_ref_t rect, lx_long_t x, lx_long_t y, lx_size_t w, lx_size_t h) {
    lx_rect_make(rect, (lx_float_t)x, (lx_float_t)y, (lx_float_t)w, (lx_float_t)h);
}

lx_void_t lx_rect_apply(lx_rect_ref_t rect, lx_matrix_ref_t matrix) {
    lx_rect_apply2(rect, rect, matrix);
}

lx_void_t lx_rect_apply2(lx_rect_ref_t rect, lx_rect_ref_t applied, lx_matrix_ref_t matrix) {
    lx_point_t points[2];
    lx_point_make(&points[0], rect->x, rect->y);
    lx_point_make(&points[1], rect->x + rect->w, rect->y + rect->h);
    lx_matrix_apply_points(matrix, points, lx_arrayn(points));
    lx_bounds_make(applied, points, lx_arrayn(points));
}

lx_void_t lx_rect_inflate(lx_rect_ref_t rect, lx_float_t dx, lx_float_t dy) {
    lx_assert(rect && dx > 0 && dy > 0);
    rect->x -= dx;
    rect->y -= dy;
    rect->w += dx * 2.0f;
    rect->h += dy * 2.0f;
}

lx_void_t lx_rect_deflate(lx_rect_ref_t rect, lx_float_t dx, lx_float_t dy) {
    lx_assert(rect && dx > 0 && dy > 0);
    lx_assert(rect->w >= dx * 2.0f && rect->h >= dy * 2.0f);
    rect->x += dx;
    rect->y += dy;
    rect->w -= dx * 2.0f;
    rect->h -= dy * 2.0f;
}
