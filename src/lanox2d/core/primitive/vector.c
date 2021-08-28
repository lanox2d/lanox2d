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

lx_bool_t lx_vector_make_unit(lx_vector_ref_t vector, lx_float_t x, lx_float_t y) {
    lx_vector_t unit;
    lx_vector_make(&unit, x, y);
    if (vector && lx_vector_normalize(&unit)) {
        *vector = unit;
        return lx_true;
    }
    return lx_false;
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

lx_bool_t lx_vector_normalize2(lx_vector_ref_t vector, lx_vector_ref_t normalized) {
    lx_vector_t unit = *vector;
    if (normalized && lx_vector_normalize(&unit)) {
        *normalized = unit;
        return lx_true;
    }
    return lx_false;
}

lx_float_t lx_vector_length(lx_vector_ref_t vector) {

    // the dx and dy
    lx_float_t dx = vector->x;
    lx_float_t dy = vector->y;

    // attempt to compute the length directly
    lx_float_t length = 0;
    lx_float_t dd = dx * dx + dy * dy;
    if (lx_isfinf(dd)) {
        length = (lx_float_t)lx_sqrt(dd);
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

