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
 * @file        tessellator.h
 *
 */
#ifndef LX_CORE_TESS_TESSELLATOR_H
#define LX_CORE_TESS_TESSELLATOR_H

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
 * algorithm
 */

/*! the tessellate polygon algorithm
 *
 * the algorithm is based on libtess2 here and
 * we optimizated some implementation and fixed some bugs.
 *
 * the differents between our algorithm and libtess2's algorithm:
 *
 *     - we change the coordinate system and the sweep direction (sweep line by horizontal here).
 *
 *     - we need not project the vertices because our graphic engine is 2d,
 *       so our algorithm will be faster.
 *
 *     - we processed more cases of the intersection with some numerical errors,
 *       so our algorithm will be more stable.
 *
 *     - we change the algorithm of comparing the active edge and make it more stable for numerical errors.
 *
 *     - we optimizate the algorithm of merging into the convex polygon from the triangulated mesh.
 *       we have not counted the vertices for each region, so it will be faster than libtess2.
 *
 * (you can see libtess2/alg_outline.md if want to known more details of algorithm.)
 *
 * there are four stages to the algorithm:
 *
 *     1. build a mesh (DCEL, be similar to quad-edge) from polygon.
 *
 *     2. tessellate the mesh into the monotone regions if the polygon is concave.
 *
 *     3. triangulate the monotone regions.
 *
 *     4. merge the triangulated regions into the convex regions.
 *
 * there are seven stages to the tessellation algorithm:
 *
 *     1. simplify the mesh and process some degenerate cases.
 *
 *     2. build a vertex event queue and sort it (uses the priority queue with min-heap).
 *
 *     3. build an active edge region list and sort it (uses the partial insertion sort).
 *
 *     4. sweep all events from the event queue using the Bentley-Ottman line-sweep algorithm
 *        and calculate the intersection and winding number.
 *
 *     5. we need fix it if the intersection with numerical errors violate
 *        the mesh topology or active edge list ordering.
 *
 *     6. process some degenerate cases for the mesh faces which were generated when we fixed some cases.
 *
 *     7. get the monotone regions with the left face marked "inside"
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the polygon tessellator mode enum
 *
 * monotone > convex > triangulation
 */
typedef enum lx_tessellator_mode_e_ {
    LX_TESSELLATOR_MODE_CONVEX        = 0     //!< make convex polygon
,   LX_TESSELLATOR_MODE_MONOTONE      = 1     //!< make monotone polygon
,   LX_TESSELLATOR_MODE_TRIANGULATION = 2     //!< make triangle
}lx_tessellator_mode_e;

/// the polygon tessellator rule enum
typedef enum lx_tessellator_rule_e_ {
    LX_TESSELLATOR_RULE_ODD     = LX_PAINT_FILL_RULE_ODD     //!< the odd rule
,   LX_TESSELLATOR_RULE_NONZERO = LX_PAINT_FILL_RULE_NONZERO //!< the non-zero rule
}lx_tessellator_rule_e;

/// the polygon tessellator ref type
typedef lx_typeref(tessellator);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init the tessellator
 *
 * @return              the tessellator
 */
lx_tessellator_ref_t    lx_tessellator_init(lx_noarg_t);

/*! exit the tessellator
 *
 * @param tessellator   the tessellator
 */
lx_void_t               lx_tessellator_exit(lx_tessellator_ref_t tessellator);

/*! get the tessellator mode
 *
 * @param tessellator   the tessellator
 *
 * @return              the mode
 */
lx_size_t               lx_tessellator_mode(lx_tessellator_ref_t tessellator);

/*! set the tessellator mode
 *
 * @param tessellator   the tessellator
 * @param mode          the mode
 */
lx_void_t               lx_tessellator_mode_set(lx_tessellator_ref_t tessellator, lx_size_t mode);

/*! set the tessellator rule
 *
 * @param tessellator   the tessellator
 * @param rule          the rule
 */
lx_void_t               lx_tessellator_rule_set(lx_tessellator_ref_t tessellator, lx_size_t rule);

/*! tessellate polygon
 *
 * @param tessellator   the tessellator
 * @param polygon       the polygon
 * @param bounds        the polygon bounds
 *
 * @return              the result polygon
 */
lx_polygon_ref_t        lx_tessellator_make(lx_tessellator_ref_t tessellator, lx_polygon_ref_t polygon, lx_rect_ref_t bounds);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


