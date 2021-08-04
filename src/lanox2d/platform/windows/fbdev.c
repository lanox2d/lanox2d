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
 * @file        fbdev.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <unistd.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the fbdev window type
typedef struct lx_window_fbdev_t_ {
    lx_window_t     base;
    lx_bitmap_ref_t bitmap;
    lx_bool_t       is_quit;
    lx_bool_t       is_shift;
    lx_int_t        devfd;
    lx_int_t        keyfd;
    lx_int_t        screensize;
    lx_byte_t*      framebuffer;
} lx_window_fbdev_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_bool_t lx_window_fbdev_keyevent_init(lx_window_fbdev_t* window) {
    lx_char_t name[64];
    lx_char_t buffer[256] = {0};
    lx_int_t i;
    lx_int_t fd = 0;
    for (i = 0; i < 32; i++) {
        lx_snprintf(name, sizeof(name), "/dev/input/event%d", i);
        if ((fd = open(name, O_RDONLY, 0)) >= 0) {
            lx_int_t len = ioctl(fd, EVIOCGNAME(sizeof(buffer)), buffer);
            if (len > 0) {
                if (lx_strstr(buffer, "keyboard")) {
                    window->keyfd = fd;
                    lx_trace_d("found input keyboard: %s, %s, fd: %d", buffer, name, fd);
                    return lx_true;
                }
            }
            close(fd);
        }
    }
    return lx_false;
}

static lx_uint16_t lx_window_fbdev_keyevent_code(lx_window_fbdev_t* window, lx_uint16_t key, lx_bool_t is_pressed) {
    lx_uint16_t code = LX_KEY_NUL;
    switch (key)
    {
    case KEY_A: code = 'a'; break;
    case KEY_B: code = 'b'; break;
    case KEY_C: code = 'c'; break;
    case KEY_D: code = 'j'; break;
    case KEY_E: code = 'e'; break;
    case KEY_F: code = 'f'; break;
    case KEY_G: code = 'g'; break;
    case KEY_H: code = 'h'; break;
    case KEY_I: code = 'i'; break;
    case KEY_J: code = 'j'; break;
    case KEY_K: code = 'k'; break;
    case KEY_L: code = 'l'; break;
    case KEY_M: code = 'm'; break;
    case KEY_N: code = 'n'; break;
    case KEY_O: code = 'o'; break;
    case KEY_P: code = 'p'; break;
    case KEY_Q: code = 'q'; break;
    case KEY_R: code = 'r'; break;
    case KEY_S: code = 's'; break;
    case KEY_T: code = 't'; break;
    case KEY_U: code = 'u'; break;
    case KEY_V: code = 'v'; break;
    case KEY_W: code = 'w'; break;
    case KEY_X: code = 'x'; break;
    case KEY_Y: code = 'y'; break;
    case KEY_Z: code = 'z'; break;
    case KEY_0: code = window->is_shift? ')' : '0'; break;
    case KEY_1: code = window->is_shift? '!' : '1'; break;
    case KEY_2: code = window->is_shift? '@' : '2'; break;
    case KEY_3: code = window->is_shift? '#' : '3'; break;
    case KEY_4: code = window->is_shift? '$' : '4'; break;
    case KEY_5: code = window->is_shift? '%' : '5'; break;
    case KEY_6: code = window->is_shift? '^' : '6'; break;
    case KEY_7: code = window->is_shift? '&' : '7'; break;
    case KEY_8: code = window->is_shift? '*' : '8'; break;
    case KEY_9: code = window->is_shift? '(' : '9'; break;
    case KEY_MINUS: code = window->is_shift? '_' : '-'; break;
    case KEY_EQUAL: code = window->is_shift? '+' : '='; break;
    case KEY_COMMA: code = window->is_shift? '<' : ','; break;
    case KEY_DOT:   code = window->is_shift? '>' : '.'; break;
    case KEY_SLASH: code = window->is_shift? '?' : '/'; break;
    case KEY_GRAVE: code = window->is_shift? '~' : '`'; break;
    case KEY_BACKSLASH:  code = window->is_shift? '|' : '\\'; break;
    case KEY_LEFTBRACE:  code = window->is_shift? '{' : '['; break;
    case KEY_RIGHTBRACE: code = window->is_shift? '}' : ']'; break;
    case KEY_SEMICOLON:  code = window->is_shift? ':' : ';'; break;
    case KEY_APOSTROPHE: code = window->is_shift? '"' : '\''; break;
    case KEY_TAB:   code = LX_KEY_TAB; break;
    case KEY_LEFT:  code = LX_KEY_LEFT; break;
    case KEY_RIGHT: code = LX_KEY_RIGHT; break;
    case KEY_UP:    code = LX_KEY_UP; break;
    case KEY_DOWN:  code = LX_KEY_DOWN; break;
    case KEY_SPACE: code = LX_KEY_SPACE; break;
    case KEY_ESC:   code = LX_KEY_ESCAPE; break;
    case KEY_ENTER: code = LX_KEY_ENTER; break;
    case KEY_CAPSLOCK:
        if (is_pressed) {
            window->is_shift = !window->is_shift;
        }
        code = LX_KEY_CAPSLOCK;
        break;
    case KEY_LEFTSHIFT:
        window->is_shift = is_pressed;
        code = LX_KEY_LSHIFT;
        break;
    case KEY_RIGHTSHIFT:
        window->is_shift = is_pressed;
        code = LX_KEY_RSHIFT;
        break;
    case KEY_LEFTCTRL:   code = LX_KEY_LCTRL; break;
    case KEY_RIGHTCTRL:  code = LX_KEY_RCTRL; break;
    case KEY_LEFTALT:    code = LX_KEY_LALT; break;
    case KEY_RIGHTALT:   code = LX_KEY_RALT; break;
    case KEY_BACKSPACE:  code = LX_KEY_BACKSPACE; break;
    default:
        lx_trace_d("%d", key);
        break;
    }
    if (code >= 'a' && code <= 'z' && window->is_shift) {
        code = lx_toupper(code);
    }
    return code;
}

static lx_void_t lx_window_fbdev_keyevent_poll(lx_window_fbdev_t* window) {
    lx_check_return(window && window->keyfd >= 0);

    // poll event
    struct pollfd pfd = {0};
    pfd.fd = window->keyfd;
    pfd.events |= POLLIN;
    if (poll(&pfd, 1, 0) > 0) {

        // read event
        struct input_event keyevent;
        if (read(window->keyfd, &keyevent, sizeof(keyevent)) == sizeof(keyevent)) {
            if (keyevent.type == EV_KEY) {
                lx_event_t event = {0};
                event.type               = LX_EVENT_TYPE_KEYBOARD;
                event.u.keyboard.pressed = keyevent.value? lx_true : lx_false;
                event.u.keyboard.code    = lx_window_fbdev_keyevent_code(window, (lx_uint16_t)keyevent.code, event.u.keyboard.pressed);
                if (window->base.on_event && event.u.keyboard.code) {
                    window->base.on_event((lx_window_ref_t)window, &event);
                }
            }
        }
    }
}

static lx_bool_t lx_window_fbdev_start(lx_window_fbdev_t* window) {
    lx_bool_t ok = lx_false;
    do {

        // open fb dev
        window->devfd = open("/dev/fb0", O_RDWR);
        lx_assert_and_check_break(window->devfd >= 0);

        // get screen info
        struct fb_fix_screeninfo finfo;
        struct fb_var_screeninfo vinfo;
        if (ioctl(window->devfd, FBIOGET_FSCREENINFO, &finfo) != 0) {
            lx_trace_e("get fix screeninfo failed!");
            break;
        }
        if (ioctl(window->devfd, FBIOGET_VSCREENINFO, &vinfo) != 0) {
            lx_trace_e("get var screeninfo failed!");
            break;
        }
        window->base.width  = vinfo.xres;
        window->base.height = vinfo.yres;
        window->screensize  = finfo.smem_len;

        // trace
        lx_trace_d("fb screen info: %dx%d bpp: %d, size: %d", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel, finfo.smem_len);

#if 0
        // activate buffer
        vinfo.activate |= FB_ACTIVATE_NOW | FB_ACTIVATE_FORCE;
        if (ioctl(window->devfd, FBIOPUT_VSCREENINFO, &vinfo) < 0) {
            lx_trace_e("failed to activate framebuffer!");
            break;
        }
#endif

        // get framebuffer
        window->framebuffer = mmap(0, window->screensize, PROT_READ | PROT_WRITE, MAP_SHARED, window->devfd, 0);
        lx_assert_and_check_break(window->framebuffer);

        // init bitmap
        lx_size_t row_bytes = vinfo.xres * (vinfo.bits_per_pixel >> 3);
        window->bitmap = lx_bitmap_init(window->framebuffer, window->base.pixfmt, window->base.width, window->base.height, row_bytes, lx_false);
        lx_assert_and_check_break(window->bitmap);

        // init key event
        if (!lx_window_fbdev_keyevent_init(window)) {
            lx_trace_e("failed to start key event!");
            break;
        }

        // init device
#if defined(LX_CONFIG_DEVICE_HAVE_BITMAP)
        window->base.device = lx_device_init_from_bitmap(window->bitmap);
#elif defined(LX_CONFIG_DEVICE_HAVE_SKIA)
        window->base.device = lx_device_init_from_skia(window->base.width, window->base.height, window->bitmap);
#endif
        lx_assert_and_check_break(window->base.device);

        // init canvas
        window->base.canvas = lx_canvas_init(window->base.device);
        lx_assert_and_check_break(window->base.canvas);

        // ok
        ok = lx_true;
    } while (0);
    return ok;
}

static lx_void_t lx_window_fbdev_runloop(lx_window_ref_t self) {
    lx_window_fbdev_t* window = (lx_window_fbdev_t*)self;
    lx_assert_and_check_return(window);

    // start sdl window
    if (!lx_window_fbdev_start(window)) {
        lx_trace_e("start sdl window failed!");
        return ;
    }

    // do loop
    lx_int_t fps_delay = 1000 / window->base.fps;
    while (!window->is_quit) {

        // poll event
        lx_window_fbdev_keyevent_poll(window);

        // draw window
        lx_hong_t starttime = lx_mclock();
        if (window->base.on_draw) {
            window->base.on_draw(self, window->base.canvas);
        }

        // compute delay for framerate
        lx_int_t  delay = 1;
        lx_hong_t time = lx_mclock();
        lx_int_t  dt = (lx_int_t)(time - starttime);
        if (fps_delay >= dt) {
            delay = fps_delay - dt;
        }

        // delay
        lx_msleep(delay);
    }

}

static lx_void_t lx_window_fbdev_quit(lx_window_ref_t self) {
    lx_window_fbdev_t* window = (lx_window_fbdev_t*)self;
    if (window) {
        window->is_quit = lx_true;
    }
}

static lx_void_t lx_window_fbdev_exit(lx_window_ref_t self) {
    lx_window_fbdev_t* window = (lx_window_fbdev_t*)self;
    if (window) {
        if (window->base.canvas) {
            lx_canvas_exit(window->base.canvas);
            window->base.canvas = lx_null;
        }
        if (window->base.device) {
            lx_device_exit(window->base.device);
            window->base.device = lx_null;
        }
        if (window->bitmap) {
            lx_bitmap_exit(window->bitmap);
            window->bitmap = lx_null;
        }
        if (window->framebuffer) {
            munmap(window->framebuffer, window->screensize);
            window->framebuffer = lx_null;
        }
        if (window->keyfd >= 0) {
            close(window->keyfd);
            window->keyfd = -1;
        }
        if (window->devfd >= 0) {
            close(window->devfd);
            window->devfd = -1;
        }
        lx_free(window);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_window_ref_t lx_window_init_fbdev(lx_size_t width, lx_size_t height, lx_char_t const* title) {
    lx_bool_t          ok = lx_false;
    lx_window_fbdev_t* window = lx_null;
    do {

        // check
        lx_assert_and_check_break(width && height);

        // init window
        window = lx_malloc0_type(lx_window_fbdev_t);
        lx_assert_and_check_break(window);

        window->base.fps         = 60;
        window->base.width       = (lx_uint16_t)width;
        window->base.height      = (lx_uint16_t)height;
        window->base.title       = title;
        window->base.runloop     = lx_window_fbdev_runloop;
        window->base.quit        = lx_window_fbdev_quit;
        window->base.exit        = lx_window_fbdev_exit;
        window->base.pixfmt      = LX_PIXFMT_RGBX8888;
        window->devfd            = -1;
        window->keyfd            = -1;

        // ok
        ok = lx_true;

    } while (0);

    if (!ok && window) {
        lx_window_exit((lx_window_ref_t)window);
        window = lx_null;
    }
    return (lx_window_ref_t)window;
}
