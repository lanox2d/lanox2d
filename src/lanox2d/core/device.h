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
 * @file        device.h
 *
 */
#ifndef LX_CORE_DEVICE_H
#define LX_CORE_DEVICE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init device from bitmap
 *
 * @param bitmap    the bitmap
 *
 * @return          the device
 */
lx_device_ref_t     lx_device_init_from_bitmap(lx_bitmap_ref_t bitmap);

/*! exit device
 *
 * @param device    the device
 */
lx_void_t           lx_device_exit(lx_device_ref_t device);

/*! get the device width
 *
 * @param device    the device
 *
 * @return          the width
 */
lx_size_t           lx_device_width(lx_device_ref_t device);

/*! get the device height
 *
 * @param device    the device
 *
 * @return          the height
 */
lx_size_t           lx_device_height(lx_device_ref_t device);

/*! resize the device
 *
 * @param device    the device
 * @param width     the width
 * @param height    the height
 */
lx_void_t           lx_device_resize(lx_device_ref_t device, lx_size_t width, lx_size_t height);

/*! clear draw and fill the given color
 *
 * @param device    the device
 * @param color     the color
 */
lx_void_t           lx_device_draw_clear(lx_device_ref_t device, lx_color_t color);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


