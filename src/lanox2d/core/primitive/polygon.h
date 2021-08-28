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
 * @file        polygon.h
 *
 */
#ifndef LX_CORE_PRIMITIVE_POLYGON_H
#define LX_CORE_PRIMITIVE_POLYGON_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline interfaces
 */

/*! make polygon
 *
 * @param polygon   the polygon
 * @param points    the points
 * @param counts    the counts
 * @param total     the total count
 * @param convex    is convex?
 */
static lx_inline lx_void_t lx_polygon_make(lx_polygon_ref_t polygon, lx_point_ref_t points, lx_uint16_t* counts, lx_size_t total, lx_bool_t convex) {
    polygon->points = points;
    polygon->counts = counts;
    polygon->total  = total;
    polygon->convex = convex;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


