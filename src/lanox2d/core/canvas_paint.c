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
 * @file        canvas_paint.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "canvas_paint.h"
#include "device.h"
#include "private/canvas.h"
#include "../base/base.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_paint_ref_t lx_canvas_paint(lx_canvas_ref_t self) {
    lx_canvas_t* canvas = (lx_canvas_t*)self;
    if (canvas && canvas->paint_stack) {
        return (lx_paint_ref_t)lx_object_stack_object(canvas->paint_stack);
    }
    return lx_null;
}

lx_paint_ref_t lx_canvas_paint_save(lx_canvas_ref_t self) {
    lx_canvas_t* canvas = (lx_canvas_t*)self;
    lx_assert_and_check_return_val(canvas && canvas->device && canvas->paint_stack, lx_null);

    // save paint
    lx_paint_ref_t paint = (lx_paint_ref_t)lx_object_stack_save(canvas->paint_stack);
    lx_device_bind_paint(canvas->device, paint);
    return paint;
}

lx_void_t lx_canvas_paint_load(lx_canvas_ref_t self) {
    lx_canvas_t* canvas = (lx_canvas_t*)self;
    lx_assert_and_check_return(canvas && canvas->device && canvas->paint_stack);

    // load paint
    lx_object_stack_load(canvas->paint_stack);
    lx_device_bind_paint(canvas->device, lx_canvas_paint(self));
}

