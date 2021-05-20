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
#ifndef LX_CORE_BASICTYPE_PREFIX_H
#define LX_CORE_BASICTYPE_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/// the max width
#define LX_WIDTH_MAX           (8192)

/// the max height
#define LX_HEIGHT_MAX          (8192)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the pixel type
typedef lx_uint32_t                 lx_pixel_t;

/// the bitmap ref type
typedef lx_typeref(bitmap);

/// the canvas ref type
typedef lx_typeref(canvas);

/// the window ref type
typedef lx_typeref(window);

/*! @def the color type
 *
 * the color type
 *
 * @code
 * union lx_c2p_t_ {
 *      lx_color_t c;
 *      lx_pixel_t p;
 * }c2p;
 * c2p.c = c;
 * return c2p.p;
 * @endcode
 *
 */
#ifdef LX_WORDS_BIGENDIAN
typedef struct lx_color_t_ {
    lx_byte_t           a;
    lx_byte_t           r;
    lx_byte_t           g;
    lx_byte_t           b;
}lx_color_t, *lx_color_ref_t;
#else
typedef struct lx_color_t_ {
    lx_byte_t           b;
    lx_byte_t           g;
    lx_byte_t           r;
    lx_byte_t           a;
}lx_color_t, *lx_color_ref_t;
#endif

#endif


