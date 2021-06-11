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
 * @file        canvas_paint.h
 *
 */
#ifndef LX_CORE_CANVAS_PAINT_H
#define LX_CORE_CANVAS_PAINT_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "paint.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! get the paint
 *
 * @param canvas    the canvas
 *
 * @return          the paint
 */
lx_paint_ref_t      lx_canvas_paint(lx_canvas_ref_t canvas);

/*! save paint
 *
 * @param canvas    the canvas
 *
 * @return          the current paint
 */
lx_paint_ref_t      lx_canvas_save_paint(lx_canvas_ref_t canvas);

/*! load paint
 *
 * @param canvas    the canvas
 */
lx_void_t           lx_canvas_load_paint(lx_canvas_ref_t canvas);

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline interfaces
 */
static lx_inline lx_void_t lx_canvas_clear_paint(lx_canvas_ref_t canvas) {
    lx_paint_clear(lx_canvas_paint(canvas));
}

static lx_inline lx_void_t lx_canvas_mode_set(lx_canvas_ref_t canvas, lx_size_t mode) {
    lx_paint_mode_set(lx_canvas_paint(canvas), mode);
}

static lx_inline lx_void_t lx_canvas_flags_set(lx_canvas_ref_t canvas, lx_size_t flags) {
    lx_paint_flags_set(lx_canvas_paint(canvas), flags);
}

static lx_inline lx_void_t lx_canvas_color_set(lx_canvas_ref_t canvas, lx_color_t color) {
    lx_paint_color_set(lx_canvas_paint(canvas), color);
}

static lx_inline lx_void_t lx_canvas_alpha_set(lx_canvas_ref_t canvas, lx_byte_t alpha) {
    lx_paint_alpha_set(lx_canvas_paint(canvas), alpha);
}

static lx_inline lx_void_t lx_canvas_stroke_width_set(lx_canvas_ref_t canvas, lx_float_t width) {
    lx_paint_stroke_width_set(lx_canvas_paint(canvas), width);
}

static lx_inline lx_void_t lx_canvas_stroke_cap_set(lx_canvas_ref_t canvas, lx_size_t cap) {
    lx_paint_stroke_cap_set(lx_canvas_paint(canvas), cap);
}

static lx_inline lx_void_t lx_canvas_stroke_join_set(lx_canvas_ref_t canvas, lx_size_t join) {
    lx_paint_stroke_join_set(lx_canvas_paint(canvas), join);
}

static lx_inline lx_void_t lx_cavas_stroke_miter_set(lx_canvas_ref_t canvas, lx_float_t miter) {
    lx_paint_stroke_miter_set(lx_canvas_paint(canvas), miter);
}

static lx_inline lx_void_t lx_canvas_fill_rule_set(lx_canvas_ref_t canvas, lx_size_t rule) {
    lx_paint_fill_rule_set(lx_canvas_paint(canvas), rule);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


