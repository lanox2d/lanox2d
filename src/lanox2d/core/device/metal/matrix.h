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
 * @file        matrix.h
 *
 */
#ifndef LX_CORE_DEVICE_METAL_MATRIX_H
#define LX_CORE_DEVICE_METAL_MATRIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline interfaces
 */

/* init matrix
 *
 * @param sx        the x-scale
 * @param kx        the x-skew
 * @param ky        the y-skew
 * @param sy        the y-scale
 * @param tx        the x-translate
 * @param ty        the y-translate
 */
static lx_inline lx_void_t lx_metal_matrix_init(lx_metal_matrix_ref_t matrix, lx_float_t sx, lx_float_t kx, lx_float_t ky, lx_float_t sy, lx_float_t tx, lx_float_t ty) {
}

#endif
