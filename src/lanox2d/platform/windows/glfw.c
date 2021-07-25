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
 * @file        glfw.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the glfw window type
typedef struct lx_window_glfw_t_ {
    lx_window_t     base;
    GLFWwindow*     window;
    lx_bool_t       is_quit;
    lx_hong_t       fps_time;
    lx_hong_t       fps_count;
    lx_int_t        fps_delay;
} lx_window_glfw_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

static lx_void_t lx_window_glfw_fullscreen(lx_window_ref_t self, lx_bool_t is_fullscreen) {
    lx_window_glfw_t* window = (lx_window_glfw_t*)self;
    if (window) {
        // TODO
    }
}

static lx_bool_t lx_window_glfw_start(lx_window_glfw_t* window) {
    lx_bool_t ok = lx_false;
    do {

        // init glfw
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, (window->base.flags & LX_WINDOW_FLAG_NOT_REISZE)? lx_false : lx_true);
        if (window->base.flags & LX_WINDOW_FLAG_HIDE_TITLEBAR) {
            glfwWindowHint(GLFW_DECORATED, lx_false);
        }
#ifdef LX_CONFIG_OS_MACOSX
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, lx_true);
#endif

        // init window
        window->window = glfwCreateWindow(window->base.width, window->base.height,
            window->base.title? window->base.title : "lanox2d (GLFW)", lx_null, lx_null);
        lx_assert_and_check_break(window->window);
        glfwMakeContextCurrent(window->window);

        // hide window
        if (window->base.flags & LX_WINDOW_FLAG_HIDDEN) {
            glfwHideWindow(window->window);
        }

        // hide window cursor
        if (window->base.flags & LX_WINDOW_FLAG_HIDE_CURSOR) {
            glfwSetInputMode(window->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        }

        // fullscreen
        if (window->base.flags & LX_WINDOW_FLAG_FULLSCREEN) {
            lx_window_glfw_fullscreen((lx_window_ref_t)window, lx_true);
        }

        // init glew
        glewExperimental = lx_true;
        if (glewInit() != GLEW_OK) {
            lx_trace_e("init glew failed!");
            break;
        }

#if 0
        // init device
#if defined(LX_CONFIG_DEVICE_HAVE_OPENGL)
        window->base.device = lx_device_init_from_opengl((lx_window_ref_t)window);
#elif defined(LX_CONFIG_DEVICE_HAVE_SKIA)
        window->base.device = lx_device_init_from_skia((lx_window_ref_t)window, lx_null);
#endif
        lx_assert_and_check_break(window->base.device);

        // init canvas
        window->base.canvas = lx_canvas_init(window->base.device);
        lx_assert_and_check_break(window->base.canvas);
#endif

        // ok
        ok = lx_true;
    } while (0);

    // failed? exit glfw
    if (!ok) {
        glfwTerminate();
    }
    return ok;
}

static lx_void_t lx_window_glfw_runloop(lx_window_ref_t self) {
    lx_window_glfw_t* window = (lx_window_glfw_t*)self;
    lx_assert_and_check_return(window);

    // start glfw window
    if (!lx_window_glfw_start(window)) {
        lx_trace_e("start glfw window failed!");
        return ;
    }

    // do loop
    while (!window->is_quit && !glfwWindowShouldClose(window->window)) {
        glfwPollEvents();
        glfwSwapBuffers(window->window);
    }

    // exit glfw
    glfwTerminate();
}

static lx_void_t lx_window_glfw_show(lx_window_ref_t self, lx_bool_t is_show) {
    lx_window_glfw_t* window = (lx_window_glfw_t*)self;
    if (window && window->window) {
        if (is_show) {
            glfwShowWindow(window->window);
        } else {
            glfwHideWindow(window->window);
        }
    }
}

static lx_void_t lx_window_glfw_show_cursor(lx_window_ref_t self, lx_bool_t is_show) {
    lx_window_glfw_t* window = (lx_window_glfw_t*)self;
    if (window && window->window) {
        glfwSetInputMode(window->window, GLFW_CURSOR, is_show? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
    }
}

static lx_void_t lx_window_glfw_quit(lx_window_ref_t self) {
    lx_window_glfw_t* window = (lx_window_glfw_t*)self;
    if (window) {
        window->is_quit = lx_true;
    }
}

static lx_void_t lx_window_glfw_exit(lx_window_ref_t self) {
    lx_window_glfw_t* window = (lx_window_glfw_t*)self;
    if (window) {
        if (window->base.canvas) {
            lx_canvas_exit(window->base.canvas);
            window->base.canvas = lx_null;
        }
        if (window->base.device) {
            lx_device_exit(window->base.device);
            window->base.device = lx_null;
        }
        lx_free(window);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_window_ref_t lx_window_init_glfw(lx_size_t width, lx_size_t height, lx_char_t const* title) {
    lx_bool_t         ok = lx_false;
    lx_window_glfw_t* window = lx_null;
    do {

        // check
        lx_assert_and_check_break(width && height);

        // init window
        window = lx_malloc0_type(lx_window_glfw_t);
        lx_assert_and_check_break(window);

        window->base.fps         = 60;
        window->base.width       = (lx_uint16_t)width;
        window->base.height      = (lx_uint16_t)height;
        window->base.title       = title;
        window->base.runloop     = lx_window_glfw_runloop;
        window->base.quit        = lx_window_glfw_quit;
        window->base.fullscreen  = lx_window_glfw_fullscreen;
        window->base.show        = lx_window_glfw_show;
        window->base.show_cursor = lx_window_glfw_show_cursor;
        window->base.exit        = lx_window_glfw_exit;
        window->fps_delay        = 1000 / window->base.fps;

        // init pixfmt
        window->base.pixfmt = lx_quality() < LX_QUALITY_TOP? LX_PIXFMT_RGB565 : LX_PIXFMT_XRGB8888;

        // ok
        ok = lx_true;

    } while (0);

    if (!ok && window) {
        lx_window_exit((lx_window_ref_t)window);
        window = lx_null;
    }
    return (lx_window_ref_t)window;
}
