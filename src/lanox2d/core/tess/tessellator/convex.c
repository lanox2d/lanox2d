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
 * @file        convex.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "convex.h"
#include "geometry.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_void_t lx_tessellator_convex_make_face(lx_tessellator_t* tessellator, lx_mesh_face_ref_t face) {
    lx_assert(tessellator && face);

    // the mesh
    lx_mesh_ref_t mesh = tessellator->mesh;
    lx_assert(mesh);

    // the face edge
    lx_mesh_edge_ref_t edge = lx_mesh_face_edge(face);
    lx_assert(edge);

    // the first vertex
    lx_mesh_vertex_ref_t vertex_first = lx_mesh_edge_org(edge);
    lx_assert(vertex_first);

    lx_mesh_face_ref_t face_sym = lx_null;
    lx_mesh_edge_ref_t edge_sym = lx_null;
    lx_mesh_edge_ref_t edge_next = lx_null;
    while (1) {

        // save the next edge
        edge_next = lx_mesh_edge_lnext(edge);

        // the sym edge
        edge_sym = lx_mesh_edge_sym(edge);

        // the neighbour face
        face_sym = lx_mesh_edge_lface(edge_sym);

        // the neighbour face is inside too? merge it
        if (edge_sym && face_sym && lx_tessellator_face_inside(face_sym)) {
            /* merge it if the result region is convex
             *
             *       a
             *       .
             *       . .
             *       .   .
             *       .     .
             *       .       .
             *       .  face   .
             *       .           .
             *       .    edge ->  .
             *     b . . . . . . . . . d
             *         .   edge_sym  .
             *           .           .
             *             . face_sym.
             *               .       .
             *                 .     .
             *                   .   .
             *                     . .
             *                       .
             *                       c
             *
             * is_ccw(a, b, c) and is_ccw(c, d, a)?
             *
             * some degenerate triangles may be not merged
             * because the is_ccw is not reliable for almost-degenerate situations
             */
            if (    lx_tessellator_vertex_is_ccw( lx_mesh_edge_org(lx_mesh_edge_lprev(edge))
                                                , lx_mesh_edge_org(edge)
                                                , lx_mesh_edge_dst(lx_mesh_edge_lnext(edge_sym)))
                &&  lx_tessellator_vertex_is_ccw( lx_mesh_edge_org(lx_mesh_edge_lprev(edge_sym))
                                                , lx_mesh_edge_org(edge_sym)
                                                , lx_mesh_edge_dst(lx_mesh_edge_lnext(edge)))) {
                // save the next edge
                edge_next = lx_mesh_edge_lnext(edge_sym);

                // disconnect edge(d, b) and merge two faces
                lx_mesh_edge_disconnect(mesh, edge_sym);
                edge = lx_null;
            }
        }

        // end?
        if (edge && lx_mesh_edge_org(lx_mesh_edge_lnext(edge)) == vertex_first) {
            break;
        }
        edge = edge_next;
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_void_t lx_tessellator_convex_make(lx_tessellator_t* tessellator) {
    lx_assert(tessellator && tessellator->mesh);
    lx_for_all (lx_mesh_face_ref_t, face, lx_mesh_face_list(tessellator->mesh)) {
        lx_check_continue(lx_tessellator_face_inside(face));
        lx_tessellator_convex_make_face(tessellator, face);
    }

#ifdef LX_DEBUG
    lx_mesh_check(tessellator->mesh);
#endif
}


