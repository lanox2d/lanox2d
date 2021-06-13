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
 * @file        stroker.h
 *
 */
#ifndef LX_CORE_PRIVATE_STROKER_H
#define LX_CORE_PRIVATE_STROKER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the default miter limit
#define LX_STROKER_DEFAULT_MITER         (4.0f)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the stroker type
typedef lx_typeref(stroker);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* init stroker
 *
 * @return                  the stroker
 */
lx_stroker_ref_t            lx_stroker_init(lx_noarg_t);

/* exit stroker
 *
 * @param stroker           the stroker
 */
lx_void_t                   lx_stroker_exit(lx_stroker_ref_t stroker);

/* clear stroker
 *
 * @param stroker           the stroker
 */
lx_void_t                   lx_stroker_clear(lx_stroker_ref_t stroker);

/* apply paint to stroker
 *
 * @param stroker           the stroker
 * @param paint             the paint
 */
lx_void_t                   lx_stroker_apply_paint(lx_stroker_ref_t stroker, lx_paint_ref_t paint);

/* close
 *
 * @param stroker           the stroker
 */
lx_void_t                   lx_stroker_clos(lx_stroker_ref_t stroker);

/*! move to the point
 *
 * @param stroker           the stroker
 * @param point             the point
 */
lx_void_t                   lx_stroker_move_to(lx_stroker_ref_t stroker, lx_point_ref_t point);

/*! line to the point
 *
 * @param stroker           the stroker
 * @param point             the point
 */
lx_void_t                   lx_stroker_line_to(lx_stroker_ref_t stroker, lx_point_ref_t point);

/*! quad to the ctrl, point
 *
 * @param stroker           the stroker
 * @param ctrl              the control point
 * @param point             the point
 */
lx_void_t                   lx_stroker_quad_to(lx_stroker_ref_t stroker, lx_point_ref_t ctrl, lx_point_ref_t point);

/*! cubic to the ctrl0, ctrl1, point
 *
 * @param stroker           the stroker
 * @param ctrl0             the control0 point
 * @param ctrl1             the control1 point
 * @param point             the point
 */
lx_void_t                   lx_stroker_cubic_to(lx_stroker_ref_t stroker, lx_point_ref_t ctrl0, lx_point_ref_t ctrl1, lx_point_ref_t point);

/* add path
 *
 * @param stroker           the stroker
 * @param path              the path
 */
lx_void_t                   lx_stroker_add_path(lx_stroker_ref_t stroker, lx_path_ref_t path);

/* add rect
 *
 * @param stroker           the stroker
 * @param rect              the rect
 */
lx_void_t                   lx_stroker_add_rect(lx_stroker_ref_t stroker, lx_rect_ref_t rect);

/* add circle
 *
 * @param stroker           the stroker
 * @param circle            the circle
 */
lx_void_t                   lx_stroker_add_circle(lx_stroker_ref_t stroker, lx_circle_ref_t circle);

/* add ellipse
 *
 * @param stroker           the stroker
 * @param ellipse           the ellipse
 */
lx_void_t                   lx_stroker_add_ellipse(lx_stroker_ref_t stroker, lx_ellipse_ref_t ellipse);

/* add lines
 *
 * @param stroker           the stroker
 * @param points            the points
 * @param count             the points count
 */
lx_void_t                   lx_stroker_add_lines(lx_stroker_ref_t stroker, lx_point_ref_t points, lx_size_t count);

/* add points
 *
 * @param stroker           the stroker
 * @param points            the points
 * @param count             the points count
 */
lx_void_t                   lx_stroker_add_points(lx_stroker_ref_t stroker, lx_point_ref_t points, lx_size_t count);

/* add polygon
 *
 * @param stroker           the stroker
 * @param polygon           the polygon
 */
lx_void_t                   lx_stroker_add_polygon(lx_stroker_ref_t stroker, lx_polygon_ref_t polygon);

/* make the stroked path
 *
 * @param stroker           the stroker
 * @param convex            is convex?
 *
 * @return                  the stroked path
 */
lx_path_ref_t               lx_stroker_make(lx_stroker_ref_t stroker, lx_bool_t convex);

/* make the stroked path from the given path
 *
 * @param stroker           the stroker
 * @param paint             the paint
 * @param path              the path
 *
 * @return                  the stroked path
 */
lx_path_ref_t               lx_stroker_make_from_path(lx_stroker_ref_t stroker, lx_paint_ref_t paint, lx_path_ref_t path);

/* make the stroked path from the given lines
 *
 * @param stroker           the stroker
 * @param paint             the paint
 * @param points            the points
 * @param count             the points count
 *
 * @return                  the stroked path
 */
lx_path_ref_t               lx_stroker_make_from_lines(lx_stroker_ref_t stroker, lx_paint_ref_t paint, lx_point_ref_t points, lx_size_t count);

/* make the stroked path from the given points
 *
 * @param stroker           the stroker
 * @param paint             the paint
 * @param points            the points
 * @param count             the points count
 *
 * @return                  the stroked path
 */
lx_path_ref_t               lx_stroker_make_from_points(lx_stroker_ref_t stroker, lx_paint_ref_t paint, lx_point_ref_t points, lx_size_t count);

/* make the stroked path from the given polygon
 *
 * @param stroker           the stroker
 * @param paint             the paint
 * @param polygon           the polygon
 * @param hint              the hint shape
 *
 * @return                  the stroked path
 */
lx_path_ref_t               lx_stroker_make_from_polygon(lx_stroker_ref_t stroker, lx_paint_ref_t paint, lx_polygon_ref_t polygon, lx_shape_ref_t hint);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


