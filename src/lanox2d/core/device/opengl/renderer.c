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
#include "bitmap_shader.h"
#include "../../quality.h"
#include "../../tess/tess.h"
#include "../../shader.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// test tessellator
//#define LX_GL_TESSELLATOR_TEST_ENABLE

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

static lx_inline lx_void_t lx_gl_renderer_enable_antialiasing(lx_opengl_device_t* device, lx_bool_t enabled) {
    if (enabled) {
        lx_glEnable(LX_GL_MULTISAMPLE);
#if 0
        lx_glEnable(LX_GL_LINE_SMOOTH);
        lx_glHint(LX_GL_LINE_SMOOTH_HINT, LX_GL_NICEST);
#endif
    } else {
        lx_glDisable(LX_GL_MULTISAMPLE);
    }
}

static lx_inline lx_void_t lx_gl_renderer_enable_blend(lx_opengl_device_t* device, lx_bool_t enabled) {
    if (enabled) {
        lx_glEnable(LX_GL_BLEND);
        lx_glBlendFunc(LX_GL_SRC_ALPHA, LX_GL_ONE_MINUS_SRC_ALPHA);
    } else {
        lx_glDisable(LX_GL_BLEND);
    }
}

static lx_inline lx_void_t lx_gl_renderer_enable_vertices(lx_opengl_device_t* device, lx_bool_t enabled) {
    if (enabled) {
        lx_gl_vertex_attribute_enable(LX_GL_PROGRAM_LOCATION_VERTICES);
        lx_gl_matrix_ref_t projection = lx_gl_matrix_projection();
        if (projection) {
            lx_gl_matrix_uniform_set(LX_GL_PROGRAM_LOCATION_MATRIX_PROJECT, projection);
        }
        lx_gl_matrix_uniform_set(LX_GL_PROGRAM_LOCATION_MATRIX_MODEL, lx_gl_matrix_modelview());
    } else {
        lx_gl_vertex_attribute_disable(LX_GL_PROGRAM_LOCATION_VERTICES);
    }
}

static lx_void_t lx_gl_renderer_apply_texture_matrix(lx_opengl_device_t* device, lx_matrix_ref_t matrix) {
    lx_gl_matrix_convert(device->matrix_texture, matrix);
    lx_gl_matrix_uniform_set(LX_GL_PROGRAM_LOCATION_MATRIX_TEXCOORD, device->matrix_texture);
}

static lx_inline lx_void_t lx_gl_renderer_apply_texture_filter(lx_opengl_device_t* device, lx_GLuint_t filter) {
    lx_assert(device && device->base.paint);
    if (!(lx_paint_flags(device->base.paint) & LX_PAINT_FLAG_FILTER_BITMAP)) {
        filter = LX_GL_NEAREST;
    }
    lx_glTexParameteri(LX_GL_TEXTURE_2D, LX_GL_TEXTURE_MAG_FILTER, filter);
    lx_glTexParameteri(LX_GL_TEXTURE_2D, LX_GL_TEXTURE_MIN_FILTER, filter);
}

static lx_inline lx_void_t lx_gl_renderer_apply_texture_coords(lx_opengl_device_t* device, lx_point_ref_t points, lx_size_t count) {
    lx_assert(device && points);
    if (device->vertex_array) {
        lx_gl_vertex_array_enable(device->vertex_array);
    }
    if (device->vertex_buffer) {
        lx_gl_vertex_buffer_enable(device->vertex_buffer);
        lx_gl_vertex_buffer_data_set(points, (sizeof(lx_point_t) * count), lx_false);
        lx_gl_vertex_attribute_set(LX_GL_PROGRAM_LOCATION_TEXCOORDS, lx_null);
    } else {
        lx_gl_vertex_attribute_set(LX_GL_PROGRAM_LOCATION_TEXCOORDS, points);
    }
}

static lx_inline lx_void_t lx_gl_renderer_apply_vertices(lx_opengl_device_t* device, lx_point_ref_t points, lx_size_t count) {
    lx_assert(device && points);
    if (device->vertex_array) {
        lx_gl_vertex_array_enable(device->vertex_array);
    }
    if (device->vertex_buffer) {
        lx_gl_vertex_buffer_enable(device->vertex_buffer);
        lx_gl_vertex_buffer_data_set(points, (sizeof(lx_point_t) * count), lx_false);
        lx_gl_vertex_attribute_set(LX_GL_PROGRAM_LOCATION_VERTICES, lx_null);
    } else {
        lx_gl_vertex_attribute_set(LX_GL_PROGRAM_LOCATION_VERTICES, points);
    }
}

static lx_inline lx_void_t lx_gl_renderer_apply_color(lx_opengl_device_t* device, lx_color_t color) {
    lx_gl_vertex_color_set(LX_GL_PROGRAM_LOCATION_COLORS, color);
}

static lx_void_t lx_gl_renderer_apply_solid(lx_opengl_device_t* device) {

    // get paint
    lx_paint_ref_t paint = device->base.paint;
    lx_assert(paint);

    // disable texture
    lx_glDisable(LX_GL_TEXTURE_2D);

    // enable blend
    lx_color_t color = lx_paint_color(paint);
    lx_byte_t alpha = lx_paint_alpha(paint);
    if (alpha != 0xff) {
        color.a = alpha;
    }
    lx_gl_renderer_enable_blend(device, alpha != 0xff);

    // apply color
    lx_gl_renderer_apply_color(device, color);
}

static lx_void_t lx_gl_renderer_apply_shader_bitmap(lx_opengl_device_t* device, lx_shader_ref_t shader, lx_rect_ref_t bounds) {

    // get bitmap texture
    lx_bitmap_shader_devdata_t* devdata = lx_bitmap_shader_devdata((lx_bitmap_shader_t*)shader);
    lx_assert(devdata && devdata->texture);

    // apply texture
    lx_glEnable(LX_GL_TEXTURE_2D);
    lx_glBindTexture(LX_GL_TEXTURE_2D, devdata->texture);
    lx_gl_vertex_attribute_enable(LX_GL_PROGRAM_LOCATION_TEXCOORDS);

    // get bitmap
    lx_bitmap_ref_t bitmap = ((lx_bitmap_shader_t*)shader)->bitmap;
    lx_assert(bitmap);

    // get bitmap width and height
    lx_size_t width = lx_bitmap_width(bitmap);
    lx_size_t height = lx_bitmap_height(bitmap);

    // get paint
    lx_paint_ref_t paint = device->base.paint;
    lx_assert(paint);

    // enable blend
    lx_byte_t alpha = lx_paint_alpha(paint);
    lx_gl_renderer_enable_blend(device, alpha != 0xff || lx_shader_tile_mode(shader) == LX_SHADER_TILE_MODE_BORDER || lx_bitmap_has_alpha(bitmap));

    // apply color (only for alpha blend)
    lx_gl_renderer_apply_color(device, lx_color_make(alpha, 0xff, 0xff, 0xff));

    // apply filter
    lx_GLuint_t filter = lx_quality() > LX_QUALITY_LOW? LX_GL_LINEAR : LX_GL_NEAREST;
    lx_gl_renderer_apply_texture_filter(device, filter);

    // get coordinate and size
    lx_float_t bx = bounds->x;
    lx_float_t by = bounds->y;
    lx_float_t bw = bounds->w;
    lx_float_t bh = bounds->h;
    lx_float_t sw = (lx_float_t)width;
    lx_float_t sh = (lx_float_t)height;

    /* get matrix in camera coordinate
     *
     *       bx        bounds of vertices
     *      -------V7---------------------V6------
     *  by |     /                          \     |
     *     |   /              |               \   |
     *     | /    camera  sw  |                 \ |
     *    V8         O--------------------------- V5-----> (matrix in camera coordinate)
     *     |      sh |||||||| | ||||||||          |
     *     |         |||||||| | ||||||||          | bh
     *     |    -----|--------.--------|------    |
     *     |         |||||||| | ||||||||          |
     *     |         |||||||| | ||||||||          |
     *    V1         |-----------------           V4
     *     | \      \|/       |                 / |
     *     |   \              |               /   |
     *     |     \                          /     |
     *      -------V2--------------------V3-------
     *                       bw
     */
    lx_matrix_t matrix = devdata->matrix;

    /* move bitmap to bounds of vertices in camera coordinate
     *
     * after scaling:
     *
     *       bx        bounds of vertices
     *      -------V7---------------------V6------
     *  by |     /                          \     |
     *     |   /              |               \   |
     *     | /    camera      |                 \ |
     *    V8         O--------------------------- V5-----> (matrix in camera coordinate)
     *     |         |||||||| | |||||||||||||||||||||||||||||
     *     |         |||||||| | |||||||||||||||||||||||||||||
     *     |    -----|--------.--------||||||||||||||||||||||
     *     |         |||||||| | |||||||||||||||||||||||||||||
     *     |         |||||||| | |||||||||||||||||||||||||||||
     *    V1         ||||||||||||||||||||||||||||||||||||||||
     *     | \       ||||||||||||||||||||||||||||||||||||||||
     *     |   \     ||||||||||||||||||||||||||||||||||||||||
     *     |     \   ||||||||||||||||||||||||||||||||||||||||
     *      -------V2||||||||||||||||||||||||||||||||||||||||
     *               ||||||||||||||||||||||||||||||||||||||||
     *               ||||||||||||||||||||||||||||||||||||||||
     *               ||||||||||||||||||||||||||||||||||||||||
     *              \|/
     *
     * after translating:
     *
     *                    texture
     *  --------------------------------------------->
     * |
     * |     bx        bounds of vertices
     *      -------V7---------------------V6------
     *  by ||||| / |||||||||||||||||||||||| \ |||||
     *     ||| / |||||||||||| | ||||||||||||| \ |||
     *     | /    camera      |                 \ |
     *    V8         O--------------------------- V5-----> (matrix in camera coordinate)
     *     ||||||||| |||||||| | |||||||||||||||||||
     *     ||||||||| |||||||| | |||||||||||||||||||
     *     |    -----|--------.--------||||||||||||
     *     ||||||||| |||||||| | ||||||||||||||||||| bh
     *     ||||||||| |||||||| | |||||||||||||||||||
     *    V1 ||||||| |||||||||||||||||||||||||||| V4
     *     | \ ||||| |||||||||||||||||||||||||| / |
     *     ||| \ ||| |||||||||||||||||||||||| / |||
     *     ||||| \|| |||||||||||||||||||||||/ |||||
     *      -------V2--------------------V3-------
     *               |       bw
     *              \|/
     */
    lx_matrix_scale(&matrix, bw / sw, bh / sh);
    lx_matrix_translate(&matrix, bx / bw, by / bh);

    /* convert to texture coordinate (0,1), because our texture vertices is in world coordinate
     *
     * before:
     *
     *    bx
     * by  ---------------------
     *    |                     |
     *    |                     |
     *    |                     | bh
     *    |                     |
     *     ---------------------
     *              bw
     *
     * after:
     *
     * 0,0 -------------------- 1,0
     *    |                     |
     *    |                     |
     *    |                     |
     *    |                     |
     * 0,1 -------------------- 1,1
     */
    lx_matrix_scale(&matrix, 1.0f / bw, 1.0f / bh);
    lx_matrix_translate(&matrix, -bx, -by);

    // apply texture matrix
    lx_gl_renderer_apply_texture_matrix(device, &matrix);
}

static lx_inline lx_void_t lx_gl_renderer_apply_shader(lx_opengl_device_t* device, lx_shader_ref_t shader, lx_rect_ref_t bounds) {
    lx_size_t shader_type = lx_shader_type(shader);
    switch (shader_type) {
    case LX_SHADER_TYPE_BITMAP:
        lx_gl_renderer_apply_shader_bitmap(device, shader, bounds);
        break;
    default:
        lx_trace_e("not supported shader type!");
        break;
    }
}

static lx_inline lx_void_t lx_gl_renderer_apply_paint(lx_opengl_device_t* device, lx_rect_ref_t bounds) {
    lx_assert(device);

    if (device->shader) {
        lx_gl_renderer_apply_shader(device, device->shader, bounds);
    } else {
        lx_gl_renderer_apply_solid(device);
    }
}

static lx_inline lx_void_t lx_gl_renderer_draw_contour(lx_opengl_device_t* device, lx_point_ref_t points, lx_size_t index, lx_uint16_t count) {
#ifdef LX_GL_TESSELLATOR_TEST_ENABLE
    // enable blend
    lx_gl_renderer_enable_blend(device, lx_true);

    // compute value
    lx_size_t i = 0;
    lx_uint32_t value = 0;
    for (i = 0; i < count; i++) {
        value += (lx_uint32_t)points[i].x;
        value += (lx_uint32_t)points[i].y;
        value *= 2166136261ul;
    }

    // apply color
    lx_color_t  color;
    color.r = (lx_byte_t)value;
    color.g = (lx_byte_t)(value >> 8);
    color.b = (lx_byte_t)(value >> 16);
    color.a = 128;
    lx_gl_renderer_apply_color(device, color);
#endif
    lx_glDrawArrays(LX_GL_TRIANGLE_FAN, (lx_GLint_t)index, (lx_GLint_t)count);
}

static lx_inline lx_void_t lx_gl_renderer_fill_polygon(lx_opengl_device_t* device, lx_polygon_ref_t polygon, lx_rect_ref_t bounds, lx_size_t rule) {
    lx_assert(device && device->tessellator);

    lx_tessellator_rule_set(device->tessellator, rule);
#ifdef LX_GL_TESSELLATOR_TEST_ENABLE
    lx_tessellator_mode_set(device->tessellator, LX_TESSELLATOR_MODE_TRIANGULATION);
//    lx_tessellator_mode_set(device->tessellator, LX_TESSELLATOR_MODE_MONOTONE);
    lx_polygon_ref_t result = lx_tessellator_make(device->tessellator, polygon, bounds);
#else
    lx_polygon_ref_t result = polygon->convex? polygon : lx_tessellator_make(device->tessellator, polygon, bounds);
#endif
    if (result) {

        // apply texture coordinate
        if (device->shader && lx_shader_type(device->shader) == LX_SHADER_TYPE_BITMAP) {
            lx_gl_renderer_apply_texture_coords(device, result->points, result->total);
        }

        // apply vertices
        lx_assert(result && result->points);
        lx_gl_renderer_apply_vertices(device, result->points, result->total);

        // draw vertices
        lx_size_t mode = lx_tessellator_mode(device->tessellator);
        if (mode == LX_TESSELLATOR_MODE_TRIANGULATION) {
            lx_size_t index = 0;
            lx_size_t total = result->total;
            while (index + 4 <= total) {
                lx_gl_renderer_draw_contour(device, result->points + index, index, 4);
                index += 4;
            }
        } else {
            lx_uint16_t  count;
            lx_size_t    index = 0;
            lx_uint16_t* counts = result->counts;
            while ((count = *counts++)) {
                lx_gl_renderer_draw_contour(device, result->points + index, index, count);
                index += count;
            }
        }
    }
}

static lx_inline lx_void_t lx_gl_renderer_stroke_lines(lx_opengl_device_t* device, lx_point_ref_t points, lx_size_t count) {
    lx_assert(device && points && count);

    lx_gl_renderer_apply_vertices(device, points, count);
    lx_glDrawArrays(LX_GL_LINES, 0, (lx_GLint_t)count);
}

static lx_inline lx_void_t lx_gl_renderer_stroke_points(lx_opengl_device_t* device, lx_point_ref_t points, lx_size_t count) {
    lx_assert(device && points && count);

    lx_gl_renderer_apply_vertices(device, points, count);
    lx_glDrawArrays(LX_GL_POINTS, 0, (lx_GLint_t)count);
}

static lx_inline lx_void_t lx_gl_renderer_stroke_polygon(lx_opengl_device_t* device, lx_polygon_ref_t polygon) {
    lx_assert(device && polygon && polygon->points && polygon->counts);

    // apply vertices
    lx_gl_renderer_apply_vertices(device, polygon->points, polygon->total);

    // draw vertices
    lx_uint16_t  count;
    lx_size_t    index = 0;
    lx_uint16_t* counts = polygon->counts;
    while ((count = *counts++)) {
        lx_glDrawArrays(LX_GL_LINE_STRIP, (lx_GLint_t)index, (lx_GLint_t)count);
        index += count;
    }
}

static lx_void_t lx_gl_renderer_stroke_fill(lx_opengl_device_t* device, lx_path_ref_t path) {
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
    lx_gl_renderer_draw_path(device, path);

    // restore the mode
    lx_paint_mode_set(device->base.paint, mode);

    // restore the fill mode
    lx_paint_fill_rule_set(device->base.paint, rule);
}

static lx_inline lx_bool_t lx_gl_renderer_stroke_only(lx_opengl_device_t* device) {
    lx_assert(device && device->base.paint && device->base.matrix);

    // width == 1 and solid? only stroke it
    return (    1.0f == lx_paint_stroke_width(device->base.paint)
            &&  1.0f == lx_abs(device->base.matrix->sx)
            &&  1.0f == lx_abs(device->base.matrix->sy)
            &&  !device->shader);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bool_t lx_gl_renderer_init(lx_opengl_device_t* device) {
    lx_assert_and_check_return_val(device && device->base.matrix && device->base.paint, lx_false);

    lx_bool_t ok = lx_false;
    do {
        // init shader
        device->shader = lx_paint_shader(device->base.paint);

        // init modelview matrix
        lx_gl_matrix_convert(lx_gl_matrix_modelview(), device->base.matrix);

        // get program
        lx_size_t program_type = device->shader? LX_GL_PROGRAM_TYPE_TEXTURE : LX_GL_PROGRAM_TYPE_SOLID;
        device->program = device->programs[program_type];
        if (device->program) {
            lx_gl_program_enable(device->program);
        }

        // init antialiasing
        lx_gl_renderer_enable_antialiasing(device, lx_paint_flags(device->base.paint) & LX_PAINT_FLAG_ANTIALIASING);

        // init vertex and matrix, we always enable vertices for filling and stroking
        lx_gl_renderer_enable_vertices(device, lx_true);

        // ok
        ok = lx_true;

    } while (0);
    return ok;
}

lx_void_t lx_gl_renderer_exit(lx_opengl_device_t* device) {
}

lx_void_t lx_gl_renderer_draw_path(lx_opengl_device_t* device, lx_path_ref_t path) {
    lx_assert(device && device->base.paint && path);

    lx_size_t mode = lx_paint_mode(device->base.paint);
    if (mode & LX_PAINT_MODE_FILL) {
        lx_gl_renderer_draw_polygon(device, lx_path_polygon(path), lx_path_hint(path), lx_path_bounds(path));
    }

    if ((mode & LX_PAINT_MODE_STROKE) && (lx_paint_stroke_width(device->base.paint) > 0)) {
        if (lx_gl_renderer_stroke_only(device)) {
            lx_gl_renderer_draw_polygon(device, lx_path_polygon(path), lx_path_hint(path), lx_path_bounds(path));
        } else {
            lx_gl_renderer_stroke_fill(device, lx_stroker_make_from_path(device->stroker, device->base.paint, path));
        }
    }
}

lx_void_t lx_gl_renderer_draw_lines(lx_opengl_device_t* device, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
    lx_assert(device && device->base.paint && points && count);
    lx_check_return(lx_paint_mode(device->base.paint) & LX_PAINT_MODE_STROKE);
    lx_check_return((lx_paint_stroke_width(device->base.paint) > 0));

    lx_gl_renderer_apply_paint(device, bounds);
    if (lx_gl_renderer_stroke_only(device)) {
        lx_gl_renderer_stroke_lines(device, points, count);
    } else {
        lx_gl_renderer_stroke_fill(device, lx_stroker_make_from_lines(device->stroker, device->base.paint, points, count));
    }
}

lx_void_t lx_gl_renderer_draw_points(lx_opengl_device_t* device, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
    lx_assert(device && device->base.paint && points && count);
    lx_check_return(lx_paint_mode(device->base.paint) & LX_PAINT_MODE_STROKE);
    lx_check_return((lx_paint_stroke_width(device->base.paint) > 0));

    lx_gl_renderer_apply_paint(device, bounds);
    if (lx_gl_renderer_stroke_only(device)) {
        lx_gl_renderer_stroke_points(device, points, count);
    } else {
        lx_gl_renderer_stroke_fill(device, lx_stroker_make_from_points(device->stroker, device->base.paint, points, count));
    }
}

lx_void_t lx_gl_renderer_draw_polygon(lx_opengl_device_t* device, lx_polygon_ref_t polygon, lx_shape_ref_t hint, lx_rect_ref_t bounds) {
    lx_assert(device && device->base.paint && polygon && polygon->points && polygon->counts);

    if (hint && hint->type == LX_SHAPE_TYPE_LINE) {
        lx_point_t points[2];
        points[0] = hint->u.line.p0;
        points[1] = hint->u.line.p1;
        lx_gl_renderer_draw_lines(device, points, 2, bounds);
        return ;
    } else if (hint && hint->type == LX_SHAPE_TYPE_POINT) {
        lx_gl_renderer_draw_points(device, &hint->u.point, 1, bounds);
        return ;
    }

    lx_gl_renderer_apply_paint(device, bounds);
    lx_size_t mode = lx_paint_mode(device->base.paint);
    if (mode & LX_PAINT_MODE_FILL) {
        lx_gl_renderer_fill_polygon(device, polygon, bounds, lx_paint_fill_rule(device->base.paint));
    }

    if ((mode & LX_PAINT_MODE_STROKE) && (lx_paint_stroke_width(device->base.paint) > 0)) {
        if (lx_gl_renderer_stroke_only(device)) {
            lx_gl_renderer_stroke_polygon(device, polygon);
        } else {
            lx_gl_renderer_stroke_fill(device, lx_stroker_make_from_polygon(device->stroker, device->base.paint, polygon, hint));
        }
    }
}

