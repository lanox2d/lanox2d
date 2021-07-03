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
 * @file        mesh.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#define LX_TRACE_DISABLED
#include "mesh.h"
#include "mesh/edge_list.h"
#include "mesh/face_list.h"
#include "mesh/vertex_list.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// set the face edge
#define lx_mesh_face_edge_set(face, val)        do { lx_assert(face); (face)->edge = (val); } while (0)

// set the vertex edge
#define lx_mesh_vertex_edge_set(vertex, val)    do { lx_assert(vertex); (vertex)->edge = (val); } while (0)

// set the edge sym
#define lx_mesh_edge_sym_set(edge, val)         do { lx_assert(edge); (edge)->sym = (val); } while (0)

// set the edge org
#define lx_mesh_edge_org_set(edge, val)         do { lx_assert(edge); (edge)->org = (val); if (val) lx_mesh_vertex_edge_set(val, edge); } while (0)

// set the edge dst
#define lx_mesh_edge_dst_set(edge, val)         do { lx_assert((edge) && (edge)->sym); (edge)->sym->org = (val); if (val) lx_mesh_vertex_edge_set(val, (edge)->sym); } while (0)

// set the edge lface
#define lx_mesh_edge_lface_set(edge, val)       do { lx_assert(edge); (edge)->lface = (val); if (val) lx_mesh_face_edge_set(val, edge); } while (0)

// set the edge lface
#define lx_mesh_edge_rface_set(edge, val)       do { lx_assert((edge) && (edge)->sym); (edge)->sym->lface = (val); if (val) lx_mesh_face_edge_set(val, (edge)->sym); } while (0)

// set the edge onext
#define lx_mesh_edge_onext_set(edge, val)       do { lx_assert(edge); (edge)->onext = (val); } while (0)

// set the edge oprev
#define lx_mesh_edge_oprev_set(edge, val)       do { lx_assert((edge) && (edge)->sym); (edge)->sym->lnext = (val); } while (0)

// set the edge lnext
#define lx_mesh_edge_lnext_set(edge, val)       do { lx_assert(edge); (edge)->lnext = (val); } while (0)

// set the edge lprev
#define lx_mesh_edge_lprev_set(edge, val)       do { lx_assert((edge) && (edge)->onext); (edge)->onext->sym = (val); } while (0)

// set the edge rnext
#define lx_mesh_edge_rnext_set(edge, val)       do { lx_assert(lx_mesh_edge_oprev(edge)); lx_mesh_edge_oprev(edge)->sym = (val); } while (0)

// set the edge rprev
#define lx_mesh_edge_rprev_set(edge, val)       do { lx_assert((edge) && (edge)->sym); (edge)->sym->onext = (val); } while (0)

// set the edge dnext
#define lx_mesh_edge_dnext_set(edge, val)       do { lx_assert(lx_mesh_edge_rprev(edge)); lx_mesh_edge_rprev(edge)->sym = (val); } while (0)

// set the edge rprev
#define lx_mesh_edge_dprev_set(edge, val)       do { lx_assert((edge) && (edge)->lnext); (edge)->lnext->sym = (val); } while (0)

// check
#ifdef LX_DEBUG
#   define lx_mesh_check_vertex(vertex)         lx_assertf((vertex) && (vertex)->id && (vertex)->edge, "invalid vertex: %p, id: %lu", vertex, (vertex)? (vertex)->id : 0)
#   define lx_mesh_check_face(face)             lx_assertf((face) && (face)->id && (face)->edge, "invalid face: %p, id: %lu", face, (face)? (face)->id : 0)
#   define lx_mesh_check_edge(edge)             do { lx_assertf((edge) && (edge)->sym && (edge)->id && (edge)->sym->id, "invalid edge: %p => %p, id: %lu => %lu", edge, (edge)? (edge)->sym : lx_null, (edge)? (edge)->id : 0, (edge)->sym? (edge)->sym->id : 0); lx_mesh_check_face((edge)->lface); lx_mesh_check_vertex((edge)->org); } while (0)
#else
#   define lx_mesh_check_vertex(vertex)
#   define lx_mesh_check_face(face)
#   define lx_mesh_check_edge(edge)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the mesh mesh type
typedef struct lx_mesh_t_ {
    lx_mesh_edge_list_ref_t         edges;
    lx_mesh_face_list_ref_t         faces;
    lx_mesh_vertex_list_ref_t       vertices;
    lx_mesh_listener_t              listener;
    lx_cpointer_t                   listener_udata;
    lx_size_t                       listener_events;
}lx_mesh_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
/* splice(a, b)
 *
 * refer to the paper of Guibas and Stolfi, simplified since we don't use flips (p98)
 *     - Primitives for the Manipulation of General Subdivisions and the Computation of Voronoi Diagrams
 *
 * This operation affects the two edge rings a Org and b Org and, independently,
 * the two edge rings a Left and b Left. In each case,
 *     - if the two rings are distinct, Splice will combine them into one;
 *     - if the two are exactly the same ring, Splice will break it in two separate pieces;
 *     - if the two are the same ring taken with opposite orientations,
 *       Splice will Flip (and reverse the order) of a segment of that ring.
 *
 *          .                                          .
 * .        .        .                        .        .        .
 *   .      .      .                            .      .      .
 *     .    .    .                                .    .    .
 *    a  .  .  . x                               a  .  .  . x
 *         ...                                        ...
 *          .                              a.lface     .       b.lface
 *                                                y  .   .
 *  a.lface      b.lface                           .       .
 *          .                                    .           . b
 *     y  .   .                                                .
 *      .       .                                                .
 *    .           . b                                              .
 *                  .
 *                    .
 *
 *                             splice
 *  a.lface == b.lface          <=>             a.lface != b.lface
 */
static lx_void_t lx_mesh_splice_edge(lx_mesh_edge_ref_t a, lx_mesh_edge_ref_t b) {
    lx_assert(a && b && a != b);

    /* x = a.onext
     * y = b.onext
     */
    lx_mesh_edge_ref_t x = lx_mesh_edge_onext(a);
    lx_mesh_edge_ref_t y = lx_mesh_edge_onext(b);

    /* a.onext' = y
     * b.onext' = x
     */
    lx_mesh_edge_onext_set(a, y);
    lx_mesh_edge_onext_set(b, x);

    /* x.oprev' = b
     * y.oprev' = a
     */
    lx_mesh_edge_oprev_set(x, b);
    lx_mesh_edge_oprev_set(y, a);
}

static lx_void_t lx_mesh_save_face_at_orbit(lx_mesh_edge_ref_t edge, lx_mesh_face_ref_t lface) {
    lx_assert(edge);
    lx_mesh_edge_ref_t scan = edge;
    do {
        lx_mesh_edge_lface_set(scan, lface);
        scan = lx_mesh_edge_lnext(scan);
    }
    while (scan != edge);
}

static lx_void_t lx_mesh_save_vertex_at_orbit(lx_mesh_edge_ref_t edge, lx_mesh_vertex_ref_t org) {
    lx_assert(edge);
    lx_mesh_edge_ref_t scan = edge;
    do {
        lx_mesh_edge_org_set(scan, org);
        scan = lx_mesh_edge_onext(scan);
    }
    while (scan != edge);
}

static lx_inline lx_void_t lx_mesh_post_event(lx_mesh_t* mesh, lx_size_t type, lx_pointer_t org, lx_pointer_t dst) {
    // this event is observing? call listener
    if ((mesh->listener_events & type) && mesh->listener) {
        lx_mesh_event_t event = {type, org, dst, mesh->listener_udata};
        mesh->listener(&event);
    }
}

static lx_inline lx_mesh_edge_ref_t lx_mesh_make_edge(lx_mesh_t* mesh, lx_bool_t is_loop, lx_bool_t is_ccw) {
    lx_assert_and_check_return_val(mesh && mesh->edges, lx_null);

    // make the edge
    lx_mesh_edge_ref_t edge = is_loop? lx_mesh_edge_list_make_loop(mesh->edges, is_ccw) : lx_mesh_edge_list_make(mesh->edges);
    lx_assert_and_check_return_val(edge, lx_null);

    // post the init event
    lx_mesh_post_event(mesh, LX_MESH_EVENT_EDGE_INIT, edge, lx_null);
    return edge;
}

static lx_inline lx_mesh_face_ref_t lx_mesh_make_face(lx_mesh_t* mesh) {
    lx_assert_and_check_return_val(mesh && mesh->faces, lx_null);

    // make the face
    lx_mesh_face_ref_t face = lx_mesh_face_list_make(mesh->faces);

    // post the init event
    lx_mesh_post_event(mesh, LX_MESH_EVENT_FACE_INIT, face, lx_null);
    return face;
}

static lx_inline lx_mesh_face_ref_t lx_mesh_make_face_at_orbit(lx_mesh_t* mesh, lx_mesh_edge_ref_t edge) {
    lx_assert_and_check_return_val(mesh && edge, lx_null);

    // make the new face
    lx_mesh_face_ref_t face_new = lx_mesh_make_face(mesh);
    lx_assert_and_check_return_val(face_new, lx_null);

    // update left face for all edges in the orbit of the edge
    lx_mesh_save_face_at_orbit(edge, face_new);
    return face_new;
}

static lx_inline lx_mesh_vertex_ref_t lx_mesh_make_vertex(lx_mesh_t* mesh) {
    lx_assert_and_check_return_val(mesh && mesh->vertices, lx_null);

    // make the vertex
    lx_mesh_vertex_ref_t vertex = lx_mesh_vertex_list_make(mesh->vertices);

    // post the init event
    lx_mesh_post_event(mesh, LX_MESH_EVENT_VERTEX_INIT, vertex, lx_null);
    return vertex;
}

static lx_inline lx_mesh_vertex_ref_t lx_mesh_make_vertex_at_orbit(lx_mesh_t* mesh, lx_mesh_edge_ref_t edge) {
    lx_assert_and_check_return_val(mesh && edge, lx_null);

    // make the new vertex
    lx_mesh_vertex_ref_t vertex_new = lx_mesh_make_vertex(mesh);
    lx_assert_and_check_return_val(vertex_new, lx_null);

    // update origin for all edges leaving the orbit of the edge
    lx_mesh_save_vertex_at_orbit(edge, vertex_new);
    return vertex_new;
}

static lx_inline lx_void_t lx_mesh_kill_edge(lx_mesh_t* mesh, lx_mesh_edge_ref_t edge) {
    lx_assert_and_check_return(mesh && mesh->edges && edge);

    lx_mesh_post_event(mesh, LX_MESH_EVENT_EDGE_EXIT, edge, lx_null);
    lx_mesh_edge_list_kill(mesh->edges, edge);
}

static lx_inline lx_void_t lx_mesh_kill_face(lx_mesh_t* mesh, lx_mesh_face_ref_t face) {
    lx_assert_and_check_return(mesh && mesh->faces && face);

    lx_mesh_post_event(mesh, LX_MESH_EVENT_FACE_EXIT, face, lx_null);
    lx_mesh_face_list_kill(mesh->faces, face);
}

static lx_inline lx_void_t lx_mesh_kill_face_at_orbit(lx_mesh_t* mesh, lx_mesh_face_ref_t face, lx_mesh_face_ref_t face_new) {
    lx_assert_and_check_return(mesh && face);

    // update lface for all edges leaving the removed face
    lx_mesh_save_face_at_orbit(lx_mesh_face_edge(face), face_new);
    lx_mesh_kill_face(mesh, face);
}

static lx_inline lx_void_t lx_mesh_kill_vertex(lx_mesh_t* mesh, lx_mesh_vertex_ref_t vertex) {
    lx_assert_and_check_return(mesh && mesh->vertices && vertex);

    lx_mesh_post_event(mesh, LX_MESH_EVENT_VERTEX_EXIT, vertex, lx_null);
    lx_mesh_vertex_list_kill(mesh->vertices, vertex);
}

static lx_inline lx_void_t lx_mesh_kill_vertex_at_orbit(lx_mesh_t* mesh, lx_mesh_vertex_ref_t vertex, lx_mesh_vertex_ref_t org_new) {
    lx_assert_and_check_return(mesh && vertex);

    // update origin for all edges leaving the removed vertex
    lx_mesh_save_vertex_at_orbit(lx_mesh_vertex_edge(vertex), org_new);
    lx_mesh_kill_vertex(mesh, vertex);
}

static lx_bool_t lx_mesh_kill_isolated_edge(lx_mesh_t* mesh, lx_mesh_edge_ref_t edge) {
    lx_assert_and_check_return_val(mesh && edge, lx_false);

    // is isolated edge?
    if (lx_mesh_edge_is_isolated(edge)) {
        lx_assert(lx_mesh_edge_org(edge) != lx_mesh_edge_dst(edge));
        lx_assert(lx_mesh_edge_lface(edge) == lx_mesh_edge_rface(edge));

        // kill the origin and destination vertices
        lx_mesh_kill_vertex(mesh, lx_mesh_edge_org(edge));
        lx_mesh_kill_vertex(mesh, lx_mesh_edge_dst(edge));

        // kill the face and edge
        lx_mesh_kill_face(mesh, lx_mesh_edge_lface(edge));
        lx_mesh_kill_edge(mesh, edge);
        return lx_true;
    } else if (lx_mesh_edge_is_isolated_loop(edge)) { // is isolated loop edge?
        lx_assert(lx_mesh_edge_org(edge) == lx_mesh_edge_dst(edge));
        lx_assert(lx_mesh_edge_lface(edge) != lx_mesh_edge_rface(edge));

        // kill the vertex
        lx_mesh_kill_vertex(mesh, lx_mesh_edge_org(edge));

        // kill the left and right face
        lx_mesh_kill_face(mesh, lx_mesh_edge_lface(edge));
        lx_mesh_kill_face(mesh, lx_mesh_edge_rface(edge));

        // kill the edge
        lx_mesh_kill_edge(mesh, edge);
        return lx_true;
    }

    // no isolated
    return lx_false;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_mesh_ref_t lx_mesh_init(lx_element_t edge_element, lx_element_t face_element, lx_element_t vertex_element) {
    lx_bool_t  ok = lx_false;
    lx_mesh_t* mesh = lx_null;
    do {
        // init mesh
        mesh = lx_malloc0_type(lx_mesh_t);
        lx_assert_and_check_break(mesh);

        // init edges
        mesh->edges = lx_mesh_edge_list_init(edge_element);
        lx_assert_and_check_break(mesh->edges);

        // init faces
        mesh->faces = lx_mesh_face_list_init(face_element);
        lx_assert_and_check_break(mesh->faces);

        // init vertices
        mesh->vertices = lx_mesh_vertex_list_init(vertex_element);
        lx_assert_and_check_break(mesh->vertices);

        // ok
        ok = lx_true;

    } while (0);

    if (!ok && mesh) {
        lx_mesh_exit((lx_mesh_ref_t)mesh);
        mesh = lx_null;
    }
    return (lx_mesh_ref_t)mesh;
}

lx_void_t lx_mesh_exit(lx_mesh_ref_t self) {
    lx_mesh_t* mesh = (lx_mesh_t*)self;
    if (mesh) {
        if (mesh->edges) {
            lx_mesh_edge_list_exit(mesh->edges);
            mesh->edges = lx_null;
        }
        if (mesh->faces) {
            lx_mesh_face_list_exit(mesh->faces);
            mesh->faces = lx_null;
        }
        if (mesh->vertices) {
            lx_mesh_vertex_list_exit(mesh->vertices);
            mesh->vertices = lx_null;
        }
        lx_free(mesh);
    }
}

lx_void_t lx_mesh_clear(lx_mesh_ref_t self) {
    lx_mesh_t* mesh = (lx_mesh_t*)self;
    if (mesh) {
        if (mesh->edges) {
            lx_mesh_edge_list_clear(mesh->edges);
        }
        if (mesh->faces) {
            lx_mesh_face_list_clear(mesh->faces);
        }
        if (mesh->vertices) {
            lx_mesh_vertex_list_clear(mesh->vertices);
        }
    }
}

lx_bool_t lx_mesh_is_empty(lx_mesh_ref_t self) {
    lx_mesh_t* mesh = (lx_mesh_t*)self;
    lx_assert_and_check_return_val(mesh, lx_true);
    if (lx_mesh_edge_list_size(mesh->edges)) {
        return lx_false;
    }
    if (lx_mesh_face_list_size(mesh->faces)) {
        return lx_false;
    }
    if (lx_mesh_vertex_list_size(mesh->vertices)) {
        return lx_false;
    }
    return lx_true;
}

lx_void_t lx_mesh_listener_set(lx_mesh_ref_t self, lx_mesh_listener_t listener, lx_cpointer_t udata) {
    lx_mesh_t* mesh = (lx_mesh_t*)self;
    if (mesh) {
        mesh->listener       = listener;
        mesh->listener_udata = udata;
    }
}

lx_void_t lx_mesh_listener_event_add(lx_mesh_ref_t self, lx_size_t events) {
    lx_mesh_t* mesh = (lx_mesh_t*)self;
    if (mesh) {
        mesh->listener_events |= events;
    }
}

lx_void_t lx_mesh_listener_event_remove(lx_mesh_ref_t self, lx_size_t events) {
    lx_mesh_t* mesh = (lx_mesh_t*)self;
    if (mesh) {
        mesh->listener_events &= ~events;
    }
}

lx_mesh_vertex_list_ref_t lx_mesh_vertex_list(lx_mesh_ref_t self) {
    lx_mesh_t* mesh = (lx_mesh_t*)self;
    return mesh? mesh->vertices : lx_null;
}

lx_size_t lx_mesh_vertex_order(lx_mesh_ref_t self) {
    lx_mesh_t* mesh = (lx_mesh_t*)self;
    lx_assert_and_check_return_val(mesh && mesh->vertices, LX_MESH_ORDER_INSERT_TAIL);

    return lx_mesh_vertex_list_order(mesh->vertices);
}

lx_void_t lx_mesh_vertex_order_set(lx_mesh_ref_t self, lx_size_t order) {
    lx_mesh_t* mesh = (lx_mesh_t*)self;
    lx_assert_and_check_return(mesh && mesh->vertices);

    lx_mesh_vertex_list_order_set(mesh->vertices, order);
}

lx_mesh_face_list_ref_t lx_mesh_face_list(lx_mesh_ref_t self) {
    lx_mesh_t* mesh = (lx_mesh_t*)self;
    return mesh? mesh->faces : lx_null;
}

lx_size_t lx_mesh_face_order(lx_mesh_ref_t self) {
    lx_mesh_t* mesh = (lx_mesh_t*)self;
    lx_assert_and_check_return_val(mesh && mesh->faces, LX_MESH_ORDER_INSERT_TAIL);

    return lx_mesh_face_list_order(mesh->faces);
}

lx_void_t lx_mesh_face_order_set(lx_mesh_ref_t self, lx_size_t order) {
    lx_mesh_t* mesh = (lx_mesh_t*)self;
    lx_assert_and_check_return(mesh && mesh->faces);

    lx_mesh_face_list_order_set(mesh->faces, order);
}

lx_mesh_edge_list_ref_t lx_mesh_edge_list(lx_mesh_ref_t self) {
    lx_mesh_t* mesh = (lx_mesh_t*)self;
    return mesh? mesh->edges : lx_null;
}

lx_size_t lx_mesh_edge_order(lx_mesh_ref_t self) {
    lx_mesh_t* mesh = (lx_mesh_t*)self;
    lx_assert_and_check_return_val(mesh && mesh->edges, LX_MESH_ORDER_INSERT_TAIL);

    return lx_mesh_edge_list_order(mesh->edges);
}

lx_void_t lx_mesh_edge_order_set(lx_mesh_ref_t self, lx_size_t order) {
    lx_mesh_t* mesh = (lx_mesh_t*)self;
    lx_assert_and_check_return(mesh && mesh->edges);

    lx_mesh_edge_list_order_set(mesh->edges, order);
}

lx_mesh_edge_ref_t lx_mesh_edge_make(lx_mesh_ref_t self) {
    lx_mesh_t* mesh = (lx_mesh_t*)self;
    lx_assert_and_check_return_val(mesh, lx_null);

    lx_bool_t               ok = lx_false;
    lx_mesh_edge_ref_t      edge = lx_null;
    lx_mesh_face_ref_t      face = lx_null;
    lx_mesh_vertex_ref_t    org = lx_null;
    lx_mesh_vertex_ref_t    dst = lx_null;
    do {
        // make the org
        org = lx_mesh_make_vertex(mesh);
        lx_assert_and_check_break(org);

        // make the dst
        dst = lx_mesh_make_vertex(mesh);
        lx_assert_and_check_break(dst);

        // make the face
        face = lx_mesh_make_face(mesh);
        lx_assert_and_check_break(face);

        // make the edge
        edge = lx_mesh_make_edge(mesh, lx_false, lx_false);
        lx_assert_and_check_break(edge);

        // the sym edge
        lx_mesh_edge_ref_t edge_sym = lx_mesh_edge_sym(edge);
        lx_assert_and_check_break(edge_sym);

        // init the edge
        lx_mesh_edge_org_set  (edge, org);
        lx_mesh_edge_lface_set(edge, face);

        // init the sym edge
        lx_mesh_edge_org_set  (edge_sym, dst);
        lx_mesh_edge_lface_set(edge_sym, face);

        // ok
        ok = lx_true;

    } while (0);

    if (!ok) {
        if (org) {
            lx_mesh_kill_vertex(mesh, org);
            org = lx_null;
        }
        if (dst) {
            lx_mesh_kill_vertex(mesh, dst);
            dst = lx_null;
        }
        if (face) {
            lx_mesh_kill_face(mesh, face);
            face = lx_null;
        }
        if (edge) {
            lx_mesh_kill_edge(mesh, edge);
            edge = lx_null;
        }
    }
    return edge;
}

lx_mesh_edge_ref_t lx_mesh_edge_make_loop(lx_mesh_ref_t self, lx_bool_t is_ccw) {
    lx_mesh_t* mesh = (lx_mesh_t*)self;
    lx_assert_and_check_return_val(mesh, lx_null);

    lx_bool_t               ok = lx_false;
    lx_mesh_edge_ref_t      edge = lx_null;
    lx_mesh_face_ref_t      lface = lx_null;
    lx_mesh_face_ref_t      rface = lx_null;
    lx_mesh_vertex_ref_t    vertex = lx_null;
    do {
        // make the vertex
        vertex = lx_mesh_make_vertex(mesh);
        lx_assert_and_check_break(vertex);

        // make the left face
        lface = lx_mesh_make_face(mesh);
        lx_assert_and_check_break(lface);

        // make the right face
        rface = lx_mesh_make_face(mesh);
        lx_assert_and_check_break(rface);

        // make the edge
        edge = lx_mesh_make_edge(mesh, lx_true, is_ccw);
        lx_assert_and_check_break(edge);

        // the sym edge
        lx_mesh_edge_ref_t edge_sym = lx_mesh_edge_sym(edge);
        lx_assert_and_check_break(edge_sym);

        // init the edge
        lx_mesh_edge_org_set  (edge, vertex);
        lx_mesh_edge_lface_set(edge, lface);

        // init the sym edge
        lx_mesh_edge_org_set  (edge_sym, vertex);
        lx_mesh_edge_lface_set(edge_sym, rface);

        // ok
        ok = lx_true;

    } while (0);

    if (!ok) {
        if (vertex) {
            lx_mesh_kill_vertex(mesh, vertex);
            vertex = lx_null;
        }
        if (lface) {
            lx_mesh_kill_face(mesh, lface);
            lface = lx_null;
        }
        if (rface) {
            lx_mesh_kill_face(mesh, rface);
            rface = lx_null;
        }
        if (edge) {
            lx_mesh_kill_edge(mesh, edge);
            edge = lx_null;
        }
    }
    return edge;
}

lx_mesh_edge_ref_t lx_mesh_edge_split(lx_mesh_ref_t self, lx_mesh_edge_ref_t edge_org) {
    lx_mesh_t* mesh = (lx_mesh_t*)self;
    lx_assert_and_check_return_val(mesh && edge_org, lx_null);

    // check edge
    lx_mesh_check_edge(edge_org);

    /* the general case
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
     */
    lx_mesh_edge_ref_t edge_new = lx_null;
    lx_mesh_edge_ref_t edge_org_dprev = lx_mesh_edge_dprev(edge_org);
    if (edge_org_dprev != edge_org) {
        /* insert a new edge
         *
         * before:
         *
         *             .                lface                  * edge_org.dprev
         *                .                                 *
         *                   .         edge_org_sym     |*_
         *        . . . . . . . org <----------------- dst
         *                    .                        .    .
         *                  .           rface         .         .
         *                .                          .              .
         *              .                                               .
         *
         * after:
         *
         *             .                               lface                             * edge_org.dprev
         *                .                                                           *
         *                   .         edge_org_sym                 edge_new_sym  |*_
         *        . . . . . . . org <----------------- vertex_new <------------- dst
         *                    .                                                  .    .
         *                  .                          rface                    .         .
         *                .                                                    .              .
         *              .                                                                         .
         */
        lx_mesh_edge_ref_t edge_new_sym = lx_mesh_edge_insert(self, edge_org_dprev, lx_mesh_edge_sym(edge_org));
        lx_assert(edge_new_sym);

        /* reverse the new edge
         *
         *             .                               lface                             *
         *                .                                                           *
         *                   .            edge_org                  edge_new      |*_
         *        . . . . . . . org -----------------> vertex_new -------------> dst
         *                    .                                                  .    .
         *                  .                          rface                    .         .
         *                .                                                    .              .
         *              .                                                                         .
         */
        edge_new = lx_mesh_edge_sym(edge_new_sym);
        lx_assert(edge_new);
    } else {
        /* the special case
         *
         *       .
         *         .        edge_org
         * . . . . . org ---------------> dst
         *       .
         *     .
         */

        /* append a new edge
         *                             lface
         *       .
         *         .        edge_org           edge_new_sym
         * . . . . . org ---------------> dst -------------> vertex_new
         *       .
         *     .                       rface
         */
        lx_mesh_edge_ref_t edge_new_sym = lx_mesh_edge_append(self, edge_org);
        lx_assert(edge_new_sym);

        // the new edge
        edge_new = lx_mesh_edge_sym(edge_new_sym);
        lx_assert(edge_new);

        /* splice(edge_org_sym, edge_new_sym)
         *                                    lface
         *       .
         *         .       edge_org_sym                    edge_new_sym
         * . . . . . org <--------------- dst          dst -------------> vertex_new
         *       .
         *     .                              rface
         */
        lx_mesh_edge_ref_t edge_org_sym = lx_mesh_edge_sym(edge_org);
        lx_mesh_splice_edge(edge_org_sym, edge_new_sym);

        /* splice(edge_org_sym, edge_new)
         *
         *       .
         *         .       edge_org_sym                       edge_new
         * . . . . . org <--------------- dst          dst <------------- vertex_new
         *       .
         *     .
         *
         *
         *       .
         *         .       edge_org_sym                    edge_new
         * . . . . . org <--------------- dst vertex_new ------------> dst
         *       .
         *     .
         *
         *       .
         *         .        edge_org                       edge_new
         * . . . . . org ---------------> dst vertex_new ------------> dst
         *       .
         *     .
         *
         */
        lx_mesh_splice_edge(edge_org_sym, edge_new);

        /* update the edge_org.dst
         *
         *
         *       .
         *         .         edge_org                  edge_new
         * . . . . . org ---------------> vertex_new ------------> dst
         *       .
         *     .
         *
         */
        lx_mesh_edge_dst_set(edge_org, lx_mesh_edge_org(edge_new));

        // update the edge of edge_new.dst, may have pointed to edge_org_sym
        lx_mesh_vertex_edge_set(lx_mesh_edge_dst(edge_new), edge_new_sym);

        // update the faces of edge_new
        lx_mesh_edge_lface_set(edge_new,        lx_mesh_edge_lface(edge_org));
        lx_mesh_edge_lface_set(edge_new_sym,    lx_mesh_edge_lface(edge_org_sym));
    }

    // check
    lx_assert(lx_mesh_edge_dst(edge_org) == lx_mesh_edge_org(edge_new));

    // post the split event, split(edge_org) => (edge_org, edge_new)
    lx_mesh_post_event(mesh, LX_MESH_EVENT_EDGE_SPLIT, edge_org, edge_new);

    // ok
    return edge_new;
}

lx_void_t lx_mesh_edge_splice(lx_mesh_ref_t self, lx_mesh_edge_ref_t edge_org, lx_mesh_edge_ref_t edge_dst) {
    lx_mesh_t* mesh = (lx_mesh_t*)self;
    lx_assert_and_check_return(mesh && edge_org && edge_dst);

    // check edges
    lx_mesh_check_edge(edge_org);
    lx_mesh_check_edge(edge_dst);

    lx_bool_t joining_faces = lx_false;
    lx_bool_t joining_vertices = lx_false;
    do {
        // is same? ok
        lx_check_break(edge_org != edge_dst);

        // two vertices are disjoint?
        if (lx_mesh_edge_org(edge_org) != lx_mesh_edge_org(edge_dst)) {

            // joins the two vertices
            joining_vertices = lx_true;

            // remove the edge_dst.org first
            lx_mesh_kill_vertex_at_orbit(mesh, lx_mesh_edge_org(edge_dst), lx_mesh_edge_org(edge_org));
        }

        // two faces are disjoint?
        if (lx_mesh_edge_lface(edge_org) != lx_mesh_edge_lface(edge_dst)) {

            // joins the two faces
            joining_faces = lx_true;

            // post the merge event, merge(edge_dst.lface, edge_org.lface) => edge_org.lface
            lx_mesh_post_event(mesh, LX_MESH_EVENT_FACE_MERGE, lx_mesh_edge_lface(edge_dst), lx_mesh_edge_lface(edge_org));

            // remove the edge_dst.lface first
            lx_mesh_kill_face_at_orbit(mesh, lx_mesh_edge_lface(edge_dst), lx_mesh_edge_lface(edge_org));
        }

        // splice two edges
        lx_mesh_splice_edge(edge_dst, edge_org);

        // two vertices are disjoint?
        if (!joining_vertices) {
            /* make new vertex at edge_dst.org
             * and update origin for all edges leaving the origin orbit of the edge_dst
             */
            lx_mesh_vertex_ref_t vertex_new = lx_mesh_make_vertex_at_orbit(mesh, edge_dst);
            lx_assert_and_check_break(vertex_new);

            // update the reference edge, the old reference edge may have been removed
            lx_mesh_vertex_edge_set(lx_mesh_edge_org(edge_org), edge_org);
        }

        // two faces are disjoint?
        if (!joining_faces) {
            /* make new face at edge_dst.lface
             * and update lface for all edges leaving the left orbit of the edge_dst
             */
            lx_mesh_face_ref_t face_new = lx_mesh_make_face_at_orbit(mesh, edge_dst);
            lx_assert_and_check_break(face_new);

            // post the split event, split(edge_org.lface) => (edge_org.lface, face_new)
            lx_mesh_post_event(mesh, LX_MESH_EVENT_FACE_SPLIT, lx_mesh_edge_lface(edge_org), face_new);

            // update the reference edge, the old reference edge may have been removed
            lx_mesh_face_edge_set(lx_mesh_edge_lface(edge_org), edge_org);
        }

    } while (0);
}

lx_mesh_edge_ref_t lx_mesh_edge_append(lx_mesh_ref_t self, lx_mesh_edge_ref_t edge_org) {
    lx_mesh_t* mesh = (lx_mesh_t*)self;
    lx_assert_and_check_return_val(mesh && edge_org, lx_null);

    lx_bool_t           ok = lx_false;
    lx_mesh_edge_ref_t  edge_new = lx_null;
    do {
        // check edge
        lx_mesh_check_edge(edge_org);

        // make the new non-loop edge
        edge_new = lx_mesh_make_edge(mesh, lx_false, lx_false);
        lx_assert_and_check_break(edge_new);

        // the new sym edge
        lx_mesh_edge_ref_t edge_sym_new = lx_mesh_edge_sym(edge_new);
        lx_assert_and_check_break(edge_sym_new);

        /* append edge
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
         *
         */
        lx_mesh_splice_edge(edge_new, lx_mesh_edge_lnext(edge_org));

        // init the new edge
        lx_mesh_edge_org_set  (edge_new,        lx_mesh_edge_dst(edge_org));
        lx_mesh_edge_lface_set(edge_new,        lx_mesh_edge_lface(edge_org));
        lx_mesh_edge_lface_set(edge_sym_new,    lx_mesh_edge_lface(edge_org));

        /* make the new vertex
         * and update origin for all edges leaving the destination orbit of the new edge
         */
        if (!lx_mesh_make_vertex_at_orbit(mesh, edge_sym_new)) {
            break;
        }
        ok = lx_true;

    } while (0);

    if (!ok && edge_new) {
        lx_mesh_kill_edge(mesh, edge_new);
        edge_new = lx_null;
    }
    return edge_new;
}

lx_mesh_edge_ref_t lx_mesh_edge_insert(lx_mesh_ref_t self, lx_mesh_edge_ref_t edge_org, lx_mesh_edge_ref_t edge_dst) {
    lx_mesh_t* mesh = (lx_mesh_t*)self;
    lx_assert_and_check_return_val(mesh && edge_org && edge_dst, lx_null);

    lx_bool_t           ok = lx_false;
    lx_mesh_edge_ref_t  edge_new = lx_null;
    do {
        // check edges
        lx_mesh_check_edge(edge_org);
        lx_mesh_check_edge(edge_dst);

        // make the new clockwise self-loop edge
        edge_new = lx_mesh_make_edge(mesh, lx_true, lx_false);
        lx_assert_and_check_break(edge_new);

        // the new sym edge
        lx_mesh_edge_ref_t edge_sym_new = lx_mesh_edge_sym(edge_new);
        lx_assert_and_check_break(edge_sym_new);

        /* insert the edge at vertex
         *
         * before:
         *
         *        edge_new
         *          ----
         *         |    |
         *          <---
         *
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
         * splice(edge_dst, edge_new):
         *
         *        .
         *             .          edge_new   edge_dst.lface
         *                  .       --->
         *      edge_org         . |    |    edge_dst
         *  ---------------------> vertex ------------------->
         *                       .        .
         * edge_org.rface     .                .
         *                 .                        .
         *              .                                .
         *           .
         *
         * splice(lx_mesh_edge_sym(edge_org), edge_sym_new):
         *
         *
         *
         *      .                                     edge_dst.lface
         *          .              edge_sym_new
         *              .       <----------------
         *     edge_sym_org .  |                 |           edge_dst
         *  <----------------- vertex       vertex_new  ------------------->
         *                                  .           .
         *                               .                 .
         *          edge_org.rface    .                       .
         *                         .                             .
         *                      .
         *
         *
         *      .                                            edge_dst.lface
         *          .
         *              .
         *       edge_org   .         edge_new                   edge_dst
         *  -----------------> vertex --------> vertex_new  ------------------->
         *                                    .           .
         *                                 .                 .
         *          edge_org.rface      .                       .
         *                           .                             .
         *                        .
         */
        lx_mesh_splice_edge(edge_dst, edge_new);
        lx_mesh_splice_edge(lx_mesh_edge_sym(edge_org), edge_sym_new);

        // init the new edge
        lx_mesh_edge_org_set  (edge_new, lx_mesh_edge_dst(edge_org));
        lx_mesh_edge_lface_set(edge_new, lx_mesh_edge_lface(edge_dst));
        lx_mesh_edge_rface_set(edge_new, lx_mesh_edge_rface(edge_org));

        /* make the new vertex
         * and update origin for all edges leaving the destination orbit of the new edge
         */
        if (!lx_mesh_make_vertex_at_orbit(mesh, edge_sym_new)) {
            break;
        }
        ok = lx_true;

    } while (0);

    if (!ok && edge_new) {
        lx_mesh_kill_edge(mesh, edge_new);
        edge_new = lx_null;
    }
    return edge_new;
}

lx_void_t lx_mesh_edge_remove(lx_mesh_ref_t self, lx_mesh_edge_ref_t edge_removed) {
    lx_mesh_t* mesh = (lx_mesh_t*)self;
    lx_assert_and_check_return(mesh && edge_removed);

    // check edge
    lx_mesh_check_edge(edge_removed);

    // isolated edge? kill it directly
    if (lx_mesh_kill_isolated_edge(mesh, edge_removed)) {
        return ;
    }

    // get the destinate edge
    lx_mesh_edge_ref_t edge_dst = lx_mesh_edge_lnext(edge_removed);
    lx_assert_and_check_return(edge_dst);

    // get the original sym edge
    lx_mesh_edge_ref_t edge_sym_org = lx_mesh_edge_oprev(edge_removed);
    lx_assert_and_check_return(edge_sym_org);

    // the sym edge
    lx_mesh_edge_ref_t edge_sym = lx_mesh_edge_sym(edge_removed);
    lx_assert_and_check_return(edge_sym);

    /* use edge_sym_org for edge_dst if the destination vertex is isolated
     *
     * before:
     *
     *      edge_org        edge_removed
     * ----------------> --------------->
     * <---------------- <---------------
     *    edge_sym_org      edge_dst'
     *      edge_dst
     */
    if (edge_dst == edge_sym) {
        edge_dst = edge_sym_org;
    } else if (edge_sym_org == edge_removed) {
        /* use edge_dst for edge_sym_org if the original vertex is isolated
         *
         * before:
         *
         *    edge_sym_org'   edge_sym_org
         *    edge_removed'   edge_dst
         * ----------------> --------------->
         * <---------------- <---------------
         *    edge_removed
         */

        // reverse edge
        lx_swap(lx_mesh_edge_ref_t, edge_removed, edge_sym);

        // update edge_sym_org
        edge_sym_org = edge_dst;
    }

    /* kill the destination vertex of the edge
     * and update origin for all edges leaving the destination orbit of the removed edge
     */
    lx_mesh_kill_vertex_at_orbit(mesh, lx_mesh_edge_dst(edge_removed), lx_mesh_edge_org(edge_sym_org));

    /* remove edge
     *
     * before:
     *
     *        .
     *             .                            edge_dst.lface
     *                  .
     *      edge_sym_org     .          edge                       edge_dst
     *  <----------------- vertex ----------------> vertex_remove ------------------->
     *                                                .    .
     *                  edge_org.rface            .             .
     *                                        .                      .
     *                                    .                               .
     *                                .
     *
     * lx_mesh_splice_edge(edge_sym_org, edge_sym):
     *
     *        .
     *             .                            edge_dst.lface
     *                  .
     *      edge_sym_org     .        edge_sym                      edge_dst
     *  <----------------- vertex <---------------- vertex_remove ------------------->
     *                                                .    .
     *                  edge_org.rface            .             .
     *                                        .                      .
     *                                    .                               .
     *                                .
     *
     *        .
     *             .                            edge_dst.lface
     *                  .
     *      edge_sym_org     .  edge_sym           edge_dst
     *  <----------------- vertex / vertex_remove ------------------->
     *                      |       |  .    .
     *     edge_org.rface    <-----.             .
     *                         .                      .
     *                     .                               .
     *                 .
     *
     * lx_mesh_splice_edge(edge_dst, edge_removed):
     *
     *        .
     *             .                            edge_dst.lface
     *                  .
     *      edge_sym_org     . edge_removed     edge_dst
     *  <----------------- vertex / vertex_remove ------------------->
     *                      |       |  .    .
     *     edge_org.rface    ----->.             .
     *                         .                      .
     *                     .                               .
     *                 .
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
     */
    lx_mesh_splice_edge(edge_sym_org,   edge_sym);
    lx_mesh_splice_edge(edge_dst,       edge_removed);

    // update the reference edge, the old reference edge may have been removed
    lx_mesh_vertex_edge_set(lx_mesh_edge_org(edge_sym_org),     edge_sym_org);
    lx_mesh_face_edge_set  (lx_mesh_edge_lface(edge_sym_org),   edge_sym_org);
    lx_mesh_face_edge_set  (lx_mesh_edge_lface(edge_dst),       edge_dst);

    // kill the edge
    lx_mesh_kill_edge(mesh, edge_removed);
}

lx_mesh_edge_ref_t lx_mesh_edge_connect(lx_mesh_ref_t self, lx_mesh_edge_ref_t edge_org, lx_mesh_edge_ref_t edge_dst) {
    lx_mesh_t* mesh = (lx_mesh_t*)self;
    lx_assert_and_check_return_val(mesh && edge_org && edge_dst, lx_null);

    // check edges
    lx_mesh_check_edge(edge_org);
    lx_mesh_check_edge(edge_dst);

    lx_bool_t           ok = lx_false;
    lx_mesh_edge_ref_t  edge_new = lx_null;
    lx_bool_t           joining_faces = lx_false;
    do {
        // make the new non-loop edge
        edge_new = lx_mesh_make_edge(mesh, lx_false, lx_false);
        lx_assert_and_check_break(edge_new);

        // the new sym edge
        lx_mesh_edge_ref_t edge_sym_new = lx_mesh_edge_sym(edge_new);
        lx_assert_and_check_break(edge_sym_new);

        // two faces are disjoint?
        if (lx_mesh_edge_lface(edge_org) != lx_mesh_edge_lface(edge_dst)) {

            // joins the two faces
            joining_faces = lx_true;

            // post the merge event, merge(edge_dst.lface, edge_org.lface) => edge_org.lface
            lx_mesh_post_event(mesh, LX_MESH_EVENT_FACE_MERGE, lx_mesh_edge_lface(edge_dst), lx_mesh_edge_lface(edge_org));

            // remove the edge_dst.lface first
            lx_mesh_kill_face_at_orbit(mesh, lx_mesh_edge_lface(edge_dst), lx_mesh_edge_lface(edge_org));
        }

        /* connect edge
         *
         * before:
         *
         *           face
         *
         *         edge_org
         *  ---------------------->
         * |                                    edge_new
         * |         face                  ----------------->
         * |
         *  <---------------------
         *         edge_dst
         *
         * lx_mesh_splice_edge(edge_new, lx_mesh_edge_lnext(edge_org)):
         *
         *           face
         *
         *         edge_org
         *  ----------------------> ----------------->
         * |                            edge_new
         * |         face
         * |
         *  <---------------------
         *         edge_dst
         *
         * lx_mesh_splice_edge(edge_sym_new, edge_dst):
         *
         *         edge_org
         *  ---------------------->
         * |                       |
         * |         face          | edge_new      face_new
         * |                      \|/
         *  <---------------------
         *         edge_dst
         */
        lx_mesh_splice_edge(edge_new, lx_mesh_edge_lnext(edge_org));
        lx_mesh_splice_edge(edge_sym_new, edge_dst);

        // init the new edge
        lx_mesh_edge_org_set  (edge_new,        lx_mesh_edge_dst(edge_org));
        lx_mesh_edge_org_set  (edge_sym_new,    lx_mesh_edge_org(edge_dst));
        lx_mesh_edge_lface_set(edge_sym_new,    lx_mesh_edge_lface(edge_org));

        // two faces are disjoint?
        if (!joining_faces) {
            // save the old face first, edge_org.lface may have been modified after making new face
            lx_mesh_face_ref_t face_old = lx_mesh_edge_lface(edge_org);

            /* make new face at edge_new.lface
             * and update lface for all edges leaving the left orbit of the edge_new
             */
            lx_mesh_face_ref_t face_new = lx_mesh_make_face_at_orbit(mesh, edge_new);
            lx_assert_and_check_break(face_new);

            // post the split event, split(edge_org.lface) => (edge_org.lface, face_new)
            lx_mesh_post_event(mesh, LX_MESH_EVENT_FACE_SPLIT, face_old, face_new);
        } else {
            // init the edge_new.lface
            lx_mesh_edge_lface_set(edge_new, lx_mesh_edge_lface(edge_org));
        }
        ok = lx_true;

    } while (0);

    // check
    lx_assert(ok);

    if (!ok && edge_new) {
        lx_mesh_kill_edge(mesh, edge_new);
        edge_new = lx_null;
    }
    return edge_new;
}

lx_void_t lx_mesh_edge_disconnect(lx_mesh_ref_t self, lx_mesh_edge_ref_t edge_removed) {
    lx_mesh_t* mesh = (lx_mesh_t*)self;
    lx_assert_and_check_return(mesh && edge_removed);

    // check edge
    lx_mesh_check_edge(edge_removed);

    lx_bool_t joining_faces = lx_false;
    do {
        // two faces are disjoint?
        if (lx_mesh_edge_lface(edge_removed) != lx_mesh_edge_rface(edge_removed)) {
            // joins the two faces
            joining_faces = lx_true;

            // post the merge event, merge(edge_removed.lface, edge_removed.rface) => edge_removed.rface
            lx_mesh_post_event(mesh, LX_MESH_EVENT_FACE_MERGE, lx_mesh_edge_lface(edge_removed), lx_mesh_edge_rface(edge_removed));

            // remove the edge_removed.lface first
            lx_mesh_kill_face_at_orbit(mesh, lx_mesh_edge_lface(edge_removed), lx_mesh_edge_rface(edge_removed));
        }

        /* before:
         *
         * edge_removed.lface != edge_removed.rface:
         *
         *         face_remove
         *
         *         edge_org
         *  ---------------------->
         * |                       |
         * |                       |
         * |         face          | edge_removed      face_remove
         * |                       |
         * |                      \|/
         *  <----------------------
         *         edge_dst
         *
         *         face_remove
         *
         *
         * edge_removed.lface == edge_removed.rface:
         *
         *
         *          edge_dst
         *  <-----------------------.
         * |        edge_removed .  |
         * |     ------------->.    |
         * |    |   edge_org   |    |
         * |    |              |    |
         * |    |              |    |
         * |     <-------------     |
         * |        face_org        |
         *  ----------------------->
         *
         * edge_removed.onext == edge_removed:
         *
         *  ----------------------> <---------------- org
         * |                       |     edge_removed
         * |                       |
         *  <----------------------
         *
         */
        if (lx_mesh_edge_onext(edge_removed) == edge_removed) {
            /* remove the edge_removed.org
             *
             * after:
             *
             *  ----------------------> <---------------- null
             * |                       |     edge_removed
             * |                       |
             *  <----------------------
             */
            lx_mesh_kill_vertex_at_orbit(mesh, lx_mesh_edge_org(edge_removed), lx_null);
        } else {
            // update the reference edge, the old reference edge may have been invalid
            lx_mesh_face_edge_set  (lx_mesh_edge_rface(edge_removed),   lx_mesh_edge_oprev(edge_removed));
            lx_mesh_vertex_edge_set(lx_mesh_edge_org(edge_removed),     lx_mesh_edge_onext(edge_removed));

            /* disjoining edges at the edge_removed.org
             *
             * after:
             *
             * edge_removed.lface != edge_removed.rface:
             *
             *
             *         edge_org
             *  ---------------------->
             * |
             * |
             * |         face
             * |
             * |
             *  <---------------------- <-----------------
             *         edge_dst              edge_removed
             *
             *
             * edge_removed.lface == edge_removed.rface:
             *
             *
             *          edge_dst
             *  <-----------------------.
             * |                     .  |
             * |                   .    |        ------------->
             * |      edge_removed      |       |   edge_org   |
             * |                        |       |              |
             * |        face_new        |       |              |
             * |                        |        <-------------
             * |                        |           face_org
             *  ----------------------->
             *
             */
            lx_mesh_splice_edge(edge_removed, lx_mesh_edge_oprev(edge_removed));

            // two faces are disjoint?
            if (!joining_faces) {
                // save the old face first, edge_removed.lface may have been modified after making new face
                lx_mesh_face_ref_t face_old = lx_mesh_edge_lface(edge_removed);

                /* make new face at edge_removed.lface
                 * and update lface for all edges leaving the left orbit of the edge_removed
                 */
                lx_mesh_face_ref_t face_new = lx_mesh_make_face_at_orbit(mesh, edge_removed);
                lx_assert_and_check_break(face_new);

                // post the split event, split(face_old) => (face_old, face_new)
                lx_mesh_post_event(mesh, LX_MESH_EVENT_FACE_SPLIT, face_old, face_new);
            }
        }

        // the sym edge
        lx_mesh_edge_ref_t edge_sym = lx_mesh_edge_sym(edge_removed);
        lx_assert_and_check_break(edge_sym);

        // the removed edge is isolated now?
        if (lx_mesh_edge_onext(edge_sym) == edge_sym) {
            /* remove the edge_removed directly
             *
             * before:
             *
             *  ---------------------->           <---------------- null
             * |                       |               edge_removed
             * |                       |
             *  <----------------------
             *
             * after:
             *
             *  ---------------------->
             * |                       |
             * |                       |
             *  <----------------------
             */
            lx_mesh_kill_vertex_at_orbit(mesh, lx_mesh_edge_org(edge_sym), lx_null);
            lx_mesh_kill_face_at_orbit(mesh, lx_mesh_edge_lface(edge_sym), lx_null);
        } else {
            // update the reference edge, the old reference edge may have been invalid
            lx_mesh_face_edge_set(lx_mesh_edge_lface(edge_removed), lx_mesh_edge_oprev(edge_sym));
            lx_mesh_vertex_edge_set(lx_mesh_edge_org(edge_sym), lx_mesh_edge_onext(edge_sym));

            /* disjoining edges at the edge_removed.dst
             *
             * after:
             *
             * edge_removed.lface != edge_removed.rface:
             *
             *
             *         edge_org
             *  ---------------------->
             * |
             * |
             * |         face
             * |
             * |
             *  <----------------------        <-----------------
             *         edge_dst                     edge_removed
             *
             *
             * edge_removed.lface == edge_removed.rface:
             *
             *
             *          edge_dst
             *  <-----------------------
             * |                        |
             * |       ---------->      |        ------------->
             * |      edge_removed      |       |   edge_org   |
             * |                        |       |              |
             * |        face_new        |       |              |
             * |                        |        <-------------
             * |                        |           face_org
             *  ----------------------->
             *
             */
            lx_mesh_splice_edge(edge_sym, lx_mesh_edge_oprev(edge_sym));
        }

        // kill this edge
        lx_mesh_kill_edge(mesh, edge_removed);

    } while (0);
}

#ifdef LX_DEBUG
lx_void_t lx_mesh_check(lx_mesh_ref_t self) {
    lx_for_all (lx_mesh_edge_ref_t, edge, lx_mesh_edge_list(self)) {
        lx_mesh_check_edge(edge);
        lx_assert(lx_mesh_edge_sym(edge) != edge);
        lx_assert(lx_mesh_edge_sym(lx_mesh_edge_sym(edge)) == edge);
        lx_assert(lx_mesh_edge_org(edge));
        lx_assert(lx_mesh_edge_dst(edge));
        lx_assert(lx_mesh_edge_sym(lx_mesh_edge_onext(lx_mesh_edge_lnext(edge))) == edge);
        lx_assert(lx_mesh_edge_lnext(lx_mesh_edge_sym(lx_mesh_edge_onext(edge))) == edge);
    }

    lx_for_all (lx_mesh_face_ref_t, face, lx_mesh_face_list(self)) {
        lx_mesh_check_face(face);
        lx_mesh_edge_ref_t head = lx_mesh_face_edge(face);
        lx_mesh_edge_ref_t edge = head;
        do {
            lx_mesh_check_edge(edge);
            lx_assert(lx_mesh_edge_sym(edge) != edge);
            lx_assert(lx_mesh_edge_sym(lx_mesh_edge_sym(edge)) == edge);
            lx_assert(lx_mesh_edge_sym(lx_mesh_edge_onext(lx_mesh_edge_lnext(edge))) == edge);
            lx_assert(lx_mesh_edge_lnext(lx_mesh_edge_sym(lx_mesh_edge_onext(edge))) == edge);
            lx_assert(lx_mesh_edge_lface(edge) == face);
            edge = lx_mesh_edge_lnext(edge);
        } while (edge != head);
    }

    lx_for_all (lx_mesh_vertex_ref_t, vertex, lx_mesh_vertex_list(self)) {
        lx_mesh_check_vertex(vertex);

        lx_mesh_edge_ref_t head = lx_mesh_vertex_edge(vertex);
        lx_mesh_edge_ref_t edge = head;
        do {
            lx_mesh_check_edge(edge);
            lx_assert(lx_mesh_edge_sym(edge) != edge);
            lx_assert(lx_mesh_edge_sym(lx_mesh_edge_sym(edge)) == edge);
            lx_assert(lx_mesh_edge_sym(lx_mesh_edge_onext(lx_mesh_edge_lnext(edge))) == edge);
            lx_assert(lx_mesh_edge_lnext(lx_mesh_edge_sym(lx_mesh_edge_onext(edge))) == edge);
            lx_assert(lx_mesh_edge_org(edge) == vertex);
            edge = lx_mesh_edge_onext(edge);
        } while (edge != head);
    }
}
#endif
