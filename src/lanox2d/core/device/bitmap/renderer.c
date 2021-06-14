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
#include "renderer/rect.h"
#include "renderer/lines.h"
#include "renderer/points.h"
#include "renderer/polygon.h"
#include "writer.h"
#include "../../private/stroker.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_bool_t lx_bitmap_renderer_apply_matrix_for_hint(lx_bitmap_device_t* device, lx_shape_ref_t hint, lx_shape_ref_t output) {
    lx_assert(device && device->base.matrix && output);

    // clear output first
    output->type = LX_SHAPE_TYPE_NONE;

    // rect and no rotation?
    if (    hint
        &&  hint->type == LX_SHAPE_TYPE_RECT
        &&  0 == device->base.matrix->kx && 0 == device->base.matrix->ky) {
        lx_rect_apply2(&hint->u.rect, &output->u.rect, device->base.matrix);
        output->type = LX_SHAPE_TYPE_RECT;
    }
    return output->type != LX_SHAPE_TYPE_NONE;
}

static lx_size_t lx_bitmap_renderer_apply_matrix_for_points(lx_bitmap_device_t* device, lx_point_ref_t points, lx_size_t count, lx_point_ref_t* output) {
    lx_assert(device && device->points && device->base.matrix && points);

    lx_array_clear(device->points);
    lx_size_t index = 0;
    for (index = 0; index < count; index++) {
        lx_point_t point;
        lx_point_apply2(points + index, &point, device->base.matrix);
        lx_array_insert_tail(device->points, &point);
    }

    if (output) *output = (lx_point_ref_t)lx_array_data(device->points);
    return lx_array_size(device->points);
}

static lx_size_t lx_bitmap_renderer_apply_matrix_for_polygon(lx_bitmap_device_t* device, lx_polygon_ref_t polygon, lx_point_ref_t* output) {
    lx_assert(device && device->points && device->base.matrix && polygon && polygon->points);

    lx_array_clear(device->points);
    lx_point_ref_t  points = polygon->points;
    lx_uint16_t*    counts = polygon->counts;
    lx_uint16_t     count = *counts++;
    lx_uint16_t     index = 0;
    while (index < count) {
        lx_point_t point;
        lx_point_apply2(points++, &point, device->base.matrix);
        lx_array_insert_tail(device->points, &point);
        index++;
        if (index == count) { // next polygon
            count = *counts++;
            index = 0;
        }
    }

    if (output) *output = (lx_point_ref_t)lx_array_data(device->points);
    return lx_array_size(device->points);
}

static lx_rect_ref_t lx_bitmap_renderer_make_bounds_for_points(lx_bitmap_device_t* device, lx_rect_ref_t bounds, lx_point_ref_t points, lx_size_t count) {
    lx_assert(device && device->base.matrix && points && count);
    if (bounds) {
        // make approximate bounds using matrix
        lx_point_t pt[4];
        lx_point_make(&pt[0], bounds->x, bounds->y);
        lx_point_make(&pt[1], bounds->x, bounds->y + bounds->h);
        lx_point_make(&pt[2], bounds->x + bounds->w, bounds->y + bounds->h);
        lx_point_make(&pt[3], bounds->x + bounds->w, bounds->y);
        lx_matrix_apply_points(device->base.matrix, pt, lx_arrayn(pt));
        lx_bounds_make(&device->bounds, pt, lx_arrayn(pt));
    } else {
        // make accurate bounds
        lx_bounds_make(&device->bounds, points, count);
    }
    return &device->bounds;
}

static lx_void_t lx_bitmap_renderer_stroke_fill(lx_bitmap_device_t* device, lx_path_ref_t path) {
    lx_assert(device && device->stroker && device->base.paint && path);
    if (!lx_path_empty(path)) {
        lx_size_t mode = lx_paint_mode(device->base.paint);
        lx_size_t rule = lx_paint_fill_rule(device->base.paint);
        lx_paint_mode_set(device->base.paint, LX_PAINT_MODE_FILL);
        lx_paint_fill_rule_set(device->base.paint, LX_PAINT_FILL_RULE_NONZERO);
        lx_bitmap_renderer_draw_path(device, path);
        lx_paint_mode_set(device->base.paint, mode);
        lx_paint_fill_rule_set(device->base.paint, rule);
    }
}

static lx_inline lx_bool_t lx_bitmap_renderer_stroke_only(lx_bitmap_device_t* device) {
    lx_assert(device && device->base.paint && device->base.matrix);
    // width == 1 and solid? only stroke it
    return (    1.0f == lx_paint_stroke_width(device->base.paint)
            &&  1.0f == lx_abs(device->base.matrix->sx)
            &&  1.0f == lx_abs(device->base.matrix->sy));
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bool_t lx_bitmap_renderer_init(lx_bitmap_device_t* device) {
    return lx_bitmap_writer_init(&device->writer, device->bitmap, device->base.paint);
}

lx_void_t lx_bitmap_renderer_exit(lx_bitmap_device_t* device) {
    lx_bitmap_writer_exit(&device->writer);
}

lx_void_t lx_bitmap_renderer_draw_path(lx_bitmap_device_t* device, lx_path_ref_t path) {
    lx_assert(device && device->base.paint && path);

    // fill it
    lx_size_t mode = lx_paint_mode(device->base.paint);
    if (mode & LX_PAINT_MODE_FILL) {
        lx_bitmap_renderer_draw_polygon(device, lx_path_polygon(path), lx_path_hint(path), lx_path_bounds(path));
    }

    // stroke it
    if ((mode & LX_PAINT_MODE_STROKE) && (lx_paint_stroke_width(device->base.paint) > 0)) {
        if (lx_bitmap_renderer_stroke_only(device)) {
            lx_bitmap_renderer_draw_polygon(device, lx_path_polygon(path), lx_path_hint(path), lx_path_bounds(path));
        } else {
            lx_bitmap_renderer_stroke_fill(device, lx_stroker_make_from_path(device->stroker, device->base.paint, path));
        }
    }
}

lx_void_t lx_bitmap_renderer_draw_lines(lx_bitmap_device_t* device, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
    lx_assert(device && device->base.paint && device->base.matrix && points && count && !(count & 0x1));
    lx_check_return(lx_paint_mode(device->base.paint) & LX_PAINT_MODE_STROKE);
    lx_check_return((lx_paint_stroke_width(device->base.paint) > 0));

    // only stroke?
    if (lx_bitmap_renderer_stroke_only(device)) {
        lx_point_ref_t  stroked_points  = lx_null;
        lx_size_t       stroked_count   = lx_bitmap_renderer_apply_matrix_for_points(device, points, count, &stroked_points);
        lx_assert(stroked_points && stroked_count);

        // TODO: clip it
        // ...

        // stroke lines
        lx_bitmap_renderer_stroke_lines(device, stroked_points, stroked_count);
    } else {
        // fill the stroked lines
        lx_bitmap_renderer_stroke_fill(device, lx_stroker_make_from_lines(device->stroker, device->base.paint, points, count));
    }
}

lx_void_t lx_bitmap_renderer_draw_points(lx_bitmap_device_t* device, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
    lx_assert(device && device->base.paint && device->base.matrix && points && count);
    lx_check_return(lx_paint_mode(device->base.paint) & LX_PAINT_MODE_STROKE);
    lx_check_return((lx_paint_stroke_width(device->base.paint) > 0));

    // only stroke?
    if (lx_bitmap_renderer_stroke_only(device)) {
        lx_point_ref_t  stroked_points  = lx_null;
        lx_size_t       stroked_count   = lx_bitmap_renderer_apply_matrix_for_points(device, points, count, &stroked_points);
        lx_assert(stroked_points && stroked_count);

        // TODO: clip it
        // ...

        // stroke points
        lx_bitmap_renderer_stroke_points(device, stroked_points, stroked_count);
    } else {
        // fill the stroked points
        lx_bitmap_renderer_stroke_fill(device, lx_stroker_make_from_points(device->stroker, device->base.paint, points, count));
    }
}

lx_void_t lx_bitmap_renderer_draw_polygon(lx_bitmap_device_t* device, lx_polygon_ref_t polygon, lx_shape_ref_t hint, lx_rect_ref_t bounds) {
    lx_assert(device && device->base.paint && polygon);

    if (hint && hint->type == LX_SHAPE_TYPE_LINE) {
        lx_point_t points[2];
        points[0] = hint->u.line.p0;
        points[1] = hint->u.line.p1;
        lx_bitmap_renderer_draw_lines(device, points, 2, bounds);
        return ;
    } else if (hint && hint->type == LX_SHAPE_TYPE_POINT) {
        lx_bitmap_renderer_draw_points(device, &hint->u.point, 1, bounds);
        return ;
    }

    // fill it
    lx_size_t mode = lx_paint_mode(device->base.paint);
    if (mode & LX_PAINT_MODE_FILL) {
        lx_polygon_t filled_polygon = {lx_null, polygon->counts, polygon->convex};
        lx_size_t    filled_count   = lx_bitmap_renderer_apply_matrix_for_polygon(device, polygon, &filled_polygon.points);
        lx_assert(filled_polygon.points && filled_count);

        // make the filled bounds
        lx_rect_ref_t filled_bounds = lx_bitmap_renderer_make_bounds_for_points(device, bounds, filled_polygon.points, filled_count);
        lx_assert(filled_bounds);

        // TODO: clip it
        lx_bool_t clipped = lx_false;

        // apply matrix to hint
        lx_shape_t filled_hint;
        if (!clipped && lx_bitmap_renderer_apply_matrix_for_hint(device, hint, &filled_hint)) {
            lx_assert(filled_hint.type == LX_SHAPE_TYPE_RECT);
            lx_bitmap_renderer_fill_rect(device, &filled_hint.u.rect);
        } else {
            lx_bitmap_renderer_fill_polygon(device, &filled_polygon, filled_bounds);
        }
    }

    // stroke it
    if ((mode & LX_PAINT_MODE_STROKE) && (lx_paint_stroke_width(device->base.paint) > 0)) {
        if (lx_bitmap_renderer_stroke_only(device)) {
            lx_polygon_t stroked_polygon = {lx_null, polygon->counts, polygon->convex};
            lx_size_t    stroked_count   = lx_bitmap_renderer_apply_matrix_for_polygon(device, polygon, &stroked_polygon.points);
            lx_assert(stroked_polygon.points && stroked_count);

            // TODO: clip it
            // ...

            // stroke polygon
            if (stroked_count) lx_bitmap_renderer_stroke_polygon(device, &stroked_polygon);
        } else {
            lx_bitmap_renderer_stroke_fill(device, lx_stroker_make_from_polygon(device->stroker, device->base.paint, polygon, hint));
        }
    }
}


