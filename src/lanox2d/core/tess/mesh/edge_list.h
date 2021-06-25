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
 * interedges
 */

/* init the mesh edge list
 *
 * @param element           the element
 *
 * @return                  the edge list
 */
lx_mesh_edge_list_ref_t     lx_mesh_edge_list_init(lx_element_t element);

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

/*! the mesh edge count
 *
 * @param list              the list
 *
 * @return                  the item count
 */
lx_size_t                   lx_mesh_edge_list_size(lx_mesh_edge_list_ref_t list);

/*! the head edge
 *
 * @param list              the list
 *
 * @return                  the edge
 */
lx_mesh_edge_ref_t          lx_mesh_edge_list_head(lx_mesh_edge_list_ref_t list);

/*! the last edge
 *
 * @param list              the list
 *
 * @return                  the edge
 */
lx_mesh_edge_ref_t          lx_mesh_edge_list_last(lx_mesh_edge_list_ref_t list);

/*! the tail edge for walking edges more flexible and faster than iterator
 *
 * @code
 * lx_mesh_edge_ref_t edge = lx_mesh_edge_list_head(list);
 * lx_mesh_edge_ref_t tail = lx_mesh_edge_list_tail(list);
 * for (; edge != tail; edge = lx_mesh_edge_next(edge))
 * {
 * }
 * @endcode
 *
 * @param list              the list
 *
 * @return                  the edge, it's members are invalid and cannot modify them
 */
lx_mesh_edge_ref_t          lx_mesh_edge_list_tail(lx_mesh_edge_list_ref_t list);

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

/*! kill the edge, release the storage
 *
 * @param list              the list
 * @param edge              the edge
 */
lx_void_t                   lx_mesh_edge_list_kill(lx_mesh_edge_list_ref_t list, lx_mesh_edge_ref_t edge);

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


