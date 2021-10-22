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
 * @file        renderer.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "renderer.h"
#include "../../quality.h"
#include "../../tess/tess.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bool_t lx_vk_renderer_init(lx_vulkan_device_t* device) {
    lx_assert_and_check_return_val(device && device->base.matrix && device->base.paint, lx_false);
    return lx_true;
}

lx_void_t lx_vk_renderer_exit(lx_vulkan_device_t* device) {
}

lx_void_t lx_vk_renderer_draw_path(lx_vulkan_device_t* device, lx_path_ref_t path) {
}

lx_void_t lx_vk_renderer_draw_lines(lx_vulkan_device_t* device, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
}

lx_void_t lx_vk_renderer_draw_points(lx_vulkan_device_t* device, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
}

lx_void_t lx_vk_renderer_draw_polygon(lx_vulkan_device_t* device, lx_polygon_ref_t polygon, lx_shape_ref_t hint, lx_rect_ref_t bounds) {
}

