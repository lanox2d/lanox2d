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
 * @file        device.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "device.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef LX_CONFIG_DEVICE_HAVE_BITMAP
lx_device_ref_t lx_device_init_from_bitmap(lx_bitmap_ref_t bitmap) {
    return lx_null;
}
#endif

lx_void_t lx_device_exit(lx_device_ref_t self) {
}

lx_size_t lx_device_type(lx_device_ref_t self) {
    return 0;
}

lx_size_t lx_device_pixfmt(lx_device_ref_t self) {
    return 0;
}

lx_size_t lx_device_width(lx_device_ref_t self) {
    return 0;
}

lx_size_t lx_device_height(lx_device_ref_t self) {
    return 0;
}

lx_void_t lx_device_resize(lx_device_ref_t self, lx_size_t width, lx_size_t height) {
}

lx_void_t lx_device_draw_clear(lx_device_ref_t self, lx_color_t color) {
}

