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
 * private implementation
 */
static lx_inline lx_void_t lx_vk_renderer_apply_paint(lx_vulkan_device_t* device, lx_rect_ref_t bounds) {
    lx_assert(device);
}

static lx_inline lx_void_t lx_vk_renderer_fill_polygon(lx_vulkan_device_t* device, lx_polygon_ref_t polygon, lx_rect_ref_t bounds, lx_size_t rule) {
}

static lx_inline lx_void_t lx_vk_renderer_stroke_lines(lx_vulkan_device_t* device, lx_point_ref_t points, lx_size_t count) {
    lx_assert(device && points && count);
}

static lx_inline lx_void_t lx_vk_renderer_stroke_points(lx_vulkan_device_t* device, lx_point_ref_t points, lx_size_t count) {
    lx_assert(device && points && count);
}

static lx_inline lx_void_t lx_vk_renderer_stroke_polygon(lx_vulkan_device_t* device, lx_polygon_ref_t polygon) {
    lx_assert(device && polygon && polygon->points && polygon->counts);
}

static lx_void_t lx_vk_renderer_stroke_fill(lx_vulkan_device_t* device, lx_path_ref_t path) {
    lx_assert(device && device->stroker && device->base.paint && path);
    lx_check_return(!lx_path_empty(path));

    // the mode
    lx_size_t mode = lx_paint_mode(device->base.paint);

    // the rule
    lx_size_t rule = lx_paint_fill_rule(device->base.paint);

    // switch to the fill mode
    lx_paint_mode_set(device->base.paint, LX_PAINT_MODE_FILL);

    // switch to the non-zero fill rule
    lx_paint_fill_rule_set(device->base.paint, LX_PAINT_FILL_RULE_NONZERO);

    // draw path
    lx_vk_renderer_draw_path(device, path);

    // restore the mode
    lx_paint_mode_set(device->base.paint, mode);

    // restore the fill mode
    lx_paint_fill_rule_set(device->base.paint, rule);
}

static lx_inline lx_bool_t lx_vk_renderer_stroke_only(lx_vulkan_device_t* device) {
    lx_assert(device && device->base.paint && device->base.matrix);

    // width == 1 and solid? only stroke it
    return (    1.0f == lx_paint_stroke_width(device->base.paint)
            &&  1.0f == lx_abs(device->base.matrix->sx)
            &&  1.0f == lx_abs(device->base.matrix->sy)
            /*&&  !device->shader*/);
}

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
    lx_assert(device && device->base.paint && path);

    lx_size_t mode = lx_paint_mode(device->base.paint);
    if (mode & LX_PAINT_MODE_FILL) {
        lx_vk_renderer_draw_polygon(device, lx_path_polygon(path), lx_path_hint(path), lx_path_bounds(path));
    }

    if ((mode & LX_PAINT_MODE_STROKE) && (lx_paint_stroke_width(device->base.paint) > 0)) {
        if (lx_vk_renderer_stroke_only(device)) {
            lx_vk_renderer_draw_polygon(device, lx_path_polygon(path), lx_path_hint(path), lx_path_bounds(path));
        } else {
            lx_vk_renderer_stroke_fill(device, lx_stroker_make_from_path(device->stroker, device->base.paint, path));
        }
    }
}

lx_void_t lx_vk_renderer_draw_lines(lx_vulkan_device_t* device, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
    lx_assert(device && device->base.paint && points && count);
    lx_check_return(lx_paint_mode(device->base.paint) & LX_PAINT_MODE_STROKE);
    lx_check_return((lx_paint_stroke_width(device->base.paint) > 0));

    lx_vk_renderer_apply_paint(device, bounds);
    if (lx_vk_renderer_stroke_only(device)) {
        lx_vk_renderer_stroke_lines(device, points, count);
    } else {
        lx_vk_renderer_stroke_fill(device, lx_stroker_make_from_lines(device->stroker, device->base.paint, points, count));
    }
}

lx_void_t lx_vk_renderer_draw_points(lx_vulkan_device_t* device, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
    lx_assert(device && device->base.paint && points && count);
    lx_check_return(lx_paint_mode(device->base.paint) & LX_PAINT_MODE_STROKE);
    lx_check_return((lx_paint_stroke_width(device->base.paint) > 0));

    lx_vk_renderer_apply_paint(device, bounds);
    if (lx_vk_renderer_stroke_only(device)) {
        lx_vk_renderer_stroke_points(device, points, count);
    } else {
        lx_vk_renderer_stroke_fill(device, lx_stroker_make_from_points(device->stroker, device->base.paint, points, count));
    }
}

lx_void_t lx_vk_renderer_draw_polygon(lx_vulkan_device_t* device, lx_polygon_ref_t polygon, lx_shape_ref_t hint, lx_rect_ref_t bounds) {
    lx_assert(device && device->base.paint && polygon && polygon->points && polygon->counts);

    if (hint && hint->type == LX_SHAPE_TYPE_LINE) {
        lx_point_t points[2];
        points[0] = hint->u.line.p0;
        points[1] = hint->u.line.p1;
        lx_vk_renderer_draw_lines(device, points, 2, bounds);
        return ;
    } else if (hint && hint->type == LX_SHAPE_TYPE_POINT) {
        lx_vk_renderer_draw_points(device, &hint->u.point, 1, bounds);
        return ;
    }

    lx_vk_renderer_apply_paint(device, bounds);
    lx_size_t mode = lx_paint_mode(device->base.paint);
    if (mode & LX_PAINT_MODE_FILL) {
        lx_vk_renderer_fill_polygon(device, polygon, bounds, lx_paint_fill_rule(device->base.paint));
    }

    if ((mode & LX_PAINT_MODE_STROKE) && (lx_paint_stroke_width(device->base.paint) > 0)) {
        if (lx_vk_renderer_stroke_only(device)) {
            lx_vk_renderer_stroke_polygon(device, polygon);
        } else {
            lx_vk_renderer_stroke_fill(device, lx_stroker_make_from_polygon(device->stroker, device->base.paint, polygon, hint));
        }
    }
}

