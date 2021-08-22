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
 * @file        mach.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the mach window type
typedef struct lx_window_mach_t_ {
    lx_window_t     base;
} lx_window_mach_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

static lx_void_t lx_window_mach_draw(lx_window_ref_t self) {
    lx_window_mach_t* window = (lx_window_mach_t*)self;
    lx_assert(window && window->base.on_draw);

    // do draw
    window->base.on_draw((lx_window_ref_t)window, window->base.canvas);
}

static lx_void_t lx_window_mach_resize(lx_window_ref_t self, lx_size_t width, lx_size_t height) {
    lx_window_mach_t* window = (lx_window_mach_t*)self;
    lx_check_return(width && height);

    lx_event_t event = {0};
    event.type = LX_EVENT_TYPE_ACTIVE;
    event.u.active.code = LX_ACTIVE_RESIZE_WINDOW;
    event.u.active.data[0] = (lx_size_t)width;
    event.u.active.data[1] = (lx_size_t)height;
    if (window->base.on_event) {
        window->base.on_event((lx_window_ref_t)window, &event);
    }
}

static lx_void_t lx_window_mach_exit(lx_window_ref_t self) {
    lx_window_mach_t* window = (lx_window_mach_t*)self;
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
lx_window_ref_t lx_window_init_mach(lx_size_t width, lx_size_t height, lx_char_t const* title, lx_cpointer_t devdata) {
    lx_bool_t         ok = lx_false;
    lx_window_mach_t* window = lx_null;
    do {

        // check
        lx_assert_and_check_break(width && height);

        // init window
        window = lx_malloc0_type(lx_window_mach_t);
        lx_assert_and_check_break(window);

        window->base.width       = (lx_uint16_t)width;
        window->base.height      = (lx_uint16_t)height;
        window->base.title       = title;
        window->base.draw        = lx_window_mach_draw;
        window->base.resize      = lx_window_mach_resize;
        window->base.exit        = lx_window_mach_exit;
        window->base.pixfmt      = LX_PIXFMT_XRGB8888;

        // init device
#if defined(LX_CONFIG_DEVICE_HAVE_OPENGL)
        window->base.device = lx_device_init_from_opengl(width, height, width, height);
#elif defined(LX_CONFIG_DEVICE_HAVE_METAL)
        window->base.device = lx_device_init_from_metal(width, height, width, height);
#endif
        lx_assert_and_check_break(window->base.device);

        // init canvas
        window->base.canvas = lx_canvas_init(window->base.device);
        lx_assert_and_check_break(window->base.canvas);

        // ok
        ok = lx_true;

    } while (0);

    if (!ok && window) {
        lx_window_exit((lx_window_ref_t)window);
        window = lx_null;
    }
    return (lx_window_ref_t)window;
}
