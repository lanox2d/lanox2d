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
 * @file        polygon_raster.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "polygon_raster.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#ifdef LX_CONFIG_SMALL
#   define LX_POLYGON_RASTER_EDGES_GROW     (1024)
#else
#   define LX_POLYGON_RASTER_EDGES_GROW     (2048)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the polygon raster edge type
typedef struct lx_polygon_raster_edge_t_ {
    /* the winding for rule
     *
     *   . <= -1
     *     .
     *       .
     *         .
     *            .
     *              .
     *            => 1
     *
     * 1:  top => bottom
     * -1: bottom => top
     */
    lx_int8_t       winding     : 2;

    // the index of next edge at the edge pool
    lx_uint16_t     next;

    // the bottom y-coordinate
    lx_int16_t      y_bottom;

    // the x-coordinate of the active edge
    lx_fixed_t      x;

    // the slope of the edge: dx / dy
    lx_fixed_t      slope;

}lx_polygon_raster_edge_t;

/* the polygon raster type
 *
 * 1. make the edge table
 *     (y)
 *      0 ----------------> .
 *      1                 .   .
 *      2               .       . e2
 *      3          e1 .           .
 *      4 ------------------------> .
 *      5         .               .
 *      6       .               .
 *      7 --> .               . e3
 *      8       .           .
 *      9      e4 .       .
 *      10          .   .
 *      11            .
 *
 * edge_table[0]: e1 e2
 * edge_table[4]: e3
 * edge_table[7]: e4
 *
 * 2. scan the edge table
 *     (y)
 *      0                   .
 *      1                 . - .
 *      2               . ----- . e2
 *      3          e1 . --------- .
 *      4           .               .
 *      5         .               .
 *      6       .               .
 *      7     .               . e3
 *      8       .           .
 *      9      e4 .       .
 *      10          .   .
 *      11            .
 *
 * active_edges: e1 e2
 *
 * 3. scan the edge table
 *     (y)
 *      0                   .
 *      1                 .   .
 *      2               .       . e2
 *      3          e1 .           .
 *      4           . ------------- .
 *      5         . ------------- .
 *      6       . ------------- .
 *      7     .               . e3
 *      8       .           .
 *      9      e4 .       .
 *      10          .   .
 *      11            .
 *
 * active_edges: e1 e3
 *
 * 4. scan the edge table
 *     (y)
 *      0                   .
 *      1                 .   .
 *      2               .       . e2
 *      3          e1 .           .
 *      4           .               .
 *      5         .               .
 *      6       .               .
 *      7     . ------------- . e3
 *      8       . --------- .
 *      9      e4 . ----- .
 *      10          . - .
 *      11            .
 *
 * active_edges: e4 e3
 *
 * active_edges: be sorted by x in ascending
 *
 */
typedef struct lx_polygon_raster_t_ {

    // the edge pool, tail: 0, index: > 0
    lx_polygon_raster_edge_t*    edge_pool;

    // the edge pool size
    lx_size_t                   edge_pool_size;

    // the edge pool maxn
    lx_size_t                   edge_pool_maxn;

    // the edge table
    lx_uint16_t*                edge_table;

    // the edge table base for the y-coordinate
    lx_long_t                   edge_table_base;

    // the edge table maxn
    lx_size_t                   edge_table_maxn;

    // the active edges
    lx_uint16_t                 active_edges;

    // the top of the polygon bounds
    lx_long_t                   top;

    // the bottom of the polygon bounds
    lx_long_t                   bottom;

}lx_polygon_raster_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_bool_t lx_polygon_raster_edge_pool_init(lx_polygon_raster_t* raster) {
    lx_assert(raster);
    if (!raster->edge_pool) {
        raster->edge_pool = lx_nalloc_type(LX_POLYGON_RASTER_EDGES_GROW, lx_polygon_raster_edge_t);
    }
    lx_assert_and_check_return_val(raster->edge_pool, lx_false);
    raster->edge_pool_size = 0;
    return lx_true;
}

static lx_void_t lx_polygon_raster_edge_pool_exit(lx_polygon_raster_t* raster) {
    lx_assert(raster);
    if (raster->edge_pool) {
        lx_free(raster->edge_pool);
        raster->edge_pool = lx_null;
    }
}

static lx_uint16_t lx_polygon_raster_edge_pool_aloc(lx_polygon_raster_t* raster) {
    lx_assert(raster && raster->edge_pool);

    // the new index
    lx_size_t index = ++raster->edge_pool_size;
    lx_assert(index < LX_MAXU16);

    // grow the edge pool
    if (index >= raster->edge_pool_maxn) {
        raster->edge_pool_maxn = index + LX_POLYGON_RASTER_EDGES_GROW;
        raster->edge_pool = lx_ralloc_type(raster->edge_pool, raster->edge_pool_maxn, lx_polygon_raster_edge_t);
        lx_assert_and_check_return_val(raster->edge_pool, 0);
    }

    // make a new edge from the edge pool
    return (lx_uint16_t)index;
}

static lx_bool_t lx_polygon_raster_edge_table_init(lx_polygon_raster_t* raster, lx_long_t table_base, lx_size_t table_size) {
    lx_assert(raster && table_size);

    if (!raster->edge_table) {
        raster->edge_table_maxn = table_size;
        raster->edge_table = lx_nalloc_type(raster->edge_table_maxn, lx_uint16_t);
    } else if (table_size > raster->edge_table_maxn) {
        raster->edge_table_maxn = table_size;
        raster->edge_table = lx_ralloc_type(raster->edge_table, raster->edge_table_maxn, lx_uint16_t);
    }
    lx_assert_and_check_return_val(raster->edge_table && table_size <= LX_MAXU16, lx_false);

    lx_memset(raster->edge_table, 0, table_size * sizeof(lx_uint16_t));
    raster->edge_table_base = table_base;
    return lx_true;
}

static lx_void_t lx_polygon_raster_edge_table_exit(lx_polygon_raster_t* raster) {
    lx_assert(raster);
    if (raster->edge_table) {
        lx_free(raster->edge_table);
        raster->edge_table = lx_null;
    }
}

static lx_bool_t lx_polygon_raster_edge_table_make(lx_polygon_raster_t* raster, lx_polygon_ref_t polygon, lx_rect_ref_t bounds) {
    lx_check_return_val(!lx_near0(bounds->w) && !lx_near0(bounds->h), lx_false);

    // init the edge pool
    if (!lx_polygon_raster_edge_pool_init(raster)) {
        return lx_false;
    }

    // init the edge table
    if (!lx_polygon_raster_edge_table_init(raster, lx_round(bounds->y), lx_round(bounds->h) + 1)) {
        return lx_false;
    }

    // make the edge table
    lx_point_t          pb;
    lx_point_t          pe;
    lx_bool_t           first       = lx_true;
    lx_long_t           top         = 0;
    lx_long_t           bottom      = 0;
    lx_uint16_t         index       = 0;
    lx_long_t           table_index = 0;
    lx_point_ref_t      points      = polygon->points;
    lx_uint16_t*        counts      = polygon->counts;
    lx_uint16_t         count       = *counts++;
    lx_uint16_t*        edge_table  = raster->edge_table;
    while (index < count) {
        pe = *points++;
        // exists edge?
        if (index) {
            // get the integer y-coordinates
            lx_long_t iyb = lx_round(pb.y);
            lx_long_t iye = lx_round(pe.y);

            // not horizontal edge?
            if (iyb != iye) {

                // get the fixed-point coordinates
                lx_fixed6_t xb = lx_float_to_fixed6(pb.x);
                lx_fixed6_t yb = lx_float_to_fixed6(pb.y);
                lx_fixed6_t xe = lx_float_to_fixed6(pe.x);
                lx_fixed6_t ye = lx_float_to_fixed6(pe.y);

                // compute the delta coordinates
                lx_fixed6_t dx = xe - xb;
                lx_fixed6_t dy = ye - yb;

                // make a new edge from the edge pool
                lx_uint16_t edge_index = lx_polygon_raster_edge_pool_aloc(raster);
                lx_assert(edge_index);

                // the edge
                lx_polygon_raster_edge_t* edge = raster->edge_pool + edge_index;

                // init the winding
                edge->winding = 1;

                // sort the points of the edge by the y-coordinate
                if (yb > ye) {
                    // reverse the edge points
                    lx_swap(lx_fixed6_t, xb, xe);
                    lx_swap(lx_fixed6_t, yb, ye);
                    lx_swap(lx_long_t, iyb, iye);

                    // reverse the winding
                    edge->winding = -1;
                }

                // compute the accurate bounds of the y-coordinate
                if (first) {
                    top     = iyb;
                    bottom  = iye;
                    first   = lx_false;
                } else {
                    if (iyb < top)    top = iyb;
                    if (iye > bottom) bottom = iye;
                }

                // check
                lx_assert(iyb < iye);

                // compute the slope
                edge->slope = lx_fixed6_div(dx, dy);

                /* compute the more accurate start x-coordinate
                 *
                 * xb + (iyb - yb + 0.5) * dx / dy
                 * => xb + ((0.5 - yb) % 1) * dx / dy
                 */
                edge->x = lx_fixed6_to_fixed(xb) + ((edge->slope * ((LX_FIXED6_HALF - yb) & 63)) >> 6);

                // init bottom y-coordinate
                edge->y_bottom = (lx_int16_t)(iye - 1);
                lx_assert(iye - 1 > LX_MINS16 && iye - 1 <= LX_MAXS16);

                // the table index
                table_index = iyb - raster->edge_table_base;
                lx_assert(table_index >= 0 && table_index < raster->edge_table_maxn);

                /* insert edge to the head of the edge table
                 *
                 * table[index]: => edge => edge => .. => 0
                 *              |
                 *            insert
                 */
                edge->next = edge_table[table_index];
                edge_table[table_index] = edge_index;
            }
        }

        // save the previous point
        pb = pe;

        // next point
        index++;

        // next polygon
        if (index == count) {
            count = *counts++;
            index = 0;
        }
    }

    // update top and bottom of the polygon
    raster->top     = top;
    raster->bottom  = bottom;
    return lx_true;
}

static lx_void_t lx_polygon_raster_active_scan_line_convex(lx_polygon_raster_t* raster, lx_long_t y, lx_polygon_raster_cb_t callback, lx_cpointer_t udata) {
    lx_assert(raster && raster->edge_pool && callback);

    // the edge index
    lx_uint16_t index = raster->active_edges;
    lx_check_return(index);

    // the edge
    lx_polygon_raster_edge_t* edge = raster->edge_pool + index;

    // the next edge index
    lx_uint16_t index_next = edge->next;
    lx_check_return(index_next);

    // the next edge
    lx_polygon_raster_edge_t* edge_next = raster->edge_pool + index_next;

    // check
    lx_assert(edge->x < edge_next->x || lx_fixed_abs(edge->x - edge_next->x) <= LX_FIXED_HALF);

    // trace
    lx_trace_d("y: %ld, %{fixed} => %{fixed}", y, &edge->x, &edge_next->x);

    // init the end y-coordinate for the only one line
    lx_long_t ye = y + 1;

    /* scan rect region? may be faster
     *
     * |    |
     * |    |
     * |    |
     */
    if (lx_fixed_abs(edge->slope) <= LX_FIXED_NEAR0 && lx_fixed_abs(edge_next->slope) <= LX_FIXED_NEAR0) {
        // get the min and max edge for the y-bottom
        lx_polygon_raster_edge_t* edge_min  = edge;
        lx_polygon_raster_edge_t* edge_max  = edge_next;
        lx_uint16_t               index_max = index_next;
        if (edge_min->y_bottom > edge_max->y_bottom) {
            edge_min  = edge_next;
            edge_max  = edge;
            index_max = index;
        }

        // compute the ye
        ye = edge_min->y_bottom + 1;

        // clear the active edges, only two edges
        raster->active_edges = 0;

        // re-insert the max edge to the edge table using the new top-y coordinate
        if (ye < edge_max->y_bottom) {
            lx_assert(ye >= raster->edge_table_base && ye - raster->edge_table_base < raster->edge_table_maxn);

            /* re-insert to the edge table using the new top-y coordinate
             *
             * table[index]: => edge => edge => .. => 0
             *              |
             *            insert
             */
            edge_max->next = raster->edge_table[ye - raster->edge_table_base];
            raster->edge_table[ye - raster->edge_table_base] = index_max;
        }
    }

    // do callback
    callback(lx_fixed_round(edge->x), lx_fixed_round(edge_next->x), y, ye, udata);
}

static lx_void_t lx_polygon_raster_active_scan_line_concave(lx_polygon_raster_t* raster, lx_long_t y, lx_size_t rule, lx_polygon_raster_cb_t callback, lx_cpointer_t udata) {
    lx_assert(raster && raster->edge_pool && callback);

    lx_long_t                 done            = 0;
    lx_long_t                 winding         = 0;
    lx_uint16_t               index           = raster->active_edges;
    lx_uint16_t               index_next      = 0;
    lx_polygon_raster_edge_t* edge            = lx_null;
    lx_polygon_raster_edge_t* edge_next       = lx_null;
    lx_polygon_raster_edge_t* edge_cache      = lx_null;
    lx_polygon_raster_edge_t* edge_cache_next = lx_null;
    lx_polygon_raster_edge_t* edge_pool       = raster->edge_pool;
    while (index) {
        // the edge
        edge = edge_pool + index;

        /* compute the winding
         *
         *    /\
         *    |            |
         *    |-1          | +1
         *    |            |
         *    |            |
         *                \/
         */
        winding += edge->winding;

        // the next edge index
        index_next = edge->next;
        lx_check_break(index_next);

        // the next edge
        edge_next = edge_pool + index_next;

        // check
        lx_assert(edge->x <= edge_next->x);

        // compute the rule
        switch (rule) {
            case LX_POLYGON_RASTER_RULE_ODD: {
                /* the odd rule
                 *
                 *    ------------------                 ------------------
                 *  /|\                 |               ||||||||||||||||||||
                 *   |     --------     |               ||||||||||||||||||||
                 *   |   /|\       |    |               ||||||        ||||||
                 * 0 | -1 |   0    | -1 | 0     =>      ||||||        ||||||
                 *   |    |       \|/   |               ||||||        ||||||
                 *   |     --------     |               ||||||||||||||||||||
                 *   |                 \|/              ||||||||||||||||||||
                 *    ------------------                 ------------------
                 */
                done = winding & 1;
                break;
            }
            case LX_POLYGON_RASTER_RULE_NONZERO: {
                /* the non-zero rule
                 *
                 *    ------------------                 ------------------
                 *  /|\                 |               ||||||||||||||||||||
                 *   |     --------     |               ||||||||||||||||||||
                 *   |   /|\       |    |               ||||||||||||||||||||
                 * 0 | -1 |   -2   | -1 | 0             ||||||||||||||||||||
                 *   |    |       \|/   |               ||||||||||||||||||||
                 *   |     --------     |               ||||||||||||||||||||
                 *   |                 \|/              ||||||||||||||||||||
                 *    ------------------                 ------------------
                 */
                done = winding;
                break;
            }
            default: {
                done = 0;
                lx_trace_e("unknown rule: %lu", rule);
            }
            break;
        }

        // trace
        lx_trace_d("y: %ld, winding: %ld, %{fixed} => %{fixed}", y, winding, &edge->x, &edge_next->x);

#if 0
        // do it for winding?
        if (done) {
            callback(lx_fixed_round(edge->x), lx_fixed_round(edge_next->x), y, y + 1, udata);
        }
#else
        // cache the conjoint edges and done them together
        if (done) {

            // no edge cache?
            if (!edge_cache && !edge_cache_next) {
                // init edge cache
                edge_cache = edge;
                edge_cache_next = edge_next;
            } else if (edge_cache_next && lx_fixed_round(edge_cache_next->x) == lx_fixed_round(edge->x)) { // is conjoint? merge it
                // merge the edges to the edge cache
                edge_cache_next = edge_next;
            } else {
                // check
                lx_assert(edge_cache && edge_cache_next);

                // do callback with edge cache
                callback(lx_fixed_round(edge_cache->x), lx_fixed_round(edge_cache_next->x), y, y + 1, udata);

                // update edge cache
                edge_cache = edge;
                edge_cache_next = edge_next;
            }
        }
#endif

        // the next edge index
        index = index_next;
    }

    // do callback with the left edge cache
    if (edge_cache && edge_cache_next) {
        callback(lx_fixed_round(edge_cache->x), lx_fixed_round(edge_cache_next->x), y, y + 1, udata);
    }
}

static lx_void_t lx_polygon_raster_active_scan_next(lx_polygon_raster_t* raster, lx_long_t y, lx_size_t* porder) {
    lx_assert(raster && raster->edge_pool && raster->edge_table && y <= raster->bottom);

    lx_size_t                 first = 1;
    lx_size_t                 order = 1;
    lx_fixed_t                x_prev = 0;
    lx_uint16_t               index_prev = 0;
    lx_uint16_t               index = raster->active_edges;
    lx_polygon_raster_edge_t* edge = lx_null;
    lx_polygon_raster_edge_t* edge_prev = lx_null;
    lx_polygon_raster_edge_t* edge_pool = raster->edge_pool;
    lx_uint16_t               active_edges = raster->active_edges;
    while (index) {
        // the edge
        edge = edge_pool + index;

        /* remove edge from the active edges if (y >= edge->y_bottom)
         *
         *             .
         *           .  .
         *         .     .
         *       .        .  <- y_bottom: end and no next y for this edge, so remove it
         *     .           . <- the start y of the next edge
         *       .        .
         *          .   .
         *            .      <- bottom
         */
        if (edge->y_bottom < y + 1) {
            // the next edge index
            index = edge->next;

            // remove this edge from head
            if (!index_prev) {
                active_edges = index;
            } else {
                // the previous edge
                edge_prev = edge_pool + index_prev;

                // remove this edge from the body
                edge_prev->next = index;
            }

            // continue
            continue;
        }

        // update the x-coordinate
        edge->x += edge->slope;

        // is order?
        if (porder) {
            if (first) {
                first = 0;
            } else if (order && edge->x < x_prev) {
                order = 0;
            }
        }

        // update the previous x-coordinate
        x_prev = edge->x;

        // update the previous edge index
        index_prev = index;

        // update the edge index
        index = edge->next;
    }

    // save order
    if (porder) *porder = order;

    // update the active edges
    raster->active_edges = active_edges;
}

static lx_void_t lx_polygon_raster_active_append(lx_polygon_raster_t* raster, lx_uint16_t index) {
    lx_assert(raster && raster->edge_pool);

    lx_uint16_t               next = 0;
    lx_polygon_raster_edge_t* edge = lx_null;
    lx_polygon_raster_edge_t* edge_pool = raster->edge_pool;
    lx_uint16_t               active_edges = raster->active_edges;
    while (index) {

        // save the next edge index
        edge = edge_pool + index;
        next = edge->next;

        // insert the edge to the head of the active edges
        edge->next = active_edges;
        active_edges = index;

        // the next edge index
        index = next;
    }

    // update the active edges
    raster->active_edges = active_edges;
}

static lx_void_t lx_polygon_raster_active_sorted_insert(lx_polygon_raster_t* raster, lx_uint16_t edge_index) {
    lx_assert(raster && raster->edge_pool && edge_index);

    // the edge pool
    lx_polygon_raster_edge_t* edge_pool = raster->edge_pool;

    // the edge
    lx_polygon_raster_edge_t* edge = edge_pool + edge_index;

    // insert edge to the active edges by x in ascending
    edge->next = 0;
    if (!raster->active_edges) {
        raster->active_edges = edge_index;
    } else {
        // find an inserted position
        lx_polygon_raster_edge_t* edge_prev    = lx_null;
        lx_polygon_raster_edge_t* edge_active  = lx_null;
        lx_uint16_t               index_active = raster->active_edges;
        while (index_active) {

            // the active edge
            edge_active = edge_pool + index_active;
            lx_assert(edge_index != index_active);

            /* is this?
             *
             * x: 1 2 3     5 6
             *               |
             *             4 or 5
             */
            if (edge->x <= edge_active->x) {
                /* same vertex?
                 *
                 *
                 * x: 1 2 3     5 6
                 *               |   .
                 *               5    .
                 *             .       .
                 *           .          .
                 *         .          active_edge
                 *       .
                 *     edge
                 *
                 * x: 1 2 3   5         6
                 *                 .    |
                 *                  .   5
                 *                   .    .
                 *                    .     .
                 *          active_edge       .
                 *                              .
                 *                                .
                 *                                  .
                 *                                   edge
                 *
                 *  x: 1 2 3   5         6
                 *                 .    |
                 *                .     5
                 *              .    .
                 *            .     .
                 *  active_edge    .
                 *                .
                 *               .
                 *              .
                 *             edge
                 *
                 *
                 * x: 1 2 3     5 6
                 *               |   .
                 *               5      .
                 *                 .       .
                 *                   .       active_edge
                 *                     .
                 *                       .
                 *                         .
                 *                           .
                 *                             .
                 *                               .
                 *                                 .
                 *                                 edge
                 */
                if (edge->x == edge_active->x) {
                    /* the edge is at the left-hand of the active edge?
                     *
                     * x: 1 2 3     5 6    <- active_edges
                     *               |   .
                     *               5    .
                     *             .       .
                     *           .          .
                     *         .        active_edge
                     *       .
                     *     edge
                     *
                     * if (edge->dx / edge->dy < active->dx / active->dy)?
                     */
                    if (edge->slope < edge_active->slope) {
                        break;
                    }

                } else {
                    break;
                }
            }

            // the previous active edge
            edge_prev = edge_active;

            // the next active edge index
            index_active = edge_prev->next;
        }

        // insert edge to the active edges: edge_prev -> edge -> edge_active
        if (!edge_prev) {
            // insert to the head
            edge->next          = raster->active_edges;
            raster->active_edges  = edge_index;
        } else {
            // insert to the body
            edge->next      = index_active;
            edge_prev->next = edge_index;
        }
    }
}

static lx_void_t lx_polygon_raster_active_sorted_append(lx_polygon_raster_t* raster, lx_uint16_t edge_index) {
    lx_assert(raster && raster->edge_pool);

    lx_uint16_t               index_next = 0;
    lx_polygon_raster_edge_t* edge = lx_null;
    lx_polygon_raster_edge_t* edge_pool = raster->edge_pool;
    while (edge_index) {

        // save the next edge index
        edge = edge_pool + edge_index;
        index_next = edge->next;

        // insert the edge to the active edges
        lx_polygon_raster_active_sorted_insert(raster, edge_index);

        // the next edge index
        edge_index = index_next;
    }
}

static lx_void_t lx_polygon_raster_active_sort(lx_polygon_raster_t* raster) {
    lx_assert(raster && raster->edge_pool);

    lx_uint16_t               index       = raster->active_edges;
    lx_uint16_t               index_next  = 0;
    lx_polygon_raster_edge_t* edge        = lx_null;
    lx_polygon_raster_edge_t* edge_next   = lx_null;
    lx_polygon_raster_edge_t  edge_tmp;
    lx_polygon_raster_edge_t* edge_pool   = raster->edge_pool;
    while (index) {

        // the next edge index
        edge = edge_pool + index;
        index_next = edge->next;
        while (index_next) {

            // the next edge
            edge_next = edge_pool + index_next;

            // need sort? swap them
            if (edge->x > edge_next->x || (edge->x == edge_next->x && edge->slope > edge_next->slope)) {

                // swap the edge
                edge_tmp = *edge;
                *edge = *edge_next;

                // restore the next index
                edge->next = edge_tmp.next;
                edge_tmp.next = edge_next->next;

                // swap the next edge
                *edge_next = edge_tmp;
            }
            index_next = edge_next->next;
        }
        index = edge->next;
    }
}

static lx_void_t lx_polygon_raster_make_convex(lx_polygon_raster_t* raster, lx_polygon_ref_t polygon, lx_rect_ref_t bounds, lx_polygon_raster_cb_t callback, lx_cpointer_t udata) {
    lx_assert(raster && polygon && polygon->convex && bounds);

    // init the active edges
    raster->active_edges = 0;

    // make the edge table
    if (!lx_polygon_raster_edge_table_make(raster, polygon, bounds)) {
        return ;
    }

    // do scan
    lx_long_t    y;
    lx_long_t    top         = raster->top;
    lx_long_t    bottom      = raster->bottom;
    lx_long_t    base        = raster->edge_table_base;
    lx_uint16_t* edge_table  = raster->edge_table;
    for (y = top; y < bottom; y++) {

        // append edges to the sorted active edges by x in ascending
        lx_polygon_raster_active_sorted_append(raster, edge_table[y - base]);

        // scan line from the active edges
        lx_polygon_raster_active_scan_line_convex(raster, y, callback, udata);

        // end?
        lx_check_break(y < bottom - 1);

        // scan the next line from the active edges
        lx_polygon_raster_active_scan_next(raster, y, lx_null);
    }
}

static lx_void_t lx_polygon_raster_make_concave(lx_polygon_raster_t* raster, lx_polygon_ref_t polygon, lx_rect_ref_t bounds, lx_size_t rule, lx_polygon_raster_cb_t callback, lx_cpointer_t udata) {
    lx_assert(raster && polygon && !polygon->convex && bounds);

    // init the active edges
    raster->active_edges = 0;

    // make the edge table
    if (!lx_polygon_raster_edge_table_make(raster, polygon, bounds)) {
        return ;
    }

    // do scan
    lx_long_t       y;
    lx_size_t       order       = 1;
    lx_long_t       top         = raster->top;
    lx_long_t       bottom      = raster->bottom;
    lx_long_t       base        = raster->edge_table_base;
    lx_uint16_t*    edge_table  = raster->edge_table;
    for (y = top; y < bottom; y++) {
        // order? append edges to the sorted active edges by x in ascending
        if (order) {
            lx_polygon_raster_active_sorted_append(raster, edge_table[y - base]);
        } else {
            // append edges to the active edges from the edge table
            lx_polygon_raster_active_append(raster, edge_table[y - base]);

            // sort by x in ascending at the active edges
            lx_polygon_raster_active_sort(raster);
        }

        // scan line from the active edges
        lx_polygon_raster_active_scan_line_concave(raster, y, rule, callback, udata);

        // end?
        lx_check_break(y < bottom - 1);

        // scan the next line from the active edges
        lx_polygon_raster_active_scan_next(raster, y, &order);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_polygon_raster_ref_t lx_polygon_raster_init() {
    return (lx_polygon_raster_ref_t)lx_malloc0_type(lx_polygon_raster_t);
}

lx_void_t lx_polygon_raster_exit(lx_polygon_raster_ref_t self) {
    lx_polygon_raster_t* raster = (lx_polygon_raster_t*)self;
    if (raster) {
        lx_polygon_raster_edge_table_exit(raster);
        lx_polygon_raster_edge_pool_exit(raster);
        lx_free(raster);
    }
}

lx_void_t lx_polygon_raster_make(lx_polygon_raster_ref_t self, lx_polygon_ref_t polygon, lx_rect_ref_t bounds, lx_size_t rule, lx_polygon_raster_cb_t callback, lx_cpointer_t udata) {
    lx_polygon_raster_t* raster = (lx_polygon_raster_t*)self;
    lx_assert_and_check_return(raster && polygon && polygon->points && polygon->counts && bounds && callback);

    // is convex polygon for each contour?
    if (polygon->convex) {
        lx_size_t       index               = 0;
        lx_point_ref_t  points              = polygon->points;
        lx_uint16_t*    counts              = polygon->counts;
        lx_uint16_t     contour_counts[2]   = {0, 0};
        lx_polygon_t    contour             = {lx_null, contour_counts, lx_true};
        while ((contour_counts[0] = *counts++)) {
            // init the polygon for this contour
            contour.points = points + index;

            // do raster for the convex contour, will be faster
            lx_polygon_raster_make_convex(raster, &contour, bounds, callback, udata);

            // update the contour index
            index += contour_counts[0];
        }
    } else {
        // do raster for the concave polygon
        lx_polygon_raster_make_concave(raster, polygon, bounds, rule, callback, udata);
    }
}

