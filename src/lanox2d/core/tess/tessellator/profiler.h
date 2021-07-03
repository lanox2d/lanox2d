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
 * @file        profiler.h
 */
#ifndef LX_UTILS_IMPL_TESSELLATOR_PROFILER_H
#define LX_UTILS_IMPL_TESSELLATOR_PROFILER_H

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

/*! init profiler file to the temporary directory, name: profiler_xxx.html
 *
 * this is a html file using raphael.js(www.raphaeljs.com) and
 * it will draw the details of making monotone mesh.
 *
 * we can profile and debug the monotone implementation using the generated html file.
 *
 * files:
 * - the temporary directory
 *   - raphael.js
 *   - profiler_0.html
 *   - profiler_1.html
 *   - profiler_2.html
 *   - ...
 *
 * @param bounds    the polygon bounds
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_tessellator_profiler_init(lx_rect_ref_t bounds);

/// exit the profiler
lx_void_t           lx_tessellator_profiler_exit(lx_noarg_t);

/*! add edge to the profiler
 *
 * @param edge      the edge
 */
lx_void_t           lx_tessellator_profiler_add_edge(lx_mesh_edge_ref_t edge);

/*! add split to the profiler
 *
 * @param edge      the edge
 */
lx_void_t           lx_tessellator_profiler_add_split(lx_mesh_edge_ref_t edge);

/*! add patch to the profiler
 *
 * @param edge      the edge
 */
lx_void_t           lx_tessellator_profiler_add_patch(lx_mesh_edge_ref_t edge);

/*! add intersection to the profiler
 *
 * @param inter     the intersection
 */
lx_void_t           lx_tessellator_profiler_add_inter(lx_mesh_vertex_ref_t inter);

/*! finish region
 *
 * @param region    the region
 */
lx_void_t           lx_tessellator_profiler_finish_region(lx_tessellator_active_region_ref_t region);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


