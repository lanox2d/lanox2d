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
#include "private/canvas.h"
#include "../base/base.h"

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

        // init device
        canvas->device = device;

        // init matrix
        lx_matrix_clear(&canvas->matrix);
        canvas->matrix_stack = lx_stack_init(8, sizeof(lx_matrix_t), lx_null);
        lx_assert_and_check_break(canvas->matrix_stack);
        lx_device_bind_matrix(canvas->device, &canvas->matrix);

        // init path
        canvas->path_stack = lx_object_stack_init(8, LX_OBJECT_STACK_TYPE_PATH);
        lx_assert_and_check_break(canvas->path_stack);
        lx_device_bind_path(canvas->device, (lx_path_ref_t)lx_object_stack_object(canvas->path_stack));

        // init paint
        canvas->paint_stack = lx_object_stack_init(8, LX_OBJECT_STACK_TYPE_PAINT);
        lx_assert_and_check_break(canvas->paint_stack);
        lx_device_bind_paint(canvas->device, (lx_paint_ref_t)lx_object_stack_object(canvas->paint_stack));

        // init clipper
        canvas->clipper_stack = lx_object_stack_init(8, LX_OBJECT_STACK_TYPE_CLIPPER);
        lx_assert_and_check_break(canvas->clipper_stack);
        lx_device_bind_clipper(canvas->device, (lx_clipper_ref_t)lx_object_stack_object(canvas->clipper_stack));

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
        if (canvas->clipper_stack) {
            lx_object_stack_exit(canvas->clipper_stack);
            canvas->clipper_stack = lx_null;
        }
        if (canvas->paint_stack) {
            lx_object_stack_exit(canvas->paint_stack);
            canvas->paint_stack = lx_null;
        }
        if (canvas->path_stack) {
            lx_object_stack_exit(canvas->path_stack);
            canvas->path_stack = lx_null;
        }
        if (canvas->matrix_stack) {
            lx_stack_exit(canvas->matrix_stack);
            canvas->matrix_stack = lx_null;
        }
        lx_free(canvas);
    }
}

