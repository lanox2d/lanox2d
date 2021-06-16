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
 * @file        edge_list.h
 *
 */
#ifndef LX_CORE_TESS_MESH_EDGE_LIST_H
#define LX_CORE_TESS_MESH_EDGE_LIST_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the mesh edge list ref type
typedef lx_typeref(mesh_edge_list);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interedges
 */

/* init the mesh edge list
 *
 * @param itemsize          the item size
 * @param itemfree          the free function of element item
 *
 * @return                  the edge list
 */
lx_mesh_edge_list_ref_t     lx_mesh_edge_list_init(lx_size_t itemsize, lx_element_free_t itemfree);

/* exit the mesh edge list
 *
 * @param list              the edge list
 */
lx_void_t                   lx_mesh_edge_list_exit(lx_mesh_edge_list_ref_t list);

/* clear the mesh edge list
 *
 * @param list              the edge list
 */
lx_void_t                   lx_mesh_edge_list_clear(lx_mesh_edge_list_ref_t list);

/*! the mesh edge iterator
 *
 * @param list              the list
 *
 * @return                  the mesh edge iterator
 */
lx_iterator_ref_t           lx_mesh_edge_list_itor(lx_mesh_edge_list_ref_t list);

/*! the mesh edge count
 *
 * @param list              the list
 *
 * @return                  the item count
 */
lx_size_t                   lx_mesh_edge_list_size(lx_mesh_edge_list_ref_t list);

/*! the mesh edge maximum count
 *
 * @param list              the list
 *
 * @return                  the item max count
 */
lx_size_t                   lx_mesh_edge_list_maxn(lx_mesh_edge_list_ref_t list);

/*! make a unconnected edge
 *
 * <pre>
 *
 *  O -----> D
 *
 * </pre>
 *
 * @param list              the list
 *
 * @return                  the new edge
 */
lx_mesh_edge_ref_t          lx_mesh_edge_list_make(lx_mesh_edge_list_ref_t list);

/*! make a self-loop edge
 *
 * create a looping edge that connects to itself at a single vertex
 *
 * <pre>
 *
 * clockwise:
 *
 *          -------
 *         |       |
 *         |       |
 *         |       |
 *         O/D <---
 *
 * counter-clockwise:
 *
 *          -------
 *         |       |
 *         |       |
 *        \|/      |
 *         D/O ----
 *
 * </pre>
 *
 * @param list              the list
 *
 * @return                  the new edge
 */
lx_mesh_edge_ref_t          lx_mesh_edge_list_make_loop(lx_mesh_edge_list_ref_t list, lx_bool_t is_ccw);

#ifdef LX_DEBUG
/*! make the edge info string
 *
 * @param list              the list
 * @param edge              the edge
 * @param data              the string data
 * @param maxn              the string maxn
 *
 * @return                  the string size
 */
lx_long_t                   lx_mesh_edge_list_cstr(lx_mesh_edge_list_ref_t list, lx_mesh_edge_ref_t edge, lx_char_t* data, lx_size_t maxn);
#endif

/*! kill the edge, release the storage
 *
 * @param list              the list
 * @param edge              the edge
 */
lx_void_t                   lx_mesh_edge_list_kill(lx_mesh_edge_list_ref_t list, lx_mesh_edge_ref_t edge);

/*! the user data
 *
 * @param list              the list
 * @param edge              the edge
 *
 * @return                  the user data
 */
lx_cpointer_t               lx_mesh_edge_list_data(lx_mesh_edge_list_ref_t list, lx_mesh_edge_ref_t edge);

/*! set the user private data
 *
 * @param list              the list
 * @param edge              the edge
 * @param data              the user data
 */
lx_void_t                   lx_mesh_edge_list_data_set(lx_mesh_edge_list_ref_t list, lx_mesh_edge_ref_t edge, lx_cpointer_t data);

/*! the list order
 *
 * @param list              the list
 *
 * @return                  the order
 */
lx_size_t                   lx_mesh_edge_list_order(lx_mesh_edge_list_ref_t list);

/*! set the list order
 *
 * @param list              the list
 * @param order             the order
 */
lx_void_t                   lx_mesh_edge_list_order_set(lx_mesh_edge_list_ref_t list, lx_size_t order);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


