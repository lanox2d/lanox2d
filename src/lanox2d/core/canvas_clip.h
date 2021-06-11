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
 * @file        canvas_clip.h
 *
 */
#ifndef LX_CORE_CANVAS_CLIP_H
#define LX_CORE_CANVAS_CLIP_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "clipper.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! get the clipper
 *
 * @param canvas    the canvas
 *
 * @return          the clipper
 */
lx_clipper_ref_t    lx_canvas_clipper(lx_canvas_ref_t canvas);

/*! save clipper
 *
 * @param canvas    the canvas
 *
 * @return          the current clipper
 */
lx_clipper_ref_t    lx_canvas_save_clipper(lx_canvas_ref_t canvas);

/*! load clipper
 *
 * @param canvas    the canvas
 */
lx_void_t           lx_canvas_load_clipper(lx_canvas_ref_t canvas);

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline interfaces
 */
static lx_inline lx_void_t lx_canvas_clear_clipper(lx_canvas_ref_t canvas) {
    lx_clipper_clear(lx_canvas_clipper(canvas));
}

static lx_inline lx_void_t lx_canvas_clip_path(lx_canvas_ref_t canvas, lx_size_t mode, lx_path_ref_t path) {
    lx_clipper_add_path(lx_canvas_clipper(canvas), mode, path);
}

static lx_inline lx_void_t lx_canvas_clip_triangle(lx_canvas_ref_t canvas, lx_size_t mode, lx_triangle_ref_t triangle) {
    lx_clipper_add_triangle(lx_canvas_clipper(canvas), mode, triangle);
}

static lx_inline lx_void_t lx_canvas_clip_triangle2(lx_canvas_ref_t canvas, lx_size_t mode, lx_float_t x0, lx_float_t y0, lx_float_t x1, lx_float_t y1, lx_float_t x2, lx_float_t y2) {
    lx_triangle_t triangle;
    lx_triangle_make(&triangle, x0, y0, x1, y1, x2, y2);
    lx_canvas_clip_triangle(canvas, mode, &triangle);
}

static lx_inline lx_void_t lx_canvas_clip_triangle2i(lx_canvas_ref_t canvas, lx_size_t mode, lx_long_t x0, lx_long_t y0, lx_long_t x1, lx_long_t y1, lx_long_t x2, lx_long_t y2) {
    lx_triangle_t triangle;
    lx_triangle_imake(&triangle, x0, y0, x1, y1, x2, y2);
    lx_canvas_clip_triangle(canvas, mode, &triangle);
}

static lx_inline lx_void_t lx_canvas_clip_rect(lx_canvas_ref_t canvas, lx_size_t mode, lx_rect_ref_t rect) {
    lx_clipper_add_rect(lx_canvas_clipper(canvas), mode, rect);
}

static lx_inline lx_void_t lx_canvas_clip_rect2(lx_canvas_ref_t canvas, lx_size_t mode, lx_float_t x, lx_float_t y, lx_float_t w, lx_float_t h) {
    lx_rect_t rect;
    lx_rect_make(&rect, x, y, w, h);
    lx_canvas_clip_rect(canvas, mode, &rect);
}

static lx_inline lx_void_t lx_canvas_clip_rect2i(lx_canvas_ref_t canvas, lx_size_t mode, lx_long_t x, lx_long_t y, lx_size_t w, lx_size_t h) {
    lx_rect_t rect;
    lx_rect_imake(&rect, x, y, w, h);
    lx_canvas_clip_rect(canvas, mode, &rect);
}

static lx_inline lx_void_t lx_canvas_clip_round_rect(lx_canvas_ref_t canvas, lx_size_t mode, lx_round_rect_ref_t rect) {
    lx_clipper_add_round_rect(lx_canvas_clipper(canvas), mode, rect);
}

static lx_inline lx_void_t lx_canvas_clip_round_rect2(lx_canvas_ref_t canvas, lx_size_t mode, lx_rect_ref_t bounds, lx_float_t rx, lx_float_t ry) {
    lx_round_rect_t rect;
    lx_round_rect_make_same(&rect, bounds, rx, ry);
    lx_canvas_clip_round_rect(canvas, mode, &rect);
}

static lx_inline lx_void_t lx_canvas_clip_round_rect2i(lx_canvas_ref_t canvas, lx_size_t mode, lx_rect_ref_t bounds, lx_size_t rx, lx_size_t ry) {
    lx_round_rect_t rect;
    lx_round_rect_imake_same(&rect, bounds, rx, ry);
    lx_canvas_clip_round_rect(canvas, mode, &rect);
}

static lx_inline lx_void_t lx_canvas_clip_circle(lx_canvas_ref_t canvas, lx_size_t mode, lx_circle_ref_t circle) {
    lx_clipper_add_circle(lx_canvas_clipper(canvas), mode, circle);
}

static lx_inline lx_void_t lx_canvas_clip_circle2(lx_canvas_ref_t canvas, lx_size_t mode, lx_float_t x0, lx_float_t y0, lx_float_t r) {
    lx_circle_t circle;
    lx_circle_make(&circle, x0, y0, r);
    lx_canvas_clip_circle(canvas, mode, &circle);
}

static lx_inline lx_void_t lx_canvas_clip_circle2i(lx_canvas_ref_t canvas, lx_size_t mode, lx_long_t x0, lx_long_t y0, lx_size_t r) {
    lx_circle_t circle;
    lx_circle_imake(&circle, x0, y0, r);
    lx_canvas_clip_circle(canvas, mode, &circle);
}

static lx_inline lx_void_t lx_canvas_clip_ellipse(lx_canvas_ref_t canvas, lx_size_t mode, lx_ellipse_ref_t ellipse) {
    lx_clipper_add_ellipse(lx_canvas_clipper(canvas), mode, ellipse);
}

static lx_inline lx_void_t lx_canvas_clip_ellipse2(lx_canvas_ref_t canvas, lx_size_t mode, lx_float_t x0, lx_float_t y0, lx_float_t rx, lx_float_t ry) {
    lx_ellipse_t ellipse;
    lx_ellipse_make(&ellipse, x0, y0, rx, ry);
    lx_canvas_clip_ellipse(canvas, mode, &ellipse);
}

static lx_inline lx_void_t lx_canvas_clip_ellipse2i(lx_canvas_ref_t canvas, lx_size_t mode, lx_long_t x0, lx_long_t y0, lx_size_t rx, lx_size_t ry) {
    lx_ellipse_t ellipse;
    lx_ellipse_imake(&ellipse, x0, y0, rx, ry);
    lx_canvas_clip_ellipse(canvas, mode, &ellipse);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


