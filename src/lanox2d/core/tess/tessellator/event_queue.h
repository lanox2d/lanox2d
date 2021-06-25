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
 * @file        event_queue.h
 */
#ifndef LX_CORE_TESS_TESSELLATOR_EVENT_QUEUE_H
#define LX_CORE_TESS_TESSELLATOR_EVENT_QUEUE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* make the vertex event queue and all events are sorted
 *
 * @param tessellator   the tessellator
 *
 * @return              lx_true or lx_false
 */
lx_bool_t               lx_tessellator_event_queue_make(lx_tessellator_t* tessellator);

/* insert the vertex event to queue
 *
 * @param tessellator   the tessellator tessellator
 * @param event         the vertex event
 */
lx_void_t               lx_tessellator_event_queue_insert(lx_tessellator_t* tessellator, lx_mesh_vertex_ref_t event);

/* remove the vertex event from queue
 *
 * @param tessellator   the tessellator tessellator
 * @param event         the vertex event
 */
lx_void_t               lx_tessellator_event_queue_remove(lx_tessellator_t* tessellator, lx_mesh_vertex_ref_t event);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


