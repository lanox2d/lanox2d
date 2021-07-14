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
 * @file        color.h
 *
 */
#ifndef LX_CORE_DEVICE_SKIA_COLOR_H
#define LX_CORE_DEVICE_SKIA_COLOR_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// get skia color
#define lx_skia_color(color)    static_cast<SkColor>(lx_color_pixel(color))

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline interfaces
 */

// get skia color type
static lx_inline SkColorType lx_skia_color_type(lx_size_t color) {
    switch (color) {
    case LX_PIXFMT_RGB565:
        return kRGB_565_SkColorType;
    case LX_PIXFMT_RGBA8888:
    case LX_PIXFMT_RGBX8888:
        return kRGBA_8888_SkColorType;
    default:
        break;
    }
    return kUnknown_SkColorType;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


