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
 * @file        canvas_path.h
 *
 */
#ifndef LX_CORE_CANVAS_PATH_H
#define LX_CORE_CANVAS_PATH_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "path.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! get the path
 *
 * @param canvas    the canvas
 *
 * @return          the path
 */
lx_path_ref_t       lx_canvas_path(lx_canvas_ref_t canvas);

/*! save path
 *
 * @param canvas    the canvas
 *
 * @return          the current path
 */
lx_path_ref_t       lx_canvas_path_save(lx_canvas_ref_t canvas);

/*! load path
 *
 * @param canvas    the canvas
 */
lx_void_t           lx_canvas_path_load(lx_canvas_ref_t canvas);

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline interfaces
 */

static lx_inline lx_void_t lx_canvas_path_clear(lx_canvas_ref_t canvas) {
    lx_path_clear(lx_canvas_path(canvas));
}

static lx_inline lx_void_t lx_canvas_convex_set(lx_canvas_ref_t canvas, lx_bool_t convex) {
    lx_path_convex_set(lx_canvas_path(canvas), convex);
}

static lx_inline lx_void_t lx_canvas_close(lx_canvas_ref_t canvas) {
    lx_path_close(lx_canvas_path(canvas));
}

static lx_inline lx_void_t lx_canvas_move_to(lx_canvas_ref_t canvas, lx_point_ref_t point) {
    lx_path_move_to(lx_canvas_path(canvas), point);
}

static lx_inline lx_void_t lx_canvas_move2_to(lx_canvas_ref_t canvas, lx_float_t x, lx_float_t y) {
    lx_point_t point;
    lx_point_make(&point, x, y);
    lx_canvas_move_to(canvas, &point);
}

static lx_inline lx_void_t lx_canvas_move2i_to(lx_canvas_ref_t canvas, lx_long_t x, lx_long_t y) {
    lx_point_t point;
    lx_point_imake(&point, x, y);
    lx_canvas_move_to(canvas, &point);
}

static lx_inline lx_void_t lx_canvas_line_to(lx_canvas_ref_t canvas, lx_point_ref_t point) {
    lx_path_line_to(lx_canvas_path(canvas), point);
}

static lx_inline lx_void_t lx_canvas_line2_to(lx_canvas_ref_t canvas, lx_float_t x, lx_float_t y) {
    lx_point_t point;
    lx_point_make(&point, x, y);
    lx_canvas_line_to(canvas, &point);
}

static lx_inline lx_void_t lx_canvas_line2i_to(lx_canvas_ref_t canvas, lx_long_t x, lx_long_t y) {
    lx_point_t point;
    lx_point_imake(&point, x, y);
    lx_canvas_line_to(canvas, &point);
}

static lx_inline lx_void_t lx_canvas_quad_to(lx_canvas_ref_t canvas, lx_point_ref_t ctrl, lx_point_ref_t point) {
    lx_path_quad_to(lx_canvas_path(canvas), ctrl, point);
}

static lx_inline lx_void_t lx_canvas_quad2_to(lx_canvas_ref_t canvas, lx_float_t cx, lx_float_t cy, lx_float_t x, lx_float_t y) {
    lx_point_t ctrl, point;
    lx_point_make(&ctrl, cx, cy);
    lx_point_make(&point, x, y);
    lx_canvas_quad_to(canvas, &ctrl, &point);
}

static lx_inline lx_void_t lx_canvas_quad2i_to(lx_canvas_ref_t canvas, lx_long_t cx, lx_long_t cy, lx_long_t x, lx_long_t y) {
    lx_point_t ctrl, point;
    lx_point_imake(&ctrl, cx, cy);
    lx_point_imake(&point, x, y);
    lx_canvas_quad_to(canvas, &ctrl, &point);
}

static lx_inline lx_void_t lx_canvas_cubic_to(lx_canvas_ref_t canvas, lx_point_ref_t ctrl0, lx_point_ref_t ctrl1, lx_point_ref_t point) {
    lx_path_cubic_to(lx_canvas_path(canvas), ctrl0, ctrl1, point);
}

static lx_inline lx_void_t lx_canvas_cubic2_to(lx_canvas_ref_t canvas, lx_float_t cx0, lx_float_t cy0, lx_float_t cx1, lx_float_t cy1, lx_float_t x, lx_float_t y) {
    lx_point_t ctrl0, ctrl1, point;
    lx_point_make(&ctrl0, cx0, cy0);
    lx_point_make(&ctrl1, cx1, cy1);
    lx_point_make(&point, x, y);
    lx_canvas_cubic_to(canvas, &ctrl0, &ctrl1, &point);
}

static lx_inline lx_void_t lx_canvas_cubic2i_to(lx_canvas_ref_t canvas, lx_long_t cx0, lx_long_t cy0, lx_long_t cx1, lx_long_t cy1, lx_long_t x, lx_long_t y) {
    lx_point_t ctrl0, ctrl1, point;
    lx_point_imake(&ctrl0, cx0, cy0);
    lx_point_imake(&ctrl1, cx1, cy1);
    lx_point_imake(&point, x, y);
    lx_canvas_cubic_to(canvas, &ctrl0, &ctrl1, &point);
}

static lx_inline lx_void_t lx_canvas_arc_to(lx_canvas_ref_t canvas, lx_arc_ref_t arc) {
    lx_path_arc_to(lx_canvas_path(canvas), arc);
}

static lx_inline lx_void_t lx_canvas_arc2_to(lx_canvas_ref_t canvas, lx_float_t x0, lx_float_t y0, lx_float_t rx, lx_float_t ry, lx_float_t ab, lx_float_t an) {
    lx_arc_t arc;
    lx_arc_make(&arc, x0, y0, rx, ry, ab, an);
    lx_canvas_arc_to(canvas, &arc);
}

static lx_inline lx_void_t lx_canvas_arc2i_to(lx_canvas_ref_t canvas, lx_long_t x0, lx_long_t y0, lx_size_t rx, lx_size_t ry, lx_long_t ab, lx_long_t an) {
    lx_arc_t arc;
    lx_arc_imake(&arc, x0, y0, rx, ry, ab, an);
    lx_canvas_arc_to(canvas, &arc);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


