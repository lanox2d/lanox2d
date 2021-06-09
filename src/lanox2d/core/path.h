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
 * @file        path.h
 *
 */
#ifndef LX_CORE_PATH_H
#define LX_CORE_PATH_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the path code enum
 *
 * the point count: |code - 1|: 1 0 1 2 3
 */
typedef enum lx_path_code_e_ {
    LX_PATH_CODE_MOVE  = 0 //!< the move-to code
,   LX_PATH_CODE_CLOSE = 1 //!< the close code
,   LX_PATH_CODE_LINE  = 2 //!< the line-to code
,   LX_PATH_CODE_QUAD  = 3 //!< the quad-to code
,   LX_PATH_CODE_CUBIC = 4 //!< the cubic-to code
,   LX_PATH_CODE_MAXN  = 5 //!< the code max count
}lx_path_code_e;

/// the path item for the iterator
typedef struct lx_path_item_t_ {

    /// the code
    lx_uint8_t      code;

    /*! the points[4]
     *
     * move-to:     points[0]
     * line-to:     points[0], points[1]
     * quad-to:     points[0], points[1], points[2]
     * cubic-to:    points[0], points[1], points[2], points[3]
     * close:       points[0]
     */
    lx_point_ref_t  points;
}lx_path_item_t, *lx_path_item_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init path
 *
 * @return          the path
 */
lx_path_ref_t       lx_path_init(lx_noarg_t);

/*! exit path
 *
 * @param path      the path
 */
lx_void_t           lx_path_exit(lx_path_ref_t path);

/*! clear path
 *
 * @param path      the path
 */
lx_void_t           lx_path_clear(lx_path_ref_t path);

/*! copy path
 *
 * @param path      the path
 * @param copied    the copied path
 */
lx_void_t           lx_path_copy(lx_path_ref_t path, lx_path_ref_t copied);

/*! is empty path?
 *
 * @param path      the path
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_path_empty(lx_path_ref_t path);

/*! the path bounds
 *
 * @param path      the path
 *
 * @return          the bounds
 */
lx_rect_ref_t       lx_path_bounds(lx_path_ref_t path);

/*! the path convex?
 *
 * @param path      the path
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_path_convex(lx_path_ref_t path);

/*! set to be convex path, convex path may draw faster
 *
 * @param path      the path
 * @param convex    is convex?
 *
 */
lx_void_t           lx_path_convex_set(lx_path_ref_t path, lx_bool_t convex);

/*! get the last point
 *
 * @param path      the path
 * @param point     the last point
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_path_last(lx_path_ref_t path, lx_point_ref_t point);

/*! set the last point
 *
 * @param path      the path
 * @param point     the last point
 */
lx_void_t           lx_path_last_set(lx_path_ref_t path, lx_point_ref_t point);

/*! the path hint shape
 *
 * @param path      the path
 *
 * @return          the hint shape
 */
lx_shape_ref_t      lx_path_hint(lx_path_ref_t path);

/*! the path polygon
 *
 * @param path      the path
 *
 * @return          the polygon
 */
lx_polygon_ref_t    lx_path_polygon(lx_path_ref_t path);

/*! apply the matrix to the path
 *
 * @param path      the path
 * @param matrix    the matrix
 */
lx_void_t           lx_path_apply(lx_path_ref_t path, lx_matrix_ref_t matrix);

/*! close the current contour
 *
 * if the current point is not equal to the first point of the contour
 * patch a line segment automatically
 *
 * @param path      the path
 */
lx_void_t           lx_path_close(lx_path_ref_t path);

/*! move to the point
 *
 * @param path      the path
 * @param point     the point
 */
lx_void_t           lx_path_move_to(lx_path_ref_t path, lx_point_ref_t point);

/*! move to the point(x, y)
 *
 * @param path      the path
 * @param x         the x-coordinate
 * @param y         the y-coordinate
 */
lx_void_t           lx_path_move2_to(lx_path_ref_t path, lx_float_t x, lx_float_t y);

/*! move to the integer point(x, y)
 *
 * @param path      the path
 * @param x         the x-coordinate
 * @param y         the y-coordinate
 */
lx_void_t           lx_path_move2i_to(lx_path_ref_t path, lx_long_t x, lx_long_t y);

/*! line to the point
 *
 * @param path      the path
 * @param point     the point
 */
lx_void_t           lx_path_line_to(lx_path_ref_t path, lx_point_ref_t point);

/*! line to the point(x, y)
 *
 * @param path      the path
 * @param x         the x-coordinate
 * @param y         the y-coordinate
 */
lx_void_t           lx_path_line2_to(lx_path_ref_t path, lx_float_t x, lx_float_t y);

/*! line to the integer point(x, y)
 *
 * @param path      the path
 * @param x         the x-coordinate
 * @param y         the y-coordinate
 */
lx_void_t           lx_path_line2i_to(lx_path_ref_t path, lx_long_t x, lx_long_t y);

/*! quad to the ctrl, point
 *
 * @param path      the path
 * @param ctrl      the control point
 * @param point     the point
 */
lx_void_t           lx_path_quad_to(lx_path_ref_t path, lx_point_ref_t ctrl, lx_point_ref_t point);

/*! quad to the ctrl(cx, cy), point(x, y)
 *
 * @param path      the path
 * @param cx        the control x-coordinate
 * @param cy        the control y-coordinate
 * @param x         the x-coordinate
 * @param y         the y-coordinate
 */
lx_void_t           lx_path_quad2_to(lx_path_ref_t path, lx_float_t cx, lx_float_t cy, lx_float_t x, lx_float_t y);

/*! quad to the integer ctrl(cx, cy), point(x, y)
 *
 * @param path      the path
 * @param cx        the control x-coordinate
 * @param cy        the control y-coordinate
 * @param x         the x-coordinate
 * @param y         the y-coordinate
 */
lx_void_t           lx_path_quad2i_to(lx_path_ref_t path, lx_long_t cx, lx_long_t cy, lx_long_t x, lx_long_t y);

/*! cubic to the ctrl0, ctrl1, point
 *
 * @param path      the path
 * @param ctrl0     the control0 point
 * @param ctrl1     the control1 point
 * @param point     the point
 */
lx_void_t           lx_path_cubic_to(lx_path_ref_t path, lx_point_ref_t ctrl0, lx_point_ref_t ctrl1, lx_point_ref_t point);

/*! cubic to the ctrl0(cx0, cy0), ctrl1(cx1, cy1), point(x, y)
 *
 * @param path      the path
 * @param cx0       the control0 x-coordinate
 * @param cy0       the control0 y-coordinate
 * @param cx1       the control1 x-coordinate
 * @param cy1       the control1 y-coordinate
 * @param x         the x-coordinate
 * @param y         the y-coordinate
 */
lx_void_t           lx_path_cubic2_to(lx_path_ref_t path, lx_float_t cx0, lx_float_t cy0, lx_float_t cx1, lx_float_t cy1, lx_float_t x, lx_float_t y);

/*! cubic to the integer ctrl0(cx0, cy0), ctrl1(cx1, cy1), point(x, y)
 *
 * @param path      the path
 * @param cx0       the control0 x-coordinate
 * @param cy0       the control0 y-coordinate
 * @param cx1       the control1 x-coordinate
 * @param cy1       the control1 y-coordinate
 * @param x         the x-coordinate
 * @param y         the y-coordinate
 */
lx_void_t           lx_path_cubic2i_to(lx_path_ref_t path, lx_long_t cx0, lx_long_t cy0, lx_long_t cx1, lx_long_t cy1, lx_long_t x, lx_long_t y);

/*! arc to the arc
 *
 * @param path      the path
 * @param arc       the arc
 */
lx_void_t           lx_path_arc_to(lx_path_ref_t path, lx_arc_ref_t arc);

/*! arc to the arc(x0, y0, rx, ry, ab, an)
 *
 * @param path      the path
 * @param x0        the x0-coordinate
 * @param y0        the y0-coordinate
 * @param rx        the x-radius
 * @param ry        the y-radius
 * @param ab        the start angle
 * @param an        the sweep angle, 0 - 360
 */
lx_void_t           lx_path_arc2_to(lx_path_ref_t path, lx_float_t x0, lx_float_t y0, lx_float_t rx, lx_float_t ry, lx_float_t ab, lx_float_t an);

/*! arc to the integer arc(x0, y0, rx, ry, ab, an)
 *
 * @param path      the path
 * @param x0        the x0-coordinate
 * @param y0        the y0-coordinate
 * @param rx        the x-radius
 * @param ry        the y-radius
 * @param ab        the start angle
 * @param an        the sweep angle, 0 - 360
 */
lx_void_t           lx_path_arc2i_to(lx_path_ref_t path, lx_long_t x0, lx_long_t y0, lx_size_t rx, lx_size_t ry, lx_long_t ab, lx_long_t an);

/*! add path and ignore the first point
 *
 * @param path      the path
 * @param added     the added path
 */
lx_void_t           lx_path_path_to(lx_path_ref_t path, lx_path_ref_t added);

/*! add path in reverse order and ignore the last point
 *
 * @param path      the path
 * @param added     the added path
 */
lx_void_t           lx_path_rpath_to(lx_path_ref_t path, lx_path_ref_t added);

/*! add path
 *
 * @param path      the path
 * @param added     the added path
 */
lx_void_t           lx_path_add_path(lx_path_ref_t path, lx_path_ref_t added);

/*! add path in reverse order
 *
 * @param path      the path
 * @param added     the added path
 */
lx_void_t           lx_path_add_rpath(lx_path_ref_t path, lx_path_ref_t added);

/*! add line
 *
 * @param path      the path
 * @param line      the line
 */
lx_void_t           lx_path_add_line(lx_path_ref_t path, lx_line_ref_t line);

/*! add line(x0, y0, x1, y1)
 *
 * @param path      the path
 * @param x0        the x0-coordinate
 * @param y0        the y0-coordinate
 * @param x1        the x1-coordinate
 * @param y1        the y1-coordinate
 */
lx_void_t           lx_path_add_line2(lx_path_ref_t path, lx_float_t x0, lx_float_t y0, lx_float_t x1, lx_float_t y1);

/*! add integer line(x0, y0, x1, y1)
 *
 * @param path      the path
 * @param x0        the x0-coordinate
 * @param y0        the y0-coordinate
 * @param x1        the x1-coordinate
 * @param y1        the y1-coordinate
 */
lx_void_t           lx_path_add_line2i(lx_path_ref_t path, lx_long_t x0, lx_long_t y0, lx_long_t x1, lx_long_t y1);

/*! add arc
 *
 * @param path      the path
 * @param arc       the arc
 */
lx_void_t           lx_path_add_arc(lx_path_ref_t path, lx_arc_ref_t arc);

/*! add arc(x0, y0, rx, ry, ab, an)
 *
 * @param path      the path
 * @param x0        the x0-coordinate
 * @param y0        the y0-coordinate
 * @param rx        the x-radius
 * @param ry        the y-radius
 * @param ab        the start angle
 * @param an        the sweep angle, 0 - 360
 */
lx_void_t           lx_path_add_arc2(lx_path_ref_t path, lx_float_t x0, lx_float_t y0, lx_float_t rx, lx_float_t ry, lx_float_t ab, lx_float_t an);

/*! add integer arc(x0, y0, rx, ry, ab, an)
 *
 * @param path      the path
 * @param x0        the x0-coordinate
 * @param y0        the y0-coordinate
 * @param rx        the x-radius
 * @param ry        the y-radius
 * @param ab        the start angle
 * @param an        the sweep angle, 0 - 360
 */
lx_void_t           lx_path_add_arc2i(lx_path_ref_t path, lx_long_t x0, lx_long_t y0, lx_size_t rx, lx_size_t ry, lx_long_t ab, lx_long_t an);

/*! add triangle
 *
 * @param path      the path
 * @param triangle  the triangle
 */
lx_void_t           lx_path_add_triangle(lx_path_ref_t path, lx_triangle_ref_t triangle);

/*! add triangle(x0, y0, x1, y1, x2, y2)
 *
 * @param path      the path
 * @param x0        the x0-coordinate
 * @param y0        the y0-coordinate
 * @param x1        the x1-coordinate
 * @param y1        the y1-coordinate
 * @param x2        the x2-coordinate
 * @param y2        the y2-coordinate
 */
lx_void_t           lx_path_add_triangle2(lx_path_ref_t path, lx_float_t x0, lx_float_t y0, lx_float_t x1, lx_float_t y1, lx_float_t x2, lx_float_t y2);

/*! add integer triangle(x0, y0, x1, y1, x2, y2)
 *
 * @param path      the path
 * @param x0        the x0-coordinate
 * @param y0        the y0-coordinate
 * @param x1        the x1-coordinate
 * @param y1        the y1-coordinate
 * @param x2        the x2-coordinate
 * @param y2        the y2-coordinate
 */
lx_void_t           lx_path_add_triangle2i(lx_path_ref_t path, lx_long_t x0, lx_long_t y0, lx_long_t x1, lx_long_t y1, lx_long_t x2, lx_long_t y2);

/*! add rect
 *
 * @param path      the path
 * @param rect      the rect
 * @param direction the direction
 */
lx_void_t           lx_path_add_rect(lx_path_ref_t path, lx_rect_ref_t rect, lx_size_t direction);

/*! add rect(x, y, w, h)
 *
 * @param path      the path
 * @param x         the x-coordinate
 * @param y         the y-coordinate
 * @param w         the width
 * @param h         the height
 * @param direction the direction
 */
lx_void_t           lx_path_add_rect2(lx_path_ref_t path, lx_float_t x, lx_float_t y, lx_float_t w, lx_float_t h, lx_size_t direction);

/*! add integer rect(x, y, w, h)
 *
 * @param path      the path
 * @param x         the x-coordinate
 * @param y         the y-coordinate
 * @param w         the width
 * @param h         the height
 * @param direction the direction
 */
lx_void_t           lx_path_add_rect2i(lx_path_ref_t path, lx_long_t x, lx_long_t y, lx_size_t w, lx_size_t h, lx_size_t direction);

/*! add round rect
 *
 * @param path      the path
 * @param rect      the rect
 * @param direction the direction
 */
lx_void_t           lx_path_add_round_rect(lx_path_ref_t path, lx_round_rect_ref_t rect, lx_size_t direction);

/*! add round rect(x, y, w, h)
 *
 * @param path      the path
 * @param bounds    the bounds
 * @param rx        the x-radius
 * @param ry        the y-radius
 * @param direction the direction
 */
lx_void_t           lx_path_add_round_rect2(lx_path_ref_t path, lx_rect_ref_t bounds, lx_float_t rx, lx_float_t ry, lx_size_t direction);

/*! add integer round rect(x, y, w, h)
 *
 * @param path      the path
 * @param bounds    the bounds
 * @param rx        the x-radius
 * @param ry        the y-radius
 * @param direction the direction
 */
lx_void_t           lx_path_add_round_rect2i(lx_path_ref_t path, lx_rect_ref_t bounds, lx_size_t rx, lx_size_t ry, lx_size_t direction);

/*! add circle
 *
 * @param path      the path
 * @param circle    the circle
 * @param direction the direction
 */
lx_void_t           lx_path_add_circle(lx_path_ref_t path, lx_circle_ref_t circle, lx_size_t direction);

/*! add circle(x0, y0, r)
 *
 * @param path      the path
 * @param x0        the x0-coordinate
 * @param y0        the y0-coordinate
 * @param r         the radius
 * @param direction the direction
 */
lx_void_t           lx_path_add_circle2(lx_path_ref_t path, lx_float_t x0, lx_float_t y0, lx_float_t r, lx_size_t direction);

/*! add integer circle(x0, y0, r)
 *
 * @param path      the path
 * @param x0        the x0-coordinate
 * @param y0        the y0-coordinate
 * @param r         the radius
 * @param direction the direction
 */
lx_void_t           lx_path_add_circle2i(lx_path_ref_t path, lx_long_t x0, lx_long_t y0, lx_size_t r, lx_size_t direction);

/*! add ellipse
 *
 * @param path      the path
 * @param ellipse   the ellipse
 * @param direction the direction
 */
lx_void_t           lx_path_add_ellipse(lx_path_ref_t path, lx_ellipse_ref_t ellipse, lx_size_t direction);

/*! add ellipse(x0, y0, rx, ry)
 *
 * @param path      the path
 * @param x0        the x0-coordinate
 * @param y0        the y0-coordinate
 * @param rx        the x-radius
 * @param ry        the y-radius
 * @param direction the direction
 */
lx_void_t           lx_path_add_ellipse2(lx_path_ref_t path, lx_float_t x0, lx_float_t y0, lx_float_t rx, lx_float_t ry, lx_size_t direction);

/*! add integer ellipse(x0, y0, rx, ry)
 *
 * @param path      the path
 * @param x0        the x0-coordinate
 * @param y0        the y0-coordinate
 * @param rx        the x-radius
 * @param ry        the y-radius
 * @param direction the direction
 */
lx_void_t           lx_path_add_ellipse2i(lx_path_ref_t path, lx_long_t x0, lx_long_t y0, lx_size_t rx, lx_size_t ry, lx_size_t direction);

#ifdef LX_DEBUG
/*! dump path
 *
 * @param path      the path
 */
lx_void_t           lx_path_dump(lx_path_ref_t path);
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


