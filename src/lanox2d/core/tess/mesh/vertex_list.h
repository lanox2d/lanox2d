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
 * @file        vertex_list.h
 *
 */
#ifndef LX_CORE_TESS_MESH_VERTEX_LIST_H
#define LX_CORE_TESS_MESH_VERTEX_LIST_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * intervertexs
 */

/* init the mesh vertex list
 *
 * @param element           the element
 *
 * @returned                the vertex list
 */
lx_mesh_vertex_list_ref_t   lx_mesh_vertex_list_init(lx_element_t element);

/* exit the mesh vertex list
 *
 * @param list              the vertex list
 */
lx_void_t                   lx_mesh_vertex_list_exit(lx_mesh_vertex_list_ref_t list);

/* clear the mesh vertex list
 *
 * @param list              the vertex list
 */
lx_void_t                   lx_mesh_vertex_list_clear(lx_mesh_vertex_list_ref_t list);

/*! the mesh vertex count
 *
 * @param list              the list
 *
 * @return                  the item count
 */
lx_size_t                   lx_mesh_vertex_list_size(lx_mesh_vertex_list_ref_t list);

/*! make a bare vertex without edges
 *
 * @param list              the list
 *
 * @return                  the new vertex
 */
lx_mesh_vertex_ref_t        lx_mesh_vertex_list_make(lx_mesh_vertex_list_ref_t list);

/*! kill the vertex, release the storage
 *
 * @param list              the list
 * @param vertex            the vertex
 */
lx_void_t                   lx_mesh_vertex_list_kill(lx_mesh_vertex_list_ref_t list, lx_mesh_vertex_ref_t vertex);

/*! the list order
 *
 * @param list              the list
 *
 * @return                  the order
 */
lx_size_t                   lx_mesh_vertex_list_order(lx_mesh_vertex_list_ref_t list);

/*! set the list order
 *
 * @param list              the list
 * @param order             the order
 */
lx_void_t                   lx_mesh_vertex_list_order_set(lx_mesh_vertex_list_ref_t list, lx_size_t order);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


