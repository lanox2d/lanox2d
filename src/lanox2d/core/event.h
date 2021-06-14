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
 * @file        event.h
 *
 */
#ifndef LX_CORE_EVENT_H
#define LX_CORE_EVENT_H

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

/// the event type enum
typedef enum lx_event_type_e_ {
    LX_EVENT_TYPE_NONE      = 0     //!< none
,   LX_EVENT_TYPE_ACTIVE    = 1     //!< active event
,   LX_EVENT_TYPE_TOUCH     = 2     //!< touch event
,   LX_EVENT_TYPE_MOUSE     = 3     //!< mouse event
,   LX_EVENT_TYPE_KEYBOARD  = 4     //!< keyboard event
,   LX_EVENT_TYPE_USER      = 5     //!< user defined event
}lx_event_type_e;

/// the keycode enum
typedef enum lx_key_e_ {

    /// ascii mapped keys
    LX_KEY_NUL              = 0x00
,   LX_KEY_BACKSPACE        = 0x08
,   LX_KEY_TAB              = 0x09
,   LX_KEY_RETURN           = 0x0d
,   LX_KEY_ESCAPE           = 0x1b
,   LX_KEY_SPACE            = 0x20
,   LX_KEY_DELETE           = 0x7f

    /// function keys
,   LX_KEY_F1               = 0x0100
,   LX_KEY_F2
,   LX_KEY_F3
,   LX_KEY_F4
,   LX_KEY_F5
,   LX_KEY_F6
,   LX_KEY_F7
,   LX_KEY_F8
,   LX_KEY_F9
,   LX_KEY_F10
,   LX_KEY_F11
,   LX_KEY_F12

    /// arrows keys
,   LX_KEY_LEFT
,   LX_KEY_UP
,   LX_KEY_RIGHT
,   LX_KEY_DOWN

    /// home/end keys
,   LX_KEY_HOME
,   LX_KEY_END
,   LX_KEY_INSERT
,   LX_KEY_PAGEUP
,   LX_KEY_PAGEDOWN

    /// miscellaneous function keys
,   LX_KEY_HELP
,   LX_KEY_PRINT
,   LX_KEY_SYSREQ
,   LX_KEY_BREAK
,   LX_KEY_MENU
,   LX_KEY_POWER                //!< power macintosh power key
,   LX_KEY_EURO                 //!< some european keyboards
,   LX_KEY_UNDO                 //!< atari keyboard has undo

    /// key state modifier keys
,   LX_KEY_NUMLOCK
,   LX_KEY_CAPSLOCK
,   LX_KEY_SCROLLLOCK
,   LX_KEY_RSHIFT
,   LX_KEY_LSHIFT
,   LX_KEY_RCTRL
,   LX_KEY_LCTRL
,   LX_KEY_RALT
,   LX_KEY_LALT
,   LX_KEY_RCMD                 //!< right command for macintosh
,   LX_KEY_LCMD                 //!< left command for macintosh

    /// other keys
,   LX_KEY_PAUSE
,   LX_KEY_SEARCH
,   LX_KEY_TABBACK
,   LX_KEY_BACK             = LX_KEY_ESCAPE
,   LX_KEY_ENTER            = LX_KEY_RETURN
,   LX_KEY_PLAY             = LX_KEY_RETURN

}lx_key_e;

/// the keyboard event type
typedef struct lx_event_keyboard_t_ {
    /*! the key code
     *
     * char:    [0, 0xff]
     * special: [0x0100, 0xffff]
     */
    lx_uint16_t             code;
    lx_uint8_t              pressed;
}lx_event_keyboard_t;

/// the mouse code enum
typedef enum lx_mouse_code_e_ {
    LX_MOUSE_NONE           = 0     //!< none
,   LX_MOUSE_DOWN           = 1     //!< mouse down
,   LX_MOUSE_UP             = 2     //!< mouse up
,   LX_MOUSE_MOVE           = 3     //!< mouse move
,   LX_MOUSE_SCROLL         = 4     //!< mouse scroll

}lx_mouse_code_e;

/// the mouse button enum
typedef enum lx_mouse_button_e_ {
    LX_MOUSE_BUTTON_NONE    = 0     //!< none
,   LX_MOUSE_BUTTON_LEFT    = 1     //!< left button
,   LX_MOUSE_BUTTON_RIGHT   = 2     //!< right button
,   LX_MOUSE_BUTTON_MIDDLE  = 3     //!< middle button
}lx_mouse_button_e;

/// the mouse event type
typedef struct lx_event_mouse_t_ {
    lx_uint8_t              code;
    lx_uint8_t              button;
    lx_point_t              cursor;
    lx_point_t              scroll;
}lx_event_mouse_t;

/// the touch code enum
typedef enum lx_touch_code_e_ {
    LX_TOUCH_NONE           = 0     //!< none
,   LX_TOUCH_BEGAN          = 1     //!< touch began
,   LX_TOUCH_MOVED          = 2     //!< touch moved
,   LX_TOUCH_ENDED          = 3     //!< touch ended
,   LX_TOUCH_CANCELED       = 4     //!< touch canceled
}lx_touch_code_e;

/// the touch type
typedef struct lx_touch_t_ {
    lx_point_t          point;
    lx_point_t          prev;
    lx_point_t          start;
}lx_touch_t;

/// the touch event type
typedef struct lx_event_touch_t_ {
    lx_uint8_t              code;
    lx_uint8_t              count;
    lx_touch_t*             touches;
}lx_event_touch_t;

/// the active code enum
typedef enum lx_active_code_e_ {
    LX_ACTIVE_NONE          = 0     //!< none
,   LX_ACTIVE_BACKGROUND    = 1     //!< enter background
,   LX_ACTIVE_FOREGROUND    = 2     //!< enter foreground
,   LX_ACTIVE_RESIZE_WINDOW = 3     //!< resize window
}lx_active_code_e;

/// the active event type
typedef struct lx_event_active_t_ {
    lx_uint8_t              code;
    lx_size_t               data[2];
}lx_event_active_t;

/// the event type
typedef struct lx_event_t_ {
    lx_uint8_t              type;
    union {
        lx_event_active_t   active;
        lx_event_touch_t    touch;
        lx_event_mouse_t    mouse;
        lx_event_keyboard_t keyboard;

    }u;
    lx_cpointer_t           target;
}lx_event_t;

/// the event ref type
typedef lx_event_t*         lx_event_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

#ifdef LX_DEBUG
/*! dump event
 *
 * @param event     the event
 */
lx_void_t           lx_event_dump(lx_event_ref_t event);
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


