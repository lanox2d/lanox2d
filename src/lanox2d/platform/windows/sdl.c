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
 * @file        sdl.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <SDL.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the sdl window type
typedef struct lx_window_sdl_t_ {
    lx_window_t     base;
    SDL_Window*     window;
    SDL_Renderer*   renderer;
    SDL_Texture*    texture;
    lx_bitmap_ref_t bitmap;
    lx_uint8_t      button;
    lx_bool_t       is_quit;
    lx_hong_t       fps_time;
    lx_hong_t       fps_count;
} lx_window_sdl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

static lx_int_t lx_window_sdl_pixfmt(lx_uint16_t pixfmt) {
    switch (pixfmt) {
    case LX_PIXFMT_RGB565:
        return SDL_PIXELFORMAT_RGB565;
    case LX_PIXFMT_ARGB1555:
    case LX_PIXFMT_XRGB1555:
        return SDL_PIXELFORMAT_ARGB1555;
    case LX_PIXFMT_ARGB8888:
    case LX_PIXFMT_XRGB8888:
        return SDL_PIXELFORMAT_ARGB8888;
    case LX_PIXFMT_RGBA8888:
    case LX_PIXFMT_RGBX8888:
        return SDL_PIXELFORMAT_RGBA8888;
    case LX_PIXFMT_RGBA8888 | LX_PIXFMT_BENDIAN:
    case LX_PIXFMT_RGBX8888 | LX_PIXFMT_BENDIAN:
        return SDL_PIXELFORMAT_ABGR8888;
    }
    return -1;
}

static lx_bool_t lx_window_sdl_start(lx_window_sdl_t* window) {
    lx_bool_t ok = lx_false;
    do {

        // init sdl
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            lx_trace_e("init sdl failed, %s", SDL_GetError());
            break;
        }

        // init window flags
        lx_int_t flags = window->base.flags & LX_WINDOW_FLAG_HIDDEN? SDL_WINDOW_HIDDEN : SDL_WINDOW_SHOWN;
        if (window->base.flags & LX_WINDOW_FLAG_FULLSCREEN) {
            flags |= SDL_WINDOW_FULLSCREEN;
        }
        if (!(window->base.flags & LX_WINDOW_FLAG_NOT_REISZE)) {
            flags |= SDL_WINDOW_RESIZABLE;
        }

        // create sdl window
        window->window = SDL_CreateWindow(window->base.title? window->base.title : "lanox2d (SDL)",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window->base.width, window->base.height, flags);
        lx_assert_and_check_break(window->window);

        // hide window cursor
        if (window->base.flags & LX_WINDOW_FLAG_HIDE_CURSOR) {
            SDL_ShowCursor(0);
        }

        // create sdl renderer
        lx_int_t pixfmt = lx_window_sdl_pixfmt(window->base.pixfmt);
        if (pixfmt != SDL_PIXELFORMAT_ABGR8888 && pixfmt != SDL_PIXELFORMAT_ARGB8888) { // maybe sdl bug? it will blink
            window->renderer = SDL_CreateRenderer(window->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        }
        if (!window->renderer) {
            window->renderer = SDL_CreateRenderer(window->window, -1, SDL_RENDERER_SOFTWARE);
        }
        lx_assert_and_check_break(window->renderer);

        // create sdl texture
        if (pixfmt != -1) {
            window->texture = SDL_CreateTexture(window->renderer, pixfmt, SDL_TEXTUREACCESS_STREAMING, window->base.width, window->base.height);
        }
        lx_assert_and_check_break(window->texture);

        // init texture bitmap
        lx_int_t      pitch = 0;
        lx_pointer_t  pixels = lx_null;
        if (0 == SDL_LockTexture(window->texture, lx_null, &pixels, &pitch)) {
            window->bitmap = lx_bitmap_init(pixels, window->base.pixfmt, window->base.width, window->base.height, pitch, lx_false);
            SDL_UnlockTexture(window->texture);
        }
        lx_assert_and_check_break(window->bitmap);

        // init device
#if defined(LX_CONFIG_DEVICE_HAVE_BITMAP)
        window->base.device = lx_device_init_from_bitmap(window->bitmap);
#elif defined(LX_CONFIG_DEVICE_HAVE_SKIA)
        window->base.device = lx_device_init_from_skia((lx_window_ref_t)window, window->bitmap);
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

static lx_void_t lx_window_sdl_event(lx_window_sdl_t* window, SDL_Event* sdlevent) {
    switch (sdlevent->type) {
    case SDL_MOUSEMOTION: {
        lx_event_t              event = {0};
        event.type              = LX_EVENT_TYPE_MOUSE;
        event.u.mouse.code      = LX_MOUSE_MOVE;
        event.u.mouse.button    = window->button;
        lx_point_imake(&event.u.mouse.cursor, sdlevent->motion.x, sdlevent->motion.y);
        if (window->base.on_event) {
            window->base.on_event((lx_window_ref_t)window, &event);
        }
        break;
    }
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN: {
        lx_event_t              event = {0};
        event.type              = LX_EVENT_TYPE_MOUSE;
        event.u.mouse.code      = sdlevent->type == SDL_MOUSEBUTTONDOWN? LX_MOUSE_DOWN : LX_MOUSE_UP;
        lx_point_imake(&event.u.mouse.cursor, sdlevent->button.x, sdlevent->button.y);

        switch (sdlevent->button.button)
        {
        case SDL_BUTTON_LEFT:   event.u.mouse.button = LX_MOUSE_BUTTON_LEFT;    break;
        case SDL_BUTTON_RIGHT:  event.u.mouse.button = LX_MOUSE_BUTTON_RIGHT;   break;
        case SDL_BUTTON_MIDDLE: event.u.mouse.button = LX_MOUSE_BUTTON_MIDDLE;  break;
        default:                event.u.mouse.button = LX_MOUSE_BUTTON_NONE;    break;
        }
        window->button = sdlevent->type == SDL_MOUSEBUTTONDOWN? event.u.mouse.button : LX_MOUSE_BUTTON_NONE;

        if (window->base.on_event) {
            window->base.on_event((lx_window_ref_t)window, &event);
        }
        break;
    }
    case SDL_KEYDOWN:
    case SDL_KEYUP: {
        lx_event_t                  event = {0};
        event.type                  = LX_EVENT_TYPE_KEYBOARD;
        event.u.keyboard.pressed    = sdlevent->type == SDL_KEYDOWN? lx_true : lx_false;

        switch ((lx_size_t)sdlevent->key.keysym.sym) {
        case SDLK_F1:           event.u.keyboard.code = LX_KEY_F1;          break;
        case SDLK_F2:           event.u.keyboard.code = LX_KEY_F2;          break;
        case SDLK_F3:           event.u.keyboard.code = LX_KEY_F3;          break;
        case SDLK_F4:           event.u.keyboard.code = LX_KEY_F4;          break;
        case SDLK_F5:           event.u.keyboard.code = LX_KEY_F5;          break;
        case SDLK_F6:           event.u.keyboard.code = LX_KEY_F6;          break;
        case SDLK_F7:           event.u.keyboard.code = LX_KEY_F7;          break;
        case SDLK_F8:           event.u.keyboard.code = LX_KEY_F8;          break;
        case SDLK_F9:           event.u.keyboard.code = LX_KEY_F9;          break;
        case SDLK_F10:          event.u.keyboard.code = LX_KEY_F10;         break;
        case SDLK_F11:          event.u.keyboard.code = LX_KEY_F11;         break;
        case SDLK_F12:          event.u.keyboard.code = LX_KEY_F12;         break;

        case SDLK_LEFT:         event.u.keyboard.code = LX_KEY_LEFT;        break;
        case SDLK_UP:           event.u.keyboard.code = LX_KEY_UP;          break;
        case SDLK_RIGHT:        event.u.keyboard.code = LX_KEY_RIGHT;       break;
        case SDLK_DOWN:         event.u.keyboard.code = LX_KEY_DOWN;        break;

        case SDLK_HOME:         event.u.keyboard.code = LX_KEY_HOME;        break;
        case SDLK_END:          event.u.keyboard.code = LX_KEY_END;         break;
        case SDLK_INSERT:       event.u.keyboard.code = LX_KEY_INSERT;      break;
        case SDLK_PAGEUP:       event.u.keyboard.code = LX_KEY_PAGEUP;      break;
        case SDLK_PAGEDOWN:     event.u.keyboard.code = LX_KEY_PAGEDOWN;    break;

        case SDLK_HELP:         event.u.keyboard.code = LX_KEY_HELP;        break;
        case SDLK_SYSREQ:       event.u.keyboard.code = LX_KEY_SYSREQ;      break;
        case SDLK_MENU:         event.u.keyboard.code = LX_KEY_MENU;        break;
        case SDLK_POWER:        event.u.keyboard.code = LX_KEY_POWER;       break;
        case SDLK_UNDO:         event.u.keyboard.code = LX_KEY_UNDO;        break;

        case SDLK_CAPSLOCK:     event.u.keyboard.code = LX_KEY_CAPSLOCK;    break;
        case SDLK_SCROLLLOCK:   event.u.keyboard.code = LX_KEY_SCROLLLOCK;  break;
        case SDLK_RSHIFT:       event.u.keyboard.code = LX_KEY_RSHIFT;      break;
        case SDLK_LSHIFT:       event.u.keyboard.code = LX_KEY_LSHIFT;      break;
        case SDLK_RCTRL:        event.u.keyboard.code = LX_KEY_RCTRL;       break;
        case SDLK_LCTRL:        event.u.keyboard.code = LX_KEY_LCTRL;       break;
        case SDLK_RALT:         event.u.keyboard.code = LX_KEY_RALT;        break;
        case SDLK_LALT:         event.u.keyboard.code = LX_KEY_LALT;        break;
        case 0x136:             event.u.keyboard.code = LX_KEY_RCMD;        break;
        case 0x135:             event.u.keyboard.code = LX_KEY_LCMD;        break;

        case SDLK_PAUSE:        event.u.keyboard.code = LX_KEY_PAUSE;       break;

        default :
            if (sdlevent->key.keysym.sym < 256) {
                event.u.keyboard.code = sdlevent->key.keysym.sym;
            }
            break;
        }

        if (event.u.keyboard.code && window->base.on_event) {
            window->base.on_event((lx_window_ref_t)window, &event);
        }
        break;
    }
    case SDL_TEXTEDITING:
    case SDL_TEXTINPUT:
        break;
    case SDL_WINDOWEVENT:
        if (sdlevent->window.event == SDL_WINDOWEVENT_RESIZED) {
            lx_event_t event = {0};
            event.type = LX_EVENT_TYPE_ACTIVE;
            event.u.active.code = LX_ACTIVE_RESIZE_WINDOW;
            event.u.active.data[0] = (lx_size_t)sdlevent->window.data1;
            event.u.active.data[1] = (lx_size_t)sdlevent->window.data2;
            if (window->base.on_event) {
                window->base.on_event((lx_window_ref_t)window, &event);
            }
        }
        break;
    default:
        // trace
        lx_trace_e("unknown event: %x", sdlevent->type);
        break;
    }
}

static lx_void_t lx_window_sdl_runloop(lx_window_ref_t self) {
    lx_window_sdl_t* window = (lx_window_sdl_t*)self;
    lx_assert_and_check_return(window);

    // start sdl window
    if (!lx_window_sdl_start(window)) {
        lx_trace_e("start sdl window failed!");
        return ;
    }

    // do loop
    SDL_Event     event;
    SDL_Texture*  texture = window->texture;
    SDL_Renderer* renderer = window->renderer;
    lx_int_t      fps_delay = 1000 / window->base.fps;
    while (!window->is_quit) {

        // draw window
        lx_int_t      pitch = 0;
        lx_pointer_t  pixels = lx_null;
        lx_hong_t     starttime = lx_mclock();
        if (window->base.on_draw && 0 == SDL_LockTexture(window->texture, lx_null, &pixels, &pitch)) {
            if (lx_bitmap_attach(window->bitmap, pixels, window->base.width, window->base.height, pitch)) {
                window->base.on_draw(self, window->base.canvas);
            }
            SDL_UnlockTexture(window->texture);
        }

        // flush window
        SDL_RenderCopyEx(renderer, texture, lx_null, lx_null, 0, lx_null, SDL_FLIP_NONE);
        SDL_RenderPresent(renderer);

        // poll event
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                window->is_quit = lx_true;
            } else {
                lx_window_sdl_event(window, &event);
            }
        }

        // compute delay for framerate
        lx_int_t  delay = 1;
        lx_hong_t time = lx_mclock();
        lx_int_t  dt = (lx_int_t)(time - starttime);
        if (fps_delay >= dt) {
            delay = fps_delay - dt;
        }

        // compute framerate
        if (window->base.flags & LX_WINDOW_FLAG_SHOW_FPS) {
            if (!window->fps_time) window->fps_time = time;
            else window->fps_count++;
            if (time > window->fps_time + 1000) {
                lx_float_t framerate = (lx_float_t)(window->fps_count * 1000) / (lx_float_t)(time - window->fps_time);
                lx_char_t title[256];
                lx_snprintf(title, sizeof(title), "%s (%0.2f fps)", window->base.title, framerate);
                SDL_SetWindowTitle(window->window, title);
                window->fps_count = 0;
                window->fps_time = time;
            }
        }

        // delay
        SDL_Delay(delay);
    }
}

static lx_void_t lx_window_sdl_fullscreen(lx_window_ref_t self, lx_bool_t is_fullscreen) {
    lx_window_sdl_t* window = (lx_window_sdl_t*)self;
    if (window) {
        if (is_fullscreen) {
            window->base.flags |= LX_WINDOW_FLAG_FULLSCREEN;
            SDL_SetWindowFullscreen(window->window, SDL_WINDOW_FULLSCREEN);
        } else {
            window->base.flags &= ~LX_WINDOW_FLAG_FULLSCREEN;
            SDL_SetWindowFullscreen(window->window, 0);
        }
    }
}

static lx_void_t lx_window_sdl_show(lx_window_ref_t self, lx_bool_t is_show) {
    lx_window_sdl_t* window = (lx_window_sdl_t*)self;
    if (window) {
        if (is_show) {
            SDL_ShowWindow(window->window);
        } else {
            SDL_HideWindow(window->window);
        }
    }
}

static lx_void_t lx_window_sdl_show_cursor(lx_window_ref_t self, lx_bool_t is_show) {
    SDL_ShowCursor(is_show);
}

static lx_void_t lx_window_sdl_quit(lx_window_ref_t self) {
    lx_window_sdl_t* window = (lx_window_sdl_t*)self;
    if (window) {
        window->is_quit = lx_true;
    }
}

static lx_void_t lx_window_sdl_exit(lx_window_ref_t self) {
    lx_window_sdl_t* window = (lx_window_sdl_t*)self;
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
        if (window->texture) {
            SDL_DestroyTexture(window->texture);
            window->texture = lx_null;
        }
        if (window->renderer) {
            SDL_DestroyRenderer(window->renderer);
            window->renderer = lx_null;
        }
        if (window->window) {
            SDL_DestroyWindow(window->window);
            window->window = lx_null;
        }
        lx_free(window);
    }
    SDL_Quit();
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_window_ref_t lx_window_init_sdl(lx_size_t width, lx_size_t height, lx_char_t const* title) {
    lx_bool_t        ok = lx_false;
    lx_window_sdl_t* window = lx_null;
    do {

        // check
        lx_assert_and_check_break(width && height);

        // init window
        window = lx_malloc0_type(lx_window_sdl_t);
        lx_assert_and_check_break(window);

        window->base.fps         = 60;
        window->base.width       = (lx_uint16_t)width;
        window->base.height      = (lx_uint16_t)height;
        window->base.title       = title;
        window->base.runloop     = lx_window_sdl_runloop;
        window->base.quit        = lx_window_sdl_quit;
        window->base.fullscreen  = lx_window_sdl_fullscreen;
        window->base.show        = lx_window_sdl_show;
        window->base.show_cursor = lx_window_sdl_show_cursor;
        window->base.exit        = lx_window_sdl_exit;

        // init pixfmt
#ifdef LX_CONFIG_DEVICE_HAVE_SKIA
        window->base.pixfmt = lx_quality() < LX_QUALITY_TOP? LX_PIXFMT_RGB565 : (LX_PIXFMT_RGBX8888 | LX_PIXFMT_BENDIAN);
#else
        window->base.pixfmt = lx_quality() < LX_QUALITY_TOP? LX_PIXFMT_RGB565 : LX_PIXFMT_RGBX8888;
#endif

        // ok
        ok = lx_true;

    } while (0);

    if (!ok && window) {
        lx_window_exit((lx_window_ref_t)window);
        window = lx_null;
    }
    return (lx_window_ref_t)window;
}
