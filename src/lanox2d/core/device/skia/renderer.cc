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
#include "../../quality.h"
#include "../../shader.h"
#include "../../private/shader.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bool_t lx_skia_renderer_init(lx_skia_device_t* device) {
    lx_assert_and_check_return_val(device && device->base.matrix && device->base.paint, lx_false);

    lx_bool_t ok = lx_false;
    do {
        // init shader
        device->shader = lx_paint_shader(device->base.paint);

        // ok
        ok = lx_true;

    } while (0);
    return ok;
}

lx_void_t lx_skia_renderer_exit(lx_skia_device_t* device) {
}

lx_void_t lx_skia_renderer_draw_path(lx_skia_device_t* device, lx_path_ref_t path) {
    lx_assert(device && device->base.paint && path);
}

lx_void_t lx_skia_renderer_draw_lines(lx_skia_device_t* device, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
    lx_assert(device && device->base.paint && points && count);
}

lx_void_t lx_skia_renderer_draw_points(lx_skia_device_t* device, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
    lx_assert(device && device->base.paint && points && count);
}

lx_void_t lx_skia_renderer_draw_polygon(lx_skia_device_t* device, lx_polygon_ref_t polygon, lx_shape_ref_t hint, lx_rect_ref_t bounds) {
    lx_assert(device && device->base.paint && polygon && polygon->points && polygon->counts);
}

