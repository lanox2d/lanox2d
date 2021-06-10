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
 * @file        canvas_path.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "canvas_path.h"
#include "device.h"
#include "private/canvas.h"
#include "../base/base.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_path_ref_t lx_canvas_path(lx_canvas_ref_t self) {
    lx_canvas_t* path = (lx_canvas_t*)self;
    if (path && path->path_stack) {
        return (lx_path_ref_t)lx_object_stack_object(path->path_stack);
    }
    return lx_null;
}

lx_path_ref_t lx_canvas_path_save(lx_canvas_ref_t self) {
    lx_canvas_t* path = (lx_canvas_t*)self;
    lx_assert_and_check_return_val(path && path->path_stack, lx_null);

    // save path
    return (lx_path_ref_t)lx_object_stack_save(path->path_stack);
}

lx_void_t lx_canvas_path_load(lx_canvas_ref_t self) {
    lx_canvas_t* path = (lx_canvas_t*)self;
    lx_assert_and_check_return(path && path->path_stack);

    // load path
    lx_object_stack_load(path->path_stack);
}
