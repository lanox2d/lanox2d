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
 * @file        monotone.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "monotone.h"
#include "geometry.h"
#include "event_queue.h"
#include "active_region.h"
#ifdef LX_DEBUG
//#   include "profiler.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// enable profiler for debug?
#ifdef LX_DEBUG
//#   define LX_TESSELLATOR_PROFILER_ENABLE
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
static lx_void_t                            lx_tessellator_sweep_event(lx_tessellator_t* tessellator, lx_mesh_vertex_ref_t event);
static lx_tessellator_active_region_ref_t   lx_tessellator_find_left_top_region(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region);
static lx_tessellator_active_region_ref_t   lx_tessellator_find_left_bottom_region(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region);
static lx_void_t                            lx_tessellator_insert_down_going_edges(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region_left, lx_mesh_edge_ref_t edge_head, lx_mesh_edge_ref_t edge_tail, lx_mesh_edge_ref_t edge_left_top, lx_bool_t fix_dirty_regions);
static lx_mesh_edge_ref_t                   lx_tessellator_finish_top_regions(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region_head, lx_tessellator_active_region_ref_t region_tail);

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_inline lx_uint8_t lx_tessellator_winding_is_inside(lx_tessellator_t* tessellator, lx_int32_t winding) {
    switch (tessellator->rule) {
        case LX_TESSELLATOR_RULE_ODD:       return (winding & 1);
        case LX_TESSELLATOR_RULE_NONZERO:   return (winding != 0);
        default:                            break;
    }
    lx_assert(0);
    return 0;
}

static lx_void_t lx_tessellator_fix_region_edge(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region, lx_mesh_edge_ref_t edge) {
    lx_assert(tessellator && tessellator->mesh && region && region->fixedge && edge);
    lx_trace_d("fix a temporary edge: %{tess_region} => %{mesh_edge}", region, edge);

    // remove the temporary edge
    lx_mesh_edge_delete(tessellator->mesh, region->edge);

    // replace the new edge
    region->edge = edge;

    // clear the "fixedge" mark
    region->fixedge = 0;

    // update the region reference to the edge
    lx_tessellator_edge_region_set(edge, region);
}

/* fix the region order at the top edge
 *
 * theoretically, this should always be true.
 *
 * however, splitting an edge into two pieces can change the results of previous tests
 * because of some numerical errors when we are calculating intersections.
 *
 * so we need fix it in fix_all_dirty_regions().
 *
 * @note this will generate one degenerate face with only two edges after fixing.
 *
 * before:
 *
 *            .
 * edge_left  .
 *            .    .
 *            .
 *            .        . edge_left'
 *            .                        the previous region order have been changed
 *            .  .         .
 *            . . edge_right'
 *            ..          .
 *            . ----------------> x (intersection with numerical error)
 *           ..    splitting  . .
 *          . .           .  .
 *         .  .       .   .
 *        .   .   .    .
 *       .    .     .
 *      . .   .  .
 *     .      .
 * edge_right
 *
 * after:
 *
 *            .
 *
 *             .
 *               edge_new
 *              .
 *
 *               *
 *                    ..  edge_left/right (with a degenerate face)
 *                          ..
 *                               x (intersection with numerical error)
 *                            . .
 *                        .  .
 *                    .   .
 *                .    .
 *            .     .
 *        .      .
 *     .      .
 *
 * so
 *
 *            .                                      .                      .
 * edge_left  .                            edge_left .                      .
 *            .  region_left                         .                      .
 *            . (rface.inside)                       .                      . edge_new (rface.inside = region_left.inside)
 *            .                                      .       fixed          .
 *            .                              . . . . . .  ----------->      * . . . . .
 *            . edge_right         edge_right  .     .                     ..
 *            . . . . . .                        .   .                     .. edge_left/right (with a degenerate face)
 *            ..           intersection            . .                     ..
 *            .        x   ------------>             x                      x intersection with numerical error
 *           ..  intersection with numerical error  ..                     ..
 *          . .                                    . .                    . .
 *         .  .                                   .  .                   .  .
 *        .   .                                  .   .                  .   .
 *       .    .                                 .    .                 .    .
 *      .     .                                .     .                .     .
 *     .      .                               .      .               .      .
 * edge_right
 *
 * so we need fix the following case:
 *
 *  .                              .
 *   .                           .
 *    .  . ------------ . ---- . --------> fix it
 *     . .              .    .
 *      ..              .  .
 *       .              ..
 *       ..            ..
 *       . .         .  .
 *     L .  .      .    .
 *       . R .   . L    . R
 *       .    .         .
 *
 */
static lx_bool_t lx_tessellator_fix_region_order_at_top(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region_left) {
    lx_assert(tessellator && tessellator->mesh && region_left);

    // the right region
    lx_tessellator_active_region_ref_t region_right = lx_tessellator_active_regions_right(tessellator, region_left);
    lx_assert(region_right);

    // the edge of the left region
    lx_mesh_edge_ref_t edge_left = region_left->edge;
    lx_assert(edge_left);

    // the edge of the right region
    lx_mesh_edge_ref_t edge_right = region_right->edge;
    lx_assert(edge_right);

    // the destination of the left edge
    lx_mesh_vertex_ref_t edge_left_dst = lx_mesh_edge_dst(edge_left);
    lx_assert(edge_left_dst);

    // the destination of the right edge
    lx_mesh_vertex_ref_t edge_right_dst = lx_mesh_edge_dst(edge_right);
    lx_assert(edge_right_dst);

    // check
    lx_assert(!lx_tessellator_vertex_eq(edge_left_dst, edge_right_dst));

    /*
     *                  .
     *                  .
     *                  .
     *                  .
     *                  . ----- . --------> fix it
     *                  .     .
     *                  .   .
     *                  . .
     *                  .
     *                . .
     *              .   .
     *            .     .
     *          .       .
     *        .         .
     *    edge_left edge_right
     */
    if (lx_tessellator_vertex_in_top_or_hleft(edge_right_dst, edge_left_dst)) {
        /* we need not fix it if the edge_left.dst is in the edge_right's left
         *
         *            edge_right
         *                .
         *                .
         *                .
         *   edge_left    .
         *        .       .
         *                .
         *         .      .
         *                .
         *          .     .
         *                .
         *           .    .
         *                .
         *            .   .
         *                .
         *                .
         */
        if (lx_tessellator_vertex_in_edge_left(edge_left_dst, edge_right_dst, lx_mesh_edge_org(edge_right))) {
            return lx_false;
        }

        /* the right edge will be split, so we need mark it and it's left edge as "dirty"
         *
         *      fix_all_dirty_regions()
         *    <-------------------------
         *
         * edge_left    edge_right   edge_xxx
         *  (dirty)      (dirty)
         *    |             |-----------|
         *    |                recheck
         *    |-------------------|
         *           recheck
         *
         */
        region_left->dirty  = 1;
        region_right->dirty = 1;

        /* split the right edge and create a new edge
         *
         *      edge_new
         *          .
         *              .
         *        lface     x dst_new
         *                  .
         *                  .
         *                  .
         *                  . ----- . --------> fix it
         *                  .     .
         *                  .   .
         *                  . .
         *                  .
         *                . .
         *              .   .
         *            .     .
         *          . lface .
         *        .         .
         *    edge_left edge_right
         */
        lx_mesh_edge_ref_t edge_new = lx_mesh_edge_split(tessellator->mesh, edge_right);
        lx_assert(edge_new);

        /* splice the left edge and right edges
         *
         *             edge_new
         *                  .
         *
         *                      .
         *
         *                          . --------> fixed
         *                        .
         *                      .
         *                    .  .
         *                  .
         *                .
         *              .     .
         *            .
         *          .
         *        .        .
         *    edge_left edge_right
         */
        lx_mesh_edge_splice(tessellator->mesh, lx_mesh_edge_sym(edge_left), edge_new);

        // the destination of the left edge cannot be changed
        lx_assert(edge_left_dst == lx_mesh_edge_org(edge_new));
        lx_assert(edge_left_dst == lx_mesh_edge_dst(edge_left));
        lx_assert(edge_left_dst == lx_mesh_edge_dst(edge_right));

        /* update the inside of the new edge
         *
         * @note only region_left.inside have been calculated and validate.
         *
         * the new edge may be the last right edge before calculating intersection and fixing region order
         *
         *                    .
         *                    .
         *        region_left . edge_right
         *          (inside)  .
         *                    .
         *      edge_left .   .
         *                  . .
         *                    .
         *                    . .
         *                    .   .
         *                    .     .
         *                    .       .
         *
         * the region order was violated with some numerical error after calculating intersection
         *
         *                    .                                             .
         *                    .        lface.inside = region_left.inside => .
         *                    .                                             .
         *                    .                                             .
         *                    .                                             .
         *                    .                         fixed               .
         *                    .     . . . . .  ---------------------->  . . x . .
         *                    .       .                                   . .
         *                    .     .                                   .   .
         *                    .   .                                   .     .
         *                    . .                                   .       . region_right
         *                    .                                   .         .
         *                  . .                                 .           .
         *                .   .                               . region_left .
         *              .     . region_right                .               .
         *            .       .                           .                 .
         *          .         .                         .                   .
         *        .           .                       .                     .
         *      . region_left .                     .                       .
         *    .               .                   .                         .
         * edge_left     edge_right           edge_left                 edge_right
         */
        lx_tessellator_face_inside_set(lx_mesh_edge_lface(edge_new), region_left->inside);
    } else {
        /*
         *                    .
         *                    .
         *                    .
         *                    .
         *          . ------- . -----------> fix it
         *            .       .
         *              .     .
         *                .   .
         *                  . .
         *                    .
         *                    . .
         *                    .   .
         *                    .     .
         *                    .       .
         *                edge_left edge_right
         */
        /* we need not fix it if the edge_right.dst is in the edge_left's right
         *
         *            edge_left
         *                .
         *                .
         *                .
         *                .   edge_right
         *                .       .
         *                .
         *                .      .
         *                .
         *                .     .
         *                .
         *                .    .
         *                .
         *                .   .
         *                .
         *                .
         */
        if (lx_tessellator_vertex_in_edge_right(edge_right_dst, edge_left_dst, lx_mesh_edge_org(edge_left))) {
            return lx_false;
        }

        /* the left edge will be split, so we need mark it and it's left edge as "dirty"
         *
         *      fix_all_dirty_regions()
         *    <-------------------------
         *
         * edge_left2   edge_left   edge_right
         *  (dirty)      (dirty)
         *    |             |-----------|
         *    |                recheck
         *    |-------------------|
         *           recheck
         *
         */
        lx_tessellator_active_region_ref_t region_left2 = lx_tessellator_active_regions_left(tessellator, region_left);
        lx_assert(region_left2);

        region_left->dirty  = 1;
        region_left2->dirty = 1;

        /* split the left edge and create a new edge
         *
         *                       edge_new
         *                          .
         *                       .
         *          (dst_new) x
         *                    .
         *   edge_right.lnext .
         *             .      .
         *          . ------- . -----------> fix it
         *            .       .
         *              .     .
         *                .   .
         *                  . .
         *                    .
         *                    . .
         *                    .   .
         *                    .     .
         *                    .       .
         *                edge_left edge_right
         */
        lx_mesh_edge_ref_t edge_new = lx_mesh_edge_split(tessellator->mesh, edge_left);
        lx_assert(edge_new);

        /* splice the left edge and right edges
         *
         *               edge_new
         *                  .
         *
         *              .
         *               .
         *          . --------------------> fixed
         *            .
         *              .
         *            .   .
         *                  .
         *                    .
         *                .     .
         *                        .
         *                          .
         *                    .       .
         *                edge_left edge_right
         */
        lx_mesh_edge_splice(tessellator->mesh, lx_mesh_edge_lnext(edge_right), lx_mesh_edge_sym(edge_left));

        // the destination of the right edge cannot be changed
        lx_assert(edge_right_dst == lx_mesh_edge_org(edge_new));
        lx_assert(edge_right_dst == lx_mesh_edge_dst(edge_left));
        lx_assert(edge_right_dst == lx_mesh_edge_dst(edge_right));

        /* update the inside of the new edge
         *
         * @note only region_left.inside have been calculated and validate.
         *
         * the new edge may be the last right edge before calculating intersection and fixing region order
         *
         *                    .
         *                    . edge_left
         *                    .
         *                    . region_left(inside)
         *                    .
         *                    .   . edge_right
         *                    . .
         *                    .
         *                  . .
         *                .   .
         *              .     .
         *            .       .
         *
         *
         * the region order was violated with some numerical error after calculating intersection
         *
         *                                                    edge_new
         *                    .                                  .
         *                    .                                  .
         *                    .  region_left                     .
         *                    . (rface.inside)                   . rface.inside = region_left.inside
         *                    .                                  .
         *      . . . . .     . -------------------------->  . . x . .
         *            .       .              fixed               . .
         *              .     .                                  .   .
         *                .   .                                  .     .
         *                  . .                                  .       .  region_right
         *                    .                                  .         .
         *                    . . region_right                   .           .
         *                    .   .                              . region_left .
         *                    .     .                            .               .
         *                    .       .                          .                 .
         *                edge_left edge_right               edge_left        edge_right
         */
        lx_tessellator_face_inside_set(lx_mesh_edge_rface(edge_new), region_left->inside);
    }

    // fixed
    lx_trace_d("fix the top order for region: %{tess_region}", region_left);
    return lx_true;
}

/* fix the region order at the bottom edge
 *
 * the main purpose is to splice down-going edges with the same
 * destinate vertex and nearly identical slopes.
 *
 * the region order may be violated
 * because sometimes we can't distinguish the slopes numerically.
 *
 * .        ..
 * .        ..
 * .        ..
 * .        ..
 * .        ..
 * .        ..
 * .        ..
 * .. ----- ..---> fix it
 * .        .
 * .        .
 *
 * however the splicing can also help us to recover from numerical errors.
 *
 * .                    .
 *  .                   .          .
 *   . L . R            .        .
 *    .  .              . L    . R
 *     . .              .    .
 *      ..              .  .
 *       .              ..
 *       .. ---------- .. ---------------> fix it
 *
 * we also can fix those cases where we can't check for intersection.
 * - the left or right edge is a fixedge edge.
 *
 * .  (fixable edge)    .      (fixable edge)
 *  .                   .          .
 *   . L . R            .        .
 *    .  .              . L    . R
 *     . .              .    .
 *      ..              .  .
 *       .              ..
 *       ..            ..
 *       . .         .  .
 *       .  . ---- . -- . ---------------> fix it
 *           .   .
 */
static lx_bool_t lx_tessellator_fix_region_order_at_bottom(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region_left) {
    lx_assert(tessellator && tessellator->mesh && region_left);

    // the right region
    lx_tessellator_active_region_ref_t region_right = lx_tessellator_active_regions_right(tessellator, region_left);
    lx_assert(region_right);

    // the edge of the left region
    lx_mesh_edge_ref_t edge_left = region_left->edge;
    lx_assert(edge_left);

    // the edge of the right region
    lx_mesh_edge_ref_t edge_right = region_right->edge;
    lx_assert(edge_right);

    // the origin of the left edge
    lx_mesh_vertex_ref_t edge_left_org = lx_mesh_edge_org(edge_left);
    lx_assert(edge_left_org);

    // the origin of the right edge
    lx_mesh_vertex_ref_t edge_right_org = lx_mesh_edge_org(edge_right);
    lx_assert(edge_right_org);

    /*
     *  edge_left  edge_right  edge_left  edge_right edge_left   edge_right     edge_left/right
     *        .       .            .          .          .           .                ..
     *         .      .             .         .           .          .                ..
     *          .     .              .        .            .         .                ..
     *           .    .               .       .             .        .                ..
     *            .   .                .      .              .       .                ..
     *             .  .                 .     .               .      .                ..
     *              . .        or        .    .       or       .     .                ..
     *               ..                   .   .                 .    .                ..
     *                .                    .  .                  .   .                .
     *                ..                    . .                   .  .                .
     *                . .                    ..                    . .                .
     *                .  .                    .                     ..                .
     *                .   . ----------------- .. ------------------- . -------------- . --> fix it
     *                .
     *                .
     */
    if (lx_tessellator_vertex_in_top_or_hleft_or_eq(edge_left_org, edge_right_org)) {
        /* we need not fix it if the edge_left.org is in the edge_right's left
         *
         *  edge_left  edge_right
         *      .         .
         *                .
         *       .        .
         *                .
         *        .       .
         *                .
         *         .      .
         *                .
         *          .     .
         *                .
         *           .    .
         *                .
         *            .   .
         *                .
         *                .
         */
        if (lx_tessellator_vertex_in_edge_left(edge_left_org, lx_mesh_edge_dst(edge_right), edge_right_org)) {
            return lx_false;
        }

        /* before:
         *
         *  edge_left  edge_right     edge_left/right
         *        .       .                 ..
         *         .      .                 ..
         *          .     .                 ..
         *           .    .                 ..
         *            .   .                 ..
         *             .  .                 ..
         *              . .                 ..
         *               ..                 ..
         *                .                 ..
         *                ..                ..
         *                . .               ..
         *                .  .              ..
         *                .   . ----------- .. --> fix it
         *                .                 .
         *                .                 .
         *
         * after:
         *
         *  edge_left  edge_right    edge_left/right
         *        .       .                ..
         *         .                       ..
         *          .                      ..
         *           .     .               ..
         *            .                    ..
         *             .                   ..
         *              .   .              ..
         *               .                 ..
         *                .                ..
         *                 . .             ..
         *                  .              ..
         *                   .             ..
         *                    . ---------- . --> fixed
         *                  .              .
         *                .                .
         *            edge_new          edge_new
         *
         */
        if (!lx_tessellator_vertex_eq(edge_left_org, edge_right_org)) {
            /* the right edge will be split, so we need mark it and it's left edge as "dirty"
             *
             *      fix_all_dirty_regions()
             *    <-------------------------
             *
             * edge_left    edge_right   edge_xxx
             *  (dirty)      (dirty)
             *    |             |-----------|
             *    |                recheck
             *    |-------------------|
             *           recheck
             *
             */
            region_left->dirty  = 1;
            region_right->dirty = 1;

            /* split the right edge and create a new edge
             *
             *  edge_left  edge_right
             *        . lface .
             *         .      .
             *          .     .
             *           .    .
             *            .   .
             *             .  .
             *              . .
             *               ..
             *                .
             *                ..
             *                . .            lface
             *                .  .
             *                .   . . . . edge_left.oprev
             *                .
             *                x org_new
             *              .
             *            .
             *       edge_new
             *
             */
            lx_mesh_edge_split(tessellator->mesh, lx_mesh_edge_sym(edge_right));

            /* splice the left edge and right edges
             *
             *  edge_left  edge_right
             *        .       .
             *         .
             *          .
             *           .     .
             *            .
             *             .
             *              .   .
             *               .
             *                .
             *                 . .
             *                  .
             *                   .
             *                    . . . . -------------> fixed
             *                  .
             *                .
             *            edge_new (org_new have been removed)
             */
            lx_mesh_edge_splice(tessellator->mesh, lx_mesh_edge_oprev(edge_left), edge_right);

            // the origin of the left edge cannot be changed
            lx_assert(edge_left_org == lx_mesh_edge_org(edge_left));
            lx_assert(edge_left_org == lx_mesh_edge_org(edge_right));
        } else if (edge_left_org != edge_right_org) {

            /* edge_left.org lies on the edge_right.org
             *
             * before:
             *
             *  edge_left  edge_right  edge_left/right
             *        .       .              ..
             *         .      .              ..
             *          .     .              ..
             *           .    .              ..
             *            .   .              ..
             *             .  .              ..
             *              . .              ..
             *               ..              ..
             *                .              ..
             *                .. ----------- .. ----> fix it (coordinate is equal, but the order is incorrect)
             *
             * after:
             *
             *  edge_left  edge_right  edge_left/right
             *        .       .              ..
             *         .      .              ..
             *          .     .              ..
             *           .    .              ..
             *            .   .              ..
             *             .  .              ..
             *              . .              ..
             *               ..              ..
             *                . ------------ . ----> fixed
             */
            lx_trace_d("splice and remove the event vertex: %{mesh_vertex}", edge_left_org);

            /* remove the origin of the left edge first
             *
             *  edge_left  edge_right
             *        . lface .
             *         .      .
             *          .     .
             *           .    .
             *            .   .
             *             .  .
             *              . .
             *               ..
             *                .
             *                .x(removed)
             *                   .     lface
             *                     .
             *                       . edge_left.oprev
             */
            lx_tessellator_event_queue_remove(tessellator, edge_left_org);

            /* splice the left edge and right edges
             *
             *  edge_left  edge_right
             *        .       .
             *         .      .
             *          .     .
             *           .    .
             *            .   .
             *             .  .
             *              . .
             *               ..
             *                .
             */
            lx_mesh_edge_splice(tessellator->mesh, edge_right, lx_mesh_edge_oprev(edge_left));
        } else {
        /* we need not fix it,
         * but we need remove one edge with same slope for inserting down-going edges
         *
         *  edge_left/right
         *      ..
         *      ..
         *      ..
         *      ..
         *      .. (one edge will be removed after inserting down-going edges)
         *      ..
         *      ..
         *      ..
         *      .
         */
            return lx_mesh_edge_dst(edge_left) == lx_mesh_edge_dst(edge_right);
        }
    } else {
        /*
         *            edge_left  edge_right
         *                .       .
         *                .      .
         *                .     .
         *                .    .
         *                .   .
         *                .  .
         *                . .
         *                ..
         *                .
         *               ..
         *              . .
         *             .  .
         *            . - . -----------------> fix it
         *                .
         *                .
         *
         */
        /* we need not fix it if the edge_right.org is in the edge_left's right
         *
         *            edge_left  edge_right
         *                .         .
         *                .
         *                .        .
         *                .
         *                .       .
         *                .
         *                .      .
         *                .
         *                .     .
         *                .
         *                .    .
         *                .
         *                .   .
         *                .
         *                .
         */
        if (lx_tessellator_vertex_in_edge_right(edge_right_org, lx_mesh_edge_dst(edge_left), edge_left_org)) {
            return lx_false;
        }

        /* the left edge will be split, so we need mark it and it's left edge as "dirty"
         *
         *      fix_all_dirty_regions()
         *    <-------------------------
         *
         * edge_left2   edge_left   edge_right
         *  (dirty)      (dirty)
         *    |             |-----------|
         *    |                recheck
         *    |-------------------|
         *           recheck
         *
         */
        lx_tessellator_active_region_ref_t region_left2 = lx_tessellator_active_regions_left(tessellator, region_left);
        lx_assert(region_left2);

        region_left->dirty  = 1;
        region_left2->dirty = 1;

        /* split the left edge and create a new edge
         *
         *            edge_left  edge_right
         *                .       .
         *                .      .
         *                .     .
         *                .    .
         *                .   .
         *                .  .
         *                . .
         *                ..
         *                .
         *               ..
         *              . .
         *             .  .
         *            . - . -----------------> fix it
         *                .
         *        org_new x
         *                  .
         *                    .
         *                 edge_new
         */
        lx_mesh_edge_ref_t edge_new = lx_mesh_edge_split(tessellator->mesh, lx_mesh_edge_sym(edge_left));
        lx_assert(edge_new && edge_new == lx_mesh_edge_oprev(edge_left));

        /* splice the left edge and right edges
         *
         *            edge_left  edge_right
         *                .       .
         *                       .
         *                      .
         *               .     .
         *                    .
         *                   .
         *              .   .
         *                 .
         *                .
         *             . .
         *              .
         *             .
         *            . ---------------------> fixed
         *              .
         *                .
         *             edge_new (org_new have been removed)
         */
        lx_mesh_edge_splice(tessellator->mesh, edge_right, edge_new);

        // the origin of the right edge cannot be changed
        lx_assert(edge_right_org == lx_mesh_edge_org(edge_left));
        lx_assert(edge_right_org == lx_mesh_edge_org(edge_right));
    }

    // fixed
    lx_trace_d("fix the bottom order for region: %{tess_region}", region_left);
    return lx_true;
}

/* we need fix four case because of the calculated intersection with some small numerical errors.
 *
 * the case 1 and 2 when calling insert_down_doing_edges():
 *
 * - the new left and right edges with the intersection point may violate the region order
 *   because the calculated intersection has some small numerical errors.
 *
 * case 1:
 *
 * edge_left
 * .
 *   . .
 *     .   .
 *       .    .
 *         .      .
 *           .        .
 *             .  event   .
 *               .   *   .    . edge_left_new
 *                 . .          . .
 *                   . -------------- x <--- intersection with numerical error
 *                   . .
 *                   .   .
 *                   .     .
 *                   .       .
 *                   .         .
 *                   .
 *                   .
 *                   .
 *               edge_right
 *
 * or case 2:
 *
 *                                     . edge_right
 *                                 . .
 *                             .   .
 *                          .    .
 *                      .      .
 *  edge_right_new  .        .
 *              .     event.
 *          .     .  *   .
 *      . .          . .
 *   x <------------ . ----------------- intersection with numerical error
 *                 . .
 *               .   .
 *             .     .
 *           .       .
 *         .         .
 *                   .
 *                   .
 *                   .
 *               edge_left
 *
 * the case 3 and 4 when calling connect_bottom_event():
 *
 * - the event will be not in the middle of the new left and right edges
 *   with the intersection point, when we are connecting the bottom event.
 *
 *   this can happen due to very small numerical errors in the intersection calculation
 *   and we cannot continue to connect the bottom event if the case has been happened.
 *
 *   so we need also fix it for this case.
 *
 * case 3:
 *
 * edge_left     edge_right
 * .                 .
 *   .       *       . .
 *     . .    *      .   .
 *       .     *   * .     .  edge_right_new
 *         .    * *  .       .
 *           .   * event       .
 *             .     .    .      .
 *               .   .         . edge_left_new
 *                 . .              .
 *                   . ---------------- x <--- intersection with numerical error
 *                   . .
 *                   .   .
 *                   .     .
 *                   .       .
 *                   .         .
 *                   .
 *                   .
 *                   .
 *
 * case 4:
 *
 *                         edge_left                 edge_right
 *                             .                         .
 *                             .                    .  .
 *                           . .                 .   .
 *                             .              .    .
 *                        .    .           .     .
 *                             .       *.      .
 *                     .       .      *      .
 *    edge_left_new            . *   *     .
 *                  .          .  * *    .
 *                         .   .   * event
 *               .     .       .     .
 *                 .           .   .
 *            . edge_right_new . .
 *          x <--------------- . ------------------- intersection with numerical error
 *                           . .
 *                         .   .
 *                       .     .
 *                     .       .
 *                   .         .
 *                             .
 *                             .
 *                             .
 *
 */
static lx_bool_t lx_tessellator_fix_region_intersection_errors(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region_left, lx_tessellator_active_region_ref_t region_right, lx_mesh_vertex_ref_t intersection) {
    lx_assert(tessellator && tessellator->mesh && region_left && region_right && intersection);

    // the event
    lx_mesh_vertex_ref_t event = tessellator->event;
    lx_assert(event);

    // the edge of the left region
    lx_mesh_edge_ref_t edge_left = region_left->edge;
    lx_assert(edge_left);

    // the edge of the right region
    lx_mesh_edge_ref_t edge_right = region_right->edge;
    lx_assert(edge_right);

    // the destination of the left edge
    lx_mesh_vertex_ref_t edge_left_dst = lx_mesh_edge_dst(edge_left);
    lx_assert(edge_left_dst);

    // the destination of the right edge
    lx_mesh_vertex_ref_t edge_right_dst = lx_mesh_edge_dst(edge_right);
    lx_assert(edge_right_dst);

    /* case 1:
     *
     * edge_left
     * .
     *   .  region_left
     *     .
     *       .                  . (finished)
     *         .              .        .
     *           .          .     . (finished)
     *             .   event .
     *               .   * .
     *                 . .    .  region_xxx
     *                   . -------------- x <--- intersection with numerical error
     *                   . .
     *                   .   .
     *                   .     .
     *                   .       .
     *                   .         .
     *                   .
     *                   . region_right
     *                   .
     *               edge_right
     */
    if (event == edge_right_dst) {
        lx_trace_d("fix intersection error for case 1");

        /* split the left edge
         *
         * edge_left
         * .
         *   .  region_left
         *     .               edge_right.lnext
         *       .                  . (finished)
         *         .    lface     .        .
         *           .          .     . (finished)
         *             .   event .
         *               .   * .
         *                 . .    .  region_xxx
         *                   x -------------- x <--- intersection with numerical error
         *                   . .
         *                   .   .      lface
         *                   .     .
         *                   .       .
         *                   .         . edge_new
         *                   .
         *                   . region_right
         *                   .
         *               edge_right
         */
        lx_mesh_edge_ref_t edge_new = lx_mesh_edge_split(tessellator->mesh, lx_mesh_edge_sym(edge_left));
        lx_assert(edge_new);

        /* splice the left and new edges into the event vertex
         * and uses the event as the real intersection
         *
         * edge_left
         * .
         *   .  region_left
         *     .
         *       .
         *         .      lface
         *           .              . (finished)
         *             .          .        .
         *               .      .     . (finished)
         *                 event .
         *                   * . ------------------- intersection
         *                 . .    .  region_xxx
         *               .   .
         *             .     .
         *           .       . region_right
         *         .  lface  .
         *       .           .
         *   edge_new - - -  . - - - - - - . (edge_new.dst)
         *                   .
         *                   .
         *                   .
         *               edge_right
         */
        lx_mesh_edge_splice(tessellator->mesh, lx_mesh_edge_lnext(edge_right), edge_new);

        // the event cannot be changed
        lx_assert(event == lx_mesh_edge_org(edge_new));
        lx_assert(event == lx_mesh_edge_org(edge_left));
        lx_assert(event == lx_mesh_edge_dst(edge_right));

        // update the new left region
        region_left = lx_tessellator_find_left_top_region(tessellator, region_left);
        lx_assert(region_left);

        // get the first region
        lx_tessellator_active_region_ref_t region_first = lx_tessellator_active_regions_right(tessellator, region_left);
        lx_assert(region_first);

        // get the left-top edge
        lx_mesh_edge_ref_t edge_left_top = region_first->edge;
        lx_assert(edge_left_top);

        /* finish and remove the top regions
         *
         *  .             edge_left_top
         *  .             .
         *  .               .
         *  . region_left     .   region_first(finished)
         *  .                   .
         *  .                     .      lface
         *  .                       .              . (finished)
         *  .                         .          .        .
         *  .                           .      .     . (finished)
         *  .                             event .
         *  .                               * . --------------------- intersection
         *  .                             . .    . region_xxx
         *  .                           .   .
         *  .                         .     .
         *  .                       .       . region_right
         *  .                     .  lface  .
         *  .                   .           .
         *  .               edge_new - - -  . - - - - - - . (edge_new.dst)
         *  .             (inserted)        .
         *                      region_new  .
         *                                  .
         *                              edge_right
         */
        lx_tessellator_finish_top_regions(tessellator, region_first, region_right);

        // check the new edge order: edge_left_top => edge_new => edge_right
        lx_assert(edge_new == lx_mesh_edge_onext(edge_left_top));

        // insert the new down-going edge without region: edge_new
        lx_tessellator_insert_down_going_edges(tessellator, region_left, edge_new, lx_mesh_edge_onext(edge_new), edge_left_top, lx_true);

        /* we need return directly from the recurse call
         * because we have fix all "dirty" regions after calling insert_down_going_edges()
         * and possibly the previous regions have been removed
         */
        return lx_true;
    }

    /* case 2:
     *
     *
     *                                       . edge_right
     *                                     .
     *          .                        .
     * (finished) .                    .
     *      .       .                .
     *          .     .            .   region_right
     *  (finished)  .  event     .
     *                  .*     .
     *                .  .   .
     *        region_xxx . .
     *   x <------------ . ----------------- intersection with numerical error
     *                 . .
     *               .   .
     *             .     .
     *           .       .
     *         .         .
     *                   . region_left
     *                   .
     *                   .
     *               edge_left
     */
    if (event == edge_left_dst) {
        lx_trace_d("fix intersection error for case 2");

        /* split the right edge
         *
         *
         *                                     . edge_right
         *                                   .
         *          .                      .
         * (finished) .                  .
         *       .      .      lface   .
         *           .    .          .   region_right
         *  (finished)   . event   .
         *                  .*   .
         *                .  . .
         *   x <------------ x ----------------- intersection with numerical error
         *                 . .
         *               .   .
         *             .     .
         *           .       .
         *         .         . lface
         *    edge_new       .
         *                   . region_left
         *                   .
         *               edge_left
         */
        lx_mesh_edge_ref_t edge_new = lx_mesh_edge_split(tessellator->mesh, lx_mesh_edge_sym(edge_right));
        lx_assert(edge_new && edge_right == lx_mesh_edge_onext(edge_new));

        /* splice the right and new edges into the event vertex
         * and uses the event as the real intersection
         *
         *                               edge_right
         *                                  .
         *                                .
         *                    lface     .
         *          .                 .
         * (finished) .             .
         *       .      .         .
         *           .    .     .      region_right
         * (finished)    . event
         *                  .* --------------------- intersection
         *               .   .  .
         *            .      .    .
         *        region_xxx .      .
         *                   .        .
         *                   . lface    .
         *      .            .            .
         * (edge_new.dst) -- . - - - - -  edge_new
         *                   .
         *                   . region_left
         *                   .
         *               edge_left
         */
        lx_mesh_edge_splice(tessellator->mesh, lx_mesh_edge_sym(edge_left), edge_right);

        // the event cannot be changed
        lx_assert(event == lx_mesh_edge_org(edge_new));
        lx_assert(event == lx_mesh_edge_org(edge_right));
        lx_assert(event == lx_mesh_edge_dst(edge_left));

        // save the left region as a patched region
        lx_tessellator_active_region_ref_t region_patch = region_left;

        // update the new left region
        region_left = lx_tessellator_find_left_bottom_region(tessellator, region_left);
        lx_assert(region_left);

        // patch a region with the edge_right.onext
        region_patch->edge = lx_mesh_edge_onext(edge_right);

        /* finish and remove the top regions from the patched region
         * and update the right region
         *
         * the left face of the patch region has been finished previously
         * but the "inside" mark has might been reset before finishing the right region.
         *
         * this case will appear in some special situation:
         *
         *  - the old lface has might been removed after some splicing operations
         *
         * .e.g
         *
         * before:
         *             .
         *               .
         *                 .  (finished)       .
         *                   .  inside: 1    .
         *                     .           .
         *               .       .       .
         *                      .  .   .
         *                           . . ----------------- intersection
         *                                   .
         *                                         .
         *
         * after calculating intersection:
         *
         * the face has been changed because two splitted degenerate edges
         * with the same origin will be joining when down-going edges.
         * (the old face might be merged and removed when splicing edges.)
         *
         *             .
         *               .
         *                 .  (finished)       .
         *                   .  inside: 0    .
         *                     .           .
         *               .       .       .
         *                      .  .   .
         *   intersection ---------- x .
         *      (event)              ..      .
         *                           .             .
         *              (two degenerate edges)
         *
         * so we patch it using the old left region
         * and finish this patched region again for computing the accurate winding and inside.
         *
         *  .                                             edge_right                .
         *  .                  . region_patch                .                      .
         *  .                    .   (finished)            .                        .
         *  .                      .           lface     .                          .
         *  .                        .   (inside:0/1)  .                            .
         *  .              (finished)  .             .                              .
         *  .                     .      .         .                                .
         *  .                         .    .     .      region_right(finished)      .
         *  .              (finished)     . event                                   .
         *  .                                .* --------------------- intersection  .
         *  .                             .   .  .                                  .
         *  . region_left              .      .    .                                .
         *  .                      region_xxx .      .                              .
         *  .                                 .        . region_new                 .
         *  .                                 . lface    .                          .
         *  .                    .            .            .                        .
         *  .               (edge_new.dst) -- . - - - - -  edge_new(inserted)       .
         *  .                                 .                                     .
         *  .                                 . (inserted)                          .
         *  .                                 .                                     .
         *                                edge_left
         *        (the old left region has been a patched region and will be finished)
         */
        lx_tessellator_finish_top_regions(tessellator, region_patch, lx_null);

        // check
        lx_assert(lx_mesh_edge_onext(lx_mesh_edge_sym(edge_left)) == edge_new);

        /* insert the new down-going edge without region: edge_left and edge_new
         *
         * the region of the left edge have been removed(gave edge_right.onext),
         * so we need insert it again.
         */
        lx_tessellator_insert_down_going_edges(tessellator, region_left, lx_mesh_edge_sym(edge_left), lx_mesh_edge_onext(edge_new), lx_null, lx_true);

        /* we need return directly from the recurse call
         * because we have fix all "dirty" regions after calling insert_down_going_edges()
         * and possibly the previous regions have been removed
         */
        return lx_true;
    }

    /* case 3
     *
     * edge_left     edge_right
     * .                 .
     *   . .     *       .
     *     .  .   *      .
     *       .   . *   * .
     *         .    * *  .
     *           .   * event
     *             .     . .
     *               .   .    .
     *                 . .       .
     *                   . ------- x <--- intersection with numerical error
     *                   . .
     *                   .   .
     *                   .     .
     *                   .       .
     *                   .         .
     *                   .
     *                   .
     *                   .
     */
    if (lx_tessellator_vertex_on_edge_or_left(event, edge_left_dst, intersection)) {
        lx_trace_d("fix intersection error for case 3");

        /* the left edge will be split, so we need mark it and it's left edge as "dirty"
         *
         *      fix_all_dirty_regions()
         *    <-------------------------
         *
         * edge_left2   edge_left   edge_right
         *  (dirty)      (dirty)
         *    |             |-----------|
         *    |                recheck
         *    |-------------------|
         *           recheck
         *
         */
        lx_tessellator_active_region_ref_t region_left2 = lx_tessellator_active_regions_left(tessellator, region_left);
        lx_assert(region_left2);

        region_left->dirty  = 1;
        region_left2->dirty = 1;

        /* split the left edge
         *
         * edge_left     edge_right
         * . region_left     .
         *   .       *       .
         *     .      *      . region_right
         *       .     *   * .
         *         .    * *  .
         *           .   * event
         *             x     .
         *               .   .
         *                 . .
         *                   .
         *                   . .
         *                   .   .
         *                   .     .
         *                   .       .
         *                   .         . edge_new
         *                   .
         *                   .
         *                   .
         */
        lx_mesh_edge_split(tessellator->mesh, lx_mesh_edge_sym(edge_left));

        /* replace the intersection by the event vertex
         * and wait for connect_bottom_event() to splice it appropriately
         *
         * edge_left     edge_right
         *  . region_left     .
         *    .       *       .
         *      .      *      . region_right
         *        .     *   * .
         *          .    * *  .
         *            .   * event
         *      event - x     .
         *  (intersection).   .
         *                  . .
         *                    .
         *                    . .
         *                    .   .
         *                    .     .
         *                    .       .
         *                    .         . edge_new
         *                    .
         *                    .
         *                    .
         *
         *
         * after connect_bottom_event()
         *
         * edge_left     edge_right
         * . (finished)       .
         *    .       *       .
         *       .     *      . region_right
         *          .   *   * .
         *             . * *  .
         *                * event
         *                    .
         *                    .  .
         *                    .     .  (region_new)
         *                    .        .
         *                    .           .
         *                    .              . edge_new(inserted)
         *                    .
         *                    .
         *                    .
         */
        lx_tessellator_vertex_point_set(lx_mesh_edge_org(edge_left), lx_tessellator_vertex_point(event));
    }

    /* case 4
     *
     *                         edge_left                 edge_right
     *                             .                         .
     *                             .                    .  .
     *                             .                 .   .
     *                             .              .    .
     *                             .           .     .
     *                             .       *.      .
     *                             .      *      .
     *                             . *   *     .
     *                             .  * *    .
     *                         .   .   * event
     *                     .       .     .
     *                 .           .   .
     *            .                . .
     *          x <--------------- . ------------------- intersection with numerical error
     *                           . .
     *                         .   .
     *                       .     .
     *                     .       .
     *                   .         .
     *                             .
     *                             .
     *                             .
     */
    if (lx_tessellator_vertex_on_edge_or_right(event, edge_right_dst, intersection)) {
        lx_trace_d("fix intersection error for case 4");

        /* the right edge will be split, so we need mark it and it's left edge as "dirty"
         *
         *      fix_all_dirty_regions()
         *    <-------------------------
         *
         * edge_left    edge_right   edge_xxx
         *  (dirty)      (dirty)
         *    |             |-----------|
         *    |                recheck
         *    |-------------------|
         *           recheck
         *
         */
        region_left->dirty  = 1;
        region_right->dirty = 1;

        /* split the right edge
         *
         *                         edge_left                 edge_right
         *                             .                         .
         *                             .                       .
         *                             .                     .
         *                             .                   .
         *                             .                 .
         *                             .       *       .
         *                             .      *      .
         *                             . *   *     .
         *                             .  * *    .
         *                             .   * event
         *                             .     x
         *                             .   .
         *                             . .
         *                             .
         *                           . .
         *                         .   .
         *                       .     .
         *                     .       .
         *                   .         .
         *           edge_new          .
         *                             .
         *                             .
         */
        lx_mesh_edge_split(tessellator->mesh, lx_mesh_edge_sym(edge_right));

        /* replace the intersection by the event vertex
         * and wait for connect_bottom_event() to splice it appropriately
         *
         *                         edge_left                 edge_right
         *                             .                         .
         *                             .                       .
         *                             .                     .
         *                             .                   .
         *                             .                 .
         *                             .       *       .
         *                             .      *      .
         *                             . *   *     .
         *                             .  * *    .
         *                             .   * event
         *                             .     x - event
         *                             .   . (intersection)
         *                             . .
         *                             .
         *                           . .
         *                         .   .
         *                       .     .
         *                     .       .
         *                   .         .
         *           edge_new          .
         *                             .
         *                             .
         *
         *
         * after connect_bottom_event()
         *
         *                         edge_left               edge_right
         *                             .                      .
         *                             .                    .
         *                             . region_left      .
         *                             .                .
         *                             .              .  region_right
         *                             .       *    .     (finished)
         *                             .      *   .
         *                             . *   *  .
         *                             .  * * .
         *                             .   * event
         *                             . . (intersection)
         *                             . region_new
         *                           . .
         *                         .   .
         *                       .     .
         *                     .       .
         *                   .         .
         *              edge_new       .
         *                             .
         *                             .
         */
        lx_tessellator_vertex_point_set(lx_mesh_edge_org(edge_right), lx_tessellator_vertex_point(event));
    }

    // the left operation will be processed in connect_bottom_event()
    return lx_false;
}

/* calculate and patch the intersection of the left and right edges of the given region
 *
 * we need return true if adding the new intersection resulted
 * in a recursive call to insert_down_doing_edges()
 *
 * - in this case all "dirty" regions have been fixed for intersections,
 *   and possibly the given region has been deleted.
 *
 * edge_left
 *       .
 *         .      region_left          . edge_right
 *           .                      .
 *             .                 .
 *               .    * event . ------------ sweep line
 *                 .       .
 *                   .  .
 *                    x --------- intersection
 *                 .     .
 *              .          .
 */
static lx_bool_t lx_tessellator_fix_region_intersection(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region_left) {
    lx_assert(tessellator && tessellator->mesh && region_left);

    // the event
    lx_mesh_vertex_ref_t event = tessellator->event;
    lx_assert(event);

    // the right region
    lx_tessellator_active_region_ref_t region_right = lx_tessellator_active_regions_right(tessellator, region_left);
    lx_assert(region_right);

    // the edge of the left region
    lx_mesh_edge_ref_t edge_left = region_left->edge;
    lx_assert(edge_left);

    // the edge of the right region
    lx_mesh_edge_ref_t edge_right = region_right->edge;
    lx_assert(edge_right);

    // the origin and destination of the left edge
    lx_mesh_vertex_ref_t edge_left_org = lx_mesh_edge_org(edge_left);
    lx_mesh_vertex_ref_t edge_left_dst = lx_mesh_edge_dst(edge_left);
    lx_assert(edge_left_org && edge_left_dst);

    // the origin and destination of the right edge
    lx_mesh_vertex_ref_t edge_right_org = lx_mesh_edge_org(edge_right);
    lx_mesh_vertex_ref_t edge_right_dst = lx_mesh_edge_dst(edge_right);
    lx_assert(edge_right_org && edge_right_dst);

    // the top vertices of two edge cannot be equal
    lx_assert(!lx_tessellator_vertex_eq(edge_left_dst, edge_right_dst));

    // the event vertex must be in the middle of two edge
    lx_assert(lx_tessellator_vertex_on_edge_or_right(event, edge_left_dst, edge_left_org));
    lx_assert(lx_tessellator_vertex_on_edge_or_left(event, edge_right_dst, edge_right_org));
    lx_assert(event != edge_left_org && event != edge_right_org);

    // two edges cannot be fixable
    lx_assert(!region_left->fixedge && !region_right->fixedge);

    /* we need not fix it if the bottom vertices are same
     *
     * . edge_left
     *   .           . edge_right
     *     .     .
     *       . (same)
     */
    lx_check_return_val(edge_left_org != edge_right_org, lx_false);

    /* no intersection?
     *
     * we reject it quickly if edge_left_max_x < edge_right_min_x
     *
     * edge_left
     *    .
     *     .                     edge_right
     *      .                      .
     *       .                    .
     *        .                  .
     *         .                .
     *          *              *
     *  edge_left_max_x < edge_right_min_x
     *
     *          (no intersection)
     */
    lx_float_t edge_left_max_x  = lx_max(lx_tessellator_vertex_point(edge_left_org)->x, lx_tessellator_vertex_point(edge_left_dst)->x);
    lx_float_t edge_right_min_x = lx_min(lx_tessellator_vertex_point(edge_right_org)->x, lx_tessellator_vertex_point(edge_right_dst)->x);
    lx_check_return_val(edge_left_max_x >= edge_right_min_x, lx_false);

    /* no intersection?
     *
     * we reject it quickly if edge_left is in the left-top of edge_right
     *
     * edge_left
     *   .
     *    .           edge_right
     *     .             .
     *      .         .
     *       *     .
     *          .
     *       .
     *    *
     */
    if (lx_tessellator_vertex_in_top_or_horizontal(edge_left_org, edge_right_org)) {
        if (lx_tessellator_vertex_in_edge_left(edge_left_org, edge_right_dst, edge_right_org)) {
            return lx_false;
        }
    } else {
        /* no intersection?
         *
         * we reject it quickly if edge_right is in the right-top of edge_left
         *
         *               edge_right
         * edge_left         .
         *   .              .
         *      .          .
         *         .      *
         *            .
         *               .
         *                  *
         */
        if (lx_tessellator_vertex_in_edge_right(edge_right_org, edge_left_dst, edge_left_org)) {
            return lx_false;
        }
    }

    // attempt to calculate the intersection of two edges
    lx_long_t ok = -1;
    lx_tessellator_vertex_local(intersection);
    if ((ok = lx_tessellator_edge_intersection(edge_left_org, edge_left_dst, edge_right_org, edge_right_dst, intersection)) <= 0) {
        /* we need fix the following case:
         * - may violate ordering because of numerical errors for nearly identical slopes
         *
         *   .              .
         *   .              ..
         *   .              ..
         *   .              ..
         *   ..             ..
         *   .              ..
         *   .    fix it    ..
         *   . . -------->   .
         *   .              .
         *   .             .
         *
         */
        if (!ok) {
            lx_tessellator_fix_region_order_at_bottom(tessellator, region_left);
        }
        return lx_false;
    }

    // trace
    lx_trace_d("intersection: %{tess_region} x %{tess_region} => %{point}", region_left, region_right, lx_tessellator_vertex_point(intersection));

    /* check the position of the intersection
     *
     * edge_left
     *    .       edge_right
     *      .     .
     *        .  .
     *          x ---- intersection
     *         .  .
     *        .     .
     *                .
     */
    lx_assert(lx_tessellator_vertex_point(intersection)->x >= edge_right_min_x);
    lx_assert(lx_tessellator_vertex_point(intersection)->x <= edge_left_max_x);
    lx_assert(lx_tessellator_vertex_point(intersection)->y >= lx_max(lx_tessellator_vertex_point(edge_left_dst)->y, lx_tessellator_vertex_point(edge_right_dst)->y));
    lx_assert(lx_tessellator_vertex_point(intersection)->y <= lx_min(lx_tessellator_vertex_point(edge_left_org)->y, lx_tessellator_vertex_point(edge_right_org)->y));

    /* we need fix it if the intersection lies slightly to
     * the upper of the sweep line because of some numerical errors.
     *
     * edge_left
     *       .
     *         .      region_left            . edge_right
     *           .                        .
     *             .                   .
     *               .              .
     *                 .  x ---- . ------------------ intersection with numerical error
     *                   . *  . event --------------- sweep line
     *                     .
     *                  .    .
     *               .         .
     *                           .
     *                             .
     *                               .
     *
     *
     *
     * it is error case if the intersection lies to the horizontal left side of the event
     * and the right edge is horizontal approximately.
     *
     * we need fix it for this case because the intersection will be not processed forever.
     *
     *                  edge_left
     *                      .
     *                      .
     *                      .                                                              . (dst)
     *    . . . . . . . . . x . . . . (dst) edge_right => horizontal approximately =>    .
     *         intersection .     event                                                .
     *                      .
     *                      .
     *                      .
     *
     *
     * it is normal case if the intersection lies to the horizontal right side of the event
     * and the intersection will be processed next.
     *
     * so we need not fix this case now.
     *
     *      . . . . .
     *      .       .
     *      .       .
     *      . . . . x . . . . .. . . . . . . . . .
     *   event      . intersection
     *              .
     *              .
     *              .
     *
     */
    if (lx_tessellator_vertex_in_top_or_hleft(intersection, event)) {
        lx_trace_d("fix intersection by the event: %{mesh_vertex}", event);

        // the points
        lx_point_ref_t point_event = lx_tessellator_vertex_point(event);
        lx_point_ref_t point_inter = lx_tessellator_vertex_point(intersection);
        lx_point_ref_t point_left_org = lx_tessellator_vertex_point(edge_left_org);
        lx_point_ref_t point_right_org = lx_tessellator_vertex_point(edge_right_org);

        /* we only need replace the intersection by the event vertex
         * if the intersection is close to the event.
         */
        lx_float_t dx = point_inter->x - point_event->x;
        if (lx_near0(dx)) {
            lx_tessellator_vertex_point_set(intersection, point_event);
        } else {
            /* the intersection might be not close to the event if an edge is horizontal approximately.
             * so we need replace the y-coordinate of the intersection by the minimum y-coordinate in edge origin
             *
             *                  edge_left
             *                      .
             *                      .
             *                      .                                                              . (dst)
             *    . . . . . . . . . x . . . . (dst) edge_right => horizontal approximately =>    .
             *         intersection .     event                                                .
             *                      .
             *                      .
             *                      .
             */
            point_inter->y = lx_min(point_left_org->y, point_right_org->y);
        }
    }

    /* similarly, we also need fix it if the intersection lies slightly to
     * the lower of the topmost origin because of some numerical errors.
     *
     * edge_left
     *       .
     *         .      region_left            . edge_right
     *           .                        .
     *             .        * event -- . ----------- sweep line
     *               .              .
     *                 .         .
     *                   .    .
     *                     .
     *    edge_org_upper *   .
     *                     x - . ------------------- intersection with numerical error
     *                           .
     *                             .
     *                               .
     *
     * it is error case if the intersection lies to the horizontal right side of the edge_org_upper
     * and the right edge is horizontal approximately.
     *
     * we need fix it for this case because the intersection will be not processed forever.
     *
     *                  edge_left
     *                      .
     *                      .
     * edge_org_upper       .                                                              . (dst)
     *    . . . . . . . . . x . . . . (dst) edge_right => horizontal approximately =>    .
     *         intersection .     event                                                .
     *                      .
     *                      .
     *                      .
     *
     *
     * it is normal case if the edge_org_upper lies to the horizontal right side of the intersection
     * and the intersection will be processed next.
     *
     *                      . . . . .
     *                      .       .
     *                      .       .
     *  . . . . . . . . . . x . . . .   <=
     * event   intersection .   edge_org_upper
     *                      .
     *                      .
     *                      .
     *
     */
    lx_mesh_vertex_ref_t edge_org_upper = lx_tessellator_vertex_in_top_or_horizontal(edge_left_org, edge_right_org)? edge_left_org : edge_right_org;
    if (lx_tessellator_vertex_in_top_or_hleft(edge_org_upper, intersection)) {
        lx_trace_d("fix intersection by the topmost origin: %{mesh_vertex}", edge_org_upper);

        // the points
        lx_point_ref_t point_upper = lx_tessellator_vertex_point(edge_org_upper);
        lx_point_ref_t point_inter = lx_tessellator_vertex_point(intersection);

        /* we only need replace the intersection by the edge_org_upper
         * if the intersection is close to the edge_org_upper.
         */
        lx_float_t dx = point_inter->x - point_upper->x;
        if (lx_near0(dx)) {
            lx_tessellator_vertex_point_set(intersection, point_upper);
        } else {
            /* the intersection might be not close to the edge_org_upper if an edge is horizontal approximately.
             * so we need replace the y-coordinate of the intersection by the y-coordinate of edge_org_upper
             *
             *                  edge_left
             *                      .
             *                      .
             * edge_org_upper       .                                                              . (dst)
             *    . . . . . . . . . x . . . . (dst) edge_right => horizontal approximately =>    .
             *         intersection .     event                                                .
             *                      .
             *                      .
             *                      .
             */
            point_inter->y = point_upper->y;
        }
    }

    /* we only need fix the bottom region order
     * if the intersection is at the one of the bottom endpoints
     *
     * edge_left
     *   .
     *    .    edge_right
     *     .      .
     *      .   .
     *       .. ------ intersection
     */
    if (    lx_tessellator_vertex_eq(intersection, edge_left_org)
        ||  lx_tessellator_vertex_eq(intersection, edge_right_org)) {
        lx_trace_d("fix intersection by splicing the origin of two edges");

        // fix the bottom region order by splicing two edges
        lx_tessellator_fix_region_order_at_bottom(tessellator, region_left);
        return lx_false;
    }

    /* we need fix four case because of the calculated intersection with some small numerical errors.
     *
     * see the comments of lx_tessellator_fix_region_intersection_errors
     */
    if (    (   !lx_tessellator_vertex_eq(event, edge_left_dst)
            &&  lx_tessellator_vertex_on_edge_or_left(event, edge_left_dst, intersection))
        ||  (   !lx_tessellator_vertex_eq(event, edge_right_dst)
            &&  lx_tessellator_vertex_on_edge_or_right(event, edge_right_dst, intersection))) {
        // fix some case because of numerical errors
        return lx_tessellator_fix_region_intersection_errors(tessellator, region_left, region_right, intersection);
    }

    /* the general case
     *
     * we will split two edges and splice them
     * and insert the new intersection to the event queue.
     *
     * the region order might be changed if the intersection has some numerical errors.
     *
     * before:
     *
     * edge_left
     *       .
     *         .      region_left          . edge_right
     *           .                      .
     *             .                 .
     *               .    * event . ------------ sweep line
     *                 .       .
     *                   .  .
     *                    x --------- intersection
     *                 .     .
     *              .          .
     *           .               .
     *                             .
     *                               .
     *
     * after:
     *
     * edge_left
     *       .
     *         .      region_left          . edge_right
     *           .                      .
     *             .                 .
     *               .    * event . ------------ sweep line
     *                 .       .
     *                   .  .
     *                    x --------- intersection (insert to the event queue)
     *                 .     .
     *              .          .
     *           .               .
     *       edge_new              .
     *                               .
     *                           edge_new
     */
    {
        /* split two edges
         *
         * we will create two new edges and a new vertex
         */
        lx_mesh_edge_split(tessellator->mesh, lx_mesh_edge_sym(edge_left));
        lx_mesh_edge_split(tessellator->mesh, lx_mesh_edge_sym(edge_right));

        // splice edges
        lx_mesh_edge_splice(tessellator->mesh, lx_mesh_edge_oprev(edge_left), edge_right);

        // check
        lx_assert(lx_mesh_edge_org(edge_left) == lx_mesh_edge_org(edge_right));

        // init the new intersection point
        lx_tessellator_vertex_point_set(lx_mesh_edge_org(edge_right), lx_tessellator_vertex_point(intersection));

        // trace
        lx_trace_d("insert the new intersection: %{mesh_vertex}", lx_mesh_edge_org(edge_right));

        // add intersection to the profiler
#ifdef LX_TESSELLATOR_PROFILER_ENABLE
        lx_tessellator_profiler_add_inter(lx_mesh_edge_org(edge_right));
#endif

        // insert the new intersection vertex to the event queue
        lx_tessellator_event_queue_insert(tessellator, lx_mesh_edge_org(edge_right));

        /* the left and right edge will be split, so we need mark them and their left edge as "dirty"
         *
         *      fix_all_dirty_regions()
         *    <-------------------------
         *
         * edge_left2   edge_left   edge_right   edge_xxxx
         *  (dirty)      (dirty)      (dirty)
         *    |             |           |------------|
         *    |             |              recheck
         *    |             |-----------------|
         *    |                   recheck
         *    |----------------------|
         *           recheck
         *
         */
        lx_tessellator_active_region_ref_t region_left2 = lx_tessellator_active_regions_left(tessellator, region_left);
        lx_assert(region_left2);

        region_left->dirty  = 1;
        region_right->dirty = 1;
        region_left2->dirty = 1;
    }

    // no recursion
    return lx_false;
}

/* fix all dirty regions which order have been violated
 * and some new dirty regions may be created when we are fixing.
 *
 * - we need calculate intersection and fix region order after insert_down_doing_edges()
 * - we need fix some new created dirty regions when we are fixing and calculating intersection
 * - we need process the recurse call for calculating intersection
 *
 * after insert_down_doing_edges()
 *
 *     .     .                  .                       .
 *     .     .                 . . .  region_left       .
 *     .     .                .   .   .                 .
 *     .     .               .     .     . dirty        .
 *     .     . dirty        .       dirty               .
 *     .     .             . dirty                      .
 *           .
 *           .
 *
 * the walking order
 *                                        |
 *                                         ----------->
 *                                                     |
 *                                     <---------------
 *                                    |
 *                                     ->
 *                                       | (new dirty regions may be created)
 *                                 <-----
 *                                |
 *                                 ->
 *                                   |
 *                            <------
 *                           |
 *                            ->
 *                              |
 *                <-------------
 *               |
 *                ->
 *                  |
 *    end <---------
 */
static lx_void_t lx_tessellator_fix_all_dirty_regions(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region_left) {
    lx_assert(tessellator && tessellator->mesh && region_left);

    // get the right region
    lx_tessellator_active_region_ref_t region_right = lx_tessellator_active_regions_right(tessellator, region_left);
    lx_assert(region_right);

    // done
    lx_mesh_edge_ref_t edge_left    = lx_null;
    lx_mesh_edge_ref_t edge_right   = lx_null;
    while (1) {
        /* find the rightmost dirty region
         *
         * we need process some new created dirty regions when we are fixing
         */
        while (region_right->dirty) {
            region_left = region_right;
            region_right = lx_tessellator_active_regions_right(tessellator, region_right);
            lx_assert(region_right);
        }

        // we break it if no more dirty regions
        if (!region_left->dirty) {
            region_right = region_left;
            region_left = lx_tessellator_active_regions_left(tessellator, region_left);
            if (!region_left || !region_left->dirty) {
                return ;
            }
        }

        // clear the dirty mark first
        region_left->dirty = 0;

        // get the left and right edges
        edge_left = region_left->edge;
        edge_right = region_right->edge;

        // trace
        lx_trace_d("fix dirty regions with %{tess_region} and %{tess_region}", region_left, region_right);

        /* fix the region order at the top edge first
         *
         * splitting an edge into two pieces may violate the previous region order
         * because of some numerical errors after calculating previous intersection.
         *
         * so we need fix it first for calculating next intersection
         *
         *            .                                      .                      .
         * edge_left  .                            edge_left .                      .
         *            .  region_left                         .                      .
         *            . (rface.inside)                       .                      . edge_new (rface.inside = region_left.inside)
         *            .                                      .       fixed          .
         *            .                              . . . . . .  ----------->      * . . . . .
         *            . edge_right         edge_right  .     .                     ..
         *            . . . . . .                        .   .                     .. edge_left/right (with a degenerate face)
         *            ..           intersection            . .                     ..
         *            .        x   ------------>             x                      x intersection with numerical error
         *           ..  intersection with numerical error  ..                     ..
         *          . .                                    . .                    . .
         *         .  .                                   .  .                   .  .
         *        .   .                                  .   .                  .   .
         *       .    .                                 .    .                 .    .
         *      .     .                                .     .                .     .
         *     .      .                               .      .               .      .
         * edge_right
         *
         */
        if (    lx_mesh_edge_dst(edge_left) != lx_mesh_edge_dst(edge_right)
            &&  lx_tessellator_fix_region_order_at_top(tessellator, region_left)) {
            /* we no longer need it if the left or right edge was marked fixable.
             *
             *                     .
             *                     .
             *                     .
             *                     . edge_new
             *                     .
             *                     * . . . . .
             *                    ..
             *         fixable -> .. edge_left/right (with a degenerate face)
             *        (delete it) ..
             *                     x intersection with numerical error
             *                    ..
             *                   . .
             *                  .  .
             *                 .   .
             *                .    .
             *               .     .
             *              .      .
             *
             */
            if (region_left->fixedge) {
                lx_trace_d("remove the left fixable edge: %{mesh_edge}", edge_left);

                // remove the left region
                lx_tessellator_active_regions_remove(tessellator, region_left);

                // delete the left fixable edge
                lx_mesh_edge_delete(tessellator->mesh, edge_left);

                // update the left region
                region_left = lx_tessellator_active_regions_left(tessellator, region_right);
                lx_assert(region_left);

                // update the left edge
                edge_left = region_left->edge;
                lx_assert(edge_left);
            } else if (region_right->fixedge) {
                lx_trace_d("remove the right fixable edge: %{mesh_edge}", edge_right);

                // remove the right region
                lx_tessellator_active_regions_remove(tessellator, region_right);

                // delete the right fixable edge
                lx_mesh_edge_delete(tessellator->mesh, edge_right);

                // update the right region
                region_right = lx_tessellator_active_regions_right(tessellator, region_left);
                lx_assert(region_right);

                // update the right edge
                edge_right = region_right->edge;
                lx_assert(edge_right);
            }
        }

        /* calculate intersection and fix the region order
         *
         *   .                .
         *   .     event      .
         *   .       .        .
         *   .     . .  .     .
         *   .   .   .     .  .
         *   . .     .        x
         *   x                .  .
         * . .                .     .
         *   .                .
         *   .
         */
        if (lx_mesh_edge_org(edge_left) != lx_mesh_edge_org(edge_right)) {
            /* we need calculate it if exists intersection
             *
             * it requires that the event lie between the left and right edges
             * for calculating intersection and neither of these is marked fixable
             *
             */
            if (    lx_mesh_edge_dst(edge_left) != lx_mesh_edge_dst(edge_right)
                &&  !region_left->fixedge && !region_right->fixedge
                &&  (   tessellator->event == lx_mesh_edge_dst(edge_left)
                    ||  tessellator->event == lx_mesh_edge_dst(edge_right))) {
                /* calculate intersection and fix the region order
                 *
                 * we must return directly if the current function was called recursively
                 * because the region order has been fixed after calling fix_region_intersection()
                 */
                if (lx_tessellator_fix_region_intersection(tessellator, region_left)) {
                    return ;
                }
            } else {
                /* we need fix the following case:
                 * - may violate ordering because of numerical errors for nearly identical slopes
                 * - the fixable and real edges are intersected.
                 * - the event does not lie between the left and right edges.
                 *
                 * case 1:
                 *
                 *   .              .
                 *   .              ..
                 *   .              ..
                 *   .              ..
                 *   ..             ..
                 *   .              ..
                 *   .    fix it    ..
                 *   . . -------->   .
                 *   .              .
                 *   .             .
                 *
                 * case 2:
                 *
                 *   .                .                                   .
                 *   .     event      .                    .    event      .
                 *   .       .        .                           .         .
                 *   .     . .  .     .                   .     . .  .       .
                 *   .   .   .     .  .        fix it         .   .     .     .
                 *   . .     .        x       ------->   .  .     .        .   .
                 *   x                .  .                .                   . .
                 * . .                .     .           .                        .
                 *   .                .                  .                     .
                 *   .             fixable                .                fixable
                 * fixable                              fixable
                 *
                 * case 3:
                 *                                                   edge_right
                 *               .                                       .
                 *               .      event             edge_left      .      event
                 *  .            .       .                  .            . dirty  .
                 *  .            .     .                    .  dirty     .     .
                 *  .            .   .                      .            .   . dirty
                 *  .            . .        intersection    .            . .
                 *  .            x        --------------->  .            x
                 *  .          . .                          .          . .
                 *  .        .   .                          .        .   .
                 *  .            .                                       .
                 *
                 *
                 */
                lx_tessellator_fix_region_order_at_bottom(tessellator, region_left);
            }
        }

        /* a degenerate loop consisting of only two edges
         * which may was generated after calling fix_region_order_at_bottom()
         *
         * we need delete one edge and region
         *
         * @note might gererate an unclosed edge
         *
         *           .
         *           ..
         *           ..
         * edge_left .. edge_right
         *           ..
         *           ..
         *           ..
         *            .
         *           .
         *          .
         */
        if (    lx_mesh_edge_org(edge_left) == lx_mesh_edge_org(edge_right)
            &&  lx_mesh_edge_dst(edge_left) == lx_mesh_edge_dst(edge_right)) {
            lx_trace_d("remove the edge for the degenerate loop: %{mesh_edge}", edge_left);

            // compute the combined winding of the right edge because the left edge will be deleted
            lx_tessellator_edge_winding_merge(edge_right, edge_left);

            // remove the left region
            lx_tessellator_active_regions_remove(tessellator, region_left);

            // delete the left edge
            lx_mesh_edge_delete(tessellator->mesh, edge_left);

            // update the left region
            region_left = lx_tessellator_active_regions_left(tessellator, region_right);
            lx_assert(region_left);
        }
    }
}

/* find the left top region of the leftmost edge with the same origin(event)
 *
 *  . edge_left                                                    . edge_right
 *  .                                                              .
 *  .                                                              .
 *  .          (leftmost)              .                           .
 *  .          edge_first             .                            .
 *  .                . region_first  .             . edge_last     .
 *  .                  .            .  region  .                   .
 *  . region_left        .         .       .                       . region_right
 *  .                      .      .    .  region_last              .
 * /.\                       .   . .                              /.\
 *  .  ------------------------ . event -------------------------- . ------- sweep line
 *  .                         .   . (org)                          .
 *  .                       .       .                              .
 *  .                     .                                        .
 *  .                                                              .
 */
static lx_tessellator_active_region_ref_t lx_tessellator_find_left_top_region(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region) {
    lx_assert(tessellator && tessellator->mesh && region && region->edge);

    // get the origin of the region
    lx_mesh_vertex_ref_t org = lx_mesh_edge_org(region->edge);
    lx_assert(org);

    // find the left top region of the leftmost edge with the same origin
    lx_tessellator_active_region_ref_t region_left = region;
    do {
        // get the left top region
        region_left = lx_tessellator_active_regions_left(tessellator, region_left);
        lx_assert(region_left && region_left->edge);
    } while (lx_mesh_edge_org(region_left->edge) == org);

    /* we fix it if the left edge was a temporary edge
     *
     *                           edge_first
     *  .               .             .
     *   .              .              .                     .
     *    .             .               . region_first    .
     *     .            .                .             .
     *      .           . region_left     .         .
     *       .          .                  .    .
     *        .         .                   .
     *         .        .                 event
     *          .       .
     *              edge_left
     *               (fixedge)
     *
     */
    if (region_left->fixedge) {
        lx_trace_d("fix the left edge: %{mesh_edge}", region_left->edge);

        // get the first region
        lx_tessellator_active_region_ref_t region_first = lx_tessellator_active_regions_right(tessellator, region_left);
        lx_assert(region_first);

        /* create a new edge and connect event to the temporary edge
         *
         *
         *                           edge_first
         *  .               .             .
         *   .              . *  edge_new  .                     .
         *    .             .    *          . region_first    .
         *     .            .       *        .             .
         *      .           . region_left     .         .
         *       .          .             *    .    .
         *        .         .                *  .
         *         .        .                 event
         *          .       .
         *              edge_left
         *               (fixedge)
         *
         */
        lx_mesh_edge_ref_t edge_new = lx_mesh_edge_connect(tessellator->mesh, lx_mesh_edge_lprev(region_first->edge), lx_mesh_edge_sym(region_left->edge));
        lx_assert(edge_new);

        /* fix the left edge to the new edge
         *
         *
         *                           edge_first
         *  .               . region_left .
         *   .                *  edge_new  .                     .
         *    .                  *          . region_first    .
         *     .                    *        .             .
         *      .                      *      .         .
         *       .                        *    .    .
         *        .                          *  .
         *         .                          event
         *          .
         */
        lx_tessellator_fix_region_edge(tessellator, region_left, edge_new);

        /* get the left region
         *
         *                   region_first
         *  .               .             .
         *   .                * edge_first .                     .
         *    .                  *          .                 .
         *     .                    *        .             .
         *      . region_left          *      .         .
         *       .                        *    .    .
         *        .                          *  .
         *         .                          event
         *          .
         *       edge_left
         */
        region_left = lx_tessellator_active_regions_left(tessellator, region_left);
        lx_assert(region_left && region_left->edge);
    }
    return region_left;
}

/* find the left bottom region of the leftmost edge with the same destination(event)
 *
 *  . edge_left                                                    . edge_right
 *  .                                                              .
 *  .                                                              .
 *  .                    .         .       .                       .
 *  .                      .      .    .                           .
 * /.\                       .   . .                              /.\
 *  .  ------------------------ . event -------------------------- . ------- sweep line
 *  .                         .  . (dst)                           .
 *  .                       .     .    .                           .
 *  . region_left         .        .       .  region_last          . region_right
 *  .                   .           .  region .                    .
 *  .                 .              .           .                 .
 *  .               . region_first    .           edge_last        .
 *  .           edge_first             .                           .
 *  .                                                              .
 */
static lx_tessellator_active_region_ref_t lx_tessellator_find_left_bottom_region(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region) {
    lx_assert(tessellator && tessellator->mesh && region && region->edge);

    // get the destination of the region
    lx_mesh_vertex_ref_t dst = lx_mesh_edge_dst(region->edge);
    lx_assert(dst);

    // find the left bottom region of the leftmost edge with the same origin
    lx_tessellator_active_region_ref_t region_left = region;
    do {
        // get the left bottom region
        region_left = lx_tessellator_active_regions_left(tessellator, region_left);
        lx_assert(region_left && region_left->edge);
    } while (lx_mesh_edge_dst(region_left->edge) == dst);
    return region_left;
}

static lx_tessellator_active_region_ref_t lx_tessellator_insert_region_at_right(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region_left, lx_mesh_edge_ref_t edge_new) {
    lx_assert(tessellator && region_left && edge_new);

    // make region
    lx_tessellator_active_region_t region;
    region.edge     = edge_new;
    region.winding  = 0;
    region.inside   = 0;
    region.bounds   = 0;
    region.fixedge  = 0;

    // insert region after the left region
    return lx_tessellator_active_regions_insert_after(tessellator, region_left, &region);
}

/* insert the down-going edges from the given range: [head, tail) and new regions
 * and update winding numbers and mesh connectivity appropriately.
 *
 * the origin vertices of all down-going edges must be same.
 *
 * the left-top edge must be setted if the event vertex has any up-going edges already processed.
 *
 *        .                                                    .
 *     .                                                         .
 *  . edge_left                                                    . edge_right
 *  .                                                              .
 *  .                                                              .
 *  .                                                              .
 *  .                                                              .
 *  .     (prev)  .                  .                             .
 *  . edge_left_top . (finished)  .                                .
 *  .  (maybe null)   .        .                                   .
 *  .                   .(top)                                     .
 *  .            -------- . event -------------------------------- . ------------------- sweep line
 *  .                   . . .                                      .
 * /.\                .   .   .                                   /.\
 *  . region_left   .    \./    . region_new2                      .
 *  .   (prev)    .       .       .                                .  region_right
 *  .           .         .                                        .
 *  .         .           . region_new1                            .
 *  .       . region_new3 .                                        .
 *  .                     .                                        .
 *  .                 edge_head                                    .
 *  .                 edge_tail                                    .
 *  .                                                              .
 *  .                                                              .
 *  .                 new edges                                    .
 *  .                                                              .
 *
 */
static lx_void_t lx_tessellator_insert_down_going_edges(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region_left, lx_mesh_edge_ref_t edge_head, lx_mesh_edge_ref_t edge_tail, lx_mesh_edge_ref_t edge_left_top, lx_bool_t fix_dirty_regions) {
    lx_assert(tessellator && tessellator->mesh && edge_head && edge_tail);

    // insert the down-going edges from the given range: [head, tail) and new regions
    lx_mesh_edge_ref_t edge = edge_head;
    do {
        // the edge must be down-going
        lx_assertf(lx_tessellator_edge_go_down(edge), "invalid edge: %{mesh_edge}", edge);

        // trace
        lx_trace_d("insert down-going edge: %{mesh_edge}", edge);

        // add edge to the profiler
#ifdef LX_TESSELLATOR_PROFILER_ENABLE
        lx_tessellator_profiler_add_edge(edge);
#endif

        // insert a new region with the new edge at the right of the left region
        lx_tessellator_insert_region_at_right(tessellator, region_left, lx_mesh_edge_sym(edge));

        // the next inserted edge
        edge = lx_mesh_edge_onext(edge);

    } while (edge != edge_tail);

    // find it if the left-top edge is null
    if (!edge_left_top) {
        // get the leftmost new region
        lx_tessellator_active_region_ref_t region_new_leftmost = lx_tessellator_active_regions_right(tessellator, region_left);
        lx_assert(region_new_leftmost);

        // get the left-top edge
        edge_left_top = lx_mesh_edge_lnext(region_new_leftmost->edge);
    }

    lx_bool_t                           is_first    = lx_true;
    lx_tessellator_active_region_ref_t  region_new  = lx_null;
    lx_tessellator_active_region_ref_t  region_prev = region_left;
    lx_mesh_edge_ref_t                  edge_new    = lx_null;
    lx_mesh_edge_ref_t                  edge_prev   = edge_left_top;
    while (1) {
        // get the next new region
        region_new = lx_tessellator_active_regions_right(tessellator, region_prev);
        lx_assert(region_new && region_new->edge);

        // get the left down-going edge of the new region
        edge_new = lx_mesh_edge_sym(region_new->edge);

        // end? the origin vertices of all down-going edges must be same
        lx_check_break(lx_mesh_edge_org(edge_new) == lx_mesh_edge_org(edge_prev));

        /* joins the two edges if edge_prev and edge_new are disjoining but the original vertices are same
         *
         * before(edge_prev.org == edge_new.org):
         *
         *     . . . . org . . . . . .
         *          .
         *       .      org . . . .
         *    .           .  .
         * .               .    .
         * edge_prev        .
         *                   .
         *                  edge_new
         *
         *
         * after:
         *
         *     . . . . org . . . . . .
         *          .   .
         *       .       .     . . . .
         *    .           .      .
         * .               .        .
         * edge_prev        .
         *                   .
         *                  edge_new
         */
        if (lx_mesh_edge_onext(edge_prev) != edge_new) {
            lx_trace_d("joins down-going edges with the same origin");

            /*
             *     . . . . org . . . . . .
             *          .            face
             *       .      org_new  . . . .
             *    .   face    .       .
             * .               . face   .
             * edge_prev        .
             *                   .
             *                  edge_new
             *
             * @note edge_new.org will be created
             */
            lx_mesh_edge_splice(tessellator->mesh, lx_mesh_edge_oprev(edge_new), edge_new);

            /*
             *     . . . . org . . . . . .
             *          .   .
             *       .       .     . . . .
             *    .           .      .
             * .               .       .
             * edge_prev        .
             *                   .
             *                  edge_new
             *
             * @note edge_new.org will be removed and merged
             */
            lx_mesh_edge_splice(tessellator->mesh, edge_prev, edge_new);
        }

        // check
        lx_assert(lx_mesh_edge_onext(edge_prev) == edge_new);

        /* compute the winding of the new region
         *
         * region_new.winding = region_prev.winding + region_new.edge.winding
         *                                       => - region_new.edge.sym.winding
         *                                       => - edge_new.winding
         */
        region_new->winding = region_prev->winding - lx_tessellator_edge_winding(edge_new);

        // mark it if the new region is inside
        region_new->inside = lx_tessellator_winding_is_inside(tessellator, region_new->winding);

        // mark the region as "dirty" for calculating intersection
        region_prev->dirty = 1;

        /* fix two new edges with same slope before calculating intersections
         * and skip the first edge: edge_left_top != edge_prev
         *
         * we need to compute the combined winding of the new edge,
         * because we merge two edges into one.
         *
         *       edge_prev/new edge_new
         * .          .          .
         * ..         ..          .
         * ..         ..          .
         * ..         ..          . (winding += edge_prev.winding)
         * ..   fix   .. remove   .
         * .. ======> .. ======>  .
         * ..         ..          .
         * ..         x          x
         * .          .          .
         * .          .          .
         */
        if (!is_first && lx_tessellator_fix_region_order_at_bottom(tessellator, region_prev)) {

            // compute the combined winding of the new edge.
            lx_tessellator_edge_winding_merge(edge_new, edge_prev);

            // remove the previous region
            lx_tessellator_active_regions_remove(tessellator, region_prev);

            // remove the previous edge
            lx_mesh_edge_delete(tessellator->mesh, edge_prev);
        }

        // update the first state
        is_first = lx_false;

        // update the previous edge and region
        edge_prev   = edge_new;
        region_prev = region_new;
    }

    // mark the region as "dirty" for calculating intersection
    region_prev->dirty = 1;

    // check winding
    lx_assert(region_new->winding == region_prev->winding - lx_tessellator_edge_winding(edge_new));

    /* fix all dirty regions
     *
     *     .     .                  .                       .
     *     .     .                 . . .  region_left       .
     *     .     .                .   .   .                 .
     *     .     .               .     .     . dirty        .
     *     .     . dirty        .       dirty |             .
     *     .     .             . dirty        |             .
     *           .                            |
     *           .                            |
     *                                        |
     *                                        |
     *                                         ----------->
     *                                                     |
     *                                     <---------------
     *                                    |
     *                                     ->
     *                                       |
     *                         ...  <--------
     *                                 fix
     */
    if (fix_dirty_regions) {
        lx_tessellator_fix_all_dirty_regions(tessellator, region_prev);
    }
}

/* remove the degenerate edges
 *
 * zero-length edges, and contours with fewer than 3 vertices.
 */
static lx_void_t lx_tessellator_remove_degenerate_edges(lx_tessellator_t* tessellator) {
    lx_assert(tessellator);

    // the mesh
    lx_mesh_ref_t mesh = tessellator->mesh;
    lx_assert(mesh);

    lx_mesh_edge_ref_t edge_next    = lx_null;
    lx_mesh_edge_ref_t edge_lnext   = lx_null;
    lx_mesh_edge_ref_t edge         = lx_mesh_edge_head(mesh);
    lx_mesh_edge_ref_t tail         = lx_mesh_edge_tail(mesh);
    for (; edge != tail; edge = edge_next) {
        // the next edge
        edge_next = lx_mesh_edge_next(edge);
        edge_lnext = lx_mesh_edge_lnext(edge);

        // the length of edge is zero and the contour has at least three edges?
        if (    lx_tessellator_vertex_eq(lx_mesh_edge_org(edge), lx_mesh_edge_dst(edge))
            &&  lx_mesh_edge_lnext(edge_lnext) != edge) {
            lx_trace_d("remove degenerate edge");

            /* remove this edge
             *
             * before:
             *
             *          edge
             * <--------.<-.<--------
             *
             * after:
             *
             * <--------<--------
             */
            lx_mesh_edge_remove(mesh, edge);

            // update the next edge
            edge = edge_lnext;

            // update the lnext edge
            edge_lnext = lx_mesh_edge_lnext(edge);
        }

        // the contour is degenerate, only has one or two edges?
        if (lx_mesh_edge_lnext(edge_lnext) == edge) {
            lx_trace_d("remove degenerate contour");

            // two edges?
            if (edge_lnext != edge) {
                // the next edge will be deleted? update the next edge
                if (edge_lnext == edge_next || edge_lnext == lx_mesh_edge_sym(edge_next))
                    edge_next = lx_mesh_edge_next(edge_next);

                // delete the lnext edge
                lx_mesh_edge_delete(mesh, edge_lnext);
            }

            // the next edge will be deleted? update the next edge
            if (edge == edge_next || edge == lx_mesh_edge_sym(edge_next)) {
                edge_next = lx_mesh_edge_next(edge_next);
            }

            // delete this edge
            lx_mesh_edge_delete(mesh, edge);
        }
    }
}

/* remove the degenerate faces
 *
 * the two places this can happen are
 *
 * - in finish_top_regions()
 *   when we splice in a "temporary" edge produced by connect_bottom_event(),
 *
 * - in fix_region_order_at_top()
 *   where we splice already-processed edges to ensure that
 *   our active region order is not violated by numerical errors.
 */
static lx_void_t lx_tessellator_remove_degenerate_faces(lx_tessellator_t* tessellator) {
    lx_assert(tessellator);

    // the mesh
    lx_mesh_ref_t mesh = tessellator->mesh;
    lx_assert(mesh);

    lx_mesh_edge_ref_t edge = lx_null;
    lx_for_all_if (lx_mesh_face_ref_t, face, lx_mesh_face_itor(mesh), face) {
        edge = lx_mesh_face_edge(face);
        lx_assert(edge && lx_mesh_edge_lnext(edge) != edge);

        // the face is inside and degenerate? only with two edges?
        if (lx_tessellator_face_inside(face) && lx_mesh_edge_lnext(lx_mesh_edge_lnext(edge)) == edge) {
            lx_trace_d("remove degenerate face");

            // only clear inside
            lx_tessellator_face_inside_set(face, 0);
        }
    }
}

/* finish one top region of the current event
 *
 * we mark the it's face "inside" if this region is inside
 * and remove this region.
 *
 *       face        .
 * .               .
 *   .  region   .
 *     .       .
 *  edge .   .
 *         .
 */
static lx_void_t lx_tessellator_finish_top_region(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region) {
    lx_assert(tessellator && region);
    lx_trace_d("finish region: %{tess_region}", region);

    // finish region to the profiler
#ifdef LX_TESSELLATOR_PROFILER_ENABLE
    lx_tessellator_profiler_finish_region(region);
#endif

    // get the edge of this region
    lx_mesh_edge_ref_t edge = region->edge;
    lx_assert(edge);

    // get the face of this region
    lx_mesh_face_ref_t face = lx_mesh_edge_rface(edge);
    lx_assert(face);

    // mark the face "inside" if the region is inside
    lx_tessellator_face_inside_set(face, region->inside);

    /* optimization for triangulation
     *
     * we attach the bottommost right edge to the face now,
     * so we will find the bottommost left and right edge more quickly for triangulation.
     *
     *     .      .
     *    .       .
     *      .   . right => face.edge
     * left   .
     *
     */
    face->edge = lx_mesh_edge_oprev(edge);

    // remove this region
    lx_tessellator_active_regions_remove(tessellator, region);
}

/* finish all top regions of the current event
 *
 * we need close off them first if the left and right edges of these regions terminate
 * at this event.
 *
 * then we mark these faces "inside" or "outside" the polygon according
 * to their winding number and remove these regions.
 *
 *  . edge_left                                                    . edge_right
 *  .                                                              .
 *  .            (prev)                                            .
 *  .          (leftmost)              .                           .
 *  .          edge_head              .                            .
 *  .                . region_head   .             . edge_last     .
 *  .                  .            .  region  .                   .
 *  . region_left        .         .       .                       . region_right/tail
 *  .                      .      .    .  region_last              .
 * /.\                       .   . .                              /.\
 *  .  ------------------------ . event -------------------------- . ------- sweep line
 *  .                         .   .                                .
 *  .                       .       .                              .
 *  .                     .                                        .
 *  .                                                              .
 */
static lx_mesh_edge_ref_t lx_tessellator_finish_top_regions(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region_head, lx_tessellator_active_region_ref_t region_tail) {
    lx_assert(tessellator && tessellator->mesh && region_head);

    // we walk as far as possible if region_tail is null
    lx_mesh_edge_ref_t                  edge            = region_head->edge;
    lx_mesh_edge_ref_t                  edge_next       = lx_null;
    lx_tessellator_active_region_ref_t  region          = region_head;
    lx_tessellator_active_region_ref_t  region_next     = lx_null;
    while (region != region_tail) {
        /* clear the temporary edge mark "fixedge"
         *
         * - quite possibly we only need clear the mark "fixedge"
         *   if the temporary edge and event are conjoint.
         *
         * - the temporary edge have been fixed now if the temporary edge and event are disjoint.
         */
        region->fixedge = 0;

        // get the next right region
        region_next = lx_tessellator_active_regions_right(tessellator, region);
        lx_assert(region_next);

        // get the next right edge
        edge_next = region_next->edge;
        lx_assert(edge_next);

        // the temporary edge and event are disjoint or end?
        if (lx_mesh_edge_org(edge_next) != lx_mesh_edge_org(edge)) {
            /* we fix it if the left edge was a temporary edge
             *
             *  edge_head
             *  .
             *   . region_head
             *    .           .
             *     .         .            edge
             *      .       .             .
             *       .     .          .                 .
             *        .   .       .                     .   region_next
             *         . .   .       region             .
             *          .                               .
             *       event                              .
             *                                      edge_next
             *                                      (fixedge)
             *
             */
            if (region_next->fixedge) {
                lx_trace_d("fix the top edge: %{mesh_edge}", edge_next);

                /* create a new edge and connect the temporary edge to the event
                 *
                 *  edge_head
                 *  .
                 *   . region_head
                 *    .           .
                 *     .         .            edge
                 *      .       .             .
                 *       .     .          .                 .
                 *        .   .       .                *    .   region_next
                 *         . .   .     region  *  edge_new  .
                 *          .       *                       .
                 *       event                              .
                 *                                      edge_next
                 *                                      (fixedge)
                 *
                 */
                lx_mesh_edge_ref_t edge_new = lx_mesh_edge_connect(tessellator->mesh, edge_next, lx_mesh_edge_oprev(edge));
                lx_assert(edge_new);

                // reverse the edge
                edge_new = lx_mesh_edge_sym(edge_new);

                /* fix the next edge to the new edge
                 *
                 *  edge_head
                 *  .
                 *   . region_head
                 *    .           .
                 *     .         .            edge
                 *      .       .             .
                 *       .     .          .                 .
                 *        .   .       .                *        region_next
                 *         . .   .     region  *  edge_next
                 *          .       *
                 *       event
                 */
                lx_tessellator_fix_region_edge(tessellator, region_next, edge_new);

                // update the next edge
                edge_next = edge_new;
            } else {
                // finish the last top region
                lx_tessellator_finish_top_region(tessellator, region);
                break;
            }
        }

        /* joins the two edges if edge and edge_next are disjoining but the original vertices are same
         *
         * before(edge.org == edge_next.org):
         *
         *
         *                 edge_next
         *                   .
         *                  .
         * edge            .
         * .              .     .
         *    .          .    .
         *       .      org . . . .
         *          .
         *     . . . . org . . . . . .
         *
         *
         * after:
         *
         *                 edge_next
         *                   .
         *                  .
         * edge            .
         * .              .        .
         *    .          .      .
         *       .      .     . . . .
         *          .  .
         *     . . . . org . . . . . .

         */
        if (lx_mesh_edge_onext(edge_next) != edge) {
            lx_trace_d("joins up-going edges with the same origin");

            /*
             *
             *                 edge_next
             *                   .
             *                  .
             * edge      face  .
             * .              .         .
             *    .          .   face .
             *       .      org_new . . . .
             *          .        face
             *     . . . . org . . . . . .
             *
             * @note edge_next.org will be created
             */
            lx_mesh_edge_splice(tessellator->mesh, lx_mesh_edge_oprev(edge_next), edge_next);

            /*
             *                 edge_next
             *                   .
             *                  .
             * edge            .
             * .              .        .
             *    .          .      .
             *       .      .     . . . .
             *          .  .
             *     . . . . org . . . . . .
             *
             * @note edge_next.org will be removed and merged
             */
            lx_mesh_edge_splice(tessellator->mesh, lx_mesh_edge_oprev(edge), edge_next);
        }

        // check
        lx_assert(lx_mesh_edge_onext(edge_next) == edge);

        // finish the top region
        lx_tessellator_finish_top_region(tessellator, region);

        /* update the edge and region
         *
         * @note region_next->edge may be changed after fixing the temporary edge
         */
        edge    = region_next->edge;
        region  = region_next;
    }

    // return the last edge
    return edge;
}

/* connect this degenerate top event to the processed portion of the mesh
 *
 * the event vertex lies exactly on an already-processed edge or vertex.
 *
 *  . edge
 *  .
 *  .                     region
 *  .
 *  .
 *  . (top)
 *  .|event ----------------------------------- sweep line
 *  .  .   .
 * /.\   .     .
 *  .      .       .
 *  .        .         .
 *  .                      .
 *  .          new_edges
 *  .
 *  .
 */
static lx_void_t lx_tessellator_connect_top_event_degenerate(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region, lx_mesh_vertex_ref_t event) {
    lx_assert(tessellator && tessellator->mesh && region && event);
    lx_trace_d("connect degenerate top event: %{point}", lx_tessellator_vertex_point(event));

    // the left edge of the region
    lx_mesh_edge_ref_t edge = region->edge;
    lx_assert(edge);

    /* the event vertex lies exactly on edge.org and edge.org is an unprocessed vertex
     *
     * because the left region have been finished and removed if the edge.org have been processed
     *
     *  . edge
     *  .
     *  .                  region
     *  .
     *  .
     *  . (top)
     *  x|event --------------------------------- sweep line
     *     .   .
     *       .     .
     *         .       .
     *           .         .
     *                         .
     *             new_edges
     *
     *
     */
    if (lx_tessellator_vertex_eq(lx_mesh_edge_org(edge), event)) {
        lx_trace_d("connect the event to the origin of the edge: %{mesh_edge}", edge);

        /* we only connect the event vertex to the origin of the left edge
         * and wait for processing at next time, because the edge.org is an unprocessed vertex
         *
         * @note edge.org cannot be changed
         */
        lx_mesh_edge_splice(tessellator->mesh, edge, lx_mesh_vertex_edge(event));
        return;
    }

    /* the event vertex lies exactly on edge.dst and the edge.dst have been processed
     *
     * . edge_left                                                        . edge_right
     * .                  edge_left_top                                   .
     * .                        .                                         .
     * .                           .                                      .
     * .                              .                                   .
     * . region_left                     .  (top)                         .
     * .                                   x|x event -------------------- . ------- sweep line
     * .                            .   .  .  .   .                       .
     * .                      .       .    .    .     .                   .
     * .                .           .     /.\     .       .               . region_right
     * .           . region_first .        .        .         .           .
     * .      edge_first        .          .                      .       .
     * .                      .            .          new_edges           .
     * .                    .              .                              .
     * .                                   .   region                     .
     * .                                   .                              .
     * .                                 edge                             .
     */
    if (lx_tessellator_vertex_eq(lx_mesh_edge_dst(edge), event)) {
        lx_trace_d("connect the event to the destination of the edge: %{mesh_edge}", edge);

        // find the left region from the given bottom region
        lx_tessellator_active_region_ref_t region_left = lx_tessellator_find_left_bottom_region(tessellator, region);
        lx_assert(region_left);

        // get the first bottom region at the same destination
        lx_tessellator_active_region_ref_t region_first = lx_tessellator_active_regions_right(tessellator, region_left);
        lx_assert(region_first);

        // get the first down-going edge at the same destination
        lx_mesh_edge_ref_t edge_first = lx_mesh_edge_sym(region_first->edge);
        lx_assert(edge_first);

        // the left top edge
        lx_mesh_edge_ref_t edge_left_top = lx_mesh_edge_oprev(edge_first);
        lx_assert(edge_left_top);

        /* we remove it and it's region if the first edge is fixable
         * since now we have some real down-going edges.
         *
         * so we no longer need a fixable edge for connecting the bottom event now.
         */
        if (region->fixedge) {
            lx_trace_d("fix the degenerate top edge: %{mesh_edge}", edge);

            // check
            lx_assert(edge_first != edge_left_top);

            // remove the first region
            lx_tessellator_active_regions_remove(tessellator, region_first);

            // remove the first edge
            lx_mesh_edge_delete(tessellator->mesh, edge_first);

            // update the first edge
            edge_first = lx_mesh_edge_onext(edge_left_top);
        }

        /* merge the new edges with event between the first edge and the left top edge
         *
         * @note we must ensure the edge order (edge_left_top.onext => new_edges.onext => edge_first)
         *       for getting the range of inserting new down-going edges
         *
         * . edge_left                                                        . edge_right
         * .                  edge_left_top                                   .
         * .                        .                                         .
         * .                           .                                      .
         * .                              .                                   .
         * . region_left  (merged position)  .  (top)                         .
         * .                                   . event ---------------------- . ------- sweep line
         * .                            .   .  .  .   .                       .
         * .                      .       .    .    .     .                   .
         * .                .           .     /.\     .       .               . region_right
         * .           . region_first .        .        .         .           .
         * .      edge_first        .          .                      .       .
         * .                      .            .          new_edges           .
         * .                    .              .                              .
         * .                                   .   region                     .
         * .                                   .                              .
         * .                                 edge                             .
         *
         */
        lx_mesh_edge_splice(tessellator->mesh, event->edge, edge_left_top);

        // check
        lx_assert(lx_mesh_edge_org(edge_left_top) == event);
        lx_assert(lx_mesh_edge_onext(edge_left_top) != edge_first);

        // insert new down-going edges at this event and create new active regions
        lx_tessellator_insert_down_going_edges(tessellator, region_left, lx_mesh_edge_onext(edge_left_top), edge_first, lx_tessellator_edge_go_up(edge_left_top)? edge_left_top : lx_null, lx_true);
    } else {
        /* the event vertex lies exactly on an already-processed edge
         *
         *  . edge
         *  .
         *  .                     region
         *  .
         *  .
         *  . (top)
         *  .|event ------------------------------ sweep line
         *  .  .   .
         * /.\   .     .
         *  .      .       .
         *  .        .         .
         *  .                      .
         *  .          new_edges
         *  .
         *  .
         */
        lx_trace_d("connect the event to the body of the edge: %{mesh_edge}", edge);

        /* split the left edge and add a new edge
         *
         *  . edge
         *  .
         *  .                     region
         *  .
         *  .
         *  . (top)
         *  .|event ------------------------------ sweep line
         *  .  .   .
         * /.\   .     .
         *  .      .       .
         *  .        .         .
         *  .                      .
         *  .          new_edges
         *  .
         *  x (new)
         *  .
         *  .
         *  .
         * \./edge_new
         *  .
         *  .
         *  . .
         *  . . .
         *  .  .  .
         *  .   .   . edge.rnext
         */
        lx_mesh_edge_ref_t edge_new = lx_mesh_edge_split(tessellator->mesh, lx_mesh_edge_sym(edge));
        lx_assert(edge_new && edge_new == lx_mesh_edge_onext(edge));

        /* delete the new edge which was created recently if the edge is fixable
         *
         *  . edge
         *  .
         *  .                     region
         *  .
         *  .
         *  . (top)
         *  .|event ------------------------------ sweep line
         *     .   .
         *       .     .
         *         .       .
         *           .         .
         *                         .
         *             new_edges
         *
         *  .
         *  . .
         *  . . .
         *  .  .  .
         *  .   .   .
         */
        if (region->fixedge) {
            lx_trace_d("fix the degenerate top edge: %{mesh_edge}", edge);

            // delete the new edge which was created recently
            lx_mesh_edge_delete(tessellator->mesh, edge_new);
            region->fixedge = 0;
        }

        /* merge edge.org and event and remove the edge.org which was created recently
         *
         *  . edge
         *  .
         *  .                     region
         *  .
         *  .
         *  . (top)
         *  . event ------------------------------ sweep line
         * /.\ .   .
         *  .    .     .
         *  .      .       .
         *  .        .         .
         *  .                      .
         *  .          new_edges
         *  .
         *  . edge_new
         *  .
         *  .
         *  . .
         *  . . .
         *  .  .  .
         *  .   .   .
         *
         * or
         *
         *  . edge
         *  .
         *  .                     region
         *  .
         *  .
         *  . (top)
         *  . event ------------------------------ sweep line
         *     .   .
         *       .     .
         *         .       .
         *           .         .
         *                         .
         *             new_edges
         *
         *  .
         *  . .
         *  . . .
         *  .  .  .
         *  .   .   .
         */
        lx_mesh_edge_splice(tessellator->mesh, event->edge, edge);

        // check
        lx_assert(lx_mesh_edge_org(edge) == event);

        // continue to process this event recursively
        lx_tessellator_sweep_event(tessellator, event);
    }
}

/* connect this top event to the processed portion of the mesh
 *
 * - the normal case:
 *
 *   - split this region to two regions by connecting this top event to
 *     the lower destinate vertex of the left or right edge
 *     if the contained(left) region is inside
 *
 *   - only add all edges to the mesh if the the contained(left) region is outside
 *
 * - the degenerate case:
 *
 *   - we merge it into that edge chain if the event vertex close enough to the left edge or right edge
 */
static lx_void_t lx_tessellator_connect_top_event(lx_tessellator_t* tessellator, lx_mesh_vertex_ref_t event) {
    lx_assert(tessellator && tessellator->mesh && event);

    // trace
    lx_trace_d("connect top event: %{point}", lx_tessellator_vertex_point(event));

    // the edge of the event vertex
    lx_mesh_edge_ref_t edge_event = lx_mesh_vertex_edge(event);
    lx_assert(edge_event);

    // the up-going edge of the event vertex
    lx_mesh_edge_ref_t edge_event_up = lx_mesh_edge_sym(edge_event);
    lx_assert(edge_event_up);

    // get the left region containing this event from the up-going edge
    lx_tessellator_active_region_ref_t region_left = lx_tessellator_active_regions_find(tessellator, edge_event_up);
    lx_assert(region_left);

    // get the right region
    lx_tessellator_active_region_ref_t region_right = lx_tessellator_active_regions_right(tessellator, region_left);
    lx_assert(region_right);

    // the left and right edge containing this event
    lx_mesh_edge_ref_t edge_left    = region_left->edge;
    lx_mesh_edge_ref_t edge_right   = region_right->edge;
    lx_assert(edge_left);
    lx_assert(edge_right);

    /* the degenerate case
     *
     * we merge it into that edge chain if the event vertex close enough to the left edge
     *
     *  . edge_left                                                    . edge_right
     *  .                                                              .
     *  .                     region_left                              .
     *  .                                                              .
     *  .                                                              .
     *  . (top)                                                        .
     *  x event ------------------------------------------------------ . ------------------ sweep line
     *  .  .   .                                                       .
     * /.\   .     .                                                  /.\
     *  .      .       .                                               .
     *  .        .         .                                           .  region_right
     *  .                      .                                       .
     *  .          new_edges                                           .
     *  .                                                              .
     *  .                                                              .
     */
    if (lx_tessellator_vertex_on_edge(event, lx_mesh_edge_dst(edge_left), lx_mesh_edge_org(edge_left))) {
        // connect the degenerate top event
        lx_tessellator_connect_top_event_degenerate(tessellator, region_left, event);
        return ;
    }

    // get the region which edge.dst is lower and we need connect it
    lx_tessellator_active_region_ref_t region_lower = lx_tessellator_vertex_in_top_or_horizontal(lx_mesh_edge_dst(edge_left), lx_mesh_edge_dst(edge_right))? region_right : region_left;

    /* we need split it if the contained(left) region is inside
     *
     * before:
     *
     * . . . . . . . . .
     * .     inside    .
     * .               .
     * .     e . . --- . ---- sweep line
     * .     .   .     .
     * .     . . .     .
     * .               .
     * .               .
     * . . . . . . . . .
     *
     * after:
     *
     * . . . . . . . . .
     * . .             .
     * .   .           .
     * .     e . . --- . ---- sweep line
     * .     .   .     .
     * .     . . .     .
     * .               .
     * .               .
     * . . . . . . . . .
     */
    if (region_left->inside || region_lower->fixedge) {
        lx_trace_d("split the region: %{tess_region}", region_left);

        /* we need connect the top event to it if the destinate vertex of the left edge is lower
         *
         *                                                             .
         *          .                                                    .
         *       .                                                         . edge_right
         *    .                                                            . (upper)
         *  . edge_left (lower)                                            .
         *  .   .                                                          .
         *  .      .                         region_new                    .
         *  .         .                                                    .
         *  .            . edge_new                                        .
         *  . region_left   .                                              .
         *  .   (inside)       .(top)                                      .
         *  .            -------- . event -------------------------------- . ------------------- sweep line
         *  .                   . . .                                      .
         * /.\                .   .   .                                   /.\
         *  .               .    \./                                       .
         *  .             .       .                                        .  region_right
         *  .                     . edge_event                             .
         *  .                                                              .
         *  .                  new edges                                   .
         *  .                                                              .
         * (fixedge ?=> edge_new)
         */
        lx_mesh_edge_ref_t edge_new = lx_null;
        if (region_lower == region_left) {
            // split the left region to two regions by connecting it
            edge_new = lx_mesh_edge_connect(tessellator->mesh, lx_mesh_edge_dnext(edge_left), edge_event);
            edge_new = lx_mesh_edge_sym(edge_new);
        } else {
            /* we need connect the top event to it if the destinate vertex of the right edge is lower
             *
             *        .
             *     .
             *  . edge_left
             *  .  (upper)
             *  .                                                          .
             *  .                                                            .
             *  .           region_left (inside)                               . edge_right
             *  .                                                          .   . (lower)
             *  .                                                     .        .
             *  .                                     edge_new   .             .
             *  .                                           .                  .
             *  .                                      .                       .
             *  .                                 .             region_new     .
             *  .                            .                                 .
             *  .                    (top)                                     .
             *  .            -------- . event -------------------------------- . ------------------- sweep line
             *  .                   . . .                                      .
             * /.\                .   .   .                                   /.\
             *  .               .    /.\                                       .
             *  .             .       .                                        .  region_right
             *  .                     .  edge_event_up                         .
             *  .                                                              .
             *  .                  new edges                                   .
             *  .                                                              .
             *                                                       (fixedge ?=> edge_new)
             */
            // split the left region to two regions by connecting it
            edge_new = lx_mesh_edge_connect(tessellator->mesh, edge_event_up, lx_mesh_edge_lnext(edge_right));
        }

        // check
        lx_assert(edge_new);

        /* we fix it if the connected edge need fix
         *
         * before:
         *                                                             .
         *          .                                                    .
         *       .                                                         . edge_right
         *    .                                                            . (upper)
         *  . edge_left (lower)                                            .
         *  .   .                                                          .
         *  .      .                                                       .
         *  .         .                                                    .
         *  .            . edge_new                                        .
         *  . region_left   .                                              .
         *  .   (inside)       .(top)                                      .
         *  .            -------- . event -------------------------------- . ------------------- sweep line
         *  .                   . . .                                      .
         * /.\                .   .   .                                   /.\
         *  .               .    \./                                       .
         *  .             .       .                                        .  region_right
         *  .                     . edge_event                             .
         *  .                                                              .
         *  .                  new edges                                   .
         *  .                                                              .
         * fixedge
         *
         *
         * after:
         *                                                             .
         *          .                                                    .
         *       .                                                         . edge_right
         *    .                                                            . (upper)
         *  . edge_left (lower)                                            .
         *      .                                                          .
         *         .                                                       .
         *            .                     region_left                    .
         *               . edge_new                                        .
         *                  .                                              .
         *                      .(top)                                     .
         *               -------- . event -------------------------------- . ------------------- sweep line
         *                      . . .                                      .
         *                    .   .   .                                   /.\
         *                  .    \./                                       .
         *                .       .                                        .  region_right
         *                        . edge_event                             .
         *                                                                 .
         *                     new edges                                   .
         */
        if (region_lower->fixedge) {
            // uses the new edge to fix the connected edge of the lower region
            lx_tessellator_fix_region_edge(tessellator, region_lower, edge_new);
        } else {
            // insert a new region with the new edge at the right of the left region
            lx_tessellator_active_region_ref_t region_new = lx_tessellator_insert_region_at_right(tessellator, region_left, edge_new);
            lx_assert(region_new);

            /* check region
             *
             * region_left <= region_new <= region_right
             */
            lx_assert(lx_tessellator_active_regions_in_left(tessellator, region_left, region_new));
            lx_assert(lx_tessellator_active_regions_in_left(tessellator, region_new, region_right));

            // compute the winding of the new region
            region_new->winding = region_left->winding + lx_tessellator_edge_winding(edge_new);

            // mark it if the new region is inside
            region_new->inside = lx_tessellator_winding_is_inside(tessellator, region_new->winding);

            // trace
            lx_trace_d("insert edge: %{mesh_edge} to region: %{tess_region}", edge_new, region_new);
        }

        // add split to the profiler
#ifdef LX_TESSELLATOR_PROFILER_ENABLE
        lx_tessellator_profiler_add_split(edge_new);
#endif

        // continue to sweep this event for the new region
        lx_tessellator_sweep_event(tessellator, event);

    } else { // only add all edges to the mesh if the the contained(left) region is outside

        /* insert all down-going edges and new regions
         * and update winding numbers and mesh connectivity appropriately
         *
         *        .                                                    .
         *     .                                                         .
         *  . edge_left                                                    . edge_right
         *  .                                                              .
         *  .                                                              .
         *  .           region_left (outside)                              .
         *  .                                                              .
         *  .                                                              .
         *  .            x (no edge_left_top)                              .
         *  .              x                                               .
         *  .                x                                             .
         *  .                   (top)                                      .
         *  .            -------- . event -------------------------------- . ------------------- sweep line
         *  .                   . . .                                      .
         * /.\                .   .   .                                   /.\
         *  .               .    \./                                       .
         *  .             .       .                                        .  region_right
         *  .                     .  edge_event                            .
         *  .                                                              .
         *  .                  new edges                                   .
         *  .                                                              .
         *
         */
        lx_tessellator_insert_down_going_edges(tessellator, region_left, edge_event, edge_event, lx_null, lx_true);
    }
}

/* connect the bottom event if no down-going edges
 *
 * since there are no down-going edges,
 * two regions (region_left and region_last) are being merged into one.
 *
 * we need add a temporary down-going edge because:
 *
 * - if the two regions (region_left and region_last) being merged are "inside",
 *   we must add an edge to keep them separated (the combined region would not be monotone).
 *
 *   .e.g
 *
 *                   .
 *                 .   .
 *               .       .
 *             .           .
 *           .               .
 *         .         .         .
 *          .      .   .      .
 *           .       e ----- . ----- sweep line
 *            .      *      .
 *             .     *     .
 *              .    *    .
 *               .   *   .
 *                .  *  .
 *                 . * .
 *                  .*.
 *                   .
 *
 *
 * - in any case, we must leave some record of event
 *   so that we can merge event with features that we have not seen yet.
 *
 *   for example, maybe there is a horizontal edge which passes just to
 *   the bottom of event. we would like to splice event into this edge.
 *
 *   .e.g
 *
 *                   .                                                        .
 *                 .   .                                                    .   .
 *               .       .                                                .       .
 *             .           .                                            .           .
 *           .               .                                        .               .
 *         .         .         .                                    .         .         .
 *          .      .   .      .                                      .      .   .      .
 *           .       e ----- . ----- sweep line   ======>             .       e       .
 *            .      *      .                   after fixing           .     *       .
 *             .     *     .                                            .   *       .
 *              .  -----  .                                              . *-----  .
 *               .  .*.  .                                                .  . .  .
 *                .  .  .                                                  .  .  .
 *                 . * .                                                    .   .
 *                  .*.                                                      . .
 *                   .                                                        .
 *
 * however, we don't want to connect event to just any vertex.
 * (maybe generate new intersections)
 *
 * so we connect event to the topmost unprocessed vertex of the combined region.
 *
 * but because of unseen vertices with all down-going edges, and also
 * new vertices which may be created by edge intersections, we don''t
 * know where that topmost unprocessed vertex is.
 *
 * in the meantime, we connect event to the closest vertex of either chain,
 * and mark the region as "fixedge".
 *
 * this flag says to remove and reconnect this edge
 * to the next processed vertex on the boundary of the combined region.
 *
 * quite possibly the vertex we connected to will turn out to be the
 * closest one, in which case we won''t need to make any changes.
 *
 *
 *  . edge_left                                                    . edge_right
 *  .                regions have been removed                     .
 *  .                                                              .
 *  .          (leftmost)              .                           .
 *  .          edge_first             .                            .
 *  .                .               .             . edge_last     .
 *  .                  .            .          .                   .
 *  . region_left        .         .       .                       . region_right
 *  .                      .      .    .                           .
 * /.\                       .   . .                              /.\
 *  .  ------------------------ . event -------------------------- . ------- sweep line
 *  .                        (bottom)                              .
 *  .               .                                              .
 *  .       .                                                      .
 *  .        edge_new                                              .
 *    .                  region_new(fixedge)                     .
 *       .                                                     .
 */
static lx_void_t lx_tessellator_connect_bottom_event(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region_left, lx_mesh_edge_ref_t edge_first, lx_mesh_edge_ref_t edge_last) {
    lx_assert(tessellator && tessellator->mesh && tessellator->event && region_left && edge_first && edge_last);
    lx_trace_d("connect bottom event: %{point}", lx_tessellator_vertex_point(tessellator->event));

    // get the right region of the event
    lx_tessellator_active_region_ref_t region_right = lx_tessellator_active_regions_right(tessellator, region_left);
    lx_assert(region_right);

    // get the left and right edge of the event
    lx_mesh_edge_ref_t edge_left    = region_left->edge;
    lx_mesh_edge_ref_t edge_right   = region_right->edge;
    lx_assert(edge_left && edge_right);

    /* attempt to fix the intersection of the left and right edges in the left region first
     *
     *           region_left
     *                                   .
     * edge_left       .               . edge_right
     *   .             .  .          .
     *     .       .   . .         .
     *       .       . ..        .
     *         .       . event . --------------------------- sweep line
     *           .           .
     *             .       .
     *               .   .
     *                 x -- intersection
     *               .   .
     *             .
     *           .
     */
    if (lx_mesh_edge_dst(edge_left) != lx_mesh_edge_dst(edge_right)) {
        lx_trace_d("fix intersection of %{mesh_edge}", edge_left);
        lx_trace_d("                  x %{mesh_edge}", edge_right);

        /* fix the intersection
         *
         * the region order might be changed if the intersection has some numerical errors.
         *
         * but we need not fix it immediately because we will fix it in insert_down_doing_edges(),
         * and before this we only inserted a fixedge edge or two new splitted edges.
         *
         * so it will not have any affect though the region order have been changed.
         * but we cannot check the region order in the insert_region_at_right().
         */
        lx_tessellator_fix_region_intersection(tessellator, region_left);
    }

    /* the degenerate case
     *
     * the edge of the left region may pass through event,
     * or may coincide with new intersection vertex after calculating intersection previously
     *
     *                edge_left
     *  .                .                                             . edge_right
     *  .                .        regions have been removed            .
     *  .                .                                             .
     *  .                .                                             .
     *  .                .             edge_first                      .
     *  .                .                 .                           .
     *  .                . region_left   .                             .
     *  .    .           .             .             . edge_last       .
     *  .      .         .           .          .                      .
     * /.\       .      /.\        .       .                          /.\ region_right
     *  .          .     .       .    .                                .
     *  .            .   .     . .                                     .
     *  .              . .   .                                         .
     *  .                .|. event ----------------------------------- . ------- sweep line
     *  .             . . (bottom)                                     .
     *  .          .   .                                               .
     *  .       .     .                                                .
     *  .            .                                                 .
     *  .           . new edges                                        .
     *  .                                                              .
     */
    lx_bool_t is_degenerate = lx_false;
    if (lx_tessellator_vertex_eq(tessellator->event, lx_mesh_edge_org(edge_left))) {
        lx_trace_d("merge the event to the origin of the left edge: %{mesh_edge}", edge_left);
        lx_assert(tessellator->event != lx_mesh_edge_org(edge_left));

        /* merge event and edge_left.org
         *
         *                edge_left
         *  .                .                                             . edge_right
         *  .                .                                             .
         *  .                .                                             .
         *  .                .                                             .
         *  .                .             edge_first                      .
         *  .                .                .                            .
         *  .                . region_left  .                              .
         *  .    .           .            .             . edge_last        .
         *  .      .         .          .          .                       .
         * /.\       .      /.\       .       .                           /.\ region_right
         *  .          .     .      .    .                                 .
         *  .            .   .    . .                                      .
         *  .              . .  .                                          .
         *  .                . event ------------------------------------- . ------- sweep line
         *  .             . . (bottom)                                     .
         *  .          .   .                                               .
         *  .       .     .                                                .
         *  .            .                                                 .
         *  .           . new edges                                        .
         *  .                                                              .
         */
        lx_mesh_edge_splice(tessellator->mesh, edge_first, lx_mesh_edge_oprev(edge_left));

        // the event vertex cannot be changed
        lx_assert(lx_mesh_edge_org(edge_left) == tessellator->event);
        lx_assert(lx_mesh_edge_org(edge_first) == tessellator->event);

        /* update the left and first region
         *
         * edge_left
         *  .                     .                                             . edge_right
         *  .                     .                                             .
         *  . region_left         .                                             .
         *  .                     .                                             .
         *  .                     .                                             .
         *  .        region_first . region_xxx     .                            .
         *  .       .             .              .                              .
         *  .         .           .            .             . edge_last        .
         *  .           .         .          .          .                       .
         * /.\            .       .        .       .                           /.\ region_right
         *  .               .     .      .    .                                 .
         *  .      edge_first .   .    . .                                      .
         *  .                   . .  .                                          .
         *  .                     . event ------------------------------------- . ------- sweep line
         *  .                  . . (bottom)                                     .
         *  .               .   .                                               .
         *  .            .     .                                                .
         *  .                 .                                                 .
         *  .                . new edges                                        .
         */
        region_left = lx_tessellator_find_left_top_region(tessellator, region_left);
        lx_assert(region_left);

        // update the first region
        lx_tessellator_active_region_ref_t region_first = lx_tessellator_active_regions_right(tessellator, region_left);
        lx_assert(region_first);

        // update the first edge
        edge_first = region_first->edge;
        lx_assert(edge_first);

        /* finish new unprocessed regions of this event and remove them
         *
         * edge_left
         *  .                     .                                             . edge_right
         *  .                     .                                             .
         *  . region_left         .                                             .
         *  .                     .                                             .
         *  .                     .                                             .
         *  .        region_first . region_xxx     .                            .
         *  .       .  (finished) .  (finished)  .                              .
         *  .         .           .            .             . edge_last        .
         *  .           .         .          .          .                       .
         * /.\            .       .        .       .                           /.\ region_right
         *  .               .     .      .    .                                 .
         *  .      edge_first .   .    . .                                      .
         *  .                   . .  .                                          .
         *  .                     . event ------------------------------------- . ------- sweep line
         *  .                  . . (bottom)                                     .
         *  .               .   .                                               .
         *  .            .     .                                                .
         *  .                 .                                                 .
         *  .                . new edges                                        .
         *
         */
        lx_tessellator_finish_top_regions(tessellator, region_first, region_right);

        // mark "degenerate" for inserting new down-going edges
        is_degenerate = lx_true;
    }

    /* the degenerate case
     *
     * the edge of the right region may pass through event
     *
     * edge_left                                   edge_right
     *  .                                             .                                .
     *  . region_left                                 .                                .
     *  .                                             .                                .
     *  .        regions have been removed            .                                .
     *  .                                             .                                .
     *  .                         edge_last           .                                .
     *  .                             .               .                 .              .
     *  .                               .             . region_right  .                .
     *  .                 edge_first      .           .             .                  .
     *  .                       .           .         .           .          .         .
     * /.\                          .         .      /.\        .       .             /.\
     *  .                               .       .     .       .    .                   .
     *  .                                   .     .   .     . .                        .
     *  .                                       .   . .   .                            .
     *  .                                    event  .|. ------------------------------ . ------- sweep line
     *  .                                   (bottom)   . .                             .
     *  .                                               .   .                          .
     *  .                                                .     . new edges             .
     *  .                                                                              .
     */
    if (lx_tessellator_vertex_eq(tessellator->event, lx_mesh_edge_org(edge_right))) {
        lx_trace_d("merge the event to the origin of the right edge: %{mesh_edge}", edge_right);
        lx_assert(tessellator->event != lx_mesh_edge_org(edge_right));

        /* merge event and edge_right.org
         *
         * edge_left                                   edge_right
         *  .                                             .                                .
         *  . region_left                                 .                                .
         *  .                                             .                                .
         *  .        regions have been removed            .                                .
         *  .                                             .                                .
         *  .                         edge_last           .                                .
         *  .                             .               .                 .              .
         *  .                               .             . region_right  .                .
         *  .               edge_first        .           .             .                  .
         *  .                    .              .         .           .          .         .
         * /.\                        .           .      /.\        .       .             /.\
         *  .                              .        .     .       .    .                   .
         *  .                                   .     .   .     . .                        .
         *  .                                        .  . .   .                            .
         *  .                                    event    . ------------------------------ . ------- sweep line
         *  .                                   (bottom)   . .                             .
         *  .                                               .   .                          .
         *  .                                                .     . new edges             .
         *  .                                                                              .
         */
        lx_mesh_edge_splice(tessellator->mesh, lx_mesh_edge_oprev(edge_last), edge_right);

        // the event vertex cannot be changed
        lx_assert(lx_mesh_edge_org(edge_first) == tessellator->event);
        lx_assert(lx_mesh_edge_org(edge_right) == tessellator->event);

        /* finish new unprocessed regions of this event and remove them.
         * update the right region and the last edge
         *
         * edge_left
         *  .                                             .                                .
         *  . region_left                                 .                                .
         *  .                                             .                                .
         *  .        regions have been removed            .                                .
         *  .                                             .                                .
         *  .                                             .                                .
         *  .                             .               .                 .              .
         *  .                               .             . region_right  . (finished)     .
         *  .               edge_first        .           . (finished)  . region_xxx       .
         *  .                    .              .         .           .          .         .
         * /.\                        .           .      /.\        .       .   edge_last /.\
         *  .                              .        .     .       .    .                   .
         *  .                                   .     .   .     . .     region_xxx         .
         *  .                                        .  . .   .         (finished)         .
         *  .                                    event    . ------------------------------ . ------- sweep line
         *  .                                   (bottom)   . .                             .
         *  .                                               .   .                          .
         *  .                                                .     . new edges             .
         *  .                                                                              .
         */
        edge_last = lx_tessellator_finish_top_regions(tessellator, region_right, lx_null);
        lx_assert(edge_last);

        // mark "degenerate" for inserting new down-going edges
        is_degenerate = lx_true;
    }

    // is degenerate case?
    if (is_degenerate) {
        // insert new down-going edges at this event and create new active regions
        lx_tessellator_insert_down_going_edges(tessellator, region_left, lx_mesh_edge_onext(edge_first), edge_last, edge_first, lx_true);
        return ;
    }

    // check
    lx_assert(lx_mesh_edge_lprev(edge_first) == lx_mesh_edge_sym(edge_last));

    /* patch a temporary edge connecting to the upper vertex of edge_left.org and edge_right.org
     *
     *                   .                                                   .
     *                 .   .                                               .   .
     *               .       .                                           .       .
     *             .           .                                       .           .
     *           .               .                                   .               .
     *         .         .         .                               .         .         *
     *          .      .   *      .                                 .      .   *      *
     *           .       e ----- . --------------------------------- . ----- e ----- * ---- sweep line
     *            .      *      .                or                   .      * face *
     * edge_new -- . - *  face .                        edge_new ----- . ----- *   *
     *              *         .                                         .         *
     *                  *    .                                           .    .
     *                      *                                             .
     *
     */
    lx_mesh_edge_ref_t edge_new = lx_tessellator_vertex_in_top_or_horizontal(lx_mesh_edge_org(edge_left), lx_mesh_edge_org(edge_right))? lx_mesh_edge_oprev(edge_left) : edge_right;
    edge_new = lx_mesh_edge_connect(tessellator->mesh, lx_mesh_edge_sym(edge_last), edge_new);
    lx_assert(edge_new && lx_mesh_edge_onext(edge_new) == edge_last);

    /* insert this new down-going edges at this event and create new active region
     *
     * delay to fix all dirty regions before marking the new temporary edge
     */
    lx_tessellator_insert_down_going_edges(tessellator, region_left, edge_new, edge_last, edge_first, lx_false);

    // the new region
    lx_tessellator_active_region_ref_t region_new = lx_tessellator_edge_region(lx_mesh_edge_sym(edge_new));
    lx_assert(region_new);

    // mark it as a temporary edge after the new region is created, then we need fix it
    region_new->fixedge = 1;

    // fix all dirty regions and calculate intersection
    lx_tessellator_fix_all_dirty_regions(tessellator, region_new);

    // add patch to the profiler
#ifdef LX_TESSELLATOR_PROFILER_ENABLE
    lx_tessellator_profiler_add_patch(edge_new);
#endif

    // trace
    lx_trace_d("patch a temporary edge: %{mesh_edge}", lx_mesh_edge_sym(edge_new));
}

/* process one event vertex at the sweep line
 *
 *      event
 *        . ------------------------ sweep line
 * . --- . --- . -------------------
 *   .  .          .
 *     . ------------- . -----------
 *   .  .               .
 *  .     .               . --------
 * . ------ . ----------------------
 */
static lx_void_t lx_tessellator_sweep_event(lx_tessellator_t* tessellator, lx_mesh_vertex_ref_t event) {
    lx_assert(tessellator && event);

    // check
#ifdef LX_DEBUG
    lx_tessellator_active_regions_check(tessellator);
#endif

    // update the current sweep event for the active_region.leq
    tessellator->event = event;

    // trace
    lx_trace_d("event: sweep: %{mesh_vertex}", event);

    // find an active region of all edges at this event
    lx_mesh_edge_ref_t                  edge = lx_mesh_vertex_edge(event);
    lx_mesh_edge_ref_t                  scan = edge;
    lx_tessellator_active_region_ref_t  region = lx_null;
    while (!(region = lx_tessellator_edge_region(scan))) {
        scan = lx_mesh_edge_onext(scan);
        lx_check_break(scan != edge);
    }

    /* the active regions of all up-going edges at this event have been not finished.
     *
     *  . edge_left                                                    . edge_right
     *  .                                                              .
     *  .                                                              .
     *  .          (leftmost)              .                           .
     *  .          edge_first             .                            .
     *  .                . region_first  .             . edge_last     .
     *  .                  .            .  region  .                   .
     *  . region_left        .         .       .                       . region_right
     *  .                      .      .    .  region_last              .
     * /.\                       .   . .                              /.\
     *  .  ------------------------ . event -------------------------- . ------- sweep line
     *  .                         .   .                                .
     *  .                       .       .                              .
     *  .                     .                                        .
     *  .                                                              .
     *
     */
    if (region) {
        // find the left top region of the leftmost edge with the same origin
        lx_tessellator_active_region_ref_t region_left = lx_tessellator_find_left_top_region(tessellator, region);
        lx_assert(region_left);

        // get the first(leftmost) top region of this event
        lx_tessellator_active_region_ref_t region_first = lx_tessellator_active_regions_right(tessellator, region_left);
        lx_assert(region_first);

        // get the first(leftmost) top edge of this event
        lx_mesh_edge_ref_t edge_first = region_first->edge;
        lx_assert(edge_first);

        /* finish all top regions of this event and remove these regions
         *
         * return the last edge and right region in the meantime
         */
        lx_mesh_edge_ref_t edge_last = lx_tessellator_finish_top_regions(tessellator, region_first, lx_null);
        lx_assert(edge_last);

        /* no down-going edges?
         *
         *  . edge_left                                                    . edge_right
         *  .                regions have been removed                     .
         *  .                                                              .
         *  .          (leftmost)              .                           .
         *  .          edge_first             .                            .
         *  .                .               .             . edge_last     .
         *  .                  .            .          .                   .
         *  . region_left        .         .       .                       . region_right
         *  .                      .      .    .                           .
         * /.\                       .   . .                              /.\
         *  .  ------------------------ . event -------------------------- . ------- sweep line
         *  .                        (bottom)                              .
         *  .                                                              .
         *  .                                                              .
         */
        if (lx_mesh_edge_onext(edge_first) == edge_last) {
            lx_tessellator_connect_bottom_event(tessellator, region_left, edge_first, edge_last);
        } else {
            // insert all down-going edges at this event and create new active regions
            lx_tessellator_insert_down_going_edges(tessellator, region_left, lx_mesh_edge_onext(edge_first), edge_last, edge_first, lx_true);
        }
    } else {
        /* all edges are new and go down.
         *
         * we need connect this top event to the processed portion of the mesh
         *
         *            (top)
         * -------- . event ------------- sweep line
         *        . . .
         *      .   .   .
         *    .     .
         *  .       .
         *          .
         *
         *       new edges
         */
        lx_tessellator_connect_top_event(tessellator, event);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_void_t lx_tessellator_monotone_make(lx_tessellator_t* tessellator, lx_rect_ref_t bounds) {
    lx_assert(tessellator && bounds);

    // trace
    lx_trace_d("");
    lx_trace_d("bounds: %{rect}", bounds);

    // init profiler
#ifdef LX_TESSELLATOR_PROFILER_ENABLE
    lx_tessellator_profiler_init(bounds);
#endif

    // the mesh
    lx_mesh_ref_t mesh = tessellator->mesh;
    lx_assert(mesh);

    // remove degenerate edges
    lx_tessellator_remove_degenerate_edges(tessellator);

    // make event queue
    if (!lx_tessellator_event_queue_make(tessellator)) {
        return ;
    }

    // make active regions
    if (!lx_tessellator_active_regions_make(tessellator, bounds)) {
        return ;
    }

    // the event queue
    lx_priority_queue_ref_t event_queue = tessellator->event_queue;
    lx_assert(event_queue);

    while (lx_priority_queue_size(event_queue)) {

        // get the minimum vertex event
        lx_mesh_vertex_ref_t event = (lx_mesh_vertex_ref_t)lx_priority_queue_get(event_queue);
        lx_assert(event);

        // pop it from the event queue first
        lx_priority_queue_pop(event_queue);

        // attempt to merge all vertices at same position as mush as possible
        while (lx_priority_queue_size(event_queue)) {

            // get the next vertex event
            lx_mesh_vertex_ref_t event_next = (lx_mesh_vertex_ref_t)lx_priority_queue_get(event_queue);
            lx_assert(event_next);

            // two vertices are exactly same?
            lx_check_break(lx_tessellator_vertex_eq(event, event_next));

            // pop the next event from the event queue
            lx_priority_queue_pop(event_queue);

            // trace
            lx_trace_d("event: merge: %{point}", lx_tessellator_vertex_point(event));

            /* merge them if two vertices have same position and remove the next event vertex
             *
             * this is more efficient than processing them one at a time
             * and is also important for correct handling of certain degenerate cases.
             *
             * we can merge them safely because all zero-length edges have been removed.
             *
             *  .
             *   .      .
             *    .   .
             *     .. merge
             *   .   .
             * .      .
             *         .
             */
            lx_mesh_edge_splice(mesh, lx_mesh_vertex_edge(event), lx_mesh_vertex_edge(event_next));
        }

        // sweep this event
        lx_tessellator_sweep_event(tessellator, event);
    }

    // remove degenerate faces
    lx_tessellator_remove_degenerate_faces(tessellator);

    // exit profiler
#ifdef LX_TESSELLATOR_PROFILER_ENABLE
    lx_tessellator_profiler_exit();
#endif

#ifdef LX_DEBUG
    // check mesh
    lx_mesh_check(mesh);
#endif
}


