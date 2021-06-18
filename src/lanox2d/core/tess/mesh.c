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
#include "mesh.h"
#include "mesh/mesh.h"

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

// the mesh impl type
typedef struct __lx_mesh_impl_t
{
    // the edges
    lx_mesh_edge_list_ref_t         edges;

    // the faces
    lx_mesh_face_list_ref_t         faces;

    // the vertices
    lx_mesh_vertex_list_ref_t       vertices;

    // the listener
    lx_mesh_listener_t              listener;

    // the user private data of the listener
    lx_cpointer_t                   listener_priv;

    // the observed listener events
    lx_size_t                       listener_events;

}lx_mesh_impl_t;

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
static lx_void_t lx_mesh_splice_edge(lx_mesh_edge_ref_t a, lx_mesh_edge_ref_t b)
{
    // check
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
static lx_void_t lx_mesh_save_face_at_orbit(lx_mesh_edge_ref_t edge, lx_mesh_face_ref_t lface)
{
    // check
    lx_assert(edge);

    // done
    lx_mesh_edge_ref_t scan = edge;
    do
    {
        // set lface
        lx_mesh_edge_lface_set(scan, lface);

        // the next edge
        scan = lx_mesh_edge_lnext(scan);
    }
    while (scan != edge);
}
static lx_void_t lx_mesh_save_vertex_at_orbit(lx_mesh_edge_ref_t edge, lx_mesh_vertex_ref_t org)
{
    // check
    lx_assert(edge);

    // done
    lx_mesh_edge_ref_t scan = edge;
    do
    {
        // set org
        lx_mesh_edge_org_set(scan, org);

        // the next edge
        scan = lx_mesh_edge_onext(scan);
    }
    while (scan != edge);
}
static lx_inline lx_void_t lx_mesh_post_event(lx_mesh_impl_t* impl, lx_size_t type, lx_pointer_t org, lx_pointer_t dst)
{
    // this event is observing? done listener
    if ((impl->listener_events & type) && impl->listener)
    {
        // init event
        lx_mesh_event_t event = {type, org, dst, impl->listener_priv};

        // done event
        impl->listener(&event);
    }
}
static lx_inline lx_mesh_edge_ref_t lx_mesh_make_edge(lx_mesh_impl_t* impl, lx_bool_t is_loop, lx_bool_t is_ccw)
{
    // check
    lx_assert_and_check_return_val(impl && impl->edges, lx_null);

    // make the edge
    lx_mesh_edge_ref_t edge = is_loop? lx_mesh_edge_list_make_loop(impl->edges, is_ccw) : lx_mesh_edge_list_make(impl->edges);
    lx_assert_and_check_return_val(edge, lx_null);

    // post the init event
    lx_mesh_post_event(impl, LX_MESH_EVENT_EDGE_INIT, edge, lx_null);

    // ok
    return edge;
}
static lx_inline lx_mesh_face_ref_t lx_mesh_make_face(lx_mesh_impl_t* impl)
{
    // check
    lx_assert_and_check_return_val(impl && impl->faces, lx_null);

    // make the face
    lx_mesh_face_ref_t face = lx_mesh_face_list_make(impl->faces);

    // post the init event
    lx_mesh_post_event(impl, LX_MESH_EVENT_FACE_INIT, face, lx_null);

    // ok
    return face;
}
static lx_inline lx_mesh_face_ref_t lx_mesh_make_face_at_orbit(lx_mesh_impl_t* impl, lx_mesh_edge_ref_t edge)
{
    // check
    lx_assert_and_check_return_val(impl && edge, lx_null);

    // make the new face
    lx_mesh_face_ref_t face_new = lx_mesh_make_face(impl);
    lx_assert_and_check_return_val(face_new, lx_null);

    // update left face for all edges in the orbit of the edge
    lx_mesh_save_face_at_orbit(edge, face_new);

    // ok
    return face_new;
}
static lx_inline lx_mesh_vertex_ref_t lx_mesh_make_vertex(lx_mesh_impl_t* impl)
{
    // check
    lx_assert_and_check_return_val(impl && impl->vertices, lx_null);

    // make the vertex
    lx_mesh_vertex_ref_t vertex = lx_mesh_vertex_list_make(impl->vertices);

    // post the init event
    lx_mesh_post_event(impl, LX_MESH_EVENT_VERTEX_INIT, vertex, lx_null);

    // ok
    return vertex;
}
static lx_inline lx_mesh_vertex_ref_t lx_mesh_make_vertex_at_orbit(lx_mesh_impl_t* impl, lx_mesh_edge_ref_t edge)
{
    // check
    lx_assert_and_check_return_val(impl && edge, lx_null);

    // make the new vertex
    lx_mesh_vertex_ref_t vertex_new = lx_mesh_make_vertex(impl);
    lx_assert_and_check_return_val(vertex_new, lx_null);

    // update origin for all edges leaving the orbit of the edge
    lx_mesh_save_vertex_at_orbit(edge, vertex_new);

    // ok
    return vertex_new;
}
static lx_inline lx_void_t lx_mesh_kill_edge(lx_mesh_impl_t* impl, lx_mesh_edge_ref_t edge)
{
    // check
    lx_assert_and_check_return(impl && impl->edges && edge);

    // post the exit event
    lx_mesh_post_event(impl, LX_MESH_EVENT_EDGE_EXIT, edge, lx_null);

    // kill the edge
    lx_mesh_edge_list_kill(impl->edges, edge);
}
static lx_inline lx_void_t lx_mesh_kill_face(lx_mesh_impl_t* impl, lx_mesh_face_ref_t face)
{
    // check
    lx_assert_and_check_return(impl && impl->faces && face);

    // post the exit event
    lx_mesh_post_event(impl, LX_MESH_EVENT_FACE_EXIT, face, lx_null);

    // kill the face
    lx_mesh_face_list_kill(impl->faces, face);
}
static lx_inline lx_void_t lx_mesh_kill_face_at_orbit(lx_mesh_impl_t* impl, lx_mesh_face_ref_t face, lx_mesh_face_ref_t face_new)
{
    // check
    lx_assert_and_check_return(impl && face);

    // update lface for all edges leaving the deleted face
    lx_mesh_save_face_at_orbit(lx_mesh_face_edge(face), face_new);

    // kill the face
    lx_mesh_kill_face(impl, face);
}
static lx_inline lx_void_t lx_mesh_kill_vertex(lx_mesh_impl_t* impl, lx_mesh_vertex_ref_t vertex)
{
    // check
    lx_assert_and_check_return(impl && impl->vertices && vertex);

    // post the exit event
    lx_mesh_post_event(impl, LX_MESH_EVENT_VERTEX_EXIT, vertex, lx_null);

    // kill the vertex
    lx_mesh_vertex_list_kill(impl->vertices, vertex);
}
static lx_inline lx_void_t lx_mesh_kill_vertex_at_orbit(lx_mesh_impl_t* impl, lx_mesh_vertex_ref_t vertex, lx_mesh_vertex_ref_t org_new)
{
    // check
    lx_assert_and_check_return(impl && vertex);

    // update origin for all edges leaving the deleted vertex
    lx_mesh_save_vertex_at_orbit(lx_mesh_vertex_edge(vertex), org_new);

    // kill the vertex
    lx_mesh_kill_vertex(impl, vertex);
}
static lx_bool_t lx_mesh_kill_isolated_edge(lx_mesh_impl_t* impl, lx_mesh_edge_ref_t edge)
{
    // check
    lx_assert_and_check_return_val(impl && edge, lx_false);

    // is isolated edge?
    if (lx_mesh_edge_is_isolated(edge))
    {
        // check
        lx_assert(lx_mesh_edge_org(edge) != lx_mesh_edge_dst(edge));
        lx_assert(lx_mesh_edge_lface(edge) == lx_mesh_edge_rface(edge));

        // kill the origin and destination vertices
        lx_mesh_kill_vertex(impl, lx_mesh_edge_org(edge));
        lx_mesh_kill_vertex(impl, lx_mesh_edge_dst(edge));

        // kill the face
        lx_mesh_kill_face(impl, lx_mesh_edge_lface(edge));

        // kill the edge
        lx_mesh_kill_edge(impl, edge);

        // ok
        return lx_true;
    }
    // is isolated loop edge?
    else if (lx_mesh_edge_is_isolated_loop(edge))
    {
        // check
        lx_assert(lx_mesh_edge_org(edge) == lx_mesh_edge_dst(edge));
        lx_assert(lx_mesh_edge_lface(edge) != lx_mesh_edge_rface(edge));

        // kill the vertex
        lx_mesh_kill_vertex(impl, lx_mesh_edge_org(edge));

        // kill the left and right face
        lx_mesh_kill_face(impl, lx_mesh_edge_lface(edge));
        lx_mesh_kill_face(impl, lx_mesh_edge_rface(edge));

        // kill the edge
        lx_mesh_kill_edge(impl, edge);

        // ok
        return lx_true;
    }

    // no isolated
    return lx_false;
}
#ifdef LX_DEBUG
static lx_long_t lx_mesh_printf_edge(lx_cpointer_t object, lx_char_t* cstr, lx_size_t maxn)
{
    // check
    lx_assert_and_check_return_val(object && cstr && maxn, -1);

    // the edge
    lx_mesh_edge_ref_t edge = (lx_mesh_edge_ref_t)object;

    // make info
    return lx_mesh_edge_list_cstr((lx_mesh_edge_list_ref_t)edge->list, edge, cstr, maxn);
}
static lx_long_t lx_mesh_printf_face(lx_cpointer_t object, lx_char_t* cstr, lx_size_t maxn)
{
    // check
    lx_assert_and_check_return_val(object && cstr && maxn, -1);

    // the face
    lx_mesh_face_ref_t face = (lx_mesh_face_ref_t)object;

    // make info
    return lx_mesh_face_list_cstr((lx_mesh_face_list_ref_t)face->list, face, cstr, maxn);
}
static lx_long_t lx_mesh_printf_vertex(lx_cpointer_t object, lx_char_t* cstr, lx_size_t maxn)
{
    // check
    lx_assert_and_check_return_val(object && cstr && maxn, -1);

    // the vertex
    lx_mesh_vertex_ref_t vertex = (lx_mesh_vertex_ref_t)object;

    // make info
    return lx_mesh_vertex_list_cstr((lx_mesh_vertex_list_ref_t)vertex->list, vertex, cstr, maxn);
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_mesh_ref_t lx_mesh_init(lx_element_t edge_element, lx_element_t face_element, lx_element_t vertex_element)
{
    // done
    lx_bool_t           ok = lx_false;
    lx_mesh_impl_t*     impl = lx_null;
    do
    {
        // make mesh
        impl = lx_malloc0_type(lx_mesh_impl_t);
        lx_assert_and_check_break(impl);

        // init edges
        impl->edges = lx_mesh_edge_list_init(edge_element);
        lx_assert_and_check_break(impl->edges);

        // init faces
        impl->faces = lx_mesh_face_list_init(face_element);
        lx_assert_and_check_break(impl->faces);

        // init vertices
        impl->vertices = lx_mesh_vertex_list_init(vertex_element);
        lx_assert_and_check_break(impl->vertices);

#ifdef LX_DEBUG
        /* register printf("%{mesh_face}",      face);
         * register printf("%{mesh_edge}",      edge);
         * register printf("%{mesh_vertex}",    vertex);
         */
        static lx_bool_t s_is_registered = lx_false;
        if (!s_is_registered)
        {
            // register them
            lx_printf_object_register("mesh_edge",      lx_mesh_printf_edge);
            lx_printf_object_register("mesh_face",      lx_mesh_printf_face);
            lx_printf_object_register("mesh_vertex",    lx_mesh_printf_vertex);

            // ok
            s_is_registered = lx_true;
        }
#endif

        // ok
        ok = lx_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) lx_mesh_exit((lx_mesh_ref_t)impl);
        impl = lx_null;
    }

    // ok?
    return (lx_mesh_ref_t)impl;
}
lx_void_t lx_mesh_exit(lx_mesh_ref_t mesh)
{
    // check
    lx_mesh_impl_t* impl = (lx_mesh_impl_t*)mesh;
    lx_assert_and_check_return(impl);

    // exit edges
    if (impl->edges) lx_mesh_edge_list_exit(impl->edges);
    impl->edges = lx_null;

    // exit faces
    if (impl->faces) lx_mesh_face_list_exit(impl->faces);
    impl->faces = lx_null;

    // exit vertices
    if (impl->vertices) lx_mesh_vertex_list_exit(impl->vertices);
    impl->vertices = lx_null;

    // exit it
    lx_free(impl);
}
lx_void_t lx_mesh_clear(lx_mesh_ref_t mesh)
{
    // check
    lx_mesh_impl_t* impl = (lx_mesh_impl_t*)mesh;
    lx_assert_and_check_return(impl);

    // clear edges
    if (impl->edges) lx_mesh_edge_list_clear(impl->edges);

    // clear faces
    if (impl->faces) lx_mesh_face_list_clear(impl->faces);

    // clear vertices
    if (impl->vertices) lx_mesh_vertex_list_clear(impl->vertices);
}
lx_bool_t lx_mesh_is_empty(lx_mesh_ref_t mesh)
{
    // the edges is non-empty?
    if (lx_iterator_size(lx_mesh_edge_itor(mesh))) return lx_false;

    // the faces is non-empty?
    if (lx_iterator_size(lx_mesh_face_itor(mesh))) return lx_false;

    // the vertices is non-empty?
    if (lx_iterator_size(lx_mesh_vertex_itor(mesh))) return lx_false;

    // is empty
    return lx_true;
}
lx_void_t lx_mesh_listener_set(lx_mesh_ref_t mesh, lx_mesh_listener_t listener, lx_cpointer_t priv)
{
    // check
    lx_mesh_impl_t* impl = (lx_mesh_impl_t*)mesh;
    lx_assert_and_check_return(impl);

    // set listener
    impl->listener      = listener;
    impl->listener_priv = priv;
}
lx_void_t lx_mesh_listener_event_add(lx_mesh_ref_t mesh, lx_size_t events)
{
    // check
    lx_mesh_impl_t* impl = (lx_mesh_impl_t*)mesh;
    lx_assert_and_check_return(impl);

    // add listener events
    impl->listener_events |= events;
}
lx_void_t lx_mesh_listener_event_del(lx_mesh_ref_t mesh, lx_size_t events)
{
    // check
    lx_mesh_impl_t* impl = (lx_mesh_impl_t*)mesh;
    lx_assert_and_check_return(impl);

    // delete listener events
    impl->listener_events &= ~events;
}
#if 0
lx_mesh_vertex_ref_t lx_mesh_vertex_head(lx_mesh_ref_t mesh)
{
    // the iterator
    lx_iterator_ref_t iterator = lx_mesh_vertex_itor(mesh);
    lx_assert_and_check_return_val(iterator, lx_null);

    // the head vertex
    return (lx_mesh_vertex_ref_t)lx_iterator_item(iterator, lx_iterator_head(iterator));
}
lx_mesh_vertex_ref_t lx_mesh_vertex_last(lx_mesh_ref_t mesh)
{
    // the iterator
    lx_iterator_ref_t iterator = lx_mesh_vertex_itor(mesh);
    lx_assert_and_check_return_val(iterator, lx_null);

    // the last vertex
    return (lx_mesh_vertex_ref_t)lx_iterator_item(iterator, lx_iterator_last(iterator));
}
#endif
lx_size_t lx_mesh_vertex_order(lx_mesh_ref_t mesh)
{
    // check
    lx_mesh_impl_t* impl = (lx_mesh_impl_t*)mesh;
    lx_assert_and_check_return_val(impl && impl->vertices, LX_MESH_ORDER_INSERT_TAIL);

    // the vertex order
    return lx_mesh_vertex_list_order(impl->vertices);
}
lx_void_t lx_mesh_vertex_order_set(lx_mesh_ref_t mesh, lx_size_t order)
{
    // check
    lx_mesh_impl_t* impl = (lx_mesh_impl_t*)mesh;
    lx_assert_and_check_return(impl && impl->vertices);

    // set the vertex order
    lx_mesh_vertex_list_order_set(impl->vertices, order);
}
#if 0
lx_mesh_face_ref_t lx_mesh_face_head(lx_mesh_ref_t mesh)
{
    // the iterator
    lx_iterator_ref_t iterator = lx_mesh_face_itor(mesh);
    lx_assert_and_check_return_val(iterator, lx_null);

    // the head face
    return (lx_mesh_face_ref_t)lx_iterator_item(iterator, lx_iterator_head(iterator));
}
lx_mesh_face_ref_t lx_mesh_face_last(lx_mesh_ref_t mesh)
{
    // the iterator
    lx_iterator_ref_t iterator = lx_mesh_face_itor(mesh);
    lx_assert_and_check_return_val(iterator, lx_null);

    // the last face
    return (lx_mesh_face_ref_t)lx_iterator_item(iterator, lx_iterator_last(iterator));
}
#endif
lx_size_t lx_mesh_face_order(lx_mesh_ref_t mesh)
{
    // check
    lx_mesh_impl_t* impl = (lx_mesh_impl_t*)mesh;
    lx_assert_and_check_return_val(impl && impl->faces, LX_MESH_ORDER_INSERT_TAIL);

    // the face order
    return lx_mesh_face_list_order(impl->faces);
}
lx_void_t lx_mesh_face_order_set(lx_mesh_ref_t mesh, lx_size_t order)
{
    // check
    lx_mesh_impl_t* impl = (lx_mesh_impl_t*)mesh;
    lx_assert_and_check_return(impl && impl->faces);

    // set the face order
    lx_mesh_face_list_order_set(impl->faces, order);
}
#if 0
lx_mesh_edge_ref_t lx_mesh_edge_head(lx_mesh_ref_t mesh)
{
    // the iterator
    lx_iterator_ref_t iterator = lx_mesh_edge_itor(mesh);
    lx_assert_and_check_return_val(iterator, lx_null);

    // the head edge
    return (lx_mesh_edge_ref_t)lx_iterator_item(iterator, lx_iterator_head(iterator));
}
lx_mesh_edge_ref_t lx_mesh_edge_last(lx_mesh_ref_t mesh)
{
    // the iterator
    lx_iterator_ref_t iterator = lx_mesh_edge_itor(mesh);
    lx_assert_and_check_return_val(iterator, lx_null);

    // the last edge
    return (lx_mesh_edge_ref_t)lx_iterator_item(iterator, lx_iterator_last(iterator));
}
lx_mesh_edge_ref_t lx_mesh_edge_tail(lx_mesh_ref_t mesh)
{
    // the iterator
    lx_iterator_ref_t iterator = lx_mesh_edge_itor(mesh);
    lx_assert_and_check_return_val(iterator, lx_null);

    // the tail edge
    return (lx_mesh_edge_ref_t)lx_iterator_item(iterator, lx_iterator_tail(iterator));
}
#endif
lx_size_t lx_mesh_edge_order(lx_mesh_ref_t mesh)
{
    // check
    lx_mesh_impl_t* impl = (lx_mesh_impl_t*)mesh;
    lx_assert_and_check_return_val(impl && impl->edges, LX_MESH_ORDER_INSERT_TAIL);

    // the edge order
    return lx_mesh_edge_list_order(impl->edges);
}
lx_void_t lx_mesh_edge_order_set(lx_mesh_ref_t mesh, lx_size_t order)
{
    // check
    lx_mesh_impl_t* impl = (lx_mesh_impl_t*)mesh;
    lx_assert_and_check_return(impl && impl->edges);

    // set the edge order
    lx_mesh_edge_list_order_set(impl->edges, order);
}
lx_mesh_edge_ref_t lx_mesh_edge_make(lx_mesh_ref_t mesh)
{
    // check
    lx_mesh_impl_t* impl = (lx_mesh_impl_t*)mesh;
    lx_assert_and_check_return_val(impl, lx_null);

    // done
    lx_bool_t               ok = lx_false;
    lx_mesh_edge_ref_t      edge = lx_null;
    lx_mesh_face_ref_t      face = lx_null;
    lx_mesh_vertex_ref_t    org = lx_null;
    lx_mesh_vertex_ref_t    dst = lx_null;
    do
    {
        // make the org
        org = lx_mesh_make_vertex(impl);
        lx_assert_and_check_break(org);

        // make the dst
        dst = lx_mesh_make_vertex(impl);
        lx_assert_and_check_break(dst);

        // make the face
        face = lx_mesh_make_face(impl);
        lx_assert_and_check_break(face);

        // make the edge
        edge = lx_mesh_make_edge(impl, lx_false, lx_false);
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

    // failed?
    if (!ok)
    {
        // kill the org
        if (org) lx_mesh_kill_vertex(impl, org);
        org = lx_null;

        // kill the dst
        if (dst) lx_mesh_kill_vertex(impl, dst);
        dst = lx_null;

        // kill the face
        if (face) lx_mesh_kill_face(impl, face);
        face = lx_null;

        // kill the edge
        if (edge) lx_mesh_kill_edge(impl, edge);
        edge = lx_null;
    }

    // ok?
    return edge;
}
lx_mesh_edge_ref_t lx_mesh_edge_make_loop(lx_mesh_ref_t mesh, lx_bool_t is_ccw)
{
    // check
    lx_mesh_impl_t* impl = (lx_mesh_impl_t*)mesh;
    lx_assert_and_check_return_val(impl, lx_null);

    // done
    lx_bool_t               ok = lx_false;
    lx_mesh_edge_ref_t      edge = lx_null;
    lx_mesh_face_ref_t      lface = lx_null;
    lx_mesh_face_ref_t      rface = lx_null;
    lx_mesh_vertex_ref_t    vertex = lx_null;
    do
    {
        // make the vertex
        vertex = lx_mesh_make_vertex(impl);
        lx_assert_and_check_break(vertex);

        // make the left face
        lface = lx_mesh_make_face(impl);
        lx_assert_and_check_break(lface);

        // make the right face
        rface = lx_mesh_make_face(impl);
        lx_assert_and_check_break(rface);

        // make the edge
        edge = lx_mesh_make_edge(impl, lx_true, is_ccw);
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

    // failed?
    if (!ok)
    {
        // kill the vertex
        if (vertex) lx_mesh_kill_vertex(impl, vertex);
        vertex = lx_null;

        // kill the left face
        if (lface) lx_mesh_kill_face(impl, lface);
        lface = lx_null;

        // kill the right face
        if (rface) lx_mesh_kill_face(impl, rface);
        rface = lx_null;

        // kill the edge
        if (edge) lx_mesh_kill_edge(impl, edge);
        edge = lx_null;
    }

    // ok?
    return edge;
}
lx_mesh_edge_ref_t lx_mesh_edge_split(lx_mesh_ref_t mesh, lx_mesh_edge_ref_t edge_org)
{
    // check
    lx_mesh_impl_t* impl = (lx_mesh_impl_t*)mesh;
    lx_assert_and_check_return_val(impl && edge_org, lx_null);

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
    if (edge_org_dprev != edge_org)
    {
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
        lx_mesh_edge_ref_t edge_new_sym = lx_mesh_edge_insert(mesh, edge_org_dprev, lx_mesh_edge_sym(edge_org));
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
    }
    /* the special case
     *
     *       .
     *         .        edge_org
     * . . . . . org ---------------> dst
     *       .
     *     .
     */
    else
    {
        /* append a new edge
         *                             lface
         *       .
         *         .        edge_org           edge_new_sym
         * . . . . . org ---------------> dst -------------> vertex_new
         *       .
         *     .                       rface
         */
        lx_mesh_edge_ref_t edge_new_sym = lx_mesh_edge_append(mesh, edge_org);
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
    lx_mesh_post_event(impl, LX_MESH_EVENT_EDGE_SPLIT, edge_org, edge_new);

    // ok
    return edge_new;
}
lx_void_t lx_mesh_edge_splice(lx_mesh_ref_t mesh, lx_mesh_edge_ref_t edge_org, lx_mesh_edge_ref_t edge_dst)
{
    // check
    lx_mesh_impl_t* impl = (lx_mesh_impl_t*)mesh;
    lx_assert_and_check_return(impl && edge_org && edge_dst);

    // check edges
    lx_mesh_check_edge(edge_org);
    lx_mesh_check_edge(edge_dst);

    // done
    lx_bool_t joining_faces = lx_false;
    lx_bool_t joining_vertices = lx_false;
    do
    {
        // is same? ok
        lx_check_break(edge_org != edge_dst);

        // two vertices are disjoint?
        if (lx_mesh_edge_org(edge_org) != lx_mesh_edge_org(edge_dst))
        {
            // joins the two vertices
            joining_vertices = lx_true;

            // remove the edge_dst.org first
            lx_mesh_kill_vertex_at_orbit(impl, lx_mesh_edge_org(edge_dst), lx_mesh_edge_org(edge_org));
        }

        // two faces are disjoint?
        if (lx_mesh_edge_lface(edge_org) != lx_mesh_edge_lface(edge_dst))
        {
            // joins the two faces
            joining_faces = lx_true;

            // post the merge event, merge(edge_dst.lface, edge_org.lface) => edge_org.lface
            lx_mesh_post_event(impl, LX_MESH_EVENT_FACE_MERGE, lx_mesh_edge_lface(edge_dst), lx_mesh_edge_lface(edge_org));

            // remove the edge_dst.lface first
            lx_mesh_kill_face_at_orbit(impl, lx_mesh_edge_lface(edge_dst), lx_mesh_edge_lface(edge_org));
        }

        // splice two edges
        lx_mesh_splice_edge(edge_dst, edge_org);

        // two vertices are disjoint?
        if (!joining_vertices)
        {
            /* make new vertex at edge_dst.org
             * and update origin for all edges leaving the origin orbit of the edge_dst
             */
            lx_mesh_vertex_ref_t vertex_new = lx_mesh_make_vertex_at_orbit(impl, edge_dst);
            lx_assert_and_check_break(vertex_new);

            // update the reference edge, the old reference edge may have been deleted
            lx_mesh_vertex_edge_set(lx_mesh_edge_org(edge_org), edge_org);
        }

        // two faces are disjoint?
        if (!joining_faces)
        {
            /* make new face at edge_dst.lface
             * and update lface for all edges leaving the left orbit of the edge_dst
             */
            lx_mesh_face_ref_t face_new = lx_mesh_make_face_at_orbit(impl, edge_dst);
            lx_assert_and_check_break(face_new);

            // post the split event, split(edge_org.lface) => (edge_org.lface, face_new)
            lx_mesh_post_event(impl, LX_MESH_EVENT_FACE_SPLIT, lx_mesh_edge_lface(edge_org), face_new);

            // update the reference edge, the old reference edge may have been deleted
            lx_mesh_face_edge_set(lx_mesh_edge_lface(edge_org), edge_org);
        }

    } while (0);
}
lx_mesh_edge_ref_t lx_mesh_edge_append(lx_mesh_ref_t mesh, lx_mesh_edge_ref_t edge_org)
{
    // check
    lx_mesh_impl_t* impl = (lx_mesh_impl_t*)mesh;
    lx_assert_and_check_return_val(impl && edge_org, lx_null);

    // done
    lx_bool_t           ok = lx_false;
    lx_mesh_edge_ref_t  edge_new = lx_null;
    do
    {
        // check edge
        lx_mesh_check_edge(edge_org);

        // make the new non-loop edge
        edge_new = lx_mesh_make_edge(impl, lx_false, lx_false);
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
        if (!lx_mesh_make_vertex_at_orbit(impl, edge_sym_new)) break;

        // ok
        ok = lx_true;

    } while (0);

    // failed
    if (!ok)
    {
        // kill the new edge
        if (edge_new) lx_mesh_kill_edge(impl, edge_new);
        edge_new = lx_null;
    }

    // ok?
    return edge_new;
}
lx_mesh_edge_ref_t lx_mesh_edge_insert(lx_mesh_ref_t mesh, lx_mesh_edge_ref_t edge_org, lx_mesh_edge_ref_t edge_dst)
{
    // check
    lx_mesh_impl_t* impl = (lx_mesh_impl_t*)mesh;
    lx_assert_and_check_return_val(impl && edge_org && edge_dst, lx_null);

    // done
    lx_bool_t           ok = lx_false;
    lx_mesh_edge_ref_t  edge_new = lx_null;
    do
    {
        // check edges
        lx_mesh_check_edge(edge_org);
        lx_mesh_check_edge(edge_dst);

        // make the new clockwise self-loop edge
        edge_new = lx_mesh_make_edge(impl, lx_true, lx_false);
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
        if (!lx_mesh_make_vertex_at_orbit(impl, edge_sym_new)) break;

        // ok
        ok = lx_true;

    } while (0);

    // failed
    if (!ok)
    {
        // kill the new edge
        if (edge_new) lx_mesh_kill_edge(impl, edge_new);
        edge_new = lx_null;
    }

    // ok?
    return edge_new;
}
lx_void_t lx_mesh_edge_remove(lx_mesh_ref_t mesh, lx_mesh_edge_ref_t edge_del)
{
    // check
    lx_mesh_impl_t* impl = (lx_mesh_impl_t*)mesh;
    lx_assert_and_check_return(impl && edge_del);

    // check edge
    lx_mesh_check_edge(edge_del);

    // isolated edge? kill it directly
    if (lx_mesh_kill_isolated_edge(impl, edge_del)) return ;

    // get the destinate edge
    lx_mesh_edge_ref_t edge_dst = lx_mesh_edge_lnext(edge_del);
    lx_assert_and_check_return(edge_dst);

    // get the original sym edge
    lx_mesh_edge_ref_t edge_sym_org = lx_mesh_edge_oprev(edge_del);
    lx_assert_and_check_return(edge_sym_org);

    // the sym edge
    lx_mesh_edge_ref_t edge_sym = lx_mesh_edge_sym(edge_del);
    lx_assert_and_check_return(edge_sym);

    /* use edge_sym_org for edge_dst if the destination vertex is isolated
     *
     * before:
     *
     *      edge_org        edge_del
     * ----------------> --------------->
     * <---------------- <---------------
     *    edge_sym_org      edge_dst'
     *      edge_dst
     */
    if (edge_dst == edge_sym) edge_dst = edge_sym_org;
    /* use edge_dst for edge_sym_org if the original vertex is isolated
     *
     * before:
     *
     *    edge_sym_org'   edge_sym_org
     *      edge_del'       edge_dst
     * ----------------> --------------->
     * <---------------- <---------------
     *      edge_del
     */
    else if (edge_sym_org == edge_del)
    {
        // reverse edge
        lx_swap(lx_mesh_edge_ref_t, edge_del, edge_sym);

        // update edge_sym_org
        edge_sym_org = edge_dst;
    }

    /* kill the destination vertex of the edge
     * and update origin for all edges leaving the destination orbit of the deleted edge
     */
    lx_mesh_kill_vertex_at_orbit(impl, lx_mesh_edge_dst(edge_del), lx_mesh_edge_org(edge_sym_org));

    /* remove edge
     *
     * before:
     *
     *        .
     *             .                            edge_dst.lface
     *                  .
     *      edge_sym_org     .          edge                       edge_dst
     *  <----------------- vertex ----------------> vertex_del ------------------->
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
     *  <----------------- vertex <---------------- vertex_del ------------------->
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
     *  <----------------- vertex / vertex_del ------------------->
     *                      |       |  .    .
     *     edge_org.rface    <-----.             .
     *                         .                      .
     *                     .                               .
     *                 .
     *
     * lx_mesh_splice_edge(edge_dst, edge_del):
     *
     *        .
     *             .                            edge_dst.lface
     *                  .
     *      edge_sym_org     . edge_del         edge_dst
     *  <----------------- vertex / vertex_del ------------------->
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
    lx_mesh_splice_edge(edge_dst,       edge_del);

    // update the reference edge, the old reference edge may have been deleted
    lx_mesh_vertex_edge_set(lx_mesh_edge_org(edge_sym_org),     edge_sym_org);
    lx_mesh_face_edge_set  (lx_mesh_edge_lface(edge_sym_org),   edge_sym_org);
    lx_mesh_face_edge_set  (lx_mesh_edge_lface(edge_dst),       edge_dst);

    // kill the edge
    lx_mesh_kill_edge(impl, edge_del);
}
lx_mesh_edge_ref_t lx_mesh_edge_connect(lx_mesh_ref_t mesh, lx_mesh_edge_ref_t edge_org, lx_mesh_edge_ref_t edge_dst)
{
    // check
    lx_mesh_impl_t* impl = (lx_mesh_impl_t*)mesh;
    lx_assert_and_check_return_val(impl && edge_org && edge_dst, lx_null);

    // check edges
    lx_mesh_check_edge(edge_org);
    lx_mesh_check_edge(edge_dst);

    // done
    lx_bool_t           ok = lx_false;
    lx_mesh_edge_ref_t  edge_new = lx_null;
    lx_bool_t           joining_faces = lx_false;
    do
    {
        // make the new non-loop edge
        edge_new = lx_mesh_make_edge(impl, lx_false, lx_false);
        lx_assert_and_check_break(edge_new);

        // the new sym edge
        lx_mesh_edge_ref_t edge_sym_new = lx_mesh_edge_sym(edge_new);
        lx_assert_and_check_break(edge_sym_new);

        // two faces are disjoint?
        if (lx_mesh_edge_lface(edge_org) != lx_mesh_edge_lface(edge_dst))
        {
            // joins the two faces
            joining_faces = lx_true;

            // post the merge event, merge(edge_dst.lface, edge_org.lface) => edge_org.lface
            lx_mesh_post_event(impl, LX_MESH_EVENT_FACE_MERGE, lx_mesh_edge_lface(edge_dst), lx_mesh_edge_lface(edge_org));

            // remove the edge_dst.lface first
            lx_mesh_kill_face_at_orbit(impl, lx_mesh_edge_lface(edge_dst), lx_mesh_edge_lface(edge_org));
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
        if (!joining_faces)
        {
            // save the old face first, edge_org.lface may have been modified after making new face
            lx_mesh_face_ref_t face_old = lx_mesh_edge_lface(edge_org);

            /* make new face at edge_new.lface
             * and update lface for all edges leaving the left orbit of the edge_new
             */
            lx_mesh_face_ref_t face_new = lx_mesh_make_face_at_orbit(impl, edge_new);
            lx_assert_and_check_break(face_new);

            // post the split event, split(edge_org.lface) => (edge_org.lface, face_new)
            lx_mesh_post_event(impl, LX_MESH_EVENT_FACE_SPLIT, face_old, face_new);
        }
        else
        {
            // init the edge_new.lface
            lx_mesh_edge_lface_set(edge_new, lx_mesh_edge_lface(edge_org));
        }

        // ok
        ok = lx_true;

    } while (0);

    // check
    lx_assert(ok);

    // failed
    if (!ok)
    {
        // kill the new edge
        if (edge_new) lx_mesh_kill_edge(impl, edge_new);
        edge_new = lx_null;
    }

    // ok?
    return edge_new;
}
lx_void_t lx_mesh_edge_delete(lx_mesh_ref_t mesh, lx_mesh_edge_ref_t edge_del)
{
    // check
    lx_mesh_impl_t* impl = (lx_mesh_impl_t*)mesh;
    lx_assert_and_check_return(impl && edge_del);

    // check edge
    lx_mesh_check_edge(edge_del);

    // done
    lx_bool_t joining_faces = lx_false;
    do
    {
        // two faces are disjoint?
        if (lx_mesh_edge_lface(edge_del) != lx_mesh_edge_rface(edge_del))
        {
            // joins the two faces
            joining_faces = lx_true;

            // post the merge event, merge(edge_del.lface, edge_del.rface) => edge_del.rface
            lx_mesh_post_event(impl, LX_MESH_EVENT_FACE_MERGE, lx_mesh_edge_lface(edge_del), lx_mesh_edge_rface(edge_del));

            // remove the edge_del.lface first
            lx_mesh_kill_face_at_orbit(impl, lx_mesh_edge_lface(edge_del), lx_mesh_edge_rface(edge_del));
        }

        /* before:
         *
         * edge_del.lface != edge_del.rface:
         *
         *         face_del
         *
         *         edge_org
         *  ---------------------->
         * |                       |
         * |                       |
         * |         face          | edge_del      face_del
         * |                       |
         * |                      \|/
         *  <----------------------
         *         edge_dst
         *
         *         face_del
         *
         *
         * edge_del.lface == edge_del.rface:
         *
         *
         *          edge_dst
         *  <-----------------------.
         * |            edge_del .  |
         * |     ------------->.    |
         * |    |   edge_org   |    |
         * |    |              |    |
         * |    |              |    |
         * |     <-------------     |
         * |        face_org        |
         *  ----------------------->
         *
         * edge_del.onext == edge_del:
         *
         *  ----------------------> <---------------- org
         * |                       |     edge_del
         * |                       |
         *  <----------------------
         *
         */
        if (lx_mesh_edge_onext(edge_del) == edge_del)
        {
            /* remove the edge_del.org
             *
             * after:
             *
             *  ----------------------> <---------------- null
             * |                       |     edge_del
             * |                       |
             *  <----------------------
             */
            lx_mesh_kill_vertex_at_orbit(impl, lx_mesh_edge_org(edge_del), lx_null);
        }
        else
        {
            // update the reference edge, the old reference edge may have been invalid
            lx_mesh_face_edge_set  (lx_mesh_edge_rface(edge_del),   lx_mesh_edge_oprev(edge_del));
            lx_mesh_vertex_edge_set(lx_mesh_edge_org(edge_del),     lx_mesh_edge_onext(edge_del));

            /* disjoining edges at the edge_del.org
             *
             * after:
             *
             * edge_del.lface != edge_del.rface:
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
             *         edge_dst              edge_del
             *
             *
             * edge_del.lface == edge_del.rface:
             *
             *
             *          edge_dst
             *  <-----------------------.
             * |                     .  |
             * |                   .    |        ------------->
             * |          edge_del      |       |   edge_org   |
             * |                        |       |              |
             * |        face_new        |       |              |
             * |                        |        <-------------
             * |                        |           face_org
             *  ----------------------->
             *
             */
            lx_mesh_splice_edge(edge_del, lx_mesh_edge_oprev(edge_del));

            // two faces are disjoint?
            if (!joining_faces)
            {
                // save the old face first, edge_del.lface may have been modified after making new face
                lx_mesh_face_ref_t face_old = lx_mesh_edge_lface(edge_del);

                /* make new face at edge_del.lface
                 * and update lface for all edges leaving the left orbit of the edge_del
                 */
                lx_mesh_face_ref_t face_new = lx_mesh_make_face_at_orbit(impl, edge_del);
                lx_assert_and_check_break(face_new);

                // post the split event, split(face_old) => (face_old, face_new)
                lx_mesh_post_event(impl, LX_MESH_EVENT_FACE_SPLIT, face_old, face_new);
            }
        }

        // the sym edge
        lx_mesh_edge_ref_t edge_sym = lx_mesh_edge_sym(edge_del);
        lx_assert_and_check_break(edge_sym);

        // the deleted edge is isolated now?
        if (lx_mesh_edge_onext(edge_sym) == edge_sym)
        {
            /* remove the edge_del directly
             *
             * before:
             *
             *  ---------------------->           <---------------- null
             * |                       |               edge_del
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
            lx_mesh_kill_vertex_at_orbit(impl, lx_mesh_edge_org(edge_sym), lx_null);
            lx_mesh_kill_face_at_orbit(impl, lx_mesh_edge_lface(edge_sym), lx_null);
        }
        else
        {
            // update the reference edge, the old reference edge may have been invalid
            lx_mesh_face_edge_set  (lx_mesh_edge_lface(edge_del),   lx_mesh_edge_oprev(edge_sym));
            lx_mesh_vertex_edge_set(lx_mesh_edge_org(edge_sym),     lx_mesh_edge_onext(edge_sym));

            /* disjoining edges at the edge_del.dst
             *
             * after:
             *
             * edge_del.lface != edge_del.rface:
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
             *         edge_dst                     edge_del
             *
             *
             * edge_del.lface == edge_del.rface:
             *
             *
             *          edge_dst
             *  <-----------------------
             * |                        |
             * |       ---------->      |        ------------->
             * |        edge_del        |       |   edge_org   |
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
        lx_mesh_kill_edge(impl, edge_del);

    } while (0);
}
#ifdef LX_DEBUG
lx_void_t lx_mesh_dump(lx_mesh_ref_t mesh)
{
    // trace
    lx_trace_i("");
    lx_trace_i("edges:");

    // dump edges
    lx_for_all_if (lx_mesh_edge_ref_t, edge, lx_mesh_edge_itor(mesh), edge)
    {
        // trace
        lx_trace_i("    %{mesh_edge}", edge);
    }

    // trace
    lx_trace_i("faces:");

    // dump faces
    lx_for_all_if (lx_mesh_face_ref_t, face, lx_mesh_face_itor(mesh), face)
    {
        // trace
        lx_trace_i("    face: %{mesh_face}", face);

        // dump face.edges
        lx_mesh_edge_ref_t head = lx_mesh_face_edge(face);
        lx_mesh_edge_ref_t edge = head;
        do
        {
            // trace
            lx_trace_i("        %{mesh_edge}", edge);

            // the next edge
            edge = lx_mesh_edge_lnext(edge);

        } while (edge != head);
    }

    // trace
    lx_trace_i("vertices:");

    // dump vertices
    lx_for_all_if (lx_mesh_vertex_ref_t, vertex, lx_mesh_vertex_itor(mesh), vertex)
    {
        // trace
        lx_trace_i("    vertex: %{mesh_vertex}", vertex);

        // dump vertex.edges
        lx_mesh_edge_ref_t head = lx_mesh_vertex_edge(vertex);
        lx_mesh_edge_ref_t edge = head;
        do
        {
            // trace
            lx_trace_i("        %{mesh_edge}", edge);

            // the next edge
            edge = lx_mesh_edge_onext(edge);

        } while (edge != head);
    }
}
lx_void_t lx_mesh_check(lx_mesh_ref_t mesh)
{
    // check edges
    lx_for_all_if (lx_mesh_edge_ref_t, edge, lx_mesh_edge_itor(mesh), edge)
    {
        // check edge
        lx_mesh_check_edge(edge);
        lx_assert(lx_mesh_edge_sym(edge) != edge);
        lx_assert(lx_mesh_edge_sym(lx_mesh_edge_sym(edge)) == edge);
        lx_assert(lx_mesh_edge_org(edge));
        lx_assert(lx_mesh_edge_dst(edge));
        lx_assert(lx_mesh_edge_sym(lx_mesh_edge_onext(lx_mesh_edge_lnext(edge))) == edge);
        lx_assert(lx_mesh_edge_lnext(lx_mesh_edge_sym(lx_mesh_edge_onext(edge))) == edge);
    }

    // check faces
    lx_for_all_if (lx_mesh_face_ref_t, face, lx_mesh_face_itor(mesh), face)
    {
        // check face
        lx_mesh_check_face(face);

        // check face.edges
        lx_mesh_edge_ref_t head = lx_mesh_face_edge(face);
        lx_mesh_edge_ref_t edge = head;
        do
        {
            // check edge
            lx_mesh_check_edge(edge);
            lx_assert(lx_mesh_edge_sym(edge) != edge);
            lx_assert(lx_mesh_edge_sym(lx_mesh_edge_sym(edge)) == edge);
            lx_assert(lx_mesh_edge_sym(lx_mesh_edge_onext(lx_mesh_edge_lnext(edge))) == edge);
            lx_assert(lx_mesh_edge_lnext(lx_mesh_edge_sym(lx_mesh_edge_onext(edge))) == edge);
            lx_assert(lx_mesh_edge_lface(edge) == face);

            // the next edge
            edge = lx_mesh_edge_lnext(edge);

        } while (edge != head);
    }

    // check vertices
    lx_for_all_if (lx_mesh_vertex_ref_t, vertex, lx_mesh_vertex_itor(mesh), vertex)
    {
        // check vertex
        lx_mesh_check_vertex(vertex);

        // check vertex.edges
        lx_mesh_edge_ref_t head = lx_mesh_vertex_edge(vertex);
        lx_mesh_edge_ref_t edge = head;
        do
        {
            // check edge
            lx_mesh_check_edge(edge);
            lx_assert(lx_mesh_edge_sym(edge) != edge);
            lx_assert(lx_mesh_edge_sym(lx_mesh_edge_sym(edge)) == edge);
            lx_assert(lx_mesh_edge_sym(lx_mesh_edge_onext(lx_mesh_edge_lnext(edge))) == edge);
            lx_assert(lx_mesh_edge_lnext(lx_mesh_edge_sym(lx_mesh_edge_onext(edge))) == edge);
            lx_assert(lx_mesh_edge_org(edge) == vertex);

            // the next edge
            edge = lx_mesh_edge_onext(edge);

        } while (edge != head);
    }
}
#endif
