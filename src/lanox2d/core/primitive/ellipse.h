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
 * @file        ellipse.h
 *
 */
#ifndef LX_CORE_PRIMITIVE_ELLIPSE_H
#define LX_CORE_PRIMITIVE_ELLIPSE_H

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

/*! make ellipse
 *
 * @param ellipse   the ellipse
 * @param x0        the x0
 * @param y0        the y0
 * @param rx        the x-radius
 * @param ry        the y-radius
 */
lx_void_t           lx_ellipse_make(lx_ellipse_ref_t ellipse, lx_float_t x0, lx_float_t y0, lx_float_t rx, lx_float_t ry);

/*! make ellipse with the integer value
 *
 * @param ellipse   the ellipse
 * @param x0        the x0
 * @param y0        the y0
 * @param rx        the x-radius
 * @param ry        the y-radius
 */
lx_void_t           lx_ellipse_imake(lx_ellipse_ref_t ellipse, lx_long_t x0, lx_long_t y0, lx_size_t rx, lx_size_t ry);

/*! make ellipse from rect
 *
 * @param ellipse   the ellipse
 * @param rect      the rect
 */
lx_void_t           lx_ellipse_make_from_rect(lx_ellipse_ref_t ellipse, lx_rect_ref_t rect);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


