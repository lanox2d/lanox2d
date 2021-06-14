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
 * @file        list_entry.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "list_entry.h"
#include "../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * iterator implementation
 */
static lx_size_t lx_list_entry_iterator_head(lx_iterator_ref_t iterator) {
    lx_list_entry_head_ref_t list = lx_container_of(lx_list_entry_head_t, itor, iterator->container);
    lx_assert(list);
    return (lx_size_t)list->next;
}

static lx_size_t lx_list_entry_iterator_tail(lx_iterator_ref_t iterator) {
    lx_list_entry_head_ref_t list = lx_container_of(lx_list_entry_head_t, itor, iterator->container);
    lx_assert(list);
    return (lx_size_t)list;
}

static lx_size_t lx_list_entry_iterator_next(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_assert(itor);
    return (lx_size_t)((lx_list_entry_ref_t)itor)->next;
}

static lx_size_t lx_list_entry_iterator_prev(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_assert(itor);
    return (lx_size_t)((lx_list_entry_ref_t)itor)->prev;
}

static lx_pointer_t lx_list_entry_iterator_item(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_list_entry_head_ref_t list = lx_container_of(lx_list_entry_head_t, itor, iterator->container);
    lx_assert(list && list->eoff < itor);
    return (lx_pointer_t)(itor - list->eoff);
}

static lx_void_t lx_list_entry_iterator_of(lx_iterator_ref_t iterator, lx_cpointer_t container) {
    static lx_iterator_op_t op = {
        lx_list_entry_iterator_head,
        lx_list_entry_iterator_tail,
        lx_list_entry_iterator_prev,
        lx_list_entry_iterator_next,
        lx_list_entry_iterator_item
    };
    iterator->container = container;
    iterator->mode      = LX_ITERATOR_MODE_FORWARD | LX_ITERATOR_MODE_REVERSE;
    iterator->op        = &op;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_void_t lx_list_entry_init_(lx_list_entry_head_ref_t list, lx_size_t entry_offset, lx_size_t entry_size) {
    lx_assert_and_check_return(list && entry_size > sizeof(lx_list_entry_t));

    list->next             = (lx_list_entry_ref_t)list;
    list->prev             = (lx_list_entry_ref_t)list;
    list->size             = 0;
    list->eoff             = entry_offset;
    list->itor.iterator_of = lx_list_entry_iterator_of;
}

lx_void_t lx_list_entry_exit(lx_list_entry_head_ref_t list) {
    lx_assert_and_check_return(list);
    list->next = (lx_list_entry_ref_t)list;
    list->prev = (lx_list_entry_ref_t)list;
    list->size = 0;
}


