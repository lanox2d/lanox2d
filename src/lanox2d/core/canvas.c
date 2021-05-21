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
 * @file        canvas.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "canvas.h"
#include "device.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the canvas type
typedef struct lx_canvas_t_ {
    lx_device_ref_t device;
}lx_canvas_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_canvas_ref_t lx_canvas_init(lx_device_ref_t device) {
    lx_bool_t    ok = lx_false;
    lx_canvas_t* canvas = lx_null;
    do {

        // init canvas
        canvas = lx_malloc0_type(lx_canvas_t);
        lx_assert_and_check_break(canvas);

        canvas->device = device;

        ok = lx_true;

    } while (0);

    if (!ok && canvas) {
        lx_canvas_exit((lx_canvas_ref_t)canvas);
        canvas = lx_null;
    }
    return (lx_canvas_ref_t)canvas;
}

lx_void_t lx_canvas_exit(lx_canvas_ref_t self) {
    lx_canvas_t* canvas = (lx_canvas_t*)self;
    if (canvas) {
        lx_free(canvas);
    }
}

lx_void_t lx_canvas_draw_clear(lx_canvas_ref_t self, lx_color_t color) {
    lx_canvas_t* canvas = (lx_canvas_t*)self;
    if (canvas && canvas->device) {
        lx_device_draw_clear(canvas->device, color);
    }
}
