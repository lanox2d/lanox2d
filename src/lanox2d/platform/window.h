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
 * @file        window.h
 *
 */
#ifndef LX_PLATFORM_WINDOW_H
#define LX_PLATFORM_WINDOW_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the window ref type
typedef lx_typeref(window);

/// the window flag enum
typedef enum lx_window_flag_e_ {
    LX_WINDOW_FLAG_NONE             = 0
,   LX_WINDOW_FLAG_FULLSCREEN       = 1
,   LX_WINDOW_FLAG_HIHE_TITLEBAR    = 2
,   LX_WINDOW_FLAG_HIHE_CURSOR      = 4
,   LX_WINDOW_FLAG_NOT_REISZE       = 8
}lx_window_flag_e;

/*! the on_draw function type
 *
 * @param window            the window
 * @param canvas            the canvas
 */
typedef lx_void_t           (*lx_window_on_draw_t)(lx_window_ref_t window, lx_canvas_ref_t canvas);

/*! the on_resize function type
 *
 * @param window            the window
 * @param canvas            the canvas
 */
typedef lx_void_t           (*lx_window_on_resize_t)(lx_window_ref_t window, lx_canvas_ref_t canvas);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init window
 *
 * @param width         the window width
 * @param height        the window height
 * @param title         the window title
 *
 * @return              the window
 */
lx_window_ref_t         lx_window_init(lx_size_t width, lx_size_t height, lx_char_t const* title);

/*! get window width
 *
 * @param window        the window
 *
 * @return              the window width
 */
lx_size_t               lx_window_width(lx_window_ref_t window);

/*! get window height
 *
 * @param window        the window
 *
 * @return              the window height
 */
lx_size_t               lx_window_height(lx_window_ref_t window);

/*! get window user data
 *
 * @param window        the window
 *
 * @return              the user data
 */
lx_cpointer_t           lx_window_udata(lx_window_ref_t window);

/*! set window user data
 *
 * @param window        the window
 * @param udata         the user data
 */
lx_void_t               lx_window_udata_set(lx_window_ref_t window, lx_cpointer_t udata);

/*! set window flags
 *
 * @param window        the window
 * @param flags         the window flags
 */
lx_void_t               lx_window_flags_set(lx_window_ref_t window, lx_uint16_t flags);

/*! register on_draw function
 *
 * @param window        the window
 * @param on_draw       the on_draw function
 *
 */
lx_void_t               lx_window_on_draw(lx_window_ref_t window, lx_window_on_draw_t on_draw);

/*! register on_resize function
 *
 * @param window        the window
 * @param on_resize     the on_resize function
 *
 */
lx_void_t               lx_window_on_resize(lx_window_ref_t window, lx_window_on_resize_t on_resize);

/*! run window loop
 *
 * @param window        the window
 */
lx_void_t               lx_window_runloop(lx_window_ref_t window);

/*! exit window
 *
 * @param window        the window
 */
lx_void_t               lx_window_exit(lx_window_ref_t window);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


