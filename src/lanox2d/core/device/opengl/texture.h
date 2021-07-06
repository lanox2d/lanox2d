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
 * @file        texture.h
 *
 */
#ifndef LX_CORE_DEVICE_OPENGL_TEXTURE_H
#define LX_CORE_DEVICE_OPENGL_TEXTURE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "device.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interface
 */

/*! create linear gradient texture
 *
 * @param device    the device
 * @param mode      the mode
 * @param gradient  the gradient
 * @param line      the line
 *
 * @return          the texture
 */
lx_texture_ref_t    lx_device_opengl_create_linear_texture(lx_device_ref_t device, lx_size_t mode, lx_gradient_ref_t gradient, lx_line_ref_t line);

/*! create radial gradient texture
 *
 * @param device    the device
 * @param mode      the mode
 * @param gradient  the gradient
 * @param circle    the circle
 *
 * @return          the texture
 */
lx_texture_ref_t    lx_device_opengl_create_radial_texture(lx_device_ref_t device, lx_size_t mode, lx_gradient_ref_t gradient, lx_circle_ref_t circle);

/*! create bitmap texture
 *
 * @param device    the device
 * @param mode      the mode
 * @param bitmap    the bitmap
 *
 * @return          the texture
 */
lx_texture_ref_t    lx_device_opengl_create_bitmap_texture(lx_device_ref_t device, lx_size_t mode, lx_bitmap_ref_t bitmap);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif
