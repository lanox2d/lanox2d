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
 * @file        priority_queue.h
 *
 */
#ifndef LX_BASE_CONTAINER_PRIORITY_QUEUE_H
#define LX_BASE_CONTAINER_PRIORITY_QUEUE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the priority queue ref type
 *
 * using the min/max heap
 */
//typedef lx_heap_ref_t       lx_priority_queue_ref_t;
typedef lx_typeref(priority_queue);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init queue, default: min-priority
 *
 * @param grow              the element grow, using the default grow if be zero
 * @param element           the element
 *
 * @return                  the queue
 */
lx_priority_queue_ref_t     lx_priority_queue_init(lx_size_t grow, lx_element_t element);

/*! exit queue
 *
 * @param queue             the queue
 */
lx_void_t                   lx_priority_queue_exit(lx_priority_queue_ref_t queue);

/*! clear the queue
 *
 * @param queue             the queue
 */
lx_void_t                   lx_priority_queue_clear(lx_priority_queue_ref_t queue);

/*! the queue size
 *
 * @param queue             the queue
 *
 * @return                  the queue size
 */
lx_size_t                   lx_priority_queue_size(lx_priority_queue_ref_t queue);

/*! the queue maxn
 *
 * @param queue             the queue
 *
 * @return                  the queue maxn
 */
lx_size_t                   lx_priority_queue_maxn(lx_priority_queue_ref_t queue);

/*! get the queue item
 *
 * @param queue             the queue
 *
 * @return                  the queue top item
 */
lx_pointer_t                lx_priority_queue_get(lx_priority_queue_ref_t queue);

/*! put the queue item
 *
 * @param queue             the queue
 * @param data              the item data
 */
lx_void_t                   lx_priority_queue_put(lx_priority_queue_ref_t queue, lx_cpointer_t data);

/*! pop the queue item
 *
 * @param queue             the queue
 */
lx_void_t                   lx_priority_queue_pop(lx_priority_queue_ref_t queue);

/*! remove the queue item
 *
 * @param queue             the queue
 * @param itor              the itor
 */
lx_void_t                   lx_priority_queue_remove(lx_priority_queue_ref_t queue, lx_size_t itor);

#ifdef __lx_debug__
/*! dump queue
 *
 * @param queue             the queue
 */
lx_void_t                   lx_priority_queue_dump(lx_priority_queue_ref_t queue);
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


