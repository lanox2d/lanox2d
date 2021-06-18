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
 *
 */
#ifndef LX_CORE_TESS_MESH_H
#define LX_CORE_TESS_MESH_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/// get the face edge
#define lx_mesh_face_edge(face)                 (lx_assert(face), (face)->edge)

/// get the face user data fastly if the item type is lx_element_mem()
#define lx_mesh_face_data_fastly(face)          (lx_assert(face), (lx_cpointer_t)((lx_mesh_face_ref_t)(face) + 1))

/// get the vertex edge
#define lx_mesh_vertex_edge(vertex)             (lx_assert(vertex), (vertex)->edge)

/// get the vertex user data fastly if the item type is lx_element_mem()
#define lx_mesh_vertex_data_fastly(vertex)      (lx_assert(vertex), (lx_cpointer_t)((lx_mesh_vertex_ref_t)(vertex) + 1))

/// get the edge sym
#define lx_mesh_edge_sym(edge)                  (lx_assert(edge), (edge)->sym)

/// get the edge org
#define lx_mesh_edge_org(edge)                  (lx_assert(edge), (edge)->org)

/// get the edge dst
#define lx_mesh_edge_dst(edge)                  (lx_assert((edge) && (edge)->sym), (edge)->sym->org)

/// get the edge next
#define lx_mesh_edge_next(edge)                 (lx_assert(edge), (edge)->next)

/// get the edge lface
#define lx_mesh_edge_lface(edge)                (lx_assert(edge), (edge)->lface)

/// get the edge rface
#define lx_mesh_edge_rface(edge)                (lx_assert((edge) && (edge)->sym), (edge)->sym->lface)

/// get the edge onext
#define lx_mesh_edge_onext(edge)                (lx_assert(edge), (edge)->onext)

/// get the edge oprev
#define lx_mesh_edge_oprev(edge)                (lx_assert((edge) && (edge)->sym), (edge)->sym->lnext)

/// get the edge lnext
#define lx_mesh_edge_lnext(edge)                (lx_assert(edge), (edge)->lnext)

/// get the edge lprev
#define lx_mesh_edge_lprev(edge)                (lx_assert((edge) && (edge)->onext), (edge)->onext->sym)

/// get the edge rnext
#define lx_mesh_edge_rnext(edge)                (lx_assert(lx_mesh_edge_oprev(edge)), lx_mesh_edge_oprev(edge)->sym)

/// get the edge rprev
#define lx_mesh_edge_rprev(edge)                (lx_assert((edge) && (edge)->sym), (edge)->sym->onext)

/// get the edge dnext
#define lx_mesh_edge_dnext(edge)                (lx_assert(lx_mesh_edge_rprev(edge)), lx_mesh_edge_rprev(edge)->sym)

/// get the edge dprev
#define lx_mesh_edge_dprev(edge)                (lx_assert((edge) && (edge)->lnext), (edge)->lnext->sym)

/// get the edge user data fastly if the item type is lx_element_mem()
#define lx_mesh_edge_data_fastly(edge)          (lx_assert(edge), (lx_cpointer_t)((lx_mesh_edge_ref_t)(edge) + 1))

/// the edge is isolated?
#define lx_mesh_edge_is_isolated(edge)          (lx_assert((edge) && (edge->sym)), (edge)->onext == (edge) && (edge)->sym->onext == (edge)->sym && (edge)->lnext == (edge->sym) && (edge)->sym->lnext == (edge))

/// the edge is isolated loop?
#define lx_mesh_edge_is_isolated_loop(edge)     (lx_assert((edge) && (edge->sym)), (edge)->onext == (edge)->sym && (edge)->sym->onext == (edge) && (edge)->lnext == (edge) && (edge)->sym->lnext == (edge)->sym)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// declaration
struct lx_mesh_edge_t_;

/// the mesh order enum
typedef enum lx_mesh_order_e_ {
    LX_MESH_ORDER_INSERT_TAIL    = 0 //!< insert new edge/face/vertex to the tail of list
,   LX_MESH_ORDER_INSERT_HEAD    = 1 //!< insert new edge/face/vertex to the head of list
}lx_mesh_order_e;

/// the mesh vertex type
typedef struct lx_mesh_vertex_t_ {
    /// the list entry
    lx_list_entry_t            entry;

    /// an arbitrary edge of the vertex
    struct lx_mesh_edge_t_*    edge;

#ifdef LX_DEBUG
    /// the id
    lx_size_t                   id;

    /*! the vertex list for dumping it in the debug mode
     *
     * format: %{mesh_vertex}
     */
    lx_pointer_t                list;
#endif

}lx_mesh_vertex_t, *lx_mesh_vertex_ref_t;

/// the mesh face type
typedef struct lx_mesh_face_t_ {
    /// the list entry
    lx_list_entry_t             entry;

    /// an arbitrary edge of the face
    struct lx_mesh_edge_t_*    edge;

#ifdef LX_DEBUG
    /// the id
    lx_size_t                   id;

    /*! the face list for dumping it in the debug mode
     *
     * format: %{mesh_face}
     */
    lx_pointer_t                list;
#endif

}lx_mesh_face_t, *lx_mesh_face_ref_t;

/*! the mesh edge type
 *
 * using the half-edge data structure
 *
 * <pre>
 *              .                                            .
 *              . edge->dst                                  . edge->sym->org
 * .............D.............                 ..............O.............
 *              .                                            .
 *              .                                            .      edge->sym->lface
 *              .       edge->rface                          .
 *              .                                            . edge->sym
 * edge->lface /.\                                          \./
 *              . edge                     edge->sym->rface  .
 *              .                                            .
 *              .                                            .
 * .............O.............                 ..............D.............
 *              . edge->org                                  . edge->sym->dst
 *              .                                            .
 *
 *
 *
 *
 *              .                                            .
 * edge->lnext  . edge->dnext                  edge->dprev   . edge->rprev
 * ......<......D.....<.......                 ......>.......D......>......
 *              .                                            .
 *              .                                            .
 *              .                                            .
 *              .                                            .
 *             /.\                                          /.\
 *              . edge                                       . edge
 *              .                                            .
 *              .                                            .
 * ......<......O.....<.......                 ......>.......O......>......
 * edge->onext  . edge->rnext                  edge->lprev   . edge->oprev
 *              .                                            .
 *
 *          next: ccw                                    prev: cw
 *
 * </pre>
 */
typedef struct lx_mesh_edge_t_ {
    /*! the next edge for the doubly-linked list
     *
     * <pre>
     * next: edge->next
     * prev: edge->sym->next->sym
     *
     *  head: ..............e.............. : head_sym
     *    |                                     /|\
     *   \|/                                     |
     * edge1: ..............e.............. : edge1->sym
     *    |                                     /|\
     *    | next                            next |
     *   \|/                                     |
     * edge2: ..............e.............. : edge2->sym       (prev)
     *    |                                     /|\
     *    | next                            next |
     *   \|/                                     |
     * edge3: ..............e.............. : edge3->sym
     *    |                                     /|\
     *   \|/                                     |
     *  head: ..............e.............. : head_sym
     *
     * </pre>
     *
     */
    struct lx_mesh_edge_t_*    next;

    /*! the next edge ccw around the origin
     *
     * <pre>
     * onext: edge->onext
     * lprev: edge->onext->sym
     * rprev: edge->sym->onext
     * dnext: edge->rprev->sym == edge->sym->onext->sym
     * </pre>
     */
    struct lx_mesh_edge_t_*    onext;

    /*! the next edge ccw around the left face
     *
     * <pre>
     * lnext: edge->lnext
     * dprev: edge->lnext->sym
     * oprev: edge->sym->lnext
     * rnext: edge->oprev->sym == edge->sym->lnext->sym
     * </pre>
     */
    struct lx_mesh_edge_t_*    lnext;

    /// the same edge, opposite direction
    struct lx_mesh_edge_t_*    sym;

    /*! the origin vertex
     *
     * <pre>
     * org: edge->org
     * dst: edge->sym->org
     * </pre>
     */
    lx_mesh_vertex_ref_t        org;

    /*! the left face
     *
     * <pre>
     * lface: edge->lface
     * rface: edge->sym->lface
     * </pre>
     */
    lx_mesh_face_ref_t          lface;

#ifdef LX_DEBUG
    /// the id
    lx_size_t                   id;

    /*! the edge list for dumping it in the debug mode
     *
     * format: %{mesh_edge}
     */
    lx_pointer_t                list;
#endif

}lx_mesh_edge_t, *lx_mesh_edge_ref_t;

/// the mesh event enum
typedef enum lx_mesh_event_e {
    LX_MESH_EVENT_NONE          = 0x0000

    // edge
,   LX_MESH_EVENT_EDGE_INIT     = 0x0001 //!< init edge
,   LX_MESH_EVENT_EDGE_EXIT     = 0x0002 //!< exit edge
,   LX_MESH_EVENT_EDGE_MERGE    = 0x0004 //!< merge edge
,   LX_MESH_EVENT_EDGE_SPLIT    = 0x0008 //!< split edge
,   LX_MESH_EVENT_EDGE_ALL      = LX_MESH_EVENT_EDGE_INIT | LX_MESH_EVENT_EDGE_EXIT

    // face
,   LX_MESH_EVENT_FACE_INIT     = 0x0010 //!< init face
,   LX_MESH_EVENT_FACE_EXIT     = 0x0020 //!< exit face
,   LX_MESH_EVENT_FACE_MERGE    = 0x0040 //!< merge face
,   LX_MESH_EVENT_FACE_SPLIT    = 0x0080 //!< split face
,   LX_MESH_EVENT_FACE_ALL      = LX_MESH_EVENT_FACE_INIT | LX_MESH_EVENT_FACE_EXIT | LX_MESH_EVENT_FACE_MERGE | LX_MESH_EVENT_FACE_SPLIT

    // vertex
,   LX_MESH_EVENT_VERTEX_INIT   = 0x0100 //!< init vertex
,   LX_MESH_EVENT_VERTEX_EXIT   = 0x0200 //!< exit vertex
,   LX_MESH_EVENT_VERTEX_ALL    = LX_MESH_EVENT_VERTEX_INIT | LX_MESH_EVENT_VERTEX_EXIT

    // all
,   LX_MESH_EVENT_ALL           = LX_MESH_EVENT_EDGE_ALL | LX_MESH_EVENT_FACE_ALL | LX_MESH_EVENT_VERTEX_ALL

}lx_mesh_event_e;

/*! the mesh listener event type
 *
 * <pre>
 *
 * init(org)
 * exit(org)
 * merge(org, dst) => dst
 * split(org) => (org, dst)
 *
 * </pre>
 */
typedef struct lx_mesh_event_t_ {
    /// the event type
    lx_size_t                   type;

    /// the org edge/face/vertex
    lx_pointer_t                org;

    /// the dst edge/face/vertex
    lx_pointer_t                dst;

    /// the user private data
    lx_cpointer_t               priv;

}lx_mesh_event_t, *lx_mesh_event_ref_t;

/*! the mesh listener type
 *
 * @param event                 the listener event
 */
typedef lx_void_t               (*lx_mesh_listener_t)(lx_mesh_event_ref_t event);

/*! the mesh ref type
 *
 * using the doubly connected edge list (DCEL)
 * like the quad-edge data structure but no edge->rot and edge->invrot
 */
typedef lx_typeref(mesh);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init the mesh
 *
 * @param edge_element          the edge element
 * @param face_element          the face element
 * @param vertex_element        the vertex element
 *
 * @return                      the mesh
 */
lx_mesh_ref_t                   lx_mesh_init(lx_element_t edge_element, lx_element_t face_element, lx_element_t vertex_element);

/*! exit the mesh
 *
 * @param mesh                  the mesh
 */
lx_void_t                       lx_mesh_exit(lx_mesh_ref_t mesh);

/*! clear the mesh
 *
 * @param mesh                  the mesh
 */
lx_void_t                       lx_mesh_clear(lx_mesh_ref_t mesh);

/*! is empty?
 *
 * @param mesh                  the mesh
 *
 * @return                      lx_true or lx_false
 */
lx_bool_t                       lx_mesh_is_empty(lx_mesh_ref_t mesh);

/*! set the mesh listener
 *
 * @param mesh                  the mesh
 * @param listener              the listener
 * @param priv                  the user private data
 */
lx_void_t                       lx_mesh_listener_set(lx_mesh_ref_t mesh, lx_mesh_listener_t listener, lx_cpointer_t priv);

/*! add the mesh listener event
 *
 * @param mesh                  the mesh
 * @param events                the observed events
 */
lx_void_t                       lx_mesh_listener_event_add(lx_mesh_ref_t mesh, lx_size_t events);

/*! delete the mesh listener event
 *
 * @param mesh                  the mesh
 * @param events                the observed events
 */
lx_void_t                       lx_mesh_listener_event_del(lx_mesh_ref_t mesh, lx_size_t events);

/*! the head vertex
 *
 * @param mesh                  the mesh
 *
 * @return                      the vertex
 */
lx_mesh_vertex_ref_t            lx_mesh_vertex_head(lx_mesh_ref_t mesh);

/*! the last vertex
 *
 * @param mesh                  the mesh
 *
 * @return                      the vertex
 */
lx_mesh_vertex_ref_t            lx_mesh_vertex_last(lx_mesh_ref_t mesh);

/*! the vertex order
 *
 * @param mesh                  the mesh
 *
 * @return                      the order
 */
lx_size_t                       lx_mesh_vertex_order(lx_mesh_ref_t mesh);

/*! set vertex order
 *
 * @param mesh                  the mesh
 * @param order                 the order
 */
lx_void_t                       lx_mesh_vertex_order_set(lx_mesh_ref_t mesh, lx_size_t order);

/*! the head face
 *
 * @param mesh                  the mesh
 *
 * @return                      the face
 */
lx_mesh_face_ref_t              lx_mesh_face_head(lx_mesh_ref_t mesh);

/*! the last face
 *
 * @param mesh                  the mesh
 *
 * @return                      the face
 */
lx_mesh_face_ref_t              lx_mesh_face_last(lx_mesh_ref_t mesh);

/*! the face order
 *
 * @param mesh                  the mesh
 *
 * @return                      the order
 */
lx_size_t                       lx_mesh_face_order(lx_mesh_ref_t mesh);

/*! set face order
 *
 * @param mesh                  the mesh
 * @param order                 the order
 */
lx_void_t                       lx_mesh_face_order_set(lx_mesh_ref_t mesh, lx_size_t order);

/*! the head edge
 *
 * @param mesh                  the mesh
 *
 * @return                      the edge
 */
lx_mesh_edge_ref_t              lx_mesh_edge_head(lx_mesh_ref_t mesh);

/*! the last edge
 *
 * @param mesh                  the mesh
 *
 * @return                      the edge
 */
lx_mesh_edge_ref_t              lx_mesh_edge_last(lx_mesh_ref_t mesh);

/*! the tail edge for walking edges more flexible and faster than iterator
 *
 * @code
 * lx_mesh_edge_ref_t edge = lx_mesh_edge_head(mesh);
 * lx_mesh_edge_ref_t tail = lx_mesh_edge_tail(mesh);
 * for (; edge != tail; edge = lx_mesh_edge_next(edge))
 * {
 * }
 * @endcode
 *
 * @param mesh                  the mesh
 *
 * @return                      the edge, it's members are invalid and cannot modify them
 */
lx_mesh_edge_ref_t              lx_mesh_edge_tail(lx_mesh_ref_t mesh);

/*! the edge order
 *
 * @param mesh                  the mesh
 *
 * @return                      the order
 */
lx_size_t                       lx_mesh_edge_order(lx_mesh_ref_t mesh);

/*! set edge order
 *
 * @param mesh                  the mesh
 * @param order                 the order
 */
lx_void_t                       lx_mesh_edge_order_set(lx_mesh_ref_t mesh, lx_size_t order);

/*! make a unconnected edge
 *
 * <pre>
 *
 *           lface
 *
 *         O -----> D
 *
 *           lface
 *
 * </pre>
 *
 * @param mesh                  the mesh
 *
 * @return                      the new edge
 */
lx_mesh_edge_ref_t              lx_mesh_edge_make(lx_mesh_ref_t mesh);

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
 *         | rface |
 *         |       |
 *         O/D <---
 *
 *           lface
 *
 *
 * counter-clockwise:
 *
 *          -------
 *         |       |
 *         | lface |
 *        \|/      |
 *         D/O ----
 *
 *           rface
 *
 * </pre>
 *
 * @param mesh                  the mesh
 * @param is_ccw                is counter-clockwise?
 *
 * @return                      the new edge
 */
lx_mesh_edge_ref_t              lx_mesh_edge_make_loop(lx_mesh_ref_t mesh, lx_bool_t is_ccw);

/*! split edge
 *
 * edge_org and edge_new will have the same left and right face.
 *
 * <pre>
 *
 * before:
 *
 *
 *             .                lface                  .
 *                .                                 .
 *                   .         edge_org          .
 *        . . . . . . . org -----------------> dst
 *                    .                        .    .
 *                  .           rface         .         .
 *                .                          .              .
 *              .                                               .
 *
 *
 * after:
 *
 *
 *             .                                lface                                 .
 *                .                                                                .
 *                   .         edge_org                      edge_new           .
 *        . . . . . . . org -----------------> vertex_new ------------------> dst
 *                    .                                                       .    .
 *                  .                           rface                       .         .
 *                .                                                       .              .
 *              .                                                                           .
 *
 * </pre>
 *
 * @param mesh                  the mesh
 * @param edge                  the original edge
 *
 * @return                      the new edge
 */
lx_mesh_edge_ref_t              lx_mesh_edge_split(lx_mesh_ref_t mesh, lx_mesh_edge_ref_t edge_org);

/*! splice edge
 *
 * refer to the paper of Guibas and Stolfi, simplified since we don't use flips (p98)
 *
 * <pre>
 *
 *          .                                          .
 * .        .        .                        .        .        .
 *   .      .      .                            .      .      .
 *     .    .    .                                .    .    .
 *    o  .  .  .                                  o .  .  .
 *         ...                                        ...
 *          . o.org             |          o.lface     . o.org      d.lface
 *                              |                    .   .
 *        o.lface               |                  .       .
 *                              |                .           .
 *          . d.org            \ /             .               . d
 *        .   .                                                  .
 *      .       .                                                  .
 *    .           . d                                                .
 *                  .
 *                    .
 *
 *                         <= splice =>
 *
 * @note the d.org and d.lface will be inserted or removed
 *
 * </pre>
 *
 * @param mesh                  the mesh
 * @param edge_org              the original edge
 * @param edge_dst              the destinate edge
 */
lx_void_t                       lx_mesh_edge_splice(lx_mesh_ref_t mesh, lx_mesh_edge_ref_t edge_org, lx_mesh_edge_ref_t edge_dst);

/*! append edge
 *
 * edge_org and edge_new will have the same left face.
 *
 * <pre>
 *
 * before:
 *
 *
 *                                lface
 *
 *       edge_org
 *  -----------------> vertex
 *                       .    .
 *          rface     .            .
 *                .                     .
 *             .                             .
 *          .
 *
 * after:
 *
 *                                lface
 *
 *        edge_org                edge_new
 *  -----------------> vertex ----------------> vertex_new
 *                       .    .
 *          rface     .            .
 *                .                     .
 *             .                             .
 *          .
 *
 * </pre>
 *
 * @param mesh                  the mesh
 * @param edge                  the original edge
 *
 * @return                      the new edge
 */
lx_mesh_edge_ref_t              lx_mesh_edge_append(lx_mesh_ref_t mesh, lx_mesh_edge_ref_t edge_org);

/*! insert edge
 *
 * <pre>
 *
 * before:
 *
 *        .
 *             .                  edge_dst.lface
 *                  .
 *      edge_org         .        edge_dst
 *  -----------------> vertex ------------------->
 *                       .    .
 * edge_org.rface     .            .
 *                .                     .
 *             .                             .
 *          .
 *
 * after:
 *
 *        .
 *             .                            edge_dst.lface
 *                  .
 *      edge_org         .       edge_new                       edge_dst
 *  -----------------> vertex ----------------> vertex_new ------------------->
 *                                                .    .
 *                  edge_org.rface            .             .
 *                                        .                      .
 *                                    .                               .
 *                                .
 *
 * the special case:
 *
 *       .       edge_org              .     edge_org           edge_new
 *         . --------------->            . ----------------> ---------------> vertex_new
 * . . . . . <---------------     => . . . <---------------- <---------------
 *       .       edge_dst              .
 *     .                             .
 *
 * </pre>
 *
 * @param mesh                  the mesh
 * @param edge_org              the original edge
 * @param edge_dst              the destinate edge
 *
 * @return                      the new edge
 */
lx_mesh_edge_ref_t              lx_mesh_edge_insert(lx_mesh_ref_t mesh, lx_mesh_edge_ref_t edge_org, lx_mesh_edge_ref_t edge_dst);

/*! remove edge
 *
 * In a sense, the remove operation is the inverse of the insert operation.
 *
 * <pre>
 *
 * before:
 *
 *        .
 *             .                            edge_dst.lface
 *                  .
 *      edge_org         .       edge_del                       edge_dst
 *  -----------------> vertex ----------------> vertex_del ------------------->
 *                                                .    .
 *                  edge_org.rface            .             .
 *                                        .                      .
 *                                    .                               .
 *                                .
 *
 * after:
 *
 *        .
 *             .                  edge_dst.lface
 *                  .
 *      edge_org         .        edge_dst
 *  -----------------> vertex ------------------->
 *                       .    .
 * edge_org.rface     .            .
 *                .                     .
 *             .                             .
 *          .
 *
 *
 *     edge_org                     edge_org           edge_del
 * --------------->              ----------------> --------------->
 * <---------------      <=      <---------------- <---------------
 *     edge_dst                     edge_dst
 *
 *     edge_dst                     edge_del           edge_dst
 * --------------->              ----------------> --------------->
 * <---------------      <=      <---------------- <---------------
 *     edge_org                                        edge_org
 *
 * </pre>
 *
 * @param mesh                  the mesh
 * @param edge_del              the removed edge
 */
lx_void_t                       lx_mesh_edge_remove(lx_mesh_ref_t mesh, lx_mesh_edge_ref_t edge_del);

/*! connect edge
 *
 * make a new edge connecting the destination of edge_org to the origin of edge_dst,
 * in such a way that edge_org.lface = edge_new.lface = edge_dst.lface after the connection is complete.
 *
 * @note edge_org.lface and edge_dst.lface
 *
 * <pre>
 *
 * edge_org.lface == edge_dst.lface? disjoining face
 *
 *           face                                    face_new
 *
 *         edge_org                                  edge_org
 *  ---------------------->                   ---------------------->
 * |                                         |                       |
 * |                                         |                       |
 * |         face             => connect =>  |         face          | edge_new      face_new
 * |                                         |                       |
 * |                                         |                      \|/
 *  <---------------------                    <----------------------
 *         edge_dst                                  edge_dst
 *
 *           face                                    face_new
 *
 *
 * edge_org.lface != edge_dst.lface? joining faces
 *
 *
 *         edge_dst                face_org                             edge_dst
 *  <----------------------                                     <-----------------------.
 * |                       |       edge_org                    |            edge_new .  |
 * |                       |    ------------->                 |     ------------->.    |
 * |                       |   |              |                |    |   edge_org   |    |
 * |       face_dst        |   |              | => connect =>  |    |              |    |
 * |                       |   |              |                |    |              |    |
 * |                       |    <-------------                 |     <-------------     |
 * |                       |                                   |        face_org        |
 *  ---------------------->                                     ----------------------->
 *
 * after: edge_new.lface == edge_org.lface == edge_dst.lface
 *
 * </pre>
 *
 * @param mesh                  the mesh
 * @param edge_org              the original edge
 * @param edge_dst              the destinate edge
 *
 * @return                      the new edge
 */
lx_mesh_edge_ref_t              lx_mesh_edge_connect(lx_mesh_ref_t mesh, lx_mesh_edge_ref_t edge_org, lx_mesh_edge_ref_t edge_dst);

/*! delete edge
 *
 * the operation will disconnect the edge e from the rest of the structure
 * (this may cause the rest of the structure to fall apart in two separate components).
 *
 * in a sense, the delete operation is the inverse of the connect operation.
 *
 * <pre>
 *
 * before: edge_del.lface == edge_org.lface == edge_dst.lface
 *
 * edge_del.lface != edge_del.rface? joining faces
 *
 *           face                                    face_del
 *
 *         edge_org                                  edge_org
 *  ---------------------->                   ---------------------->
 * |                                         |                       |
 * |                                         |                       |
 * |         face             <= delete <=   |         face          | edge_del      face_del
 * |                                         |                       |
 * |                                         |                      \|/
 *  <---------------------                    <----------------------
 *         edge_dst                                  edge_dst
 *
 *           face                                    face_del
 *
 *
 * edge_del.lface == edge_del.rface? disjoining face
 *
 *
 *         edge_dst                                                     edge_dst
 *  <----------------------                                     <-----------------------.
 * |                       |       edge_org                    |            edge_del .  |
 * |                       |    ------------->                 |     ------------->.    |
 * |                       |   |              |                |    |   edge_org   |    |
 * |       face_new        |   |              | <= delete <=   |    |              |    |
 * |                       |   |              |                |    |              |    |
 * |                       |    <-------------                 |     <-------------     |
 * |                       |       face_org                    |        face_org        |
 *  ---------------------->                                     ----------------------->
 *
 * </pre>
 *
 * @param mesh                  the mesh
 * @param edge_del              the deleted edge
 */
lx_void_t                       lx_mesh_edge_delete(lx_mesh_ref_t mesh, lx_mesh_edge_ref_t edge_del);

#ifdef LX_DEBUG
/*! dump mesh
 * @param mesh                  the mesh
 */
lx_void_t                       lx_mesh_dump(lx_mesh_ref_t mesh);

/*! check mesh
 * @param mesh                  the mesh
 */
lx_void_t                       lx_mesh_check(lx_mesh_ref_t mesh);
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


