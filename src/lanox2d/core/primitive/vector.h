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
 * @file        vector.h
 *
 */
#ifndef LX_CORE_PRIMITIVE_VECTOR_H
#define LX_CORE_PRIMITIVE_VECTOR_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// vector is equal?
#define lx_vector_eq(a, b)       ((a)->x == (b)->x && (a)->y == (b)->y)

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! make the unit vector
 *
 * @param vector        the vector
 * @param x             the x
 * @param y             the y
 *
 * @return              lx_true or lx_false
 */
lx_bool_t               lx_vector_make_unit(lx_vector_ref_t vector, lx_float_t x, lx_float_t y);

/*! rotate 90 degrees to the given vector
 *
 * @param vector        the vector
 * @param rotated       the rotated vector
 * @param direction     the direction
 */
lx_void_t               lx_vector_rotate2(lx_vector_ref_t vector, lx_vector_ref_t rotated, lx_size_t direction);

/*! the vector length
 *
 * @param vector        the vector
 *
 * @return              the vector length
 */
lx_float_t              lx_vector_length(lx_vector_ref_t vector);

/*! set the vector length
 *
 * @param vector        the vector
 * @param length        the vector length
 *
 * @return              lx_true or lx_false
 */
lx_bool_t               lx_vector_length_set(lx_vector_ref_t vector, lx_float_t length);

/*! normalize to the given vector
 *
 * @param vector        the vector
 * @param normalized    the normalized vector
 *
 * @return              lx_true or lx_false
 */
lx_bool_t               lx_vector_normalize2(lx_vector_ref_t vector, lx_vector_ref_t normalized);

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines interfaces
 */

/*! make vector
 *
 * @param vector        the vector
 * @param x             the x
 * @param y             the y
 */
static lx_inline lx_void_t lx_vector_make(lx_vector_ref_t vector, lx_float_t x, lx_float_t y) {
    vector->x = x;
    vector->y = y;
}

/*! make vector with the integer value
 *
 * @param vector        the vector
 * @param x             the x
 * @param y             the y
 */
static lx_inline lx_void_t lx_vector_imake(lx_vector_ref_t vector, lx_long_t x, lx_long_t y) {
    lx_vector_make(vector, (lx_float_t)x, (lx_float_t)y);
}

/*! make vector from the given point
 *
 * @param vector        the vector
 * @param point         the point
 */
static lx_inline lx_void_t lx_vector_make_from_point(lx_vector_ref_t vector, lx_point_ref_t point) {
    *vector = *((lx_vector_ref_t)point);
}

/*! make vector from the given two points
 *
 * @param vector        the vector
 * @param before        the before point
 * @param after         the after point
 */
static lx_inline lx_void_t lx_vector_make_from_two_points(lx_vector_ref_t vector, lx_point_ref_t before, lx_point_ref_t after) {
    lx_vector_make(vector, after->x - before->x, after->y - before->y);
}

/*! make the unit vector
 *
 * @param vector        the vector
 * @param x             the x
 * @param y             the y
 *
 * @return              lx_true or lx_false
 */
static lx_inline lx_bool_t lx_vector_imake_unit(lx_vector_ref_t vector, lx_long_t x, lx_long_t y) {
    return lx_vector_make_unit(vector, (lx_float_t)x, (lx_float_t)y);
}

/*! nagate the vector
 *
 * @param vector        the vector
 */
static lx_inline lx_void_t lx_vector_negate(lx_vector_ref_t vector) {
    vector->x = -vector->x;
    vector->y = -vector->y;
}

/*! nagate the vector to the given vector
 *
 * @param vector        the vector
 * @param negated       the negated vector
 */
static lx_inline lx_void_t lx_vector_negate2(lx_vector_ref_t vector, lx_vector_ref_t negated) {
    negated->x = -vector->x;
    negated->y = -vector->y;
}

/*! rotate 90 degrees
 *
 * @param vector        the vector
 * @param direction     the direction
 */
static lx_inline lx_void_t lx_vector_rotate(lx_vector_ref_t vector, lx_size_t direction) {
    lx_vector_rotate2(vector, vector, direction);
}

/*! scale the given value to the given vector
 *
 * @param vector        the vector
 * @param scaled        the scaled vector
 * @param scale         the scale
 */
static lx_inline lx_void_t lx_vector_scale2(lx_vector_ref_t vector, lx_vector_ref_t scaled, lx_float_t scale) {
    scaled->x = vector->x * scale;
    scaled->y = vector->y * scale;
}

/*! scale the given value
 *
 * @param vector        the vector
 * @param scale         the scale
 */
static lx_inline lx_void_t lx_vector_scale(lx_vector_ref_t vector, lx_float_t scale) {
    lx_vector_scale2(vector, vector, scale);
}

/*! can normalize the vector?
 *
 * @param vector        the vector
 *
 * @return              lx_true or lx_false
 */
static lx_inline lx_bool_t lx_vector_can_normalize(lx_vector_ref_t vector) {
    lx_float_t dx = vector->x;
    lx_float_t dy = vector->y;
    return (dx * dx + dy * dy) > (LX_NEAR0 * LX_NEAR0);
}

/*! normalize the vector
 *
 * @param vector        the vector
 *
 * @return              lx_true or lx_false
 */
static lx_inline lx_bool_t lx_vector_normalize(lx_vector_ref_t vector) {
    return lx_vector_length_set(vector, 1.0f);
}

/*! compute the dot product for the two vectors
 *
 * dot = |vector| * |other| * cos(a)
 *
 * @param vector        the vector
 * @param other         the other vector
 *
 * @return              the dot product
 */
static lx_inline lx_float_t lx_vector_dot(lx_vector_ref_t vector, lx_vector_ref_t other) {
    lx_float_t ax = vector->x;
    lx_float_t ay = vector->y;
    lx_float_t bx = other->x;
    lx_float_t by = other->y;
    return (ax * bx) + (ay * by);
}

/*! compute the cross product for the two vectors
 *
 * cross = |vector| * |other| * sin(a)
 *
 * @param vector        the vector
 * @param other         the other vector
 *
 * @return              the cross product
 */
static lx_inline lx_float_t lx_vector_cross(lx_vector_ref_t vector, lx_vector_ref_t other) {
    lx_float_t ax = vector->x;
    lx_float_t ay = vector->y;
    lx_float_t bx = other->x;
    lx_float_t by = other->y;
    return (ax * by) - (ay * bx);
}

/*! the other vector is in the clockwise of the vector
 *
 * <pre>
 * . . . . . . . . . vector
 * .
 * .
 * .
 * .
 * other
 * </pre>
 *
 * @param vector        the vector
 * @param other         the other vector
 *
 * @return              lx_true or lx_false
 */
static lx_inline lx_bool_t lx_vector_is_clockwise(lx_vector_ref_t vector, lx_vector_ref_t other) {
    return (vector->x * other->y) > (vector->y * other->x);
}

/*! be equal to the other vector?
 *
 * @param vector        the vector
 * @param other         the other vector
 *
 * @return              lx_true or lx_false
 */
static lx_inline lx_bool_t lx_vector_near_eq(lx_vector_ref_t vector, lx_vector_ref_t other) {
    return (lx_near_eq(vector->x, other->x)) && (lx_near_eq(vector->y, other->y));
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


