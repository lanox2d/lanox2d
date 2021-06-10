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
 * @file        canvas_matrix.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "canvas_matrix.h"
#include "device.h"
#include "private/canvas.h"
#include "../base/base.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_matrix_ref_t lx_canvas_matrix(lx_canvas_ref_t self) {
    lx_canvas_t* canvas = (lx_canvas_t*)self;
    return canvas? &canvas->matrix : lx_null;
}

lx_matrix_ref_t lx_canvas_matrix_save(lx_canvas_ref_t self) {
    lx_canvas_t* canvas = (lx_canvas_t*)self;
    lx_assert_and_check_return_val(canvas && canvas->device && canvas->matrix_stack, lx_null);

    // save matrix
    lx_stack_push(canvas->matrix_stack, &canvas->matrix);
    lx_device_bind_matrix(canvas->device, &canvas->matrix);
    return &canvas->matrix;
}
lx_void_t lx_canvas_matrix_load(lx_canvas_ref_t self) {
    lx_canvas_t* canvas = (lx_canvas_t*)self;
    lx_assert_and_check_return(canvas && canvas->device && canvas->matrix_stack);

    // init matrix
    lx_matrix_ref_t matrix = (lx_matrix_ref_t)lx_stack_top(canvas->matrix_stack);
    lx_assert_and_check_return(matrix);

    // load matrix
    canvas->matrix = *matrix;
    lx_device_bind_matrix(canvas->device, &canvas->matrix);
    lx_stack_pop(canvas->matrix_stack);
}

