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
 * @file        point.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "point.h"
#include "vector.h"
#include "matrix.h"
#include "../../base/base.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

lx_void_t lx_point_make(lx_point_ref_t point, lx_float_t x, lx_float_t y) {
    point->x = x;
    point->y = y;
}

lx_void_t lx_point_imake(lx_point_ref_t point, lx_long_t x, lx_long_t y) {
    lx_point_make(point, (lx_float_t)(x), (lx_float_t)(y));
}

lx_void_t lx_point_apply(lx_point_ref_t point, lx_matrix_ref_t matrix) {
    lx_float_t x = point->x;
    lx_float_t y = point->y;
    point->x = lx_matrix_apply_x(matrix, x, y);
    point->y = lx_matrix_apply_y(matrix, x, y);
}

lx_void_t lx_point_apply2(lx_point_ref_t point, lx_point_ref_t applied, lx_matrix_ref_t matrix) {
    lx_float_t x = point->x;
    lx_float_t y = point->y;
    applied->x = lx_matrix_apply_x(matrix, x, y);
    applied->y = lx_matrix_apply_y(matrix, x, y);
}

lx_float_t lx_point_distance(lx_point_ref_t point, lx_point_ref_t other) {
    lx_vector_t vector;
    lx_vector_make(&vector, other->x - point->x, other->y - point->y);
    return lx_vector_length(&vector);
}

lx_bool_t lx_point_near_eq(lx_point_ref_t point, lx_point_ref_t other) {
    return (lx_near_eq(point->x, other->x)) && (lx_near_eq(point->y, other->y));
}
