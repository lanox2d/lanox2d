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
 * @file        circle.h
 *
 */
#ifndef LX_CORE_PRIMITIVE_CIRCLE_H
#define LX_CORE_PRIMITIVE_CIRCLE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "point.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline interfaces
 */

/*! make circle
 *
 * @param circle    the circle
 * @param x0        the x0
 * @param y0        the y0
 * @param r         the radius
 */
static lx_inline lx_void_t lx_circle_make(lx_circle_ref_t circle, lx_float_t x0, lx_float_t y0, lx_float_t r) {
    circle->c.x = x0;
    circle->c.y = y0;
    circle->r   = r;
}

/*! make circle with the integer value
 *
 * @param circle    the circle
 * @param x0        the x0
 * @param y0        the y0
 * @param r         the radius
 */
static lx_inline lx_void_t lx_circle_imake(lx_circle_ref_t circle, lx_long_t x0, lx_long_t y0, lx_size_t r) {
    lx_circle_make(circle, (lx_float_t)(x0), (lx_float_t)y0, (lx_float_t)r);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


