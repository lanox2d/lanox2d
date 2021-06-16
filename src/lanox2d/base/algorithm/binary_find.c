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
 * @file        binary_find.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "binary_find.h"
#include "binary_find_if.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_size_t lx_binary_find(lx_iterator_ref_t iterator, lx_size_t head, lx_size_t tail, lx_cpointer_t item) {
    return lx_binary_find_if(iterator, head, tail, lx_iterator_comp, item);
}
lx_size_t lx_binary_find_all(lx_iterator_ref_t iterator, lx_cpointer_t item) {
    return lx_binary_find_all_if(iterator, lx_iterator_comp, item);
}

