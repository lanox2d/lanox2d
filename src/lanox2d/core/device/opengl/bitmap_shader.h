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
 * @file        bitmap_shader.h
 *
 */
#ifndef LX_CORE_DEVICE_OPENGL_BITMAP_SHADER_H
#define LX_CORE_DEVICE_OPENGL_BITMAP_SHADER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "gl.h"
#include "../../private/shader.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the bitmap shader devdata type
typedef struct lx_bitmap_shader_devdata_t_ {
    lx_matrix_t     matrix;
    lx_GLuint_t     texture;
}lx_bitmap_shader_devdata_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* get devdata of bitmap shader
 *
 * @param shader            the bitmap shader
 *
 * @return                  the devdata
 */
lx_bitmap_shader_devdata_t* lx_bitmap_shader_devdata(lx_bitmap_shader_t* shader);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif
