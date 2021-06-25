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
 * @file        face_list.h
 */
#ifndef LX_CORE_TESS_MESH_FACE_LIST_H
#define LX_CORE_TESS_MESH_FACE_LIST_H

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

/* init the mesh face list
 *
 * @param element           the element
 *
 * @return                  the face list
 */
lx_mesh_face_list_ref_t     lx_mesh_face_list_init(lx_element_t element);

/* exit the mesh face list
 *
 * @param list              the face list
 */
lx_void_t                   lx_mesh_face_list_exit(lx_mesh_face_list_ref_t list);

/* clear the mesh face list
 *
 * @param list              the face list
 */
lx_void_t                   lx_mesh_face_list_clear(lx_mesh_face_list_ref_t list);

/*! the mesh face count
 *
 * @param list              the list
 *
 * @return                  the item count
 */
lx_size_t                   lx_mesh_face_list_size(lx_mesh_face_list_ref_t list);

/*! the head face
 *
 * @param list              the list
 *
 * @return                  the face
 */
lx_mesh_face_ref_t          lx_mesh_face_list_head(lx_mesh_face_list_ref_t list);

/*! the last face
 *
 * @param list              the list
 *
 * @return                  the face
 */
lx_mesh_face_ref_t          lx_mesh_face_list_last(lx_mesh_face_list_ref_t list);

/*! make a bare face without faces
 *
 * @param list              the list
 *
 * @return                  the new face
 */
lx_mesh_face_ref_t          lx_mesh_face_list_make(lx_mesh_face_list_ref_t list);

/*! kill the face, release the storage
 *
 * @param list              the list
 * @param face              the face
 */
lx_void_t                   lx_mesh_face_list_kill(lx_mesh_face_list_ref_t list, lx_mesh_face_ref_t face);

/*! the list order
 *
 * @param list              the list
 *
 * @return                  the order
 */
lx_size_t                   lx_mesh_face_list_order(lx_mesh_face_list_ref_t list);

/*! set the list order
 *
 * @param list              the list
 * @param order             the order
 */
lx_void_t                   lx_mesh_face_list_order_set(lx_mesh_face_list_ref_t list, lx_size_t order);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


