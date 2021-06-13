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
 * @file        rect.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "rect.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_void_t lx_bitmap_renderer_fill_rect(lx_bitmap_device_t* device, lx_rect_ref_t rect) {
    lx_assert(device && rect);
    lx_bitmap_biltter_done_r(&device->biltter, (lx_long_t)rect->x, (lx_long_t)rect->y, (lx_long_t)rect->w, (lx_long_t)rect->h);
}
