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
 * @file        canvas_draw.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "canvas_draw.h"
#include "canvas_path.h"
#include "device.h"
#include "private/canvas.h"
#include "../base/base.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_void_t lx_canvas_draw_clear(lx_canvas_ref_t self, lx_color_t color) {
    lx_canvas_t* canvas = (lx_canvas_t*)self;
    if (canvas && canvas->device) {
        lx_device_draw_clear(canvas->device, color);
    }
}

lx_void_t lx_canvas_draw(lx_canvas_ref_t self) {
    lx_canvas_draw_path(self, lx_canvas_path(self));
}

lx_void_t lx_canvas_draw_path(lx_canvas_ref_t self, lx_path_ref_t path) {
    lx_canvas_t* canvas = (lx_canvas_t*)self;
    lx_assert_and_check_return(canvas && canvas->device && path);
    lx_device_draw_path(canvas->device, path);
}

lx_void_t lx_canvas_draw_point(lx_canvas_ref_t self, lx_point_ref_t point) {
    lx_canvas_t* canvas = (lx_canvas_t*)self;
    lx_assert_and_check_return(canvas && canvas->device);
    lx_device_draw_points(canvas->device, point, 1, lx_null);
}

lx_void_t lx_canvas_draw_point2(lx_canvas_ref_t self, lx_float_t x, lx_float_t y) {
    lx_point_t point;
    lx_point_make(&point, x, y);
    lx_canvas_draw_point(self, &point);
}

lx_void_t lx_canvas_draw_point2i(lx_canvas_ref_t self, lx_long_t x, lx_long_t y) {
    lx_point_t point;
    lx_point_imake(&point, x, y);
    lx_canvas_draw_point(self, &point);
}

lx_void_t lx_canvas_draw_line(lx_canvas_ref_t self, lx_line_ref_t line) {
    lx_canvas_t* canvas = (lx_canvas_t*)self;
    lx_assert_and_check_return(canvas && canvas->device && line);

    lx_rect_t bounds;
    lx_point_t points[] = {line->p0, line->p1};
    lx_bounds_make(&bounds, points, lx_arrayn(points));
    lx_device_draw_lines(canvas->device, points, 2, &bounds);
}

lx_void_t lx_canvas_draw_line2(lx_canvas_ref_t self, lx_float_t x0, lx_float_t y0, lx_float_t x1, lx_float_t y1) {
    lx_line_t line;
    lx_line_make(&line, x0, y0, x1, y1);
    lx_canvas_draw_line(self, &line);
}

lx_void_t lx_canvas_draw_line2i(lx_canvas_ref_t self, lx_long_t x0, lx_long_t y0, lx_long_t x1, lx_long_t y1) {
    lx_line_t line;
    lx_line_imake(&line, x0, y0, x1, y1);
    lx_canvas_draw_line(self, &line);
}

lx_void_t lx_canvas_draw_arc(lx_canvas_ref_t self, lx_arc_ref_t arc) {
    lx_canvas_t* canvas = (lx_canvas_t*)self;
    lx_assert_and_check_return(canvas && arc);

    lx_path_ref_t path = lx_canvas_save_path(self);
    if (path) {
        lx_path_clear(path);
        lx_path_add_arc(path, arc);
        lx_canvas_draw_path(self, path);
        lx_canvas_load_path(self);
    }
}

lx_void_t lx_canvas_draw_arc2(lx_canvas_ref_t self, lx_float_t x0, lx_float_t y0, lx_float_t rx, lx_float_t ry, lx_float_t ab, lx_float_t an) {
    lx_arc_t arc;
    lx_arc_make(&arc, x0, y0, rx, ry, ab, an);
    lx_canvas_draw_arc(self, &arc);
}

lx_void_t lx_canvas_draw_arc2i(lx_canvas_ref_t self, lx_long_t x0, lx_long_t y0, lx_size_t rx, lx_size_t ry, lx_long_t ab, lx_long_t an) {
    lx_arc_t arc;
    lx_arc_imake(&arc, x0, y0, rx, ry, ab, an);
    lx_canvas_draw_arc(self, &arc);
}

lx_void_t lx_canvas_draw_triangle(lx_canvas_ref_t self, lx_triangle_ref_t triangle) {
    lx_canvas_t* canvas = (lx_canvas_t*)self;
    lx_assert_and_check_return(canvas && canvas->device && triangle);

    // init polygon
    lx_point_t      points[] = {triangle->p0, triangle->p1, triangle->p2, triangle->p0};
    lx_uint16_t     counts[] = {4, 0};
    lx_polygon_t    polygon = {points, counts, lx_true};

    // init hint
    lx_shape_t      hint;
    hint.type       = LX_SHAPE_TYPE_TRIANGLE;
    hint.u.triangle = *triangle;

    // init bounds
    lx_rect_t       bounds;
    lx_bounds_make(&bounds, points, lx_arrayn(points));

    // draw it
    lx_device_draw_polygon(canvas->device, &polygon, &hint, &bounds);
}

lx_void_t lx_canvas_draw_triangle2(lx_canvas_ref_t self, lx_float_t x0, lx_float_t y0, lx_float_t x1, lx_float_t y1, lx_float_t x2, lx_float_t y2) {
    lx_triangle_t triangle;
    lx_triangle_make(&triangle, x0, y0, x1, y1, x2, y2);
    lx_canvas_draw_triangle(self, &triangle);
}

lx_void_t lx_canvas_draw_triangle2i(lx_canvas_ref_t self, lx_long_t x0, lx_long_t y0, lx_long_t x1, lx_long_t y1, lx_long_t x2, lx_long_t y2) {
    lx_triangle_t triangle;
    lx_triangle_imake(&triangle, x0, y0, x1, y1, x2, y2);
    lx_canvas_draw_triangle(self, &triangle);
}

lx_void_t lx_canvas_draw_rect(lx_canvas_ref_t self, lx_rect_ref_t rect) {
    lx_canvas_t* canvas = (lx_canvas_t*)self;
    lx_assert_and_check_return(canvas && canvas->device && rect);

    // init polygon
    lx_point_t      points[5];
    lx_uint16_t     counts[] = {5, 0};
    lx_polygon_t    polygon = {points, counts, lx_true};

    // init points
    points[0].x = rect->x;
    points[0].y = rect->y;
    points[1].x = rect->x + rect->w;
    points[1].y = rect->y;
    points[2].x = rect->x + rect->w;
    points[2].y = rect->y + rect->h;
    points[3].x = rect->x;
    points[3].y = rect->y + rect->h;
    points[4] = points[0];

    // init hint
    lx_shape_t      hint;
    hint.type       = LX_SHAPE_TYPE_RECT;
    hint.u.rect     = *rect;

    // draw it
    lx_device_draw_polygon(canvas->device, &polygon, &hint, rect);
}

lx_void_t lx_canvas_draw_rect2(lx_canvas_ref_t self, lx_float_t x, lx_float_t y, lx_float_t w, lx_float_t h) {
    lx_rect_t rect;
    lx_rect_make(&rect, x, y, w, h);
    lx_canvas_draw_rect(self, &rect);
}

lx_void_t lx_canvas_draw_rect2i(lx_canvas_ref_t self, lx_long_t x, lx_long_t y, lx_size_t w, lx_size_t h) {
    lx_rect_t rect;
    lx_rect_imake(&rect, x, y, w, h);
    lx_canvas_draw_rect(self, &rect);
}

lx_void_t lx_canvas_draw_round_rect(lx_canvas_ref_t self, lx_round_rect_ref_t rect) {
    // draw rect or ellipse?
    if (lx_round_rect_is_rect(rect)) {
        lx_canvas_draw_rect(self, &rect->bounds);
        return ;
    } else if (lx_round_rect_is_ellipse(rect)) {
        lx_ellipse_t ellipse;
        lx_ellipse_make_from_rect(&ellipse, &rect->bounds);
        lx_canvas_draw_ellipse(self, &ellipse);
        return ;
    }

    lx_path_ref_t path = lx_canvas_save_path(self);
    if (path) {
        lx_path_clear(path);
        lx_path_add_round_rect(path, rect, LX_ROTATE_DIRECTION_CW);
        lx_canvas_draw_path(self, path);
        lx_canvas_load_path(self);
    }
}

lx_void_t lx_canvas_draw_round_rect2(lx_canvas_ref_t self, lx_rect_ref_t bounds, lx_float_t rx, lx_float_t ry) {
    lx_round_rect_t rect;
    lx_round_rect_make_same(&rect, bounds, rx, ry);
    lx_canvas_draw_round_rect(self, &rect);
}

lx_void_t lx_canvas_draw_round_rect2i(lx_canvas_ref_t self, lx_rect_ref_t bounds, lx_size_t rx, lx_size_t ry) {
    lx_round_rect_t rect;
    lx_round_rect_imake_same(&rect, bounds, rx, ry);
    lx_canvas_draw_round_rect(self, &rect);
}

lx_void_t lx_canvas_draw_circle(lx_canvas_ref_t self, lx_circle_ref_t circle) {
    lx_path_ref_t path = lx_canvas_save_path(self);
    if (path) {
        lx_path_clear(path);
        lx_path_add_circle(path, circle, LX_ROTATE_DIRECTION_CW);
        lx_canvas_draw_path(self, path);
        lx_canvas_load_path(self);
    }
}

lx_void_t lx_canvas_draw_circle2(lx_canvas_ref_t self, lx_float_t x0, lx_float_t y0, lx_float_t r) {
    lx_circle_t circle;
    lx_circle_make(&circle, x0, y0, r);
    lx_canvas_draw_circle(self, &circle);
}

lx_void_t lx_canvas_draw_circle2i(lx_canvas_ref_t self, lx_long_t x0, lx_long_t y0, lx_size_t r) {
    lx_circle_t circle;
    lx_circle_imake(&circle, x0, y0, r);
    lx_canvas_draw_circle(self, &circle);
}

lx_void_t lx_canvas_draw_ellipse(lx_canvas_ref_t self, lx_ellipse_ref_t ellipse) {
    lx_path_ref_t path = lx_canvas_save_path(self);
    if (path) {
        lx_path_clear(path);
        lx_path_add_ellipse(path, ellipse, LX_ROTATE_DIRECTION_CW);
        lx_canvas_draw_path(self, path);
        lx_canvas_load_path(self);
    }
}

lx_void_t lx_canvas_draw_ellipse2(lx_canvas_ref_t self, lx_float_t x0, lx_float_t y0, lx_float_t rx, lx_float_t ry) {
    lx_ellipse_t ellipse;
    lx_ellipse_make(&ellipse, x0, y0, rx, ry);
    lx_canvas_draw_ellipse(self, &ellipse);
}

lx_void_t lx_canvas_draw_ellipse2i(lx_canvas_ref_t self, lx_long_t x0, lx_long_t y0, lx_size_t rx, lx_size_t ry) {
    lx_ellipse_t ellipse;
    lx_ellipse_imake(&ellipse, x0, y0, rx, ry);
    lx_canvas_draw_ellipse(self, &ellipse);
}

lx_void_t lx_canvas_draw_polygon(lx_canvas_ref_t self, lx_polygon_ref_t polygon) {
    lx_canvas_t* canvas = (lx_canvas_t*)self;
    lx_assert_and_check_return(canvas && canvas->device);
    lx_device_draw_polygon(canvas->device, polygon, lx_null, lx_null);
}

lx_void_t lx_canvas_draw_lines(lx_canvas_ref_t self, lx_point_ref_t points, lx_size_t count) {
    lx_canvas_t* canvas = (lx_canvas_t*)self;
    lx_assert_and_check_return(canvas && canvas->device && count && !(count & 0x1));
    lx_device_draw_lines(canvas->device, points, count, lx_null);
}

lx_void_t lx_canvas_draw_points(lx_canvas_ref_t self, lx_point_ref_t points, lx_size_t count) {
    lx_canvas_t* canvas = (lx_canvas_t*)self;
    lx_assert_and_check_return(canvas && canvas->device && count);
    lx_device_draw_points(canvas->device, points, count, lx_null);
}
