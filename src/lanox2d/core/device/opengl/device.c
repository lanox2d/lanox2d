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
 * @file        device.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "device.h"
#include "renderer.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

static lx_void_t lx_device_opengl_draw_clear(lx_device_ref_t self, lx_color_t color) {
    lx_glClearColor((lx_GLfloat_t)color.r / 0xff, (lx_GLfloat_t)color.g / 0xff, (lx_GLfloat_t)color.b / 0xff, (lx_GLfloat_t)color.a / 0xff);
    lx_glClear(LX_GL_COLOR_BUFFER_BIT);
}

static lx_void_t lx_device_opengl_draw_lines(lx_device_ref_t self, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
    lx_opengl_device_t* device = (lx_opengl_device_t*)self;
    lx_assert(device && points && count);

    if (lx_gl_renderer_init(device)) {
        lx_gl_renderer_draw_lines(device, points, count, bounds);
        lx_gl_renderer_exit(device);
    }
}

static lx_void_t lx_device_opengl_draw_points(lx_device_ref_t self, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
    lx_opengl_device_t* device = (lx_opengl_device_t*)self;
    lx_assert(device && points && count);

    if (lx_gl_renderer_init(device)) {
        lx_gl_renderer_draw_points(device, points, count, bounds);
        lx_gl_renderer_exit(device);
    }
}

static lx_void_t lx_device_opengl_draw_polygon(lx_device_ref_t self, lx_polygon_ref_t polygon, lx_shape_ref_t hint, lx_rect_ref_t bounds) {
    lx_opengl_device_t* device = (lx_opengl_device_t*)self;
    lx_assert(device && polygon);

    if (lx_gl_renderer_init(device)) {
        lx_gl_renderer_draw_polygon(device, polygon, hint, bounds);
        lx_gl_renderer_exit(device);
    }
}

static lx_void_t lx_device_opengl_draw_path(lx_device_ref_t self, lx_path_ref_t path) {
    lx_opengl_device_t* device = (lx_opengl_device_t*)self;
    lx_assert(device && path);

    if (lx_gl_renderer_init(device)) {
        lx_gl_renderer_draw_path(device, path);
        lx_gl_renderer_exit(device);
    }
}

static lx_void_t lx_device_opengl_exit(lx_device_ref_t self) {
    lx_opengl_device_t* device = (lx_opengl_device_t*)self;
    if (device) {
        if (device->tessellator) {
            lx_tessellator_exit(device->tessellator);
            device->tessellator = lx_null;
        }
        if (device->stroker) {
            lx_stroker_exit(device->stroker);
            device->stroker = lx_null;
        }
        lx_size_t i = 0;
        for (i = 0; i < LX_GL_PROGRAM_TYPE_MAXN; i++) {
            if (device->programs[i]) {
                lx_gl_program_exit(device->programs[i]);
                device->programs[i] = 0;
            }
        }
        if (device->texcoord_buffer) {
            lx_gl_vertex_buffer_exit(device->texcoord_buffer);
            device->texcoord_buffer = 0;
        }
        if (device->vertex_buffer) {
            lx_gl_vertex_buffer_exit(device->vertex_buffer);
            device->vertex_buffer = 0;
        }
        if (device->vertex_array) {
            lx_gl_vertex_array_exit(device->vertex_array);
            device->vertex_array = 0;
        }
        lx_free(device);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_device_ref_t lx_device_init_from_opengl(lx_size_t width, lx_size_t height, lx_size_t framewidth, lx_size_t frameheight) {
    lx_assert_and_check_return_val(width && height, lx_null);

    lx_bool_t           ok = lx_false;
    lx_opengl_device_t* device = lx_null;
    do {

        // init gl context first
        if (!lx_gl_context_init(width, height, framewidth, frameheight)) {
            break;
        }

        // init device
        device = lx_malloc0_type(lx_opengl_device_t);
        lx_assert_and_check_break(device);

        device->base.draw_clear            = lx_device_opengl_draw_clear;
        device->base.draw_lines            = lx_device_opengl_draw_lines;
        device->base.draw_points           = lx_device_opengl_draw_points;
        device->base.draw_polygon          = lx_device_opengl_draw_polygon;
        device->base.draw_path             = lx_device_opengl_draw_path;
        device->base.exit                  = lx_device_opengl_exit;

        // init stroker
        device->stroker = lx_stroker_init();
        lx_assert_and_check_break(device->stroker);

        // init tessellator
        device->tessellator = lx_tessellator_init();
        lx_assert_and_check_break(device->tessellator);

        // init tessellator mode
        lx_tessellator_mode_set(device->tessellator, LX_TESSELLATOR_MODE_CONVEX);

#if LX_GL_API_VERSION >= 20
        // init solid program
        device->programs[LX_GL_PROGRAM_TYPE_SOLID] = lx_gl_program_init_solid();
        lx_assert_and_check_break(device->programs[LX_GL_PROGRAM_TYPE_SOLID]);

        // init texture program
        device->programs[LX_GL_PROGRAM_TYPE_TEXTURE] = lx_gl_program_init_texture();
        lx_assert_and_check_break(device->programs[LX_GL_PROGRAM_TYPE_TEXTURE]);
#endif

        // init vertex array
        device->vertex_array = lx_gl_vertex_array_init();

        // init vertex buffer
        device->vertex_buffer = lx_gl_vertex_buffer_init();

        // init texcoord buffer
        device->texcoord_buffer = lx_gl_vertex_buffer_init();

        // ok
        ok = lx_true;

    } while (0);

    // failed?
    if (!ok && device) {
        lx_device_exit((lx_device_ref_t)device);
        device = lx_null;
    }
    return (lx_device_ref_t)device;
}


