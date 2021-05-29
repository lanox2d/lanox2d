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
 * @file        stack.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "stack.h"
#include "array.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

lx_stack_ref_t lx_stack_init(lx_size_t grow, lx_size_t itemsize, lx_element_free_t itemfree) {
    return (lx_stack_ref_t)lx_array_init(grow, itemsize, itemfree);
}

lx_void_t lx_stack_exit(lx_stack_ref_t self) {
    lx_array_exit((lx_array_ref_t)self);
}

lx_pointer_t lx_stack_head(lx_stack_ref_t self) {
    return lx_array_head((lx_array_ref_t)self);
}

lx_pointer_t lx_stack_last(lx_stack_ref_t self) {
    return lx_array_last((lx_array_ref_t)self);
}

lx_void_t lx_stack_clear(lx_stack_ref_t self) {
    lx_array_clear((lx_array_ref_t)self);
}

lx_void_t lx_stack_copy(lx_stack_ref_t self, lx_stack_ref_t copy) {
    lx_array_copy((lx_array_ref_t)self, (lx_array_ref_t)copy);
}

lx_void_t lx_stack_put(lx_stack_ref_t self, lx_cpointer_t data) {
    lx_array_insert_tail((lx_array_ref_t)self, data);
}

lx_void_t lx_stack_pop(lx_stack_ref_t self) {
    lx_array_remove_last((lx_array_ref_t)self);
}

lx_pointer_t lx_stack_top(lx_stack_ref_t self) {
    return lx_array_last((lx_array_ref_t)self);
}

lx_size_t lx_stack_size(lx_stack_ref_t self) {
    return lx_array_size((lx_array_ref_t)self);
}

lx_void_t lx_stack_foreach(lx_stack_ref_t self, lx_element_foreach_t foreach, lx_cpointer_t udata) {
    lx_array_foreach((lx_array_ref_t)self, foreach, udata);
}



