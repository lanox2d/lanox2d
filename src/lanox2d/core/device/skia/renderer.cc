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
 * @file        renderer.cc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "renderer.h"
#include "color.h"
#include "../../quality.h"
#include "../../shader.h"
#include "../../private/shader.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_inline lx_void_t lx_skia_apply_matrix(lx_skia_device_t* device) {
    SkMatrix        sk_matrix;
    lx_matrix_ref_t matrix = device->base.matrix;
    lx_assert(matrix);

    sk_matrix.setAll(matrix->sx, matrix->kx, matrix->tx, matrix->ky, matrix->sy, matrix->ty, 0, 0, 1);
    device->canvas->setMatrix(sk_matrix);
}

static lx_inline lx_void_t lx_skia_apply_paint(lx_skia_device_t* device) {
    lx_paint_ref_t paint = device->base.paint;
    SkPaint*       sk_paint = device->paint;
    lx_assert(paint && sk_paint);

    sk_paint->reset();
    sk_paint->setColor(lx_skia_color(lx_paint_color(paint)));
}

static lx_inline lx_void_t lx_skia_renderer_draw_rect(lx_skia_device_t* device, lx_rect_ref_t rect) {
    SkPaint* sk_paint = device->paint;
    lx_assert(rect && sk_paint);
    device->canvas->drawRect(SkRect::MakeXYWH(rect->x, rect->y, rect->w, rect->h), *sk_paint);
}

static lx_void_t lx_skia_renderer_draw_shape(lx_skia_device_t* device, lx_shape_ref_t shape, lx_rect_ref_t bounds) {
    switch (shape->type) {
    case LX_SHAPE_TYPE_RECT:
        lx_skia_renderer_draw_rect(device, &shape->u.rect);
    default:
        break;
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bool_t lx_skia_renderer_init(lx_skia_device_t* device) {
    lx_assert(device && device->base.matrix && device->base.paint);
    lx_assert(device->canvas && device->paint);
    device->canvas->save();
    lx_skia_apply_matrix(device);
    lx_skia_apply_paint(device);
    return lx_true;
}

lx_void_t lx_skia_renderer_exit(lx_skia_device_t* device) {
    lx_assert(device && device->canvas);
    device->canvas->restore();
}

lx_void_t lx_skia_renderer_draw_path(lx_skia_device_t* device, lx_path_ref_t path) {
    lx_assert(device && device->base.paint && path);

    // draw hint shape
    lx_shape_ref_t hint = lx_path_hint(path);
    if (hint) {
        lx_skia_renderer_draw_shape(device, hint, lx_path_bounds(path));
    }

    // draw path
}

lx_void_t lx_skia_renderer_draw_lines(lx_skia_device_t* device, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
    lx_assert(device && device->base.paint && points && count);
}

lx_void_t lx_skia_renderer_draw_points(lx_skia_device_t* device, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
    lx_assert(device && device->base.paint && points && count);
}

lx_void_t lx_skia_renderer_draw_polygon(lx_skia_device_t* device, lx_polygon_ref_t polygon, lx_shape_ref_t hint, lx_rect_ref_t bounds) {
    lx_assert(device && device->base.paint && polygon && polygon->points && polygon->counts);

    // draw hint shape
    if (hint) {
        lx_skia_renderer_draw_shape(device, hint, bounds);
    }

    // draw polygon
}

