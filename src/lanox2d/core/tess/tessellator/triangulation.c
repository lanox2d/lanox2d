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
 * @file        triangulation.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "geometry.h"
#include "triangulation.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#if 1
/* walk edges in counter-clockwise order from bottom to top
 *
 * we attach the bottommost right edge to the face when we made monotone polygon.
 * so we will find the bottommost left and right edge more quickly for triangulation.
 *
 *     .      .
 *    .       .
 *      .   . right => face.edge
 * left   .
 *
 *  |            |
 *   ----------->
 *       ccw
 */
#   define lx_tessellator_edge_go_up_(a)                        lx_tessellator_edge_go_down(a)
#   define lx_tessellator_edge_go_down_(a)                      lx_tessellator_edge_go_up(a)
#   define lx_tessellator_vertex_in_top_or_horizontal_(a, b)    (!lx_tessellator_vertex_in_top_or_horizontal(a, b))
#   define lx_tessellator_vertex_on_edge_or_left_(a, b, c)      lx_tessellator_vertex_on_edge_or_right(a, c, b)
#   define lx_tessellator_vertex_on_edge_or_right_(a, b, c)     lx_tessellator_vertex_on_edge_or_left(a, c, b)
#else
/* walk edges in counter-clockwise order from top to bottom
 *
 *          ccw
 *    <------------
 *   |             |
 *
 *          .
 * left   .   .  right
 *      .       .
 *       .        .
 *        .       .
 */
#   define lx_tessellator_edge_go_up_(a)                        lx_tessellator_edge_go_up(a)
#   define lx_tessellator_edge_go_down_(a)                      lx_tessellator_edge_go_down(a)
#   define lx_tessellator_vertex_in_top_or_horizontal_(a, b)    lx_tessellator_vertex_in_top_or_horizontal(a, b)
#   define lx_tessellator_vertex_on_edge_or_left_(a, b, c)      lx_tessellator_vertex_on_edge_or_left(a, b, c)
#   define lx_tessellator_vertex_on_edge_or_right_(a, b, c)     lx_tessellator_vertex_on_edge_or_right(a, b, c)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

/* make triangulation region
 *
 * TODO need optimization, maybe generate some degenerated triangles
 *
 * the region of this face must be horizontal monotone and counter-clockwise loop
 *
 * before:
 *
 *                  ccw
 *             <------------
 *            |             |
 *
 *                   1
 *                 .   .    right
 *      left     .       2
 *             .       .
 *           .       .
 *         3        4
 *          .        .
 *           .        .
 *            5        .
 *           .          .
 *          .            .
 *         .              .
 *        6                .
 *         .                .
 *          .               7
 *           .             .
 *            8          .
 *            .        9
 *            .        .
 *           10        11
 *             .      .
 *                .  .
 *                 12
 *
 *     |                        |
 *     |                        |
 *    \ /                      \ /
 *
 *
 * after:
 *                   1
 *                 .   .    R2
 *      L1       .     . 2
 *             .   .L4 .
 *           . .     .  R3
 *         3 . . R6 4
 *          .      . .
 *        L5 .  .L8   .
 *            5        .
 *           .   .      . R7
 *       L9 .     R11    .
 *         .         .    .
 *        6  .  R12        .
 *         .        .     . .
 *      L10 .             . 7
 *           .   L14 .     .
 *            8 .        . R13
 *            .  R16 . 9
 *        L15 .        .
 *           10  L18 . 11
 *              .     .
 *           L19  .  .  R17
 *                 12
 *
 */
static lx_void_t lx_tessellator_triangulation_make_face(lx_tessellator_t* tessellator, lx_mesh_face_ref_t face) {
    lx_assert(tessellator && face);

    // the mesh
    lx_mesh_ref_t mesh = tessellator->mesh;
    lx_assert(mesh);

    // the face edge
    lx_mesh_edge_ref_t edge = lx_mesh_face_edge(face);

    // must be triangle region at least
    lx_assert(edge && lx_mesh_edge_lnext(edge) != edge && lx_mesh_edge_lnext(lx_mesh_edge_lnext(edge)) != edge);

    /* get the uppermost left edge
     *
     * @note the face edge has been optimizated when we made monotone polygon.
     *
     *          .
     * left   .   .  right
     *      .       .
     *       .        .
     *        .       .
     *         .      .
     */
    lx_mesh_edge_ref_t left = edge;
    while (lx_tessellator_edge_go_down_(left)) {
        left = lx_mesh_edge_lprev(left);
    }
    while (lx_tessellator_edge_go_up_(left)) {
        left = lx_mesh_edge_lnext(left);
    }

    // get the uppermost right edge
    lx_mesh_edge_ref_t right = lx_mesh_edge_lprev(left);
    while (lx_mesh_edge_lnext(left) != right) {
        /* the right edge is too lower? done some left edges
         *
         *          .
         * left   .   . right
         *  dst .       .
         *       .        . org
         *        .       .
         *         .      .
         */
        if (lx_tessellator_vertex_in_top_or_horizontal_(lx_mesh_edge_dst(left), lx_mesh_edge_org(right))) {
            /* done some left edges
             *
             * go up? connect it
             *
             *     .
             *     .    .
             *     .        .
             *     .            .
             *  L  .   L1   .    .
             *     .     .        .
             *     .  .            .
             *     .  .  .  .  . .  .
             *             L2     .
             *                  . R
             *                .
             *
             *
             * on left? connect it
             *
             * TODO: will generate some degenerated triangles
             *
             *             .
             *           . .
             * on left?. . .
             *       .     .
             *     .    .  .
             *     .       .
             *   L .  .    . R
             *     ..      .
             *     .       .
             *    .        .
             *   .         .
             *             .
             *             .
             */
            while ( lx_mesh_edge_lnext(right) != left
                &&  (   lx_tessellator_edge_go_up_(lx_mesh_edge_lprev(left))
                    ||  lx_tessellator_vertex_on_edge_or_left_( lx_mesh_edge_org(left)
                                                            ,   lx_mesh_edge_org(lx_mesh_edge_lprev(left))
                                                            ,   lx_mesh_edge_dst(left)))) {
                // connect it
                edge = lx_mesh_edge_connect(mesh, left, lx_mesh_edge_lprev(left));
                lx_assert_and_check_return(edge);

                // update the left edge
                left = lx_mesh_edge_sym(edge);
            }

            // the next left edge
            left = lx_mesh_edge_lnext(left);
        } else {
            /* the left edge is too lower? done some right edges
             *
             *           .
             *   left  .   . right
             *       .       . org
             * dst .        .
             *     .       .
             *     .      .
             */
            /* make some right edges
             *
             * go down? connect it
             *
             *                      .
             *                 .    .
             *             .        .
             *         .            . R
             *        .   .   R1    .
             *       .       .      .
             *      .  R2       .   .
             *     . . . . . . . .  .
             *       .
             *      L  .
             *           .
             *
             * on right? connect it
             *
             * TODO: will generate some degenerated triangles
             *
             *     .
             *     .  .
             *     .  .  .
             *     .   .   .
             *     .    .    . on right?
             *     .     .   .
             *   L .      .  . R
             *     .       . .
             *     .         .
             *     .           .
             *     .             .
             *     .
             *     .
             */
            while ( lx_mesh_edge_lnext(right) != left
                &&  (   lx_tessellator_edge_go_down_(lx_mesh_edge_lnext(right))
                    ||  lx_tessellator_vertex_on_edge_or_right_(lx_mesh_edge_dst(right)
                                                            , lx_mesh_edge_dst(lx_mesh_edge_lnext(right))
                                                            , lx_mesh_edge_org(right)))) {
                // connect it
                edge = lx_mesh_edge_connect(mesh, lx_mesh_edge_lnext(right), right);
                lx_assert_and_check_return(edge);

                // update the right edge
                right = lx_mesh_edge_sym(edge);
            }

            // the next right edge
            right = lx_mesh_edge_lprev(right);
        }
    }

    // the last region must be triangle at least
    lx_assert(lx_mesh_edge_lnext(right) != left);

    /* tessellate the remaining region
     *
     *     . . . .
     *   .        .
     * .           .
     *    .         .
     * left  .    . right
     *          .
     *
     */
    while (lx_mesh_edge_lnext(lx_mesh_edge_lnext(right)) != left) {
        edge = lx_mesh_edge_connect(mesh, lx_mesh_edge_lnext(right), right);
        lx_assert_and_check_return(edge);

        right = lx_mesh_edge_sym(edge);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_void_t lx_tessellator_triangulation_make(lx_tessellator_t* tessellator) {
    lx_assert(tessellator && tessellator->mesh);

    // the new face must be inserted to the head of faces
    lx_assert(lx_mesh_face_order(tessellator->mesh) == LX_MESH_ORDER_INSERT_HEAD);

    // get iterator
    lx_iterator_t iterator;
    lx_iterator_of(&iterator, lx_mesh_face_list(tessellator->mesh));

    lx_size_t           itor = lx_iterator_head(&iterator);
    lx_size_t           tail = lx_iterator_tail(&iterator);
    lx_mesh_face_ref_t  face = lx_null;
    while (itor != tail) {
        face = (lx_mesh_face_ref_t)lx_iterator_item(&iterator, itor);
        lx_assert(face);

        /* the next face
         *
         * @note we don't process the new faces at the head
         */
        itor = lx_iterator_next(&iterator, itor);

        // the face is inside?
        if (lx_tessellator_face_inside(face)) {
            // make triangulation for the face region
            lx_tessellator_triangulation_make_face(tessellator, face);
        }
    }

#ifdef LX_DEBUG
    lx_mesh_check(tessellator->mesh);
#endif
}


