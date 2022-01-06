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
 * @file        tessellator.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tessellator.h"
#include "tessellator/mesh.h"
#include "tessellator/convex.h"
#include "tessellator/geometry.h"
#include "tessellator/monotone.h"
#include "tessellator/triangulation.h"
#include "mesh/face_list.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the output points grow
#ifdef LX_CONFIG_SMALL
#   define LX_TESSELLATOR_POLYGON_POINTS_GROW                          (32)
#else
#   define LX_TESSELLATOR_POLYGON_POINTS_GROW                          (64)
#endif

// the output counts grow
#ifdef LX_CONFIG_SMALL
#   define LX_TESSELLATOR_POLYGON_COUNTS_GROW                          (8)
#else
#   define LX_TESSELLATOR_POLYGON_COUNTS_GROW                          (16)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_void_t lx_tessellator_result_clear(lx_tessellator_t* tessellator) {

    // clear polygon
    tessellator->polygon.total = 0;
    tessellator->polygon.points = lx_null;
    tessellator->polygon.counts = lx_null;

    // clear polygon points
    if (!tessellator->polygon_points) {
        tessellator->polygon_points = lx_array_init(LX_TESSELLATOR_POLYGON_POINTS_GROW, lx_element_mem(sizeof(lx_point_t), lx_null));
    }
    lx_array_clear(tessellator->polygon_points);

    // clear polygon counts
    if (tessellator->mode != LX_TESSELLATOR_MODE_TRIANGULATION) { // we need not counts to optimize memory if be only triangulation
        if (!tessellator->polygon_counts) {
            tessellator->polygon_counts = lx_array_init(LX_TESSELLATOR_POLYGON_COUNTS_GROW, lx_element_mem(sizeof(lx_uint16_t), lx_null));
        }
        lx_array_clear(tessellator->polygon_counts);
    }
}

static lx_void_t lx_tessellator_result_append(lx_tessellator_t* tessellator) {
    lx_assert(tessellator && tessellator->mesh);

    lx_array_ref_t polygon_points = tessellator->polygon_points;
    lx_array_ref_t polygon_counts = tessellator->polygon_counts;
    lx_assert(polygon_points);

    lx_for_all (lx_mesh_face_ref_t, face, lx_mesh_face_list(tessellator->mesh)) {
        if (lx_tessellator_face_inside(face)) {

            // make contour
            lx_mesh_edge_ref_t  head        = lx_mesh_face_edge(face);
            lx_mesh_edge_ref_t  edge        = head;
            lx_point_ref_t      point       = lx_null;
            lx_point_ref_t      point_first = lx_null;
            lx_uint16_t         count       = 0;
            do {
                point = lx_tessellator_vertex_point(lx_mesh_edge_org(edge));
                lx_assert(point);

                count++;
                lx_array_insert_tail(polygon_points, point);
                if (!point_first) {
                    point_first = point;
                }
                edge = lx_mesh_edge_lnext(edge);
            } while (edge != head);

            if (lx_array_size(polygon_points) > 2) {

                // close it
                lx_assert(lx_array_data(polygon_points));
                if (tessellator->flags & LX_TESSELLATOR_FLAG_AUTOCLOSED) {
                    lx_array_insert_tail(polygon_points, point_first);
                    count++;
                }

                // update count
                if (polygon_counts) {
                    lx_array_insert_tail(polygon_counts, &count);
                }
                tessellator->polygon.total += count;
            }
        }
    }

    // bind polygon points data
    if (tessellator->polygon.total) {
        tessellator->polygon.points = (lx_point_ref_t)lx_array_data(polygon_points);
        if (polygon_counts) {
            lx_uint16_t zero = 0;
            lx_array_insert_tail(polygon_counts, &zero);
            tessellator->polygon.counts = (lx_uint16_t*)lx_array_data(polygon_counts);
        }
    }
}

static lx_void_t lx_tessellator_make_from_convex(lx_tessellator_t* tessellator, lx_polygon_ref_t polygon, lx_rect_ref_t bounds) {
    lx_assert(tessellator && polygon && bounds);

    // only one convex contour
    lx_assert(polygon->convex && polygon->counts && !polygon->counts[1]);

    // must be triangulation mode now
    lx_assert_and_check_return(tessellator->mode == LX_TESSELLATOR_MODE_TRIANGULATION);

    // make mesh
    if (!lx_tessellator_mesh_make(tessellator, polygon)) {
        return ;
    }

    // only two faces
    lx_mesh_face_list_ref_t fact_list = lx_mesh_face_list(tessellator->mesh);
    lx_assert(fact_list && lx_mesh_face_list_size(fact_list) == 2);

    // the arbitrary face is marked "inside" and the triangulation will be not effected
    lx_tessellator_face_inside_set(lx_mesh_face_list_head(fact_list), 1);

    // make triangulation region
    lx_tessellator_triangulation_make(tessellator);

    // append result
    lx_tessellator_result_append(tessellator);
}

static lx_void_t lx_tessellator_make_from_concave(lx_tessellator_t* tessellator, lx_polygon_ref_t polygon, lx_rect_ref_t bounds) {
    lx_assert(tessellator && polygon && !polygon->convex && bounds);

    // make mesh
    if (!lx_tessellator_mesh_make(tessellator, polygon)) {
        return ;
    }

    // make horizontal monotone region
    lx_tessellator_monotone_make(tessellator, bounds);

    // need make convex or triangulation polygon?
    if (tessellator->mode == LX_TESSELLATOR_MODE_CONVEX || tessellator->mode == LX_TESSELLATOR_MODE_TRIANGULATION) {

        // make triangulation region for each horizontal monotone region
        lx_tessellator_triangulation_make(tessellator);

        // make convex?
        if (tessellator->mode == LX_TESSELLATOR_MODE_CONVEX) {
            // merge triangles to the convex polygon
            lx_tessellator_convex_make(tessellator);
        }
    }

    // append result
    lx_tessellator_result_append(tessellator);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_tessellator_ref_t lx_tessellator_init() {
    return (lx_tessellator_ref_t)lx_malloc0_type(lx_tessellator_t);
}

lx_void_t lx_tessellator_exit(lx_tessellator_ref_t self) {
    lx_tessellator_t* tessellator = (lx_tessellator_t*)self;
    if (tessellator) {
        if (tessellator->mesh) {
            lx_mesh_exit(tessellator->mesh);
            tessellator->mesh = lx_null;
        }
        tessellator->polygon.total = 0;
        if (tessellator->polygon_points) {
            lx_array_exit(tessellator->polygon_points);
            tessellator->polygon_points = lx_null;
        }
        if (tessellator->polygon_counts) {
            lx_array_exit(tessellator->polygon_counts);
            tessellator->polygon_counts = lx_null;
        }
        if (tessellator->event_queue) {
            lx_priority_queue_exit(tessellator->event_queue);
            tessellator->event_queue = lx_null;
        }
        if (tessellator->active_regions) {
            lx_list_exit(tessellator->active_regions);
            tessellator->active_regions = lx_null;
        }
        lx_free(tessellator);
    }
}

lx_size_t lx_tessellator_mode(lx_tessellator_ref_t self) {
    lx_tessellator_t* tessellator = (lx_tessellator_t*)self;
    return tessellator? tessellator->mode : LX_TESSELLATOR_MODE_CONVEX;
}

lx_void_t lx_tessellator_mode_set(lx_tessellator_ref_t self, lx_size_t mode) {
    lx_tessellator_t* tessellator = (lx_tessellator_t*)self;
    if (tessellator) {
        tessellator->mode = mode;
    }
}

lx_void_t lx_tessellator_rule_set(lx_tessellator_ref_t self, lx_size_t rule) {
    lx_tessellator_t* tessellator = (lx_tessellator_t*)self;
    if (tessellator) {
        tessellator->rule = rule;
    }
}

lx_void_t lx_tessellator_flags_set(lx_tessellator_ref_t self, lx_size_t flags) {
    lx_tessellator_t* tessellator = (lx_tessellator_t*)self;
    if (tessellator) {
        tessellator->flags = flags;
    }
}

lx_polygon_ref_t lx_tessellator_make(lx_tessellator_ref_t self, lx_polygon_ref_t polygon, lx_rect_ref_t bounds) {
    lx_tessellator_t* tessellator = (lx_tessellator_t*)self;
    lx_assert_and_check_return_val(tessellator && polygon && polygon->points && polygon->counts && bounds, lx_null);

    // we need not do tessellation if it has been convex or monotone polygon
    if (polygon->convex &&
        (tessellator->mode == LX_TESSELLATOR_MODE_CONVEX || tessellator->mode == LX_TESSELLATOR_MODE_MONOTONE)) {
        return polygon;
    }

    // clear result
    lx_tessellator_result_clear(tessellator);

    // do triangulation for each contex contour? it will be faster
    if (polygon->convex) {
        lx_polygon_t    contour;
        lx_size_t       index               = 0;
        lx_point_ref_t  points              = polygon->points;
        lx_uint16_t*    counts              = polygon->counts;
        lx_uint16_t     contour_counts[2]   = {0, 0};
        lx_polygon_make(&contour, lx_null, contour_counts, 0, lx_true);
        while ((contour_counts[0] = *counts++)) {
            contour.points = points + index;
            lx_tessellator_make_from_convex(tessellator, &contour, bounds);
            index += contour_counts[0];
        }
    } else {
        lx_tessellator_make_from_concave(tessellator, polygon, bounds);
    }
    return tessellator->polygon.total? &tessellator->polygon : lx_null;
}
