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
 * @file        active_region.h
 */
#ifndef LX_CORE_TESS_TESSELLATOR_ACTIVE_REGION_H
#define LX_CORE_TESS_TESSELLATOR_ACTIVE_REGION_H

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

/* make the active regions and all regions are sorted
 *
 * @param tessellator               the tessellator
 * @param bounds                    the bounds of the polygon
 *
 * @return                          lx_true or lx_false
 */
lx_bool_t                           lx_tessellator_active_regions_make(lx_tessellator_t* tessellator, lx_rect_ref_t bounds);

/* find the region containing the given edge from the regions
 *
 *         =>
 * .                .
 * .                .
 * .     region     .
 * .                .
 * .         .      .
 * .       .        .
 * .     . edge     .
 *
 * @param tessellator               the tessellator tessellator
 * @param edge                      the edge
 *
 * @return                          the region
 */
lx_tessellator_active_region_ref_t  lx_tessellator_active_regions_find(lx_tessellator_t* tessellator, lx_mesh_edge_ref_t edge);

/* get the left region of the given region
 *
 * @param tessellator               the tessellator tessellator
 * @param region                    the region
 *
 * @return                          the left region
 */
lx_tessellator_active_region_ref_t  lx_tessellator_active_regions_left(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region);

/* get the right region of the given region
 *
 * @param tessellator               the tessellator tessellator
 * @param region                    the region
 *
 * @return                          the right region
 */
lx_tessellator_active_region_ref_t  lx_tessellator_active_regions_right(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region);

/* the region1 is in the left of the region2
 *
 * @param tessellator               the tessellator tessellator
 * @param region1                   the region1
 * @param region2                   the region2
 *
 * @return                          lx_true or lx_false
 */
lx_bool_t                           lx_tessellator_active_regions_in_left(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region1, lx_tessellator_active_region_ref_t region2);

/* remove region from the regions
 *
 * @param tessellator               the tessellator tessellator
 * @param region                    the removed region
 */
lx_void_t                           lx_tessellator_active_regions_remove(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region);

/* insert a new active region in ascending order
 *
 * @code
    {
        // init region
        lx_tessellator_active_region_t region;
        region.edge         = ...; //< must be initialized
        region.inside       = 0;
        region.winding      = 0;
        ...

        // insert region
        lx_tessellator_active_regions_insert(active_regions, &region);
    }
 * @endcode
 *
 * @param tessellator               the tessellator tessellator
 * @param region                    the initialized region, but be not real region
 *
 * @return                          the real new region
 */
lx_tessellator_active_region_ref_t  lx_tessellator_active_regions_insert(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region);

/* insert a new active region after the previous region in ascending order
 *
 * r0 ----> r1 ------> r2 -------> r3 ---> ... ---->
 *                 region_prev
 *                     |----------------->
 *                           insert
 *
 * @param tessellator               the tessellator tessellator
 * @param region_prev               the previous region
 * @param region                    the initialized region, but be not real region
 *
 * @return                          the real new region
 */
lx_tessellator_active_region_ref_t  lx_tessellator_active_regions_insert_after(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region_prev, lx_tessellator_active_region_ref_t region);

#ifdef LX_DEBUG
/* check the active regions
 *
 * @param tessellator               the tessellator tessellator
 */
lx_void_t                           lx_tessellator_active_regions_check(lx_tessellator_t* tessellator);
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


