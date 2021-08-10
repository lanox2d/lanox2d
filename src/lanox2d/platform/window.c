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
 * @file        window.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "window.h"
#if defined(LX_CONFIG_WINDOW_HAVE_SDL)
#   include "windows/sdl.c"
#elif defined(LX_CONFIG_WINDOW_HAVE_GLUT)
#   include "windows/glut.c"
#elif defined(LX_CONFIG_WINDOW_HAVE_GLFW)
#   include "windows/glfw.c"
#elif defined(LX_CONFIG_WINDOW_HAVE_FBDEV)
#   include "windows/fbdev.c"
#elif defined(LX_CONFIG_WINDOW_HAVE_ANDROID)
#   include "windows/android.c"
#elif defined(LX_CONFIG_WINDOW_HAVE_IOS)
#   include "windows/ios.c"
#else
#   include "windows/prefix.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_window_ref_t lx_window_init(lx_size_t width, lx_size_t height, lx_char_t const* title) {
#if defined(LX_CONFIG_WINDOW_HAVE_SDL)
    return lx_window_init_sdl(width, height, title);
#elif defined(LX_CONFIG_WINDOW_HAVE_GLUT)
    return lx_window_init_glut(width, height, title);
#elif defined(LX_CONFIG_WINDOW_HAVE_GLFW)
    return lx_window_init_glfw(width, height, title);
#elif defined(LX_CONFIG_WINDOW_HAVE_FBDEV)
    return lx_window_init_fbdev(width, height, title);
#elif defined(LX_CONFIG_WINDOW_HAVE_ANDROID)
    return lx_window_init_android(width, height, title);
#elif defined(LX_CONFIG_WINDOW_HAVE_IOS)
    return lx_window_init_ios(width, height, title);
#else
    lx_trace_e("window not found!");
    return lx_null;
#endif
}

lx_size_t lx_window_width(lx_window_ref_t self) {
    lx_window_t* window = (lx_window_t*)self;
    return window? window->width : 0;
}

lx_size_t lx_window_height(lx_window_ref_t self) {
    lx_window_t* window = (lx_window_t*)self;
    return window? window->height : 0;
}

lx_size_t lx_window_pixfmt(lx_window_ref_t self) {
    lx_window_t* window = (lx_window_t*)self;
    return window? window->pixfmt : LX_PIXFMT_NONE;
}

lx_bool_t lx_window_is_fullscreen(lx_window_ref_t self) {
    lx_window_t* window = (lx_window_t*)self;
    return window? window->flags & LX_WINDOW_FLAG_FULLSCREEN : lx_false;
}

lx_cpointer_t lx_window_udata(lx_window_ref_t self) {
    lx_window_t* window = (lx_window_t*)self;
    return window? window->udata : lx_null;
}

lx_void_t lx_window_udata_set(lx_window_ref_t self, lx_cpointer_t udata) {
    lx_window_t* window = (lx_window_t*)self;
    if (window) {
        window->udata = udata;
    }
}

lx_void_t lx_window_flags_set(lx_window_ref_t self, lx_uint16_t flags) {
    lx_window_t* window = (lx_window_t*)self;
    if (window) {
        window->flags = flags;
    }
}

lx_void_t lx_window_fps_set(lx_window_ref_t self, lx_uint16_t fps) {
    lx_window_t* window = (lx_window_t*)self;
    if (window) {
        window->fps = fps;
    }
}

lx_void_t lx_window_on_draw(lx_window_ref_t self, lx_window_on_draw_t on_draw) {
    lx_window_t* window = (lx_window_t*)self;
    if (window) {
        window->on_draw = on_draw;
    }
}

lx_void_t lx_window_on_event(lx_window_ref_t self, lx_window_on_event_t on_event) {
    lx_window_t* window = (lx_window_t*)self;
    if (window) {
        window->on_event = on_event;
    }
}

lx_void_t lx_window_runloop(lx_window_ref_t self) {
    lx_window_t* window = (lx_window_t*)self;
    lx_assert_and_check_return(window && window->runloop);
    window->runloop(self);
}

lx_void_t lx_window_draw(lx_window_ref_t self) {
    lx_window_t* window = (lx_window_t*)self;
    if (window && window->draw) {
        window->draw(self);
    }
}

lx_void_t lx_window_resize(lx_window_ref_t self, lx_size_t width, lx_size_t height) {
    lx_window_t* window = (lx_window_t*)self;
    if (window && window->resize) {
        window->resize(self, width, height);
    }
}

lx_void_t lx_window_notify(lx_window_ref_t self, lx_event_ref_t event) {
    lx_window_t* window = (lx_window_t*)self;
    if (window && window->on_event && event) {
        window->on_event(self, event);
    }
}

lx_void_t lx_window_fullscreen(lx_window_ref_t self, lx_bool_t is_fullscreen) {
    lx_window_t* window = (lx_window_t*)self;
    if (window && window->fullscreen) {
        window->fullscreen(self, is_fullscreen);
    }
}

lx_void_t lx_window_show(lx_window_ref_t self, lx_bool_t is_show) {
    lx_window_t* window = (lx_window_t*)self;
    if (window && window->show) {
        window->show(self, is_show);
    }
}

lx_void_t lx_window_show_cursor(lx_window_ref_t self, lx_bool_t is_show) {
    lx_window_t* window = (lx_window_t*)self;
    if (window && window->show_cursor) {
        window->show_cursor(self, is_show);
    }
}

lx_void_t lx_window_quit(lx_window_ref_t self) {
    lx_window_t* window = (lx_window_t*)self;
    if (window && window->quit) {
        window->quit(self);
    }
}

lx_void_t lx_window_exit(lx_window_ref_t self) {
    lx_window_t* window = (lx_window_t*)self;
    if (window) {
        window->exit(self);
    }
}
