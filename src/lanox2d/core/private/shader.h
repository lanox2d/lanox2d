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
 * @file        shader.h
 *
 */
#ifndef LX_CORE_PRIVATE_SHADER_H
#define LX_CORE_PRIVATE_SHADER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the shader type
typedef struct lx_shader_t_ {
    lx_uint8_t          type;
    lx_uint8_t          tile_mode;
    lx_matrix_t         matrix;
    lx_pointer_t        devdata; // the device private data
    lx_void_t           (*devdata_free)(lx_shader_ref_t shader);
}lx_shader_t;

// the linear grandient shader type
typedef struct lx_linear_gradient_shader_t_ {
    lx_shader_t         base;
    lx_gradient_t       gradient;
    lx_line_t           line;
}lx_linear_gradient_shader_t;

// the radial gradient shader type
typedef struct lx_radial_gradient_shader_t_ {
    lx_shader_t         base;
    lx_gradient_t       gradient;
    lx_circle_t         circle;
}lx_radial_gradient_shader_t;

// the bitmap shader type
typedef struct lx_bitmap_shader_t_ {
    lx_shader_t         base;
    lx_bitmap_ref_t     bitmap;
}lx_bitmap_shader_t;

#endif


