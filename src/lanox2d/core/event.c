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
 * @file        event.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "event.h"
#include "../base/base.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef LX_DEBUG
lx_void_t lx_event_dump(lx_event_ref_t event) {
    switch (event->type) {
    case LX_EVENT_TYPE_MOUSE: {
        lx_char_t const* code_cstr[] = {"none", "down", "up", "move", "scroll"};
        lx_assert_and_check_break(event->u.mouse.code < lx_arrayn(code_cstr));

        lx_char_t const* button_cstr[] = {"none", "left", "right", "middle"};
        lx_assert_and_check_break(event->u.mouse.button < lx_arrayn(button_cstr));

        // trace
        lx_trace_i("mouse: %s %s, cursor: %0.2f,%0.2f, scroll: %0.2f,%0.2f",
            button_cstr[event->u.mouse.button], code_cstr[event->u.mouse.code],
            event->u.mouse.cursor.x, event->u.mouse.cursor.y,
            event->u.mouse.scroll.x, event->u.mouse.scroll.y);
        break;
    }
    case LX_EVENT_TYPE_KEYBOARD: {
        lx_char_t cstr[64] = {0};
        if (event->u.keyboard.code < 256) {
            if (lx_isgraph(event->u.keyboard.code) && !lx_isspace(event->u.keyboard.code)) {
                lx_snprintf(cstr, sizeof(cstr) - 1, "%c", event->u.keyboard.code);
            } else {
                lx_char_t const* code_cstr = lx_null;
                switch (event->u.keyboard.code) {
                case LX_KEY_NUL:        code_cstr = "nul";          break;
                case LX_KEY_BACKSPACE:  code_cstr = "backspace";    break;
                case LX_KEY_TAB:        code_cstr = "tab";          break;
                case LX_KEY_RETURN:     code_cstr = "return";       break;
                case LX_KEY_ESCAPE:     code_cstr = "escape";       break;
                case LX_KEY_SPACE:      code_cstr = "space";        break;
                case LX_KEY_DELETE:     code_cstr = "delete";       break;
                }
                if (code_cstr) {
                    lx_snprintf(cstr, sizeof(cstr) - 1, "%s", code_cstr);
                } else {
                    lx_snprintf(cstr, sizeof(cstr) - 1, "%#x", event->u.keyboard.code);
                }
            }
        } else {
            lx_size_t code = event->u.keyboard.code - 256;
            lx_char_t const* code_cstr[] = {
                "f1"
            ,   "f2"
            ,   "f3"
            ,   "f4"
            ,   "f5"
            ,   "f6"
            ,   "f7"
            ,   "f8"
            ,   "f9"
            ,   "f10"
            ,   "f11"
            ,   "f12"

            ,   "left"
            ,   "up"
            ,   "right"
            ,   "down"

            ,   "home"
            ,   "end"
            ,   "insert"
            ,   "pageup"
            ,   "pagedown"

            ,   "help"
            ,   "print"
            ,   "sysreq"
            ,   "break"
            ,   "menu"
            ,   "power"
            ,   "euro"
            ,   "undo"

            ,   "numlock"
            ,   "capslock"
            ,   "scrolllock"
            ,   "rshift"
            ,   "lshift"
            ,   "rctrl"
            ,   "lctrl"
            ,   "ralt"
            ,   "lalt"
            ,   "rcmd"
            ,   "lcmd"

            ,   "pause"
            ,   "search"
            ,   "tabback"
            };
            lx_assert_and_check_break(code < lx_arrayn(code_cstr));
            lx_snprintf(cstr, sizeof(cstr) - 1, "%s", code_cstr[code]);
        }
        lx_trace_i("key: %s, pressed: %u", cstr, event->u.keyboard.pressed);
        break;
    }
    case LX_EVENT_TYPE_TOUCH: {
        lx_char_t const* code_cstr[] = {
            "none"
        ,   "began"
        ,   "moved"
        ,   "ended"
        ,   "canceled"
        };
        lx_assert_and_check_break(event->u.touch.code < lx_arrayn(code_cstr));
        lx_trace_i("touch: %s, count: %u", code_cstr[event->u.touch.code], event->u.touch.count);
        break;
    }
    case LX_EVENT_TYPE_ACTIVE: {
        lx_char_t const* code_cstr[] =
        {
            "none"
        ,   "background"
        ,   "foreground"
        };
        lx_assert_and_check_break(event->u.active.code < lx_arrayn(code_cstr));
        lx_trace_i("active: %s", code_cstr[event->u.active.code]);
        break;
    }
    default:
        lx_trace_e("invalid type: %u", event->type);
        break;
    }
}
#endif
