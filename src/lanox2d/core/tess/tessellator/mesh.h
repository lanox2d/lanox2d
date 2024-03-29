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
 * @file        mesh.h
 */
#ifndef LX_CORE_TESS_TESSELLATOR_MESH_H
#define LX_CORE_TESS_TESSELLATOR_MESH_H

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

/* make mesh from the given polygon
 *
 * @param tessellator   the tessellator
 * @param polygon       the polygon
 *
 * @return              lx_true or lx_false
 */
lx_bool_t               lx_tessellator_mesh_make(lx_tessellator_t* tessellator, lx_polygon_ref_t polygon);

/* make an empty non-loop edge and initialize it
 *
 * @param tessellator   the tessellator
 * @param org           the original point, do not initialize it if be null
 * @param dst           the destinate point, do not initialize it if be null
 *
 * @return              the edge
 */
lx_mesh_edge_ref_t      lx_tessellator_mesh_make_edge(lx_tessellator_t* tessellator, lx_point_ref_t org, lx_point_ref_t dst);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


