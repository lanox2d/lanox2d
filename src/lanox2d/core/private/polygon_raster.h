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
 * @file        polygon_raster.h
 *
 */
#ifndef LX_CORE_PRIVATE_POLYGON_RASTER_H
#define LX_CORE_PRIVATE_POLYGON_RASTER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../paint.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the polygon raster rule enum
typedef enum lx_polygon_raster_rule_e_ {
    LX_POLYGON_RASTER_RULE_ODD      = LX_PAINT_FILL_RULE_ODD     //< odd
,   LX_POLYGON_RASTER_RULE_NONZERO  = LX_PAINT_FILL_RULE_NONZERO //< non-zero
}lx_polygon_raster_rule_e;

// the polygon raster ref type
typedef lx_typeref(polygon_raster);

/* the polygon raster callback type
 *
 * @param lx            the left x-coordinate
 * @param rx            the right x-coordinate
 * @param yb            the top y-coordinate
 * @param ye            the bottom y-coordinate
 * @param udata         the user data
 */
typedef lx_void_t       (*lx_polygon_raster_cb_t)(lx_long_t lx, lx_long_t rx, lx_long_t yb, lx_long_t ye, lx_cpointer_t udata);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* init raster
 *
 * @return              the raster
 */
lx_polygon_raster_ref_t lx_polygon_raster_init(lx_noarg_t);

/* exit raster
 *
 * @param raster        the raster
 */
lx_void_t               lx_polygon_raster_exit(lx_polygon_raster_ref_t raster);

/* make raster
 *
 * @param raster        the raster
 * @param polygon       the polygon
 * @param bounds        the bounds
 * @param rule          the raster rule
 * @param callback      the raster callback
 * @param udata         the user data
 */
lx_void_t               lx_polygon_raster_make(lx_polygon_raster_ref_t raster, lx_polygon_ref_t polygon, lx_rect_ref_t bounds, lx_size_t rule, lx_polygon_raster_cb_t callback, lx_cpointer_t udata);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


