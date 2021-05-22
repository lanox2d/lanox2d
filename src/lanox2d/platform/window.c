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
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_window_ref_t lx_window_init(lx_size_t width, lx_size_t height, lx_char_t const* title) {
#if defined(LX_CONFIG_WINDOW_HAVE_SDL)
    return lx_window_init_sdl(width, height, title);
#else
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

lx_uint16_t lx_window_pixfmt(lx_window_ref_t self) {
    lx_window_t* window = (lx_window_t*)self;
    return window? window->pixfmt : 0;
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

lx_void_t lx_window_on_draw(lx_window_ref_t self, lx_window_on_draw_t on_draw) {
    lx_window_t* window = (lx_window_t*)self;
    if (window) {
        window->on_draw = on_draw;
    }
}

lx_void_t lx_window_on_resize(lx_window_ref_t self, lx_window_on_resize_t on_resize) {
    lx_window_t* window = (lx_window_t*)self;
    if (window) {
        window->on_resize = on_resize;
    }
}

lx_void_t lx_window_runloop(lx_window_ref_t self) {
    lx_window_t* window = (lx_window_t*)self;
    lx_assert_and_check_return(window && window->runloop);
    window->runloop(self);
}

lx_void_t lx_window_exit(lx_window_ref_t self) {
    lx_window_t* window = (lx_window_t*)self;
    if (window) {
        window->exit(self);
    }
}
