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
 * @file        active_region.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "active_region.h"
#include "geometry.h"
#include "mesh.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// enable test?
#define LX_ACTIVE_REGION_TEST_ENABLE    (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_bool_t lx_tessellator_active_region_leq(lx_tessellator_active_region_ref_t lregion, lx_tessellator_active_region_ref_t rregion)
{
    // check
    lx_assert(lregion && lregion->edge && rregion && rregion->edge);

    // the origin and destination of the left edge
    lx_mesh_vertex_ref_t ledge_org = lx_mesh_edge_org(lregion->edge);
    lx_mesh_vertex_ref_t ledge_dst = lx_mesh_edge_dst(lregion->edge);

    // the origin and destination of the right edge
    lx_mesh_vertex_ref_t redge_org = lx_mesh_edge_org(rregion->edge);
    lx_mesh_vertex_ref_t redge_dst = lx_mesh_edge_dst(rregion->edge);

    /* two edges must go up
     *
     *  / \     / \
     *   |       |
     */
    lx_assertf(lx_tessellator_edge_go_up(lregion->edge), "%{mesh_edge}", lregion->edge);
    lx_assertf(lx_tessellator_edge_go_up(rregion->edge), "%{mesh_edge}", rregion->edge);

    /*
     *             .
     *     ledge .   .
     *         .       . redge
     *       .           .
     */
    if (ledge_dst == redge_dst)
    {
        /*
         *             .
         *             ..
         *             ..
         *       ledge .. redge
         *             ..
         *             ..
         *             .
         */
        if (ledge_org == redge_org) return 1;
        /*
         *             .
         *     ledge .   .
         *         .       . redge
         *       . --------  .
         *                     .
         *                       .
         *
         */
        else if (lx_tessellator_vertex_in_top_or_hleft_or_eq(ledge_org, redge_org))
            return lx_tessellator_vertex_on_edge_or_left(ledge_org, redge_dst, redge_org);
        /*
         *           .
         *         .   . redge
         * ledge .       .
         *     . --------- .
         *   .
         * .
         *
         */
        else return lx_tessellator_vertex_on_edge_or_right(redge_org, ledge_dst, ledge_org);
    }
    /*
     *      .                  .
     *        .              .
     *    ledge .          . redge
     *            .      .
     *
     * or
     *
     *      .                   .
     *        .               .
     *    ledge .           . redge
     *            .       .
     *              .   .
     *                .
     *
     * or
     *          .           .
     *            .       .
     *        ledge .   . redge
     *                .
     *              .   .
     *            .       .
     *          .           .
     */
    else
    {
        /*
         *  .
         *    .
         *      . ---------------- .
         *        .              .
         *    ledge .          . redge
         *            .      .
         *
         * or
         *
         *  .
         *    .
         *      . ----------------- .
         *        .               .
         *    ledge .           . redge
         *            .       .
         *              .   .
         *                .
         *
         * or
         *      .
         *        .
         *          . --------- .
         *            .       .
         *        ledge .   . redge
         *                .
         *              .   .
         *            .       .
         *          .           .
         */
        if (lx_tessellator_vertex_in_top_or_hleft_or_eq(ledge_dst, redge_dst))
            return lx_tessellator_vertex_on_edge_or_right(redge_dst, ledge_dst, ledge_org);
        /*
         *                             .
         *                           .
         *      . ---------------- .
         *        .              .
         *    ledge .          . redge
         *            .      .
         *
         * or
         *
         *                              .
         *                            .
         *      . ----------------- .
         *        .               .
         *    ledge .           . redge
         *            .       .
         *              .   .
         *                .
         *
         * or
         *                          .
         *                        .
         *          . --------- .
         *            .       .
         *        ledge .   . redge
         *                .
         *              .   .
         *            .       .
         *          .           .
         */
        else return lx_tessellator_vertex_on_edge_or_left(ledge_dst, redge_dst, redge_org);
    }

    // failed
    lx_assert(0);
    return 0;
}
static lx_long_t lx_tessellator_active_region_comp(lx_cpointer_t ldata, lx_cpointer_t rdata)
{
    // lregion <= rregion ? -1 : 1
    return (!lx_tessellator_active_region_leq((lx_tessellator_active_region_ref_t)ldata, (lx_tessellator_active_region_ref_t)rdata) << 1) - 1;
}
#ifdef LX_DEBUG
static lx_char_t const* lx_tessellator_active_region_cstr(lx_element_ref_t element, lx_cpointer_t data, lx_char_t* cstr, lx_size_t maxn)
{
    // check
    lx_tessellator_active_region_ref_t region = (lx_tessellator_active_region_ref_t)data;
    lx_assert_and_check_return_val(region, lx_null);

    // make info
    lx_long_t size = lx_snprintf(cstr, maxn, "%{tess_region}.%{mesh_edge}", region, region->edge);
    if (size >= 0) cstr[size] = '\0';

    // ok?
    return cstr;
}
static lx_long_t lx_tessellator_active_region_printf(lx_cpointer_t object, lx_char_t* cstr, lx_size_t maxn)
{
    // check
    lx_assert_and_check_return_val(object && cstr && maxn, -1);

    // the region
    lx_tessellator_active_region_ref_t region = (lx_tessellator_active_region_ref_t)object;

    // is bounds region?
    if (region->bounds) return lx_snprintf(cstr, maxn, "(e%lu.bounds)", region->edge->id);

    // is fixable edge region?
    if (region->fixedge) return lx_snprintf(cstr, maxn, "(e%lu.fixable)", region->edge->id);

    // make info
    return lx_snprintf( cstr
                    ,   maxn
                    ,   "(e%lu.w%d, winding: %ld, inside: %d)"
                    ,   region->edge->id
                    ,   lx_tessellator_edge_winding(region->edge)
                    ,   region->winding
                    ,   region->inside);
}
#endif
/* insert region in ascending order and save the region position
 *
 * r0 ----> r1 ------> r2 -------> r3 ---> ... ---->
 *                 region_prev
 *                     |----------------->
 *                           insert
 *
 */
static lx_tessellator_active_region_ref_t lx_tessellator_active_regions_insert_done(lx_tessellator_t* tessellator, lx_size_t prev, lx_tessellator_active_region_ref_t region)
{
    // check
    lx_assert(tessellator && tessellator->active_regions && region && region->edge);

    // the edge must go up
    lx_assertf(lx_tessellator_edge_go_up(region->edge), "%{mesh_edge}", region->edge);

    // trace
    lx_trace_d("insert: %{mesh_edge}", region->edge);

    // reverse to find the inserted position
    lx_size_t itor = lx_find_if(tessellator->active_regions, prev, lx_iterator_tail(tessellator->active_regions), lx_predicate_beq, region);

    // trace
    lx_trace_d("insert: find count: %lu", lx_distance(tessellator->active_regions, prev, itor));

    // insert the region to the next position
    itor = lx_list_insert_prev(tessellator->active_regions, itor, region);
    lx_assert(itor != lx_iterator_tail(tessellator->active_regions));

    // get the real region reference
    region = (lx_tessellator_active_region_ref_t)lx_iterator_item(tessellator->active_regions, itor);
    lx_assert(region);

    // save the region position
    region->position = itor;

    // save the region reference to the edge
    lx_tessellator_edge_region_set(region->edge, region);

    // ok
    return region;
}
/* insert region for the bounds in ascending order
 *
 * dst(event)
 * / \
 *  |  region
 *  |
 * org
 */
static lx_void_t lx_tessellator_active_regions_insert_bounds(lx_tessellator_t* tessellator, lx_float_t x, lx_float_t y_org, lx_float_t y_dst)
{
    // check
    lx_assert(tessellator && tessellator->mesh && tessellator->active_regions);

    // init two points of the new edge
    lx_point_t org;
    lx_point_t dst;
    lx_point_make(&org, x, y_org);
    lx_point_make(&dst, x, y_dst);

    // make edge
    lx_mesh_edge_ref_t edge = lx_tessellator_mesh_make_edge(tessellator, &org, &dst);
    lx_assert(edge);

    // update the current sweep event for inserting region in ascending order
    tessellator->event = lx_mesh_edge_dst(edge);

    // make region
    lx_tessellator_active_region_t region;
    region.edge     = edge;
    region.winding  = 0;
    region.inside   = 0;
    region.fixedge  = 0;
    region.bounds   = 1;

    // insert region
    lx_tessellator_active_regions_insert(tessellator, &region);
}
#if LX_ACTIVE_REGION_TEST_ENABLE && defined(LX_DEBUG)
static lx_void_t lx_tessellator_active_regions_test_insert(lx_tessellator_t* tessellator, lx_float_t sweep_xb, lx_float_t sweep_xe, lx_float_t sweep_y)
{
    // check
    lx_assert(tessellator && tessellator->mesh && tessellator->active_regions);

    // init coordinates
    lx_long_t xb = lx_random_range(lx_null, lx_float_to_long(sweep_xb), lx_float_to_long(sweep_xe));
    lx_long_t xe = lx_random_range(lx_null, lx_float_to_long(sweep_xb), lx_float_to_long(sweep_xe));
    lx_long_t yb = lx_random_range(lx_null, 1, 200);
    lx_long_t ye = lx_random_range(lx_null, 1, 200);

    // init two points of the new edge
    lx_point_t org;
    lx_point_t dst;
    lx_point_make(&org, lx_long_to_float(xb), sweep_y + lx_long_to_float(yb));
    lx_point_make(&dst, lx_long_to_float(xe), sweep_y - lx_long_to_float(ye));

    // make edge
    lx_mesh_edge_ref_t edge = lx_tessellator_mesh_make_edge(tessellator, &org, &dst);
    lx_assert(edge);

    // make region
    lx_tessellator_active_region_t region;
    region.edge     = edge;
    region.winding  = 0;
    region.inside   = 0;
    region.bounds   = 1;

    // insert region
    lx_tessellator_active_regions_insert(tessellator, &region);
}
static lx_void_t lx_tessellator_active_regions_test(lx_tessellator_t* tessellator, lx_float_t sweep_xb, lx_float_t sweep_xe, lx_float_t sweep_y)
{
    // check
    lx_assert(tessellator && tessellator->active_regions);

    // make the current sweep event point
    lx_point_t point;
    lx_point_make(&point, lx_avg(sweep_xb, sweep_xe), sweep_y);

    // insert some regions
    __lx_volatile__ lx_size_t count = 20;
    while (count--) lx_tessellator_active_regions_test_insert(tessellator, sweep_xb, sweep_xe, sweep_y);

    // make the codes for drawing sweep line
    lx_printf(  "    lx_canvas_color_set(canvas, LX_COLOR_BLACK);\n");
    lx_printf(  "    lx_canvas_draw_line2i(canvas, %ld, %ld, %ld, %ld);\n"
            ,   lx_float_to_long(sweep_xb)
            ,   lx_float_to_long(sweep_y)
            ,   lx_float_to_long(sweep_xe)
            ,   lx_float_to_long(sweep_y));

    // dump the codes for drawing
    lx_size_t index = 1;
    lx_for_all_if (lx_tessellator_active_region_ref_t, region, tessellator->active_regions, region)
    {
        // the edge
        lx_mesh_edge_ref_t edge = region->edge;

        // the points
        lx_point_ref_t org = lx_tessellator_vertex_point(lx_mesh_edge_org(edge));
        lx_point_ref_t dst = lx_tessellator_vertex_point(lx_mesh_edge_dst(edge));

        // make the codes
        lx_printf(  "    lx_canvas_color_set(canvas, lx_color_from_index(%lu));\n", index++);
        lx_printf(  "    lx_canvas_draw_line2i(canvas, %ld, %ld, %ld, %ld);\n"
                ,   lx_float_to_long(org->x)
                ,   lx_float_to_long(org->y)
                ,   lx_float_to_long(dst->x)
                ,   lx_float_to_long(dst->y));
    }
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bool_t lx_tessellator_active_regions_make(lx_tessellator_t* tessellator, lx_rect_ref_t bounds)
{
    // check
    lx_assert(tessellator && bounds && bounds->w > 0 && bounds->h > 0);

    // init active regions
    if (!tessellator->active_regions)
    {
        // make active region element
        lx_element_t element = lx_element_mem(sizeof(lx_tessellator_active_region_t), lx_null, lx_null);

        // init the comparator
        element.comp = lx_tessellator_active_region_comp;

#ifdef LX_DEBUG
        // init the c-string function for lx_list_dump
        element.cstr = lx_tessellator_active_region_cstr;

        // register printf("%{tess_region}", region);
        static lx_bool_t s_is_registered = lx_false;
        if (!s_is_registered)
        {
            // register it
            lx_printf_object_register("tess_region", lx_tessellator_active_region_printf);

            // ok
            s_is_registered = lx_true;
        }
#endif

        // make active regions
        tessellator->active_regions = lx_list_init(0, element);
    }
    lx_assert_and_check_return_val(tessellator->active_regions, lx_false);

    // clear active regions first
    lx_list_clear(tessellator->active_regions);

    /* insert two regions for the bounds to avoid special cases
     *
     * their coordinates are big enough that they will never be merged with real input features.
     *
     * (xb, yb)
     *   / \         .        .    .     / \
     *    |  region    .     .     .      |  region
     *    |              .  .    .        |
     *                                 (xe, ye)
     */
    lx_float_t xb = bounds->x - LX_ONE;
    lx_float_t yb = bounds->y - LX_ONE;
    lx_float_t xe = bounds->x + bounds->w + LX_ONE;
    lx_float_t ye = bounds->y + bounds->h + LX_ONE;
    lx_tessellator_active_regions_insert_bounds(tessellator, xb, ye, yb);
    lx_tessellator_active_regions_insert_bounds(tessellator, xe, ye, yb);

#if LX_ACTIVE_REGION_TEST_ENABLE && defined(LX_DEBUG)
    // test the active regions
    lx_tessellator_active_regions_test(tessellator, xb, xe, lx_avg(yb, ye));
#endif

    // ok
    return lx_list_size(tessellator->active_regions);
}
lx_tessellator_active_region_ref_t lx_tessellator_active_regions_find(lx_tessellator_t* tessellator, lx_mesh_edge_ref_t edge)
{
    // check
    lx_assert(tessellator && tessellator->active_regions && edge);

    // make a temporary region with the given edge for finding the real region containing it
    lx_tessellator_active_region_t region_temp;
    region_temp.edge = edge;

    // the edge must go up
    lx_assertf(lx_tessellator_edge_go_up(edge), "%{mesh_edge}", edge);

    /* reverse to find the region containing the given edge from the regions
     *
     * region.edge
     * |
     * .                .     .
     * .                .      .
     * .     region1    .       .
     * .                .        . region3
     * .         .      .         .
     * .       .        . region2  .
     * .     . edge     .
     *          |
     *        found                <= find direction
     *
     *
     */
    lx_size_t itor = lx_rfind_all_if(tessellator->active_regions, lx_predicate_le, &region_temp);

    // get the found item
    return (itor != lx_iterator_tail(tessellator->active_regions))? (lx_tessellator_active_region_ref_t)lx_iterator_item(tessellator->active_regions, itor) : lx_null;
}
lx_tessellator_active_region_ref_t lx_tessellator_active_regions_left(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region)
{
    // check
    lx_assert(tessellator && tessellator->active_regions);
    lx_assert(region && region->position != lx_iterator_tail(tessellator->active_regions));

    // get the prev position
    lx_size_t position = lx_iterator_prev(tessellator->active_regions, region->position);

    // no left region?
    lx_check_return_val(position != lx_iterator_tail(tessellator->active_regions), lx_null);

    // ok
    return (lx_tessellator_active_region_ref_t)lx_iterator_item(tessellator->active_regions, position);
}
lx_tessellator_active_region_ref_t lx_tessellator_active_regions_right(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region)
{
    // check
    lx_assert(tessellator && tessellator->active_regions);
    lx_assert(region && region->position != lx_iterator_tail(tessellator->active_regions));

    // get the next position
    lx_size_t position = lx_iterator_next(tessellator->active_regions, region->position);

    // no right region?
    lx_check_return_val(position != lx_iterator_tail(tessellator->active_regions), lx_null);

    // ok
    return (lx_tessellator_active_region_ref_t)lx_iterator_item(tessellator->active_regions, position);
}
lx_bool_t lx_tessellator_active_regions_in_left(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region1, lx_tessellator_active_region_ref_t region2)
{
    // region1 <= region2
    return lx_tessellator_active_region_leq(region1, region2);
}
lx_void_t lx_tessellator_active_regions_remove(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region)
{
    // check
    lx_assert(tessellator && tessellator->active_regions && region && region->edge);
    lx_assert(region->position != lx_iterator_tail(tessellator->active_regions));

    // it can not be a real edge if the left edge need fix, then we will remove it
    lx_assert(!region->fixedge || !lx_tessellator_edge_winding(region->edge));

    // clear the region reference for the edge
    lx_tessellator_edge_region_set(region->edge, lx_null);

    // remove it
    lx_list_remove(tessellator->active_regions, region->position);
}
lx_tessellator_active_region_ref_t lx_tessellator_active_regions_insert(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region)
{
    // check
    lx_assert(tessellator && tessellator->active_regions && region);

    // insert it
    return lx_tessellator_active_regions_insert_done(tessellator, lx_iterator_head(tessellator->active_regions), region);
}
lx_tessellator_active_region_ref_t lx_tessellator_active_regions_insert_after(lx_tessellator_t* tessellator, lx_tessellator_active_region_ref_t region_prev, lx_tessellator_active_region_ref_t region)
{
    // check
    lx_assert(tessellator && tessellator->active_regions && region_prev && region);
    lx_assert(region_prev->position != lx_iterator_tail(tessellator->active_regions));

    // region_prev <= region
    lx_assert(lx_iterator_comp(tessellator->active_regions, region_prev, region) <= 0);

    // insert it
    return lx_tessellator_active_regions_insert_done(tessellator, region_prev->position, region);
}
#ifdef LX_DEBUG
lx_void_t lx_tessellator_active_regions_check(lx_tessellator_t* tessellator)
{
    // check
    lx_assert(tessellator && tessellator->active_regions);

    // done
    lx_tessellator_active_region_ref_t region_prev = lx_null;
    lx_for_all_if (lx_tessellator_active_region_ref_t, region, tessellator->active_regions, region)
    {
        // check order
        if (region_prev)
        {
            // the order is error?
            if (lx_iterator_comp(tessellator->active_regions, region_prev, region) > 0)
            {
                // trace
                lx_trace_i("the order of the active regions is error with event: %{mesh_vertex}", tessellator->event);

                // dump it
#           ifdef LX_DEBUG
                lx_list_dump(tessellator->active_regions);
#           endif

                // trace
                lx_trace_i("%{mesh_edge}", region_prev->edge);
                lx_trace_i("<?=");
                lx_trace_i("%{mesh_edge}", region->edge);

                // abort it
                lx_assert(0);
            }
        }

        // the edge must go up
        lx_assertf(lx_tessellator_edge_go_up(region->edge), "%{mesh_edge}", region->edge);

        // update the previous region
        region_prev = region;
    }
}
#endif

