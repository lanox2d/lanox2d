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
 * @file        array.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "array.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

lx_array_ref_t lx_array_init(lx_size_t grow, lx_size_t itemsize, lx_element_free_t free, lx_cpointer_t udata) {
    return lx_null;
}

lx_void_t lx_array_exit(lx_array_ref_t array) {
}

lx_pointer_t lx_array_data(lx_array_ref_t array) {
    return lx_null;
}

lx_pointer_t lx_array_head(lx_array_ref_t array) {
    return lx_null;
}

lx_pointer_t lx_array_last(lx_array_ref_t array) {
    return lx_null;
}

lx_bool_t lx_array_resize(lx_array_ref_t array, lx_size_t size) {
    return lx_true;
}

lx_void_t lx_array_clear(lx_array_ref_t array) {
}

lx_void_t lx_array_copy(lx_array_ref_t array, lx_array_ref_t copy) {
}

lx_void_t lx_array_insert_prev(lx_array_ref_t array, lx_size_t itor, lx_cpointer_t data) {
}

lx_void_t lx_array_insert_next(lx_array_ref_t array, lx_size_t itor, lx_cpointer_t data) {
}

lx_void_t lx_array_insert_head(lx_array_ref_t array, lx_cpointer_t data) {
}

lx_void_t lx_array_insert_tail(lx_array_ref_t array, lx_cpointer_t data) {
}

lx_void_t lx_array_replace(lx_array_ref_t array, lx_size_t itor, lx_cpointer_t data) {
}

lx_void_t lx_array_replace_head(lx_array_ref_t array, lx_cpointer_t data) {
}

lx_void_t lx_array_replace_last(lx_array_ref_t array, lx_cpointer_t data) {
}

lx_void_t lx_array_remove(lx_array_ref_t array, lx_size_t itor) {
}

lx_void_t lx_array_remove_head(lx_array_ref_t array) {
}

lx_void_t lx_array_remove_last(lx_array_ref_t array) {
}

lx_size_t lx_array_size(lx_array_ref_t array) {
    return 0;
}

lx_void_t lx_array_foreach(lx_array_ref_t array, lx_element_foreach_t foreach, lx_cpointer_t udata) {
}



