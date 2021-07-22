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
static lx_inline SkPathFillType lx_skia_renderer_path_fill_type(lx_skia_device_t* device) {
    lx_assert(device && device->base.paint);
    SkPathFillType filltype = SkPathFillType::kEvenOdd;
    if (lx_paint_fill_rule(device->base.paint) == LX_PAINT_FILL_RULE_NONZERO) {
        filltype = SkPathFillType::kWinding;
    }
    return filltype;
}

static lx_inline SkTileMode lx_skia_renderer_texture_tile_mode(lx_size_t tile_mode) {
    static SkTileMode s_tile_mode[] =
    {
        SkTileMode::kDecal
    ,   SkTileMode::kDecal
    ,   SkTileMode::kClamp
    ,   SkTileMode::kRepeat
    ,   SkTileMode::kMirror
    };
    lx_assert(tile_mode < lx_arrayn(s_tile_mode));
    return s_tile_mode[tile_mode];
}

static lx_inline SkPaint::Style lx_skia_renderer_paint_style(lx_skia_device_t* device) {
    lx_assert(device && device->base.paint);
    SkPaint::Style style = SkPaint::kFill_Style;
    lx_size_t mode = lx_paint_mode(device->base.paint);
    switch (mode) {
    case LX_PAINT_MODE_FILL:
        style = SkPaint::kFill_Style;
        break;
    case LX_PAINT_MODE_STROKE:
        style = SkPaint::kStroke_Style;
        break;
    case LX_PAINT_MODE_FILL_STROKE:
        style = SkPaint::kStrokeAndFill_Style;
        break;
    }
    return style;
}

static lx_inline SkPaint::Cap lx_skia_renderer_paint_stroke_cap(lx_skia_device_t* device) {
    lx_assert(device && device->base.paint);
    SkPaint::Cap cap = SkPaint::kButt_Cap;
    switch (lx_paint_stroke_cap(device->base.paint)) {
    case LX_PAINT_STROKE_CAP_BUTT:
        cap = SkPaint::kButt_Cap;
        break;
    case LX_PAINT_STROKE_CAP_ROUND:
        cap = SkPaint::kRound_Cap;
        break;
    case LX_PAINT_STROKE_CAP_SQUARE:
        cap = SkPaint::kSquare_Cap;
        break;
    default:
        break;
    }
    return cap;
}

static lx_inline SkPaint::Join lx_skia_renderer_paint_stroke_join(lx_skia_device_t* device) {
    lx_assert(device && device->base.paint);
    SkPaint::Join join = SkPaint::kMiter_Join;
    switch (lx_paint_stroke_join(device->base.paint)) {
    case LX_PAINT_STROKE_JOIN_MITER:
        join = SkPaint::kMiter_Join;
        break;
    case LX_PAINT_STROKE_JOIN_ROUND:
        join = SkPaint::kRound_Join;
        break;
    case LX_PAINT_STROKE_JOIN_BEVEL:
        join = SkPaint::kBevel_Join;
        break;
    default:
        break;
    }
    return join;
}

static lx_inline lx_void_t lx_skia_renderer_apply_matrix(lx_skia_device_t* device) {
    SkMatrix        sk_matrix;
    lx_matrix_ref_t matrix = device->base.matrix;
    lx_assert(matrix);

    sk_matrix.setAll(matrix->sx, matrix->kx, matrix->tx, matrix->ky, matrix->sy, matrix->ty, 0, 0, 1);
    device->canvas->setMatrix(sk_matrix);
}

static lx_inline lx_void_t lx_skia_renderer_apply_shader_bitmap(lx_skia_device_t* device, lx_shader_ref_t shader) {
    lx_assert(device && shader);
    lx_bitmap_ref_t bitmap = ((lx_bitmap_shader_t*)shader)->bitmap;
    if (!device->texture) {
        device->texture = new SkBitmap();
    }
    lx_assert(bitmap);

    lx_pointer_t data   = lx_bitmap_data(bitmap);
    lx_size_t width     = lx_bitmap_width(bitmap);
    lx_size_t height    = lx_bitmap_height(bitmap);
    lx_size_t row_bytes = lx_bitmap_row_bytes(bitmap);
    lx_size_t pixfmt    = lx_bitmap_pixfmt(bitmap);
    lx_assert_and_check_break(data && width && height && row_bytes);

    SkImageInfo const bitmap_info = SkImageInfo::Make((lx_int_t)width, (lx_int_t)height,
        lx_skia_color_type(pixfmt), kOpaque_SkAlphaType);
    device->texture->setInfo(bitmap_info, (lx_int_t)row_bytes);
    device->texture->setPixels(data);

    SkMatrix sk_matrix;
    lx_matrix_ref_t matrix = lx_shader_matrix(shader);
    sk_matrix.setAll(matrix->sx, matrix->kx, matrix->tx, matrix->ky, matrix->sy, matrix->ty, 0, 0, 1);

    SkFilterMode filter = SkFilterMode::kNearest;
    if (lx_paint_flags(device->base.paint) & LX_PAINT_FLAG_FILTER_BITMAP) {
        filter = SkFilterMode::kLinear;
    }
    SkTileMode tile_mode = lx_skia_renderer_texture_tile_mode(lx_shader_tile_mode(shader));

    SkSamplingOptions options(filter, SkMipmapMode::kNone);
    SkPaint*        sk_paint = device->paint;
    sk_sp<SkShader> sk_shader = device->texture->makeShader(tile_mode, tile_mode, options, &sk_matrix);
    lx_assert(sk_shader && sk_paint);
    sk_paint->setShader(sk_shader);
}

static lx_inline lx_void_t lx_skia_renderer_apply_shader(lx_skia_device_t* device, lx_shader_ref_t shader) {
    switch (lx_shader_type(shader)) {
    case LX_SHADER_TYPE_BITMAP:
        lx_skia_renderer_apply_shader_bitmap(device, shader);
        break;
    default:
        break;
    }
}

static lx_inline lx_void_t lx_skia_renderer_apply_paint(lx_skia_device_t* device) {
    lx_paint_ref_t paint = device->base.paint;
    SkPaint*       sk_paint = device->paint;
    lx_assert(paint && sk_paint);

    sk_paint->reset();
    sk_paint->setColor(lx_skia_color(lx_paint_color(paint)));
    sk_paint->setAntiAlias(lx_paint_flags(paint) & LX_PAINT_FLAG_ANTIALIASING? true : false);
    sk_paint->setStyle(lx_skia_renderer_paint_style(device));
    if (lx_paint_mode(paint) & LX_PAINT_MODE_STROKE) {
        sk_paint->setStrokeMiter(lx_paint_stroke_miter(paint));
        sk_paint->setStrokeWidth(lx_paint_stroke_width(paint));
        sk_paint->setStrokeCap(lx_skia_renderer_paint_stroke_cap(device));
        sk_paint->setStrokeJoin(lx_skia_renderer_paint_stroke_join(device));
    }
    lx_shader_ref_t shader = lx_paint_shader(paint);
    if (shader) {
        lx_skia_renderer_apply_shader(device, shader);
    }
}

static lx_inline lx_void_t lx_skia_renderer_draw_line(lx_skia_device_t* device, lx_line_ref_t line) {
    SkPaint* sk_paint = device->paint;
    lx_assert(line && sk_paint);
    device->canvas->drawLine(line->p0.x, line->p0.y, line->p1.x, line->p1.y, *sk_paint);
}

static lx_inline lx_void_t lx_skia_renderer_draw_point(lx_skia_device_t* device, lx_point_ref_t point) {
    SkPaint* sk_paint = device->paint;
    lx_assert(point && sk_paint);
    device->canvas->drawPoint(point->x, point->y, *sk_paint);
}

static lx_inline lx_void_t lx_skia_renderer_draw_rect(lx_skia_device_t* device, lx_rect_ref_t rect) {
    SkPaint* sk_paint = device->paint;
    lx_assert(rect && sk_paint);
    device->canvas->drawRect(SkRect::MakeXYWH(rect->x, rect->y, rect->w, rect->h), *sk_paint);
}

static lx_inline lx_void_t lx_skia_renderer_draw_circle(lx_skia_device_t* device, lx_circle_ref_t circle) {
    SkPaint* sk_paint = device->paint;
    lx_assert(circle && sk_paint);
    device->canvas->drawCircle(circle->c.x, circle->c.y, circle->r, *sk_paint);
}

static lx_inline lx_void_t lx_skia_renderer_draw_ellipse(lx_skia_device_t* device, lx_ellipse_ref_t ellipse) {
    SkPaint* sk_paint = device->paint;
    lx_assert(ellipse && sk_paint);
    device->canvas->drawOval(SkRect::MakeXYWH(ellipse->c.x - ellipse->rx, ellipse->c.y - ellipse->ry, ellipse->rx + ellipse->rx, ellipse->ry + ellipse->ry), *sk_paint);
}

static lx_inline lx_void_t lx_skia_renderer_draw_triangle(lx_skia_device_t* device, lx_triangle_ref_t triangle) {
    SkPaint* sk_paint = device->paint;
    SkPath*  sk_path = device->path;
    lx_assert(triangle && sk_paint && sk_path);
    sk_path->reset();
    sk_path->incReserve(3);
    sk_path->setFillType(lx_skia_renderer_path_fill_type(device));
    sk_path->moveTo(triangle->p0.x, triangle->p0.y);
    sk_path->lineTo(triangle->p1.x, triangle->p1.y);
    sk_path->lineTo(triangle->p2.x, triangle->p2.y);
    sk_path->close();
    device->canvas->drawPath(*sk_path, *sk_paint);
}

static lx_inline lx_void_t lx_skia_renderer_draw_arc(lx_skia_device_t* device, lx_arc_ref_t arc) {
    SkPaint* sk_paint = device->paint;
    lx_assert(arc && sk_paint);
    device->canvas->drawArc(SkRect::MakeXYWH(arc->c.x - arc->rx, arc->c.y - arc->ry, arc->rx * 2.0f, arc->ry * 2), arc->ab, arc->an, false, *sk_paint);
}

static lx_void_t lx_skia_renderer_draw_shape(lx_skia_device_t* device, lx_shape_ref_t shape, lx_rect_ref_t bounds) {
    switch (shape->type) {
    case LX_SHAPE_TYPE_LINE:
        lx_skia_renderer_draw_line(device, &shape->u.line);
        break;
    case LX_SHAPE_TYPE_RECT:
        lx_skia_renderer_draw_rect(device, &shape->u.rect);
        break;
    case LX_SHAPE_TYPE_CIRCLE:
        lx_skia_renderer_draw_circle(device, &shape->u.circle);
        break;
    case LX_SHAPE_TYPE_ELLIPSE:
        lx_skia_renderer_draw_ellipse(device, &shape->u.ellipse);
        break;
    case LX_SHAPE_TYPE_TRIANGLE:
        lx_skia_renderer_draw_triangle(device, &shape->u.triangle);
        break;
    case LX_SHAPE_TYPE_ARC:
        lx_skia_renderer_draw_arc(device, &shape->u.arc);
        break;
    case LX_SHAPE_TYPE_POINT:
        lx_skia_renderer_draw_point(device, &shape->u.point);
        break;
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
    lx_skia_renderer_apply_matrix(device);
    lx_skia_renderer_apply_paint(device);
    return lx_true;
}

lx_void_t lx_skia_renderer_exit(lx_skia_device_t* device) {
    lx_assert(device && device->canvas);
    device->canvas->restore();
}

lx_void_t lx_skia_renderer_draw_path(lx_skia_device_t* device, lx_path_ref_t path) {
    lx_assert(device && device->paint && path);

    // draw hint shape
    lx_shape_ref_t hint = lx_path_hint(path);
    if (hint) {
        lx_skia_renderer_draw_shape(device, hint, lx_path_bounds(path));
        return ;
    }

    // draw path
    SkPath* sk_path = device->path;
    lx_assert(sk_path);
    sk_path->reset();
    sk_path->incReserve(256);
    sk_path->setFillType(lx_skia_renderer_path_fill_type(device));
    lx_for_all_if (lx_path_item_ref_t, item, path, item) {
        switch (item->code) {
        case LX_PATH_CODE_MOVE:
            sk_path->moveTo(item->points[0].x, item->points[0].y);
            break;
        case LX_PATH_CODE_LINE:
            sk_path->lineTo(item->points[1].x, item->points[1].y);
            break;
        case LX_PATH_CODE_QUAD:
            sk_path->quadTo(item->points[1].x, item->points[1].y, item->points[2].x, item->points[2].y);
            break;
        case LX_PATH_CODE_CUBIC:
            sk_path->cubicTo(item->points[1].x, item->points[1].y, item->points[2].x, item->points[2].y, item->points[3].x, item->points[3].y);
            break;
        case LX_PATH_CODE_CLOSE:
            sk_path->close();
            break;
        default:
            break;
        }
    }
    device->canvas->drawPath(*sk_path, *device->paint);
}

lx_void_t lx_skia_renderer_draw_lines(lx_skia_device_t* device, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
    lx_assert(device && device->base.paint && points && count);

    lx_line_t line;
    lx_size_t index = 0;
    for (index = 0; index + 1 < count; index += 2) {
        line.p0 = points[index];
        line.p1 = points[index + 1];
        lx_skia_renderer_draw_line(device, &line);
    }
}

lx_void_t lx_skia_renderer_draw_points(lx_skia_device_t* device, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
    lx_assert(device && device->base.paint && points && count);

    lx_size_t index = 0;
    for (index = 0; index < count; index++) {
        lx_skia_renderer_draw_point(device, points + index);
    }
}

lx_void_t lx_skia_renderer_draw_polygon(lx_skia_device_t* device, lx_polygon_ref_t polygon, lx_shape_ref_t hint, lx_rect_ref_t bounds) {
    lx_assert(device && device->base.paint && polygon && polygon->points && polygon->counts);

    // draw hint shape
    if (hint) {
        lx_skia_renderer_draw_shape(device, hint, bounds);
        return ;
    }

    // init path
    SkPath* sk_path = device->path;
    lx_assert(sk_path);
    sk_path->reset();
    sk_path->incReserve(256);
    sk_path->setFillType(lx_skia_renderer_path_fill_type(device));

    // draw polygon
    lx_uint16_t     index = 0;
    lx_point_ref_t  points = polygon->points;
    lx_uint16_t*    counts = polygon->counts;
    lx_uint16_t     count = *counts++;
    while (index < count) {
        if (index == 0) {
            sk_path->moveTo(points[index].x, points[index].y);
        } else {
            sk_path->lineTo(points[index].x, points[index].y);
        }
        index++;
        if (index == count) { // next polygon
            count = *counts++;
            index = 0;
            sk_path->close();
            device->canvas->drawPath(*sk_path, *device->paint);
        }
    }
}

