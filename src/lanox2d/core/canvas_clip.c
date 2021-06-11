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
 * @file        canvas_clip.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "canvas_clip.h"
#include "device.h"
#include "private/canvas.h"
#include "../base/base.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_clipper_ref_t lx_canvas_clipper(lx_canvas_ref_t self) {
    lx_canvas_t* canvas = (lx_canvas_t*)self;
    if (canvas && canvas->clipper_stack) {
        return (lx_clipper_ref_t)lx_object_stack_object(canvas->clipper_stack);
    }
    return lx_null;
}

lx_clipper_ref_t lx_canvas_save_clipper(lx_canvas_ref_t self) {
    lx_canvas_t* canvas = (lx_canvas_t*)self;
    lx_assert_and_check_return_val(canvas && canvas->clipper_stack, lx_null);

    // save clipper
    lx_clipper_ref_t clipper = (lx_clipper_ref_t)lx_object_stack_save(canvas->clipper_stack);
    lx_device_bind_clipper(canvas->device, clipper);
    return clipper;
}

lx_void_t lx_canvas_load_clipper(lx_canvas_ref_t self) {
    lx_canvas_t* canvas = (lx_canvas_t*)self;
    lx_assert_and_check_return(canvas && canvas->clipper_stack);

    // load clipper
    lx_object_stack_load(canvas->clipper_stack);
    lx_device_bind_clipper(canvas->device, lx_canvas_clipper(self));
}
