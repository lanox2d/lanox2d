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
#include "device/prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

lx_void_t lx_device_exit(lx_device_ref_t self) {
    lx_device_t* device = (lx_device_t*)self;
    if (device && device->exit) {
        device->exit(device);
    }
}

lx_size_t lx_device_pixfmt(lx_device_ref_t self) {
    lx_device_t* device = (lx_device_t*)self;
    return device? device->pixfmt : LX_PIXFMT_NONE;
}

lx_size_t lx_device_width(lx_device_ref_t self) {
    lx_device_t* device = (lx_device_t*)self;
    return device? device->width : 0;
}

lx_size_t lx_device_height(lx_device_ref_t self) {
    lx_device_t* device = (lx_device_t*)self;
    return device? device->height : 0;
}

lx_void_t lx_device_resize(lx_device_ref_t self, lx_size_t width, lx_size_t height) {
    lx_device_t* device = (lx_device_t*)self;
    if (device && device->resize) {
        device->resize(device, width, height);
    }
}

lx_void_t lx_device_draw_clear(lx_device_ref_t self, lx_color_t color) {
    lx_device_t* device = (lx_device_t*)self;
    if (device && device->draw_clear) {
        device->draw_clear(device, color);
    }
}

