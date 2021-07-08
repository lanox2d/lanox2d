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
#include "../../tess/tess.h"
#include "../../shader.h"
#include "../../private/shader.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// test tessellator
//#define LX_GL_TESSELLATOR_TEST_ENABLE

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

static lx_void_t lx_gl_renderer_enable_antialiasing(lx_opengl_device_t* device, lx_bool_t enabled) {
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

static lx_void_t lx_gl_renderer_enable_blend(lx_opengl_device_t* device, lx_bool_t enabled) {
    if (enabled) {
        lx_glEnable(LX_GL_BLEND);
        lx_glBlendFunc(LX_GL_SRC_ALPHA, LX_GL_ONE_MINUS_SRC_ALPHA);
    } else {
        lx_glDisable(LX_GL_BLEND);
    }
}

static lx_void_t lx_gl_renderer_enable_vertices(lx_opengl_device_t* device, lx_bool_t enabled) {
    if (enabled) {
        if (device->glversion >= 0x20) {
            lx_assert(device->program);

            // enable vertex
            lx_glEnableVertexAttribArray(lx_gl_program_location(device->program, LX_GL_PROGRAM_LOCATION_VERTICES));

            // apply projection matrix
            lx_glUniformMatrix4fv(lx_gl_program_location(device->program, LX_GL_PROGRAM_LOCATION_MATRIX_PROJECT), 1, LX_GL_FALSE, device->matrix_project);

            // apply vertex matrix
            lx_glUniformMatrix4fv(lx_gl_program_location(device->program, LX_GL_PROGRAM_LOCATION_MATRIX_MODEL), 1, LX_GL_FALSE, device->matrix_vertex);

        } else {
            // enable vertex
            lx_glEnableClientState(LX_GL_VERTEX_ARRAY);

            // apply vertex matrix
            lx_glMatrixMode(LX_GL_MODELVIEW);
            lx_glPushMatrix();
            lx_glLoadIdentity();
            lx_glMultMatrixf(device->matrix_vertex);
        }
    } else {
        if (device->glversion >= 0x20) {
            lx_assert_and_check_return(device->program);
            lx_glDisableVertexAttribArray(lx_gl_program_location(device->program, LX_GL_PROGRAM_LOCATION_VERTICES));
            lx_glDisableVertexAttribArray(lx_gl_program_location(device->program, LX_GL_PROGRAM_LOCATION_TEXCOORDS));
        } else {
            lx_glMatrixMode(LX_GL_MODELVIEW);
            lx_glPopMatrix();
            lx_glDisableClientState(LX_GL_VERTEX_ARRAY);
            lx_glDisableClientState(LX_GL_TEXTURE_COORD_ARRAY);
        }
    }
}

static lx_void_t lx_gl_renderer_apply_texture(lx_opengl_device_t* device) {
    lx_glEnable(LX_GL_TEXTURE_2D);
    lx_glBindTexture(LX_GL_TEXTURE_2D, device->texture);
    if (device->glversion >= 0x20) {
        lx_glEnableVertexAttribArray(lx_gl_program_location(device->program, LX_GL_PROGRAM_LOCATION_TEXCOORDS));
    } else {
        lx_glEnableClientState(LX_GL_TEXTURE_COORD_ARRAY);
        lx_glTexEnvi(LX_GL_TEXTURE_ENV, LX_GL_TEXTURE_ENV_MODE, LX_GL_MODULATE);
    }
}

static lx_void_t lx_gl_renderer_apply_vertices(lx_opengl_device_t* device, lx_point_ref_t points) {
    lx_assert(device && points);

    // apply vertices
    if (device->glversion >= 0x20) {
        lx_assert(device->program);
        lx_glVertexAttribPointer(lx_gl_program_location(device->program, LX_GL_PROGRAM_LOCATION_VERTICES), 2, LX_GL_FLOAT, LX_GL_FALSE, 0, points);
    } else {
        lx_glVertexPointer(2, LX_GL_FLOAT, 0, points);
    }
}

static lx_void_t lx_gl_renderer_apply_color(lx_opengl_device_t* device, lx_color_t color) {
    if (device->glversion >= 0x20) {
        lx_assert(device->program);
        lx_glVertexAttrib4f(lx_gl_program_location(device->program, LX_GL_PROGRAM_LOCATION_COLORS), (lx_float_t)color.r / 0xff, (lx_float_t)color.g / 0xff, (lx_float_t)color.b / 0xff, (lx_float_t)color.a / 0xff);
    } else {
        lx_glColor4f((lx_float_t)color.r / 0xff, (lx_float_t)color.g / 0xff, (lx_float_t)color.b / 0xff, (lx_float_t)color.a / 0xff);
    }
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

static lx_void_t lx_gl_renderer_apply_shader_bitmap(lx_opengl_device_t* device, lx_shader_ref_t shader) {

    // get bitmap
    lx_bitmap_ref_t bitmap = ((lx_bitmap_shader_t*)shader)->bitmap;
    lx_assert(bitmap);

    // get paint
    lx_paint_ref_t paint = device->base.paint;
    lx_assert(paint);

    // apply texture
    lx_gl_renderer_apply_texture(device);

    // enable blend
    lx_byte_t alpha = lx_paint_alpha(paint);
    lx_gl_renderer_enable_blend(device, alpha != 0xff || lx_shader_tile_mode(shader) == LX_SHADER_TILE_MODE_BORDER || lx_bitmap_has_alpha(bitmap));

    // apply color
    lx_gl_renderer_apply_color(device, lx_color_make(alpha, 0xff, 0xff, 0xff));

    // apply wrap and filter
    // TODO

    // apply texture matrix
    // TODO

    // apply texcoords & vertices
    // TODO
}

static lx_void_t lx_gl_renderer_apply_shader(lx_opengl_device_t* device, lx_shader_ref_t shader) {
    lx_size_t shader_type = lx_shader_type(shader);
    switch (shader_type) {
    case LX_SHADER_TYPE_BITMAP:
        lx_gl_renderer_apply_shader_bitmap(device, shader);
        break;
    default:
        lx_trace_e("not supported shader type!");
        break;
    }
}

static lx_void_t lx_gl_renderer_apply_paint(lx_opengl_device_t* device) {
    lx_assert(device);

    if (device->shader) {
        lx_gl_renderer_apply_shader(device, device->shader);
    } else {
        lx_gl_renderer_apply_solid(device);
    }
}

static lx_void_t lx_gl_renderer_fill_convex(lx_point_ref_t points, lx_uint16_t count, lx_cpointer_t udata) {
    lx_assert(udata && points && count);

    // apply it
    lx_gl_renderer_apply_vertices((lx_opengl_device_t*)udata, points);

#ifndef LX_GL_TESSELLATOR_TEST_ENABLE
    // draw it
    lx_glDrawArrays(LX_GL_TRIANGLE_FAN, 0, (lx_GLint_t)count);
#else
    // the device
    lx_opengl_device_t* device = (lx_opengl_device_t*)udata;

    // make crc32
    lx_uint32_t crc32 = 0xffffffff ^ lx_crc_encode(LX_CRC_MODE_32_IEEE_LE, 0xffffffff, (lx_byte_t const*)points, count * sizeof(lx_point_t));

    // enable blend
    lx_gl_renderer_enable_blend(device, lx_true);

    // apply color
    lx_color_t color;
    color.r = (lx_byte_t)crc32;
    color.g = (lx_byte_t)(crc32 >> 8);
    color.b = (lx_byte_t)(crc32 >> 16);
    color.a = 128;
    lx_gl_renderer_apply_color(device, color);

    // draw the edges of the filled contour
    lx_glDrawArrays(LX_GL_TRIANGLE_FAN, 0, (lx_GLint_t)count);
#endif
}

static lx_void_t lx_gl_renderer_fill_polygon(lx_opengl_device_t* device, lx_polygon_ref_t polygon, lx_rect_ref_t bounds, lx_size_t rule) {
    lx_assert(device && device->tessellator);

#ifdef LX_GL_TESSELLATOR_TEST_ENABLE
    lx_tessellator_mode_set(device->tessellator, LX_TESSELLATOR_MODE_TRIANGULATION);
//    lx_tessellator_mode_set(device->tessellator, LX_TESSELLATOR_MODE_MONOTONE);
#endif

    lx_tessellator_rule_set(device->tessellator, rule);
    lx_tessellator_callback_set(device->tessellator, lx_gl_renderer_fill_convex, device);
    lx_tessellator_make(device->tessellator, polygon, bounds);
}

static lx_void_t lx_gl_renderer_stroke_lines(lx_opengl_device_t* device, lx_point_ref_t points, lx_size_t count) {
    lx_assert(device && points && count);

    lx_gl_renderer_apply_vertices(device, points);
    lx_glDrawArrays(LX_GL_LINES, 0, (lx_GLint_t)count);
}

static lx_void_t lx_gl_renderer_stroke_points(lx_opengl_device_t* device, lx_point_ref_t points, lx_size_t count) {
    lx_assert(device && points && count);

    lx_gl_renderer_apply_vertices(device, points);
    lx_glDrawArrays(LX_GL_POINTS, 0, (lx_GLint_t)count);
}

static lx_void_t lx_gl_renderer_stroke_polygon(lx_opengl_device_t* device, lx_point_ref_t points, lx_uint16_t const* counts) {
    lx_assert(device && points && counts);

    // apply vertices
    lx_gl_renderer_apply_vertices(device, points);

    lx_uint16_t count;
    lx_size_t   index = 0;
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
            &&  !device->shader)? lx_true : lx_false;
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

        // init vertex matrix
        lx_gl_matrix_convert(device->matrix_vertex, device->base.matrix);

        // init programs
        if (device->glversion >= 0x20) {

            // get program
            lx_size_t program_type = device->shader? LX_GL_PROGRAM_TYPE_TEXTURE : LX_GL_PROGRAM_TYPE_SOLID;
            device->program = device->programs[program_type];
            lx_assert_and_check_break(device->program);

            // bind this program to the current gl context
            lx_gl_program_bind(device->program);
        }

        // init antialiasing
        lx_gl_renderer_enable_antialiasing(device, lx_paint_flags(device->base.paint) & LX_PAINT_FLAG_ANTIALIASING);

        // init vertex and matrix
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

    lx_gl_renderer_apply_paint(device);
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

    lx_gl_renderer_apply_paint(device);
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

    lx_gl_renderer_apply_paint(device);
    lx_size_t mode = lx_paint_mode(device->base.paint);
    if (mode & LX_PAINT_MODE_FILL) {
        lx_gl_renderer_fill_polygon(device, polygon, bounds, lx_paint_fill_rule(device->base.paint));
    }

    if ((mode & LX_PAINT_MODE_STROKE) && (lx_paint_stroke_width(device->base.paint) > 0)) {
        if (lx_gl_renderer_stroke_only(device)) {
            lx_gl_renderer_stroke_polygon(device, polygon->points, polygon->counts);
        } else {
            lx_gl_renderer_stroke_fill(device, lx_stroker_make_from_polygon(device->stroker, device->base.paint, polygon, hint));
        }
    }
}

