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
 * @file        list.h
 *
 */
#ifndef LX_BASE_CONTAINER_LIST_H
#define LX_BASE_CONTAINER_LIST_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "iterator.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the doubly-linked list ref type
 *
 *
 * <pre>
 * list: tail => |-----| => |-------------------------------------------------=> |------| => |------| => tail
 *        |       head                                                                         last       |
 *        |                                                                                               |
 *        <-----------------------------------------------------------------------------------------------
 *
 * performance:
 *
 * insert:
 * insert midd: fast
 * insert head: fast
 * insert tail: fast
 * insert next: fast
 *
 * remove:
 * remove midd: fast
 * remove head: fast
 * remove last: fast
 * remove next: fast
 *
 * iterator:
 * next: fast
 * prev: fast
 * </pre>
 *
 */
typedef lx_typeref(list);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init list
 *
 * @param grow      the grow size
 * @param element   the element
 *
 * @return          the list
 */
lx_list_ref_t       lx_list_init(lx_size_t grow, lx_element_t element);

/*! exit list
 *
 * @param list      the list
 */
lx_void_t           lx_list_exit(lx_list_ref_t list);

/*! clear list
 *
 * @param list      the list
 */
lx_void_t           lx_list_clear(lx_list_ref_t list);

/*! the list head item
 *
 * @param list      the list
 *
 * @return          the head item
 */
lx_pointer_t        lx_list_head(lx_list_ref_t list);

/*! insert the prev item
 *
 * @param list      the list
 * @param itor      the item itor
 * @param data      the item data
 *
 * @return          the item itor
 */
lx_size_t           lx_list_insert_prev(lx_list_ref_t list, lx_size_t itor, lx_cpointer_t data);

/*! insert the next item
 *
 * @param list      the list
 * @param itor      the item itor
 * @param data      the item data
 *
 * @return          the item itor
 */
lx_size_t           lx_list_insert_next(lx_list_ref_t list, lx_size_t itor, lx_cpointer_t data);

/*! insert the head item
 *
 * @param list      the list
 * @param data      the item data
 *
 * @return          the item itor
 */
lx_size_t           lx_list_insert_head(lx_list_ref_t list, lx_cpointer_t data);

/*! insert the tail item
 *
 * @param list      the list
 * @param data      the item data
 *
 * @return          the item itor
 */
lx_size_t           lx_list_insert_tail(lx_list_ref_t list, lx_cpointer_t data);

/*! replace the item
 *
 * @param list      the list
 * @param itor      the item itor
 * @param data      the item data
 */
lx_void_t           lx_list_replace(lx_list_ref_t list, lx_size_t itor, lx_cpointer_t data);

/*! remove the item
 *
 * @param list      the list
 * @param itor      the item itor
 *
 * @return          the next item
 */
lx_size_t           lx_list_remove(lx_list_ref_t list, lx_size_t itor);

/*! the item count
 *
 * @param list      the list
 *
 * @return          the item count
 */
lx_size_t           lx_list_size(lx_list_ref_t list);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif

