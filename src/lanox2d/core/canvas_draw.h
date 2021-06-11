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
 * @file        canvas_draw.h
 *
 */
#ifndef LX_CORE_CANVAS_DRAW_H
#define LX_CORE_CANVAS_DRAW_H

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

/*! clear draw and fill the given color
 *
 * @param canvas    the canvas
 * @param color     the color
 */
lx_void_t           lx_canvas_draw_clear(lx_canvas_ref_t canvas, lx_color_t color);

/*! draw the current path
 *
 * @param canvas    the canvas
 */
lx_void_t           lx_canvas_draw(lx_canvas_ref_t canvas);

/*! draw the given path
 *
 * @param canvas    the canvas
 * @param path      the path
 */
lx_void_t           lx_canvas_draw_path(lx_canvas_ref_t canvas, lx_path_ref_t path);

/*! draw point
 *
 * @param canvas    the canvas
 * @param point     the point
 */
lx_void_t           lx_canvas_draw_point(lx_canvas_ref_t canvas, lx_point_ref_t point);

/*! draw point(x, y)
 *
 * @param canvas    the canvas
 * @param x         the x-coordinate
 * @param y         the y-coordinate
 */
lx_void_t           lx_canvas_draw_point2(lx_canvas_ref_t canvas, lx_float_t x, lx_float_t y);

/*! draw integer point(x, y)
 *
 * @param canvas    the canvas
 * @param x         the x-coordinate
 * @param y         the y-coordinate
 */
lx_void_t           lx_canvas_draw_point2i(lx_canvas_ref_t canvas, lx_long_t x, lx_long_t y);

/*! draw line
 *
 * @param canvas    the canvas
 * @param line      the line
 */
lx_void_t           lx_canvas_draw_line(lx_canvas_ref_t canvas, lx_line_ref_t line);

/*! draw line(x0, y0, x1, y1)
 *
 * @param canvas    the canvas
 * @param x0        the x0-coordinate
 * @param y0        the y0-coordinate
 * @param x1        the x1-coordinate
 * @param y1        the y1-coordinate
 */
lx_void_t           lx_canvas_draw_line2(lx_canvas_ref_t canvas, lx_float_t x0, lx_float_t y0, lx_float_t x1, lx_float_t y1);

/*! draw integer line(x0, y0, x1, y1)
 *
 * @param canvas    the canvas
 * @param x0        the x0-coordinate
 * @param y0        the y0-coordinate
 * @param x1        the x1-coordinate
 * @param y1        the y1-coordinate
 */
lx_void_t           lx_canvas_draw_line2i(lx_canvas_ref_t canvas, lx_long_t x0, lx_long_t y0, lx_long_t x1, lx_long_t y1);

/*! draw arc
 *
 * @param canvas    the canvas
 * @param arc       the arc
 */
lx_void_t           lx_canvas_draw_arc(lx_canvas_ref_t canvas, lx_arc_ref_t arc);

/*! draw arc(x0, y0, rx, ry, ab, an)
 *
 * @param canvas    the canvas
 * @param x0        the x0-coordinate
 * @param y0        the y0-coordinate
 * @param rx        the x-radius
 * @param ry        the y-radius
 * @param ab        the start angle
 * @param an        the sweep angle, 0 - 360
 */
lx_void_t           lx_canvas_draw_arc2(lx_canvas_ref_t canvas, lx_float_t x0, lx_float_t y0, lx_float_t rx, lx_float_t ry, lx_float_t ab, lx_float_t an);

/*! draw integer arc(x0, y0, rx, ry, ab, an)
 *
 * @param canvas    the canvas
 * @param x0        the x0-coordinate
 * @param y0        the y0-coordinate
 * @param rx        the x-radius
 * @param ry        the y-radius
 * @param ab        the start angle
 * @param an        the sweep angle, 0 - 360
 */
lx_void_t           lx_canvas_draw_arc2i(lx_canvas_ref_t canvas, lx_long_t x0, lx_long_t y0, lx_size_t rx, lx_size_t ry, lx_long_t ab, lx_long_t an);

/*! draw triangle
 *
 * @param canvas    the canvas
 * @param triangle  the triangle
 */
lx_void_t           lx_canvas_draw_triangle(lx_canvas_ref_t canvas, lx_triangle_ref_t triangle);

/*! draw triangle(x0, y0, x1, y1, x2, y2)
 *
 * @param canvas    the canvas
 * @param x0        the x0-coordinate
 * @param y0        the y0-coordinate
 * @param x1        the x1-coordinate
 * @param y1        the y1-coordinate
 * @param x2        the x2-coordinate
 * @param y2        the y2-coordinate
 */
lx_void_t           lx_canvas_draw_triangle2(lx_canvas_ref_t canvas, lx_float_t x0, lx_float_t y0, lx_float_t x1, lx_float_t y1, lx_float_t x2, lx_float_t y2);

/*! draw integer triangle(x0, y0, x1, y1, x2, y2)
 *
 * @param canvas    the canvas
 * @param x0        the x0-coordinate
 * @param y0        the y0-coordinate
 * @param x1        the x1-coordinate
 * @param y1        the y1-coordinate
 * @param x2        the x2-coordinate
 * @param y2        the y2-coordinate
 */
lx_void_t           lx_canvas_draw_triangle2i(lx_canvas_ref_t canvas, lx_long_t x0, lx_long_t y0, lx_long_t x1, lx_long_t y1, lx_long_t x2, lx_long_t y2);

/*! draw rect
 *
 * @param canvas    the canvas
 * @param rect      the rect
 */
lx_void_t           lx_canvas_draw_rect(lx_canvas_ref_t canvas, lx_rect_ref_t rect);

/*! draw rect(x, y, w, h)
 *
 * @param canvas    the canvas
 * @param x         the x-coordinate
 * @param y         the y-coordinate
 * @param w         the width
 * @param h         the height
 */
lx_void_t           lx_canvas_draw_rect2(lx_canvas_ref_t canvas, lx_float_t x, lx_float_t y, lx_float_t w, lx_float_t h);

/*! draw integer rect(x, y, w, h)
 *
 * @param canvas    the canvas
 * @param x         the x-coordinate
 * @param y         the y-coordinate
 * @param w         the width
 * @param h         the height
 */
lx_void_t           lx_canvas_draw_rect2i(lx_canvas_ref_t canvas, lx_long_t x, lx_long_t y, lx_size_t w, lx_size_t h);

/*! draw round rect
 *
 * @param canvas    the canvas
 * @param rect      the round rect
 */
lx_void_t           lx_canvas_draw_round_rect(lx_canvas_ref_t canvas, lx_round_rect_ref_t rect);

/*! draw round rect(x, y, w, h)
 *
 * @param canvas    the canvas
 * @param bounds    the bounds
 * @param rx        the x-radius
 * @param ry        the y-radius
 */
lx_void_t           lx_canvas_draw_round_rect2(lx_canvas_ref_t canvas, lx_rect_ref_t bounds, lx_float_t rx, lx_float_t ry);

/*! draw integer round rect(x, y, w, h)
 *
 * @param canvas    the canvas
 * @param bounds    the bounds
 * @param rx        the x-radius
 * @param ry        the y-radius
 */
lx_void_t           lx_canvas_draw_round_rect2i(lx_canvas_ref_t canvas, lx_rect_ref_t bounds, lx_size_t rx, lx_size_t ry);

/*! draw circle
 *
 * @param canvas    the canvas
 * @param circle    the circle
 */
lx_void_t           lx_canvas_draw_circle(lx_canvas_ref_t canvas, lx_circle_ref_t circle);

/*! draw circle(x0, y0, r)
 *
 * @param canvas    the canvas
 * @param x0        the x0-coordinate
 * @param y0        the y0-coordinate
 * @param r         the radius
 */
lx_void_t           lx_canvas_draw_circle2(lx_canvas_ref_t canvas, lx_float_t x0, lx_float_t y0, lx_float_t r);

/*! draw integer circle(x0, y0, r)
 *
 * @param canvas    the canvas
 * @param x0        the x0-coordinate
 * @param y0        the y0-coordinate
 * @param r         the radius
 */
lx_void_t           lx_canvas_draw_circle2i(lx_canvas_ref_t canvas, lx_long_t x0, lx_long_t y0, lx_size_t r);

/*! draw ellipse
 *
 * @param canvas    the canvas
 * @param ellipse   the ellipse
 */
lx_void_t           lx_canvas_draw_ellipse(lx_canvas_ref_t canvas, lx_ellipse_ref_t ellipse);

/*! draw ellipse(x0, y0, rx, ry)
 *
 * @param canvas    the canvas
 * @param x0        the x0-coordinate
 * @param y0        the y0-coordinate
 * @param rx        the x-radius
 * @param ry        the y-radius
 */
lx_void_t           lx_canvas_draw_ellipse2(lx_canvas_ref_t canvas, lx_float_t x0, lx_float_t y0, lx_float_t rx, lx_float_t ry);

/*! draw integer ellipse(x0, y0, rx, ry)
 *
 * @param canvas    the canvas
 * @param x0        the x0-coordinate
 * @param y0        the y0-coordinate
 * @param rx        the x-radius
 * @param ry        the y-radius
 */
lx_void_t           lx_canvas_draw_ellipse2i(lx_canvas_ref_t canvas, lx_long_t x0, lx_long_t y0, lx_size_t rx, lx_size_t ry);

/*! draw polygon
 *
 * @param canvas    the canvas
 * @param polygon   the polygon
 */
lx_void_t           lx_canvas_draw_polygon(lx_canvas_ref_t canvas, lx_polygon_ref_t polygon);

/*! draw lines
 *
 * @param canvas    the canvas
 * @param points    the points
 * @param count     the points count
 */
lx_void_t           lx_canvas_draw_lines(lx_canvas_ref_t canvas, lx_point_ref_t points, lx_size_t count);

/*! draw points
 *
 * @param canvas    the canvas
 * @param points    the points
 * @param count     the points count
 */
lx_void_t           lx_canvas_draw_points(lx_canvas_ref_t canvas, lx_point_ref_t points, lx_size_t count);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


