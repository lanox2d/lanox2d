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
 * @file        prefix.h
 *
 */
#ifndef LX_CORE_TESS_TESSELLATOR_PREFIX_H
#define LX_CORE_TESS_TESSELLATOR_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../mesh.h"
#include "../tessellator.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the tessellator edge
#define lx_tessellator_edge(edge)                       ((lx_tessellator_edge_ref_t)lx_mesh_edge_data(edge))

// the winding of tessellator edge
#define lx_tessellator_edge_winding(edge)               (lx_tessellator_edge(edge)->winding)

// set the winding of tessellator edge
#define lx_tessellator_edge_winding_set(edge, val)      do { lx_tessellator_edge(edge)->winding = (val); } while (0)

// add the winding of tessellator edge
#define lx_tessellator_edge_winding_add(edge, val)      do { lx_tessellator_edge(edge)->winding += (val); } while (0)

/* merge the winding of two tessellator edges
 *
 * when we merge two edges into one, we need to compute the combined winding of the new edge.
 */
#define lx_tessellator_edge_winding_merge(edge, other)  do { lx_tessellator_edge_winding_add(edge, lx_tessellator_edge_winding(other)); lx_tessellator_edge_winding_add(lx_mesh_edge_sym(edge), lx_tessellator_edge_winding(lx_mesh_edge_sym(other))); } while (0)

// the region of tessellator edge
#define lx_tessellator_edge_region(edge)                (lx_tessellator_edge(edge)->region)

// set the region of tessellator edge
#define lx_tessellator_edge_region_set(edge, val)       do { lx_tessellator_edge(edge)->region = (val); } while (0)

// the tessellator face
#define lx_tessellator_face(face)                       ((lx_tessellator_face_ref_t)lx_mesh_face_data(face))

// the tessellator face inside
#define lx_tessellator_face_inside(face)                (lx_tessellator_face(face)->inside)

// set the tessellator face inside
#define lx_tessellator_face_inside_set(face, val)       do { lx_tessellator_face(face)->inside = (val); } while (0)

// the tessellator vertex
#define lx_tessellator_vertex(vertex)                   ((lx_tessellator_vertex_ref_t)lx_mesh_vertex_data(vertex))

// the tessellator vertex point
#define lx_tessellator_vertex_point(vertex)             (&(lx_tessellator_vertex(vertex)->point))

// set the tessellator vertex point
#define lx_tessellator_vertex_point_set(vertex, val)    do { lx_tessellator_vertex(vertex)->point = *(val); } while (0)

// define the local tessellator vertex variable
#define lx_tessellator_vertex_local(name) \
    lx_byte_t __name##_data[sizeof(lx_mesh_vertex_t) + sizeof(lx_tessellator_vertex_t)]; \
    lx_mesh_vertex_ref_t name = (lx_mesh_vertex_ref_t)__name##_data;

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/* the active region type
 *
 *  /.\                                     .              /.\
 *   .                 .                   .   .            .
 *   .               .   .                .       .         .
 *   .  region1    .       .             .           .      .
 *   .           .           .  region3 .                   .
 *   . ------- . - region2 --- . ---- event --------------- . ----- sweep line
 *   .       .                   .     .                    .
 *   .    edge2                edge3   .    region4         . region5
 *   .                                 .                    .
 * edge1                             edge4                edge5
 *(bound)                                                (bound)
 */
typedef struct lx_tessellator_active_region_t_ {

    // the region position
    lx_size_t                           position;

    // the left edge and it goes up
    lx_mesh_edge_ref_t                  edge;

    // the winding
    lx_long_t                           winding;

    // is dirty? we need fix the region ordering
    lx_uint8_t                          dirty   : 1;

    // is inside?
    lx_uint8_t                          inside  : 1;

    // is bounds?
    lx_uint8_t                          bounds  : 1;

    /* the left edge need fix?
     *
     * see lx_tessellator_connect_bottom_event
     */
    lx_uint8_t                          fixedge : 1;

}lx_tessellator_active_region_t, *lx_tessellator_active_region_ref_t;

// the tessellator edge type
typedef struct lx_tessellator_edge_t_ {
    lx_int32_t                          winding;
    lx_tessellator_active_region_ref_t  region;
} lx_tessellator_edge_t, *lx_tessellator_edge_ref_t;

// the tessellator face type
typedef struct lx_tessellator_face_t_ {
    lx_uint8_t                          inside      : 1;
} lx_tessellator_face_t, *lx_tessellator_face_ref_t;

// the tessellator vertex type
typedef struct lx_tessellator_vertex_t_ {
    lx_point_t                          point;
} lx_tessellator_vertex_t, *lx_tessellator_vertex_ref_t;

// TODO
typedef lx_typeref(list);
typedef lx_typeref(priority_queue);

// the tessellator type
typedef struct lx_tessellator_t_ {

    // the mode
    lx_size_t                           mode;

    // the winding rule
    lx_size_t                           rule;

    // the callback
    lx_tessellator_cb_t                 callback;

    // the user private data
    lx_cpointer_t                       udata;

    // the mesh
    lx_mesh_ref_t                       mesh;

    // the current sweep event for the active_region.leq
    lx_mesh_vertex_ref_t                event;

    // the output points
    lx_vector_ref_t                     outputs;

    // the event queue for vertex
    lx_priority_queue_ref_t             event_queue;

    // the active regions
    lx_list_ref_t                       active_regions;

}lx_tessellator_t;

#endif


