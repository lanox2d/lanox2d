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
#   define LX_TESSELLATOR_OUTPUTS_GROW                          (32)
#else
#   define LX_TESSELLATOR_OUTPUTS_GROW                          (64)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_void_t lx_tessellator_make_output(lx_tessellator_t* tessellator) {
    lx_assert(tessellator && tessellator->mesh && tessellator->callback);

    if (!tessellator->outputs) {
        tessellator->outputs = lx_array_init(LX_TESSELLATOR_OUTPUTS_GROW, lx_element_mem(sizeof(lx_point_t), lx_null));
    }
    lx_array_ref_t outputs = tessellator->outputs;
    lx_assert(outputs);

    lx_for_all (lx_mesh_face_ref_t, face, lx_mesh_face_list(tessellator->mesh)) {
        if (lx_tessellator_face_inside(face)) {
            lx_array_clear(outputs);

            // make contour
            lx_mesh_edge_ref_t  head        = lx_mesh_face_edge(face);
            lx_mesh_edge_ref_t  edge        = head;
            lx_point_ref_t      point       = lx_null;
            lx_point_ref_t      point_first = lx_null;
            do {
                point = lx_tessellator_vertex_point(lx_mesh_edge_org(edge));
                lx_assert(point);

                lx_array_insert_tail(outputs, point);
                if (!point_first) {
                    point_first = point;
                }
                edge = lx_mesh_edge_lnext(edge);
            } while (edge != head);

            if (lx_array_size(outputs) > 2) {
                lx_assert(lx_array_data(outputs));
                lx_array_insert_tail(outputs, point_first); // close it
                tessellator->callback((lx_point_ref_t)lx_array_data(outputs), (lx_uint16_t)lx_array_size(outputs), tessellator->udata);
            }
        }
    }
}

static lx_void_t lx_tessellator_make_convex(lx_tessellator_t* tessellator, lx_polygon_ref_t polygon, lx_rect_ref_t bounds) {
    lx_assert(tessellator && tessellator->callback && polygon && bounds);

    // only one convex contour
    lx_assert(polygon->convex && polygon->counts && !polygon->counts[1]);

    // make convex or monotone? done it directly
    if (tessellator->mode == LX_TESSELLATOR_MODE_CONVEX || tessellator->mode == LX_TESSELLATOR_MODE_MONOTONE) {
        tessellator->callback(polygon->points, polygon->counts[0], tessellator->udata);
        return ;
    }

    // must be triangulation mode now
    lx_assert(tessellator->mode == LX_TESSELLATOR_MODE_TRIANGULATION);

    // make mesh
    if (!lx_tessellator_mesh_make(tessellator, polygon)) {
        return ;
    }

    // only two faces
    lx_mesh_ref_t mesh = tessellator->mesh;
    lx_assert(mesh && lx_mesh_face_list_size(lx_mesh_face_list(mesh)) == 2);

    // the arbitrary face is marked "inside" and the triangulation will be not effected
    lx_tessellator_face_inside_set(lx_mesh_face_list_head(lx_mesh_face_list(mesh)), 1);

    // make triangulation region
    lx_tessellator_triangulation_make(tessellator);

    // make output
    lx_tessellator_make_output(tessellator);
}

static lx_void_t lx_tessellator_make_concave(lx_tessellator_t* tessellator, lx_polygon_ref_t polygon, lx_rect_ref_t bounds) {
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

    // make output
    lx_tessellator_make_output(tessellator);
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
        if (tessellator->outputs) {
            lx_array_exit(tessellator->outputs);
            tessellator->outputs = lx_null;
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

lx_void_t lx_tessellator_callback_set(lx_tessellator_ref_t self, lx_tessellator_cb_t callback, lx_cpointer_t udata) {
    lx_tessellator_t* tessellator = (lx_tessellator_t*)self;
    if (tessellator) {
        tessellator->callback = callback;
        tessellator->udata = udata;
    }
}

lx_void_t lx_tessellator_make(lx_tessellator_ref_t self, lx_polygon_ref_t polygon, lx_rect_ref_t bounds) {
    lx_tessellator_t* tessellator = (lx_tessellator_t*)self;
    lx_assert_and_check_return(tessellator && tessellator->callback && polygon && polygon->points && polygon->counts && bounds);

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

            // make convex contour, will be faster
            lx_tessellator_make_convex(tessellator, &contour, bounds);

            // update the contour index
            index += contour_counts[0];
        }
    } else {
        // make the concave polygon
        lx_tessellator_make_concave(tessellator, polygon, bounds);
    }
}
