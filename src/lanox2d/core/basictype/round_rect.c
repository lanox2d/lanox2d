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
 * @file        round_rect.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "round_rect.h"
#include "utils.h"
#include "point.h"
#include "matrix.h"
#include "vector.h"
#include "../../base/base.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_void_t lx_round_rect_make(lx_round_rect_ref_t rect, lx_rect_ref_t bounds, lx_vector_t radius[]) {
    lx_assert(rect && bounds && radius);

    rect->radius[0] = radius[0];
    rect->radius[1] = radius[1];
    rect->radius[2] = radius[2];
    rect->radius[3] = radius[3];
}

lx_void_t lx_round_rect_make_same(lx_round_rect_ref_t rect, lx_rect_ref_t bounds, lx_float_t rx, lx_float_t ry) {
    lx_assert(rect && bounds);

    lx_vector_t radius;
    lx_vector_make(&radius, rx, ry);

    rect->radius[0] = radius;
    rect->radius[1] = radius;
    rect->radius[2] = radius;
    rect->radius[3] = radius;
    rect->bounds    = *bounds;
}

lx_void_t lx_round_rect_imake_same(lx_round_rect_ref_t rect, lx_rect_ref_t bounds, lx_size_t rx, lx_size_t ry) {
    lx_round_rect_make_same(rect, bounds, (lx_float_t)rx, (lx_float_t)ry);
}

lx_bool_t lx_round_rect_is_rect(lx_round_rect_ref_t rect) {
    lx_assert(rect);
    return (    (rect->radius[0].x <= 0)
            &&  (rect->radius[0].y <= 0)
            &&  (rect->radius[1].x <= 0)
            &&  (rect->radius[1].y <= 0)
            &&  (rect->radius[2].x <= 0)
            &&  (rect->radius[2].y <= 0)
            &&  (rect->radius[3].x <= 0)
            &&  (rect->radius[3].y <= 0));
}

lx_bool_t lx_round_rect_is_ellipse(lx_round_rect_ref_t rect) {
    lx_assert(rect);
    lx_float_t rx = rect->bounds.w / 2.0f;
    lx_float_t ry = rect->bounds.h / 2.0f;
    return (    rect->radius[0].x >= rx
            &&  rect->radius[0].y >= ry
            &&  rect->radius[1].x >= rx
            &&  rect->radius[1].y >= ry
            &&  rect->radius[2].x >= rx
            &&  rect->radius[2].y >= ry
            &&  rect->radius[3].x >= rx
            &&  rect->radius[3].y >= ry);
}
