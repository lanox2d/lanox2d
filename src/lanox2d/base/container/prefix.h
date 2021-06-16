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
 * @file        prefix.h
 *
 */
#ifndef LX_BASE_CONTAINER_PREFIX_H
#define LX_BASE_CONTAINER_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the element type
typedef struct lx_element_t_ {
    lx_size_t size;
    lx_void_t (*free)(lx_pointer_t item);
}lx_element_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline interfaces
 */
static lx_element_t lx_inline lx_element_mem(lx_size_t size, lx_void_t (*free)(lx_pointer_t item)) {
    lx_element_t element;
    element.size = size;
    element.free = free;
    return element;
}

#endif


