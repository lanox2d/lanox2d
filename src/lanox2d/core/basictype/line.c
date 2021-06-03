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
 * @file        line.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "line.h"
#include "point.h"
#include "../../base/base.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_void_t lx_line_make(lx_line_ref_t line, lx_float_t x0, lx_float_t y0, lx_float_t x1, lx_float_t y1) {
    line->p0.x = x0;
    line->p0.y = y0;
    line->p1.x = x1;
    line->p1.y = y1;
}

lx_void_t lx_line_imake(lx_line_ref_t line, lx_long_t x0, lx_long_t y0, lx_long_t x1, lx_long_t y1) {
    lx_line_make(line, (lx_float_t)x0, (lx_float_t)y0, (lx_float_t)x1, (lx_float_t)y1);
}

lx_void_t lx_line_apply(lx_line_ref_t line, lx_matrix_ref_t matrix) {
    lx_point_apply(&line->p0, matrix);
    lx_point_apply(&line->p1, matrix);
}

lx_void_t lx_line_apply2(lx_line_ref_t line, lx_line_ref_t applied, lx_matrix_ref_t matrix) {
    lx_point_apply2(&line->p0, &applied->p0, matrix);
    lx_point_apply2(&line->p1, &applied->p1, matrix);
}
