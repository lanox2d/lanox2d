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
 * @file        round_rect.h
 *
 */
#ifndef LX_CORE_PRIMITIVE_ROUND_RECT_H
#define LX_CORE_PRIMITIVE_ROUND_RECT_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! make round rect
 *
 * @param rect      the rect
 * @param bounds    the bounds
 * @param radius    the radius of the four corners
 */
lx_void_t           lx_round_rect_make(lx_round_rect_ref_t rect, lx_rect_ref_t bounds, lx_vector_t radius[]);

/*! make round rect using the same x and y radius
 *
 * @param rect      the rect
 * @param bounds    the bounds
 * @param rx        the x-radius
 * @param ry        the y-radius
 */
lx_void_t           lx_round_rect_make_same(lx_round_rect_ref_t rect, lx_rect_ref_t bounds, lx_float_t rx, lx_float_t ry);

/*! make round rect using the same integer x and y radius
 *
 * @param rect      the rect
 * @param bounds    the bounds
 * @param rx        the x-radius
 * @param ry        the y-radius
 */
lx_void_t           lx_round_rect_imake_same(lx_round_rect_ref_t rect, lx_rect_ref_t bounds, lx_size_t rx, lx_size_t ry);

/*! the round rect is rect?
 *
 * @param rect      the rect
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_round_rect_is_rect(lx_round_rect_ref_t rect);

/*! the round rect is ellipse?
 *
 * @param rect      the rect
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_round_rect_is_ellipse(lx_round_rect_ref_t rect);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


