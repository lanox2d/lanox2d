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
        return SDL_PIXELFORMAT_ABGR1555;
    case LX_PIXFMT_ARGB8888:
    case LX_PIXFMT_XRGB8888:
        return SDL_PIXELFORMAT_ARGB8888;
    }
    return -1;
}

static lx_bool_t lx_window_sdl_start(lx_window_sdl_t* window) {
    lx_bool_t ok = lx_false;
    do {

        // init sdl
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            break;
        }

        // create sdl window
        window->window = SDL_CreateWindow(window->base.title? window->base.title : "lanox2d (SDL)",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window->base.width, window->base.height, SDL_WINDOW_SHOWN);
        lx_assert_and_check_break(window->window);

        // create sdl renderer
        window->renderer = SDL_CreateRenderer(window->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        lx_assert_and_check_break(window->renderer);

        // create sdl texture
        lx_int_t pixfmt = lx_window_sdl_pixfmt(window->base.pixfmt);
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
        window->base.device = lx_device_init_from_bitmap(window->bitmap);
        lx_assert_and_check_break(window->base.device);

        // init canvas
        window->base.canvas = lx_canvas_init(window->base.device);
        lx_assert_and_check_break(window->base.canvas);

        // ok
        ok = lx_true;
    } while (0);
    return ok;
}

static lx_void_t lx_window_sdl_runloop(lx_window_ref_t self) {

    // check
    lx_window_sdl_t* window = (lx_window_sdl_t*)self;
    lx_assert_and_check_return(window);

    // start sdl window
    if (!lx_window_sdl_start(window)) {
        lx_trace_e("start sdl window failed!");
        return ;
    }

    // do loop
    lx_bool_t     stop = lx_false;
    SDL_Event     event;
    SDL_Texture*  texture = window->texture;
    SDL_Renderer* renderer = window->renderer;
    lx_int_t      fps_delay = 1000 / window->base.fps;
    while (!stop) {

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
            switch (event.type) {
                case SDL_QUIT:
                    stop = lx_true;
                    break;
                default:
                    break;
            }
        }

        // compute delay for framerate
        lx_int_t  delay = 0;
        lx_hong_t time = lx_mclock();
        lx_int_t  dt = (lx_int_t)(time - starttime);
        if (fps_delay >= dt) {
            delay = fps_delay - dt;
        }

        // compute framerate
        if (window->base.flags & LX_WINDOW_FLAG_SHOW_FPS) {
            if (!window->fps_time) window->fps_time = time;
            window->fps_count++;
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

        window->base.fps     = 60;
        window->base.width   = width;
        window->base.height  = height;
        window->base.title   = title;
        window->base.runloop = lx_window_sdl_runloop;
        window->base.exit    = lx_window_sdl_exit;

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
