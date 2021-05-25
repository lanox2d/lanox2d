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
 * @file        vector.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "vector.h"
#include "../../base/base.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_void_t lx_vector_make(lx_vector_ref_t vector, lx_float_t x, lx_float_t y) {
    vector->x = x;
    vector->y = y;
}

lx_void_t lx_vector_imake(lx_vector_ref_t vector, lx_long_t x, lx_long_t y) {
    lx_vector_make(vector, (lx_float_t)x, (lx_float_t)y);
}

lx_void_t lx_vector_make_from_point(lx_vector_ref_t vector, lx_point_ref_t point) {
    *vector = *((lx_vector_ref_t)point);
}

lx_void_t lx_vector_make_from_two_points(lx_vector_ref_t vector, lx_point_ref_t before, lx_point_ref_t after) {
    lx_vector_make(vector, after->x - before->x, after->y - before->y);
}

lx_bool_t lx_vector_make_unit(lx_vector_ref_t vector, lx_float_t x, lx_float_t y) {
    lx_vector_t unit;
    lx_vector_make(&unit, x, y);
    if (vector && lx_vector_normalize(&unit)) {
        *vector = unit;
        return lx_true;
    }
    return lx_false;
}

lx_bool_t lx_vector_imake_unit(lx_vector_ref_t vector, lx_long_t x, lx_long_t y) {
    return lx_vector_make_unit(vector, (lx_float_t)x, (lx_float_t)y);
}

lx_void_t lx_vector_negate(lx_vector_ref_t vector) {
    vector->x = -vector->x;
    vector->y = -vector->y;
}

lx_void_t lx_vector_negate2(lx_vector_ref_t vector, lx_vector_ref_t negated) {
    negated->x = -vector->x;
    negated->y = -vector->y;
}

lx_void_t lx_vector_rotate(lx_vector_ref_t vector, lx_size_t direction) {
    lx_vector_rotate2(vector, vector, direction);
}

lx_void_t lx_vector_rotate2(lx_vector_ref_t vector, lx_vector_ref_t rotated, lx_size_t direction) {
    /* rotate it
     *
     * @note using t for avoiding the case of vector == rotated
     */
    lx_float_t t = vector->x;
    if (direction == LX_ROTATE_DIRECTION_CW) {
        rotated->x = -vector->y;
        rotated->y = t;
    } else {
        rotated->x = vector->y;
        rotated->y = -t;
    }
}

lx_void_t lx_vector_scale(lx_vector_ref_t vector, lx_float_t scale) {
    lx_vector_scale2(vector, vector, scale);
}

lx_void_t lx_vector_scale2(lx_vector_ref_t vector, lx_vector_ref_t scaled, lx_float_t scale) {
    scaled->x = vector->x * scale;
    scaled->y = vector->y * scale;
}

lx_float_t lx_vector_length(lx_vector_ref_t vector) {

    // the dx and dy
    lx_float_t dx = vector->x;
    lx_float_t dy = vector->y;

    // attempt to compute the length directly
    lx_float_t length = 0;
    lx_float_t dd = dx * dx + dy * dy;
    if (lx_isfinf(dd)) {
        length = lx_sqrt(dd);
    } else {
        // compute the length using the double value
        lx_double_t xx = dx;
        lx_double_t yy = dy;
        length = (lx_float_t)lx_sqrt(xx * xx + yy * yy);
    }
    return length;
}

lx_bool_t lx_vector_length_set(lx_vector_ref_t vector, lx_float_t length) {

    // check
    lx_assert(vector);
    lx_assert(length > LX_NEAR0 && lx_isfinf(length));

    // the self length
    lx_float_t length_self = lx_vector_length(vector);

    // check the self length
    lx_check_return_val(length_self > LX_NEAR0 && lx_isfinf(length_self), lx_false);

    // compute the scale
    lx_float_t scale = length / length_self;

    // this scale is zero nearly?
    if (scale <= LX_NEAR0) {

        // compute the length using the double value
        lx_double_t xx = (lx_double_t)vector->x;
        lx_double_t yy = (lx_double_t)vector->y;
        lx_double_t ll = lx_sqrt(xx * xx + yy * yy);
        lx_double_t ss = (lx_double_t)length / ll;

        // compute the new vector
        vector->x = (lx_float_t)(xx * ss);
        vector->y = (lx_float_t)(yy * ss);
        return lx_true;
    }

    // compute the new vector
    vector->x = vector->x * scale;
    vector->y = vector->y * scale;
    return lx_true;
}

lx_bool_t lx_vector_can_normalize(lx_vector_ref_t vector) {
    lx_float_t dx = vector->x;
    lx_float_t dy = vector->y;
    return (dx * dx + dy * dy) > (LX_NEAR0 * LX_NEAR0);
}

lx_bool_t lx_vector_normalize(lx_vector_ref_t vector) {
    return lx_vector_length_set(vector, 1.0f);
}

lx_bool_t lx_vector_normalize2(lx_vector_ref_t vector, lx_vector_ref_t normalized) {
    lx_vector_t unit = *vector;
    if (normalized && lx_vector_normalize(&unit)) {
        *normalized = unit;
        return lx_true;
    }
    return lx_false;
}

lx_float_t lx_vector_dot(lx_vector_ref_t vector, lx_vector_ref_t other) {
    lx_float_t ax = vector->x;
    lx_float_t ay = vector->y;
    lx_float_t bx = other->x;
    lx_float_t by = other->y;
    return (ax * bx) + (ay * by);
}

lx_float_t lx_vector_cross(lx_vector_ref_t vector, lx_vector_ref_t other) {
    lx_float_t ax = vector->x;
    lx_float_t ay = vector->y;
    lx_float_t bx = other->x;
    lx_float_t by = other->y;
    return (ax * by) - (ay * bx);
}

lx_bool_t lx_vector_is_clockwise(lx_vector_ref_t vector, lx_vector_ref_t other) {
    return (vector->x * other->y) > (vector->y * other->x);
}

lx_bool_t lx_vector_near_eq(lx_vector_ref_t vector, lx_vector_ref_t other) {
    return (lx_near_eq(vector->x, other->x)) && (lx_near_eq(vector->y, other->y));
}
