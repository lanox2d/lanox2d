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
 * @file        paint.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "paint.h"
#include "quality.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_clipper_ref_t lx_clipper_init() {
    return (lx_clipper_ref_t)1;
}

lx_void_t lx_clipper_exit(lx_clipper_ref_t self) {
    lx_trace_noimpl();
}

lx_size_t lx_clipper_size(lx_clipper_ref_t self) {
    lx_trace_noimpl();
    return 0;
}

lx_void_t lx_clipper_clear(lx_clipper_ref_t self) {
    lx_trace_noimpl();
}

lx_void_t lx_clipper_copy(lx_clipper_ref_t self, lx_clipper_ref_t copied) {
    lx_trace_noimpl();
}

lx_matrix_ref_t lx_clipper_matrix(lx_clipper_ref_t self) {
    lx_trace_noimpl();
    return lx_null;
}

lx_void_t lx_clipper_matrix_set(lx_clipper_ref_t self, lx_matrix_ref_t matrix) {
    lx_trace_noimpl();
}

lx_void_t lx_clipper_add_path(lx_clipper_ref_t self, lx_size_t mode, lx_path_ref_t path) {
    lx_trace_noimpl();
}

lx_void_t lx_clipper_add_triangle(lx_clipper_ref_t self, lx_size_t mode, lx_triangle_ref_t triangle) {
    lx_trace_noimpl();
}

lx_void_t lx_clipper_add_rect(lx_clipper_ref_t self, lx_size_t mode, lx_rect_ref_t rect) {
    lx_trace_noimpl();
}

lx_void_t lx_clipper_add_round_rect(lx_clipper_ref_t self, lx_size_t mode, lx_round_rect_ref_t rect) {
}

lx_void_t lx_clipper_add_circle(lx_clipper_ref_t self, lx_size_t mode, lx_circle_ref_t circle) {
    lx_trace_noimpl();
}

lx_void_t lx_clipper_add_ellipse(lx_clipper_ref_t self, lx_size_t mode, lx_ellipse_ref_t ellipse) {
    lx_trace_noimpl();
}
