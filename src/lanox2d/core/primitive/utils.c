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
 * @file        utils.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "utils.h"
#include "point.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

lx_void_t lx_bounds_make(lx_rect_ref_t bounds, lx_point_ref_t points, lx_size_t count) {
    lx_assert(bounds && points && count);
    lx_point_ref_t point = points++; count--;
    lx_float_t x0 = point->x;
    lx_float_t y0 = point->y;
    lx_float_t x1 = x0;
    lx_float_t y1 = y0;
    lx_float_t x;
    lx_float_t y;
    while (count--) {
        x = points->x;
        y = points->y;
        if (x < x0) x0 = x;
        if (y < y0) y0 = y;
        if (x > x1) x1 = x;
        if (y > y1) y1 = y;
        points++;
    }
    bounds->x = x0;
    bounds->y = y0;
    bounds->w = x1 - x0;
    bounds->h = y1 - y0;
}

lx_void_t lx_matrix_apply_points(lx_matrix_ref_t matrix, lx_point_ref_t points, lx_size_t count) {
    lx_assert(matrix && points && count);
    lx_size_t i = 0;
    for (i = 0; i < count; i++) {
        lx_point_apply(points + i, matrix);
    }
}

