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

/*! make vector
 *
 * @param vector        the vector
 * @param x             the x
 * @param y             the y
 */
lx_void_t               lx_vector_make(lx_vector_ref_t vector, lx_float_t x, lx_float_t y);

/*! make vector with the integer value
 *
 * @param vector        the vector
 * @param x             the x
 * @param y             the y
 */
lx_void_t               lx_vector_imake(lx_vector_ref_t vector, lx_long_t x, lx_long_t y);

/*! make vector from the given point
 *
 * @param vector        the vector
 * @param point         the point
 */
lx_void_t               lx_vector_make_from_point(lx_vector_ref_t vector, lx_point_ref_t point);

/*! make vector from the given two points
 *
 * @param vector        the vector
 * @param before        the before point
 * @param after         the after point
 */
lx_void_t               lx_vector_make_from_two_points(lx_vector_ref_t vector, lx_point_ref_t before, lx_point_ref_t after);

/*! make the unit vector
 *
 * @param vector        the vector
 * @param x             the x
 * @param y             the y
 *
 * @return              lx_true or lx_false
 */
lx_bool_t               lx_vector_make_unit(lx_vector_ref_t vector, lx_float_t x, lx_float_t y);

/*! make unit vector with the integer value
 *
 * @param vector        the vector
 * @param x             the x
 * @param y             the y
 *
 * @return              lx_true or lx_false
 */
lx_bool_t               lx_vector_imake_unit(lx_vector_ref_t vector, lx_long_t x, lx_long_t y);

/*! nagate the vector
 *
 * @param vector        the vector
 */
lx_void_t               lx_vector_negate(lx_vector_ref_t vector);

/*! nagate the vector to the given vector
 *
 * @param vector        the vector
 * @param negated       the negated vector
 */
lx_void_t               lx_vector_negate2(lx_vector_ref_t vector, lx_vector_ref_t negated);

/*! rotate 90 degrees
 *
 * @param vector        the vector
 * @param direction     the direction
 */
lx_void_t               lx_vector_rotate(lx_vector_ref_t vector, lx_size_t direction);

/*! rotate 90 degrees to the given vector
 *
 * @param vector        the vector
 * @param rotated       the rotated vector
 * @param direction     the direction
 */
lx_void_t               lx_vector_rotate2(lx_vector_ref_t vector, lx_vector_ref_t rotated, lx_size_t direction);

/*! scale the given value
 *
 * @param vector        the vector
 * @param scale         the scale
 */
lx_void_t               lx_vector_scale(lx_vector_ref_t vector, lx_float_t scale);

/*! scale the given value to the given vector
 *
 * @param vector        the vector
 * @param scaled        the scaled vector
 * @param scale         the scale
 */
lx_void_t               lx_vector_scale2(lx_vector_ref_t vector, lx_vector_ref_t scaled, lx_float_t scale);

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

/*! can normalize the vector?
 *
 * @param vector        the vector
 *
 * @return              lx_true or lx_false
 */
lx_bool_t               lx_vector_can_normalize(lx_vector_ref_t vector);

/*! normalize the vector
 *
 * @param vector        the vector
 *
 * @return              lx_true or lx_false
 */
lx_bool_t               lx_vector_normalize(lx_vector_ref_t vector);

/*! normalize to the given vector
 *
 * @param vector        the vector
 * @param normalized    the normalized vector
 *
 * @return              lx_true or lx_false
 */
lx_bool_t               lx_vector_normalize2(lx_vector_ref_t vector, lx_vector_ref_t normalized);

/*! compute the dot product for the two vectors
 *
 * dot = |vector| * |other| * cos(a)
 *
 * @param vector        the vector
 * @param other         the other vector
 *
 * @return              the dot product
 */
lx_float_t              lx_vector_dot(lx_vector_ref_t vector, lx_vector_ref_t other);

/*! compute the cross product for the two vectors
 *
 * cross = |vector| * |other| * sin(a)
 *
 * @param vector        the vector
 * @param other         the other vector
 *
 * @return              the cross product
 */
lx_float_t              lx_vector_cross(lx_vector_ref_t vector, lx_vector_ref_t other);

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
lx_bool_t               lx_vector_is_clockwise(lx_vector_ref_t vector, lx_vector_ref_t other);

/*! be equal to the other vector?
 *
 * @param vector        the vector
 * @param other         the other vector
 *
 * @return              lx_true or lx_false
 */
lx_bool_t               lx_vector_near_eq(lx_vector_ref_t vector, lx_vector_ref_t other);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


