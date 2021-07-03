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

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_void_t lx_tessellator_listener(lx_mesh_event_ref_t event) {
    lx_assert(event);
    switch (event->type) {
    case LX_MESH_EVENT_FACE_SPLIT: {
            // the org and dst face
            lx_mesh_face_ref_t face_org = (lx_mesh_face_ref_t)event->org;
            lx_mesh_face_ref_t face_dst = (lx_mesh_face_ref_t)event->dst;

            /* split(face_org) => (face_org, face_dst)
             *
             * the new face will inherit the inside attribute of the old face
             */
            lx_tessellator_face_inside_set(face_dst, lx_tessellator_face_inside(face_org));
        }
        break;
    case LX_MESH_EVENT_EDGE_SPLIT: {
            // the org and dst edge
            lx_mesh_edge_ref_t edge_org = (lx_mesh_edge_ref_t)event->org;
            lx_mesh_edge_ref_t edge_dst = (lx_mesh_edge_ref_t)event->dst;

            /* split(edge_org) => (edge_org, edge_dst)
             *
             * the new edge will inherit the winding attribute of the old edge
             */
            lx_tessellator_edge_winding_set(edge_dst, lx_tessellator_edge_winding(edge_org));
            lx_tessellator_edge_winding_set(lx_mesh_edge_sym(edge_dst), lx_tessellator_edge_winding(lx_mesh_edge_sym(edge_org)));
        }
        break;
    default:
        lx_assertf(0, "unknown listener event: %lx", event->type);
        break;
    }
}

#ifdef LX_DEBUG
static lx_int_t lx_tessellator_mesh_vsnprintf_edge(lx_char_t* s, lx_size_t n, lx_cpointer_t object) {
    lx_mesh_edge_ref_t edge = (lx_mesh_edge_ref_t)object;
    lx_assert(edge);
    return lx_snprintf(s, n, "(e%lu.w%d: %{tess_mesh_vertex} => %{tess_mesh_vertex})",
            edge->id, lx_tessellator_edge_winding(edge), edge->org, edge->sym->org);
}

static lx_int_t lx_tessellator_mesh_vsnprintf_face(lx_char_t* s, lx_size_t n, lx_cpointer_t object) {
    lx_mesh_face_ref_t face = (lx_mesh_face_ref_t)object;
    lx_assert(face);
    return lx_snprintf(s, n, "(f%lu: inside: %d)", face->id, lx_tessellator_face_inside(face));
}

static lx_int_t lx_tessellator_mesh_vsnprintf_vertex(lx_char_t* s, lx_size_t n, lx_cpointer_t object) {
    lx_mesh_vertex_ref_t vertex = (lx_mesh_vertex_ref_t)object;
    lx_assert(vertex);
    return lx_snprintf(s, n, "(v%lu: %{point})", vertex->id, lx_tessellator_vertex_point(vertex));
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bool_t lx_tessellator_mesh_make(lx_tessellator_t* tessellator, lx_polygon_ref_t polygon) {
    lx_assert(tessellator && polygon);

    // the points
    lx_point_ref_t      points = polygon->points;
    lx_uint16_t const*  counts = polygon->counts;
    lx_assert_and_check_return_val(points && counts, lx_false);

    // not exists mesh?
    if (!tessellator->mesh) {

        // init mesh
        lx_element_t edge_element    = lx_element_mem(sizeof(lx_tessellator_edge_t), lx_null);
        lx_element_t face_element    = lx_element_mem(sizeof(lx_tessellator_face_t), lx_null);
        lx_element_t vertex_element  = lx_element_mem(sizeof(lx_tessellator_vertex_t), lx_null);
        tessellator->mesh = lx_mesh_init(edge_element, face_element, vertex_element);

        /* init the order
         *
         * the new edges/faces/vertice will be inserted to the head of list
         */
        lx_mesh_edge_order_set(tessellator->mesh,      LX_MESH_ORDER_INSERT_HEAD);
        lx_mesh_face_order_set(tessellator->mesh,      LX_MESH_ORDER_INSERT_HEAD);
        lx_mesh_vertex_order_set(tessellator->mesh,    LX_MESH_ORDER_INSERT_HEAD);

        // init listener
        lx_mesh_listener_set(tessellator->mesh, lx_tessellator_listener, tessellator->mesh);
        lx_mesh_listener_event_add(tessellator->mesh, LX_MESH_EVENT_FACE_SPLIT | LX_MESH_EVENT_EDGE_SPLIT);

#ifdef LX_DEBUG
        /* register printf("%{tess_mesh_face}",   face);
         * register printf("%{tess_mesh_edge}",   edge);
         * register printf("%{tess_mesh_vertex}", vertex);
         */
        static lx_bool_t s_is_registered = lx_false;
        if (!s_is_registered) {
            lx_vsnprintf_object_register("tess_mesh_edge",   lx_tessellator_mesh_vsnprintf_edge);
            lx_vsnprintf_object_register("tess_mesh_face",   lx_tessellator_mesh_vsnprintf_face);
            lx_vsnprintf_object_register("tess_mesh_vertex", lx_tessellator_mesh_vsnprintf_vertex);
            s_is_registered = lx_true;
        }
#endif
    }

    // check
    lx_mesh_ref_t mesh = tessellator->mesh;
    lx_assert_and_check_return_val(mesh, lx_false);

    // clear mesh first
    lx_mesh_clear(mesh);

    lx_point_ref_t      point       = lx_null;
    lx_uint16_t         count       = *counts++;
    lx_size_t           index       = 0;
    lx_mesh_edge_ref_t  edge        = lx_null;
    lx_mesh_edge_ref_t  edge_first  = lx_null;
    while (index < count) {
        point = points++;
        if (!index) {
            lx_assertf(lx_point_eq(point, point + count - 1), "this contour(%lu: %{point} => %{point}) is not closed!", count, point, point + count - 1);
            edge = lx_null;
            edge_first = lx_null;
            lx_trace_d("move_to: %{point}", point);
        } else if (index + 1 == count) { // closed? connect an edge to the first edge
            edge = lx_mesh_edge_connect(mesh, edge, edge_first);
            lx_tessellator_face_inside_set(lx_mesh_edge_lface(edge), 0);
            lx_tessellator_face_inside_set(lx_mesh_edge_rface(edge), 0);
            lx_trace_d("closed: %{point}", point);
        } else {
            lx_trace_d("line_to: %{point}", point);
            if (edge_first) {
                edge = lx_mesh_edge_append(mesh, edge);
            } else {
                edge = lx_mesh_edge_make(mesh);
                edge_first = edge;
            }
        }

        // has new edge?
        if (edge) {
            // init edge.winding
            lx_tessellator_edge_winding_set(edge, 1);
            lx_tessellator_edge_winding_set(lx_mesh_edge_sym(edge), -1);

            // init edge.region
            lx_tessellator_edge_region_set(edge, lx_null);
            lx_tessellator_edge_region_set(lx_mesh_edge_sym(edge), lx_null);

            // init edge.dst
            lx_tessellator_vertex_point_set(lx_mesh_edge_dst(edge), point);
        }
        index++;
        if (index == count) { // next polygon
            count = *counts++;
            index = 0;
        }
    }

#ifdef LX_DEBUG
    lx_mesh_check(mesh);
#endif
    return !lx_mesh_is_empty(mesh);
}

lx_mesh_edge_ref_t lx_tessellator_mesh_make_edge(lx_tessellator_t* tessellator, lx_point_ref_t org, lx_point_ref_t dst) {
    lx_assert(tessellator && tessellator->mesh);

    // make edge
    lx_mesh_edge_ref_t edge = lx_mesh_edge_make(tessellator->mesh);
    lx_assert_and_check_return_val(edge, lx_null);

    // init edge.winding
    lx_tessellator_edge_winding_set(edge, 0);
    lx_tessellator_edge_winding_set(lx_mesh_edge_sym(edge), 0);

    // init edge.region
    lx_tessellator_edge_region_set(edge, lx_null);
    lx_tessellator_edge_region_set(lx_mesh_edge_sym(edge), lx_null);

    // init edge.faces.inside, lface == rface
    lx_tessellator_face_inside_set(lx_mesh_edge_lface(edge), 0);

    // init edge.org
    if (org) {
        lx_tessellator_vertex_point_set(lx_mesh_edge_org(edge), org);
    }

    // init edge.dst
    if (dst) {
        lx_tessellator_vertex_point_set(lx_mesh_edge_dst(edge), dst);
    }
    return edge;
}

