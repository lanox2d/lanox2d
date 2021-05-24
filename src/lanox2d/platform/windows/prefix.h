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
 * @file        prefix.h
 *
 */
#ifndef LX_PLATFORM_WINDOWS_PREFIX_H
#define LX_PLATFORM_WINDOWS_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../window.h"
#include "../../core/core.h"
#include "../../base/base.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the window type
typedef struct lx_window_t_ {

    // the basic information
    lx_uint16_t         fps;
    lx_uint16_t         flags;
    lx_uint16_t         width;
    lx_uint16_t         height;
    lx_uint16_t         pixfmt;
    lx_char_t const*    title;
    lx_cpointer_t       udata;

    // the device
    lx_device_ref_t     device;

    // the canvas
    lx_canvas_ref_t     canvas;

    // notification functions
    lx_void_t           (*on_draw)(lx_window_ref_t window, lx_canvas_ref_t canvas);
    lx_void_t           (*on_event)(lx_window_ref_t window, lx_event_ref_t event);
    lx_void_t           (*on_resize)(lx_window_ref_t window, lx_canvas_ref_t canvas);

    // inner functions
    lx_void_t           (*runloop)(lx_window_ref_t window);
    lx_void_t           (*fullscreen)(lx_window_ref_t window, lx_bool_t is_fullscreen);
    lx_void_t           (*show)(lx_window_ref_t window, lx_bool_t is_show);
    lx_void_t           (*show_cursor)(lx_window_ref_t window, lx_bool_t is_show);
    lx_void_t           (*quit)(lx_window_ref_t window);
    lx_void_t           (*exit)(lx_window_ref_t window);

}lx_window_t;

#endif


