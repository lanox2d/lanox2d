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
 * @file        geometry.h
 */
#ifndef LX_CORE_TESS_TESSELLATOR_GEOMETRY_H
#define LX_CORE_TESS_TESSELLATOR_GEOMETRY_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../geometry.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// vertex: a == b?
#define lx_tessellator_vertex_eq(a, b)            lx_point_eq(lx_tessellator_vertex_point(a), lx_tessellator_vertex_point(b))

/* vertex: a <= b?
 *
 * sweep direction: horizontal
 *
 * v0 -------> v1-----
 * ---> v2 -------> v3
 * v4 ----------------
 * --------> v5 ------
 *
 * v0 <= v1 <= v2 <= v3 <= v4 <= v5
 */
#define lx_tessellator_vertex_leq(a, b)  \
    (   lx_tessellator_vertex_point(a)->y < lx_tessellator_vertex_point(b)->y \
    ||  (   lx_tessellator_vertex_point(a)->y == lx_tessellator_vertex_point(b)->y \
        &&  lx_tessellator_vertex_point(a)->x <= lx_tessellator_vertex_point(b)->x))

/* vertex: a < b?
 *
 * sweep direction: horizontal
 *
 * v0 -------> v1-----
 * ---> v2 -------> v3
 * v4 ----------------
 * --------> v5 ------
 *
 * v0 < v1 < v2 < v3 < v4 < v5
 */
#define lx_tessellator_vertex_le(a, b)  \
    (   lx_tessellator_vertex_point(a)->y < lx_tessellator_vertex_point(b)->y \
    ||  (   lx_tessellator_vertex_point(a)->y == lx_tessellator_vertex_point(b)->y \
        &&  lx_tessellator_vertex_point(a)->x < lx_tessellator_vertex_point(b)->x))

// a is in b's top?
#define lx_tessellator_vertex_in_top(a, b)                  lx_point_in_top(lx_tessellator_vertex_point(a), lx_tessellator_vertex_point(b))

// a is in b's top or horizontal?
#define lx_tessellator_vertex_in_top_or_horizontal(a, b)    lx_point_in_top_or_horizontal(lx_tessellator_vertex_point(a), lx_tessellator_vertex_point(b))

/* a is in b's top or b's horizontal left or a == b?
 *
 * a
 *   .
 *     .             or        a . . . . . . b
 *       .
 *         .
 *           b
 */
#define lx_tessellator_vertex_in_top_or_hleft(a, b)         lx_tessellator_vertex_le(a, b)

/* a is in b's top or b's horizontal left or a == b?
 *
 * a
 *   .
 *     .             or        a . . . . . . b           or         .(a/b)
 *       .
 *         .
 *           b
 */
#define lx_tessellator_vertex_in_top_or_hleft_or_eq(a, b)   lx_tessellator_vertex_leq(a, b)

// v is on edge(u, l)?
#define lx_tessellator_vertex_on_edge(v, u, l)              (!lx_point_to_segment_position_h(lx_tessellator_vertex_point(v), lx_tessellator_vertex_point(u), lx_tessellator_vertex_point(l)))

// the three vertices are counter-clockwise?
#define lx_tessellator_vertex_is_ccw(a, b, c)               lx_points_is_ccw(lx_tessellator_vertex_point(a), lx_tessellator_vertex_point(b), lx_tessellator_vertex_point(c))

// v is in edge(u, l)'s left?
#define lx_tessellator_vertex_in_edge_left(v, u, l)         lx_point_in_segment_left(lx_tessellator_vertex_point(v), lx_tessellator_vertex_point(u), lx_tessellator_vertex_point(l))

// v is on edge(u, l) or it's left?
#define lx_tessellator_vertex_on_edge_or_left(v, u, l)      lx_point_on_segment_or_left(lx_tessellator_vertex_point(v), lx_tessellator_vertex_point(u), lx_tessellator_vertex_point(l))

// v is in edge(u, l)'s right?
#define lx_tessellator_vertex_in_edge_right(v, u, l)        lx_point_in_segment_right(lx_tessellator_vertex_point(v), lx_tessellator_vertex_point(u), lx_tessellator_vertex_point(l))

// v is on edge(u, l) or it's right?
#define lx_tessellator_vertex_on_edge_or_right(v, u, l)     lx_point_on_segment_or_right(lx_tessellator_vertex_point(v), lx_tessellator_vertex_point(u), lx_tessellator_vertex_point(l))

// v is in edge(u, l)'s top?
#define lx_tessellator_vertex_in_edge_top(v, l, r)          lx_point_in_segment_top(lx_tessellator_vertex_point(v), lx_tessellator_vertex_point(l), lx_tessellator_vertex_point(r))

// v is on edge(u, l) or it's top?
#define lx_tessellator_vertex_on_edge_or_top(v, l, r)       lx_point_on_segment_or_top(lx_tessellator_vertex_point(v), lx_tessellator_vertex_point(l), lx_tessellator_vertex_point(r))

// v is in edge(u, l)'s bottom?
#define lx_tessellator_vertex_in_edge_bottom(v, l, r)       lx_point_in_segment_bottom(lx_tessellator_vertex_point(v), lx_tessellator_vertex_point(l), lx_tessellator_vertex_point(r))

// v is on edge(u, l) or it's bottom?
#define lx_tessellator_vertex_on_edge_or_bottom(v, l, r)    lx_point_on_segment_or_bottom(lx_tessellator_vertex_point(v), lx_tessellator_vertex_point(l), lx_tessellator_vertex_point(r))

// the horizontal distance of v to edge(u, l)
#define lx_tessellator_vertex_to_edge_distance_h(v, u, l)   lx_point_to_segment_distance_h(lx_tessellator_vertex_point(v), lx_tessellator_vertex_point(u), lx_tessellator_vertex_point(l))

/* the edge goes up?
 *  __                   __
 * | .                   . |
 *     .               .
 *       .           .            or  <------------------
 *         .       .
 *           .   .
 *
 */
#define lx_tessellator_edge_go_up(edge)                     lx_tessellator_vertex_in_top_or_hleft(lx_mesh_edge_dst(edge), lx_mesh_edge_org(edge))

/* the edge goes down?
 *
 *           .   .
 *         .       .
 *       .           .            or  ------------------>
 *     .               .
 * | .                   .|
 *  --                  --
 */
#define lx_tessellator_edge_go_down(edge)                   lx_tessellator_vertex_in_top_or_hleft(lx_mesh_edge_org(edge), lx_mesh_edge_dst(edge))

/* calculate the intersection of two edges
 *
 *    org2
 *       .   intersection
 *         .   |
 *           . |
 * . . . . . . . . . . . . . . . . dst1
 * org1          .
 *                 .
 *                   .
 *                  dst2
 */
#define lx_tessellator_edge_intersection(org1, dst1, org2, dst2, result) \
            lx_segment_intersection(    lx_tessellator_vertex_point(org1) \
                                    ,   lx_tessellator_vertex_point(dst1) \
                                    ,   lx_tessellator_vertex_point(org2) \
                                    ,   lx_tessellator_vertex_point(dst2) \
                                    ,   lx_tessellator_vertex_point(result))

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


