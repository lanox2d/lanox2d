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
 * @file        event_queue.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "event_queue.h"
#include "geometry.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_long_t lx_tessellator_event_queue_comp(lx_cpointer_t ldata, lx_cpointer_t rdata) {
    lx_assert(ldata && rdata);
    ldata = *((lx_cpointer_t*)ldata);
    rdata = *((lx_cpointer_t*)rdata);
#if 0
    // lvertex <= rvertex ? -1 : 1
    return (!lx_tessellator_vertex_leq((lx_mesh_vertex_ref_t)ldata, (lx_mesh_vertex_ref_t)rdata) << 1) - 1;
#else
    // the left and right point
    lx_point_ref_t lpoint = lx_tessellator_vertex_point(ldata);
    lx_point_ref_t rpoint = lx_tessellator_vertex_point(rdata);
    lx_assert(lpoint && rpoint);

    // lpoint < rpoint?
    return (lpoint->y < rpoint->y)? -1 : ((lpoint->y > rpoint->y)? 1 : ((lpoint->x < rpoint->x)? -1 : (lpoint->x > rpoint->x)));
#endif
}

static lx_bool_t lx_tessellator_event_queue_find(lx_iterator_ref_t iterator, lx_cpointer_t item, lx_cpointer_t value) {
    return *((lx_cpointer_t*)item) == value;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bool_t lx_tessellator_event_queue_make(lx_tessellator_t* tessellator) {
    lx_assert(tessellator);
    if (!tessellator->event_queue) {
        lx_element_t element = lx_element_mem(sizeof(lx_cpointer_t), lx_null, lx_null);
        element.comp = lx_tessellator_event_queue_comp;
        tessellator->event_queue = lx_priority_queue_init(0, element);
    }
    lx_assert_and_check_return_val(tessellator->event_queue, lx_false);
    lx_priority_queue_clear(tessellator->event_queue);

    lx_mesh_ref_t mesh = tessellator->mesh;
    lx_assert(mesh);
    lx_for_all (lx_mesh_vertex_ref_t, vertex, lx_mesh_vertex_list(mesh)) {
        lx_priority_queue_put(tessellator->event_queue, &vertex);
    }
    return lx_priority_queue_size(tessellator->event_queue) != 0;
}

lx_void_t lx_tessellator_event_queue_insert(lx_tessellator_t* tessellator, lx_mesh_vertex_ref_t event) {
    lx_assert(tessellator && tessellator->event_queue && event);
    lx_priority_queue_put(tessellator->event_queue, &event);
}

lx_void_t lx_tessellator_event_queue_remove(lx_tessellator_t* tessellator, lx_mesh_vertex_ref_t event) {
    lx_assert(tessellator && tessellator->event_queue && event);

    lx_iterator_t iterator;
    lx_iterator_of(&iterator, tessellator->event_queue);
    lx_size_t itor = lx_find_all_if(&iterator, lx_tessellator_event_queue_find, event);
    if (itor != lx_iterator_tail(&iterator)) {
        lx_iterator_remove(&iterator, itor);
    }
}
