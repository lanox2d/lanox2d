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
 * @file        list.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "list.h"
#include "list_entry.h"
#include "iterator.h"
#include "../libc/libc.h"
#include "../memory/memory.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the self grow
#ifdef LX_CONFIG_SMALL
#   define LX_LIST_GROW             (128)
#else
#   define LX_LIST_GROW             (256)
#endif

// the self maxn
#ifdef LX_CONFIG_SMALL
#   define LX_LIST_MAXN             (1 << 16)
#else
#   define LX_LIST_MAXN             (1 << 30)
#endif

#define lx_list_iterator_head_(list)    (lx_size_t)lx_list_entry_head(&(list)->head)
#define lx_list_iterator_tail_(list)    (lx_size_t)lx_list_entry_tail(&(list)->head)
#define lx_list_iterator_next_(itor)    (lx_size_t)lx_list_entry_next((lx_list_entry_t*)(itor))
#define lx_list_iterator_prev_(itor)    (lx_size_t)lx_list_entry_prev((lx_list_entry_t*)(itor))
#define lx_list_iterator_item_(itor)    (lx_pointer_t)(((lx_list_entry_t*)(itor)) + 1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the list type
typedef struct lx_list_t {
    lx_iterator_base_t          base;
    lx_fixed_pool_ref_t         pool;
    lx_list_entry_head_t        head;
    lx_element_t                element;
}lx_list_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

static lx_void_t lx_list_item_exit(lx_pointer_t data, lx_cpointer_t udata) {
    lx_list_t* list = (lx_list_t*)udata;
    if (list && list->element.free) {
        list->element.free((lx_pointer_t)(((lx_list_entry_t*)data) + 1));
    }
}

static lx_size_t lx_list_iterator_head(lx_iterator_ref_t iterator) {
    lx_assert(iterator && iterator->container);
    lx_list_t* list = (lx_list_t*)iterator->container;
    return lx_list_iterator_head_(list);
}

static lx_size_t lx_list_iterator_tail(lx_iterator_ref_t iterator) {
    lx_assert(iterator && iterator->container);
    lx_list_t* list = (lx_list_t*)iterator->container;
    return lx_list_iterator_tail_(list);
}

static lx_size_t lx_list_iterator_next(lx_iterator_ref_t iterator, lx_size_t itor) {
    return lx_list_iterator_next_(itor);
}

static lx_size_t lx_list_iterator_prev(lx_iterator_ref_t iterator, lx_size_t itor) {
    return lx_list_iterator_prev_(itor);
}

static lx_pointer_t lx_list_iterator_item(lx_iterator_ref_t iterator, lx_size_t itor) {
    return lx_list_iterator_item_(itor);
}

static lx_size_t lx_list_iterator_size(lx_iterator_ref_t iterator) {
    lx_assert(iterator && iterator->container);
    return lx_list_size((lx_list_ref_t)iterator->container);
}

static lx_long_t lx_list_iterator_comp(lx_iterator_ref_t iterator, lx_cpointer_t litem, lx_cpointer_t ritem) {
    lx_assert(iterator && iterator->container);
    lx_list_t* list = (lx_list_t*)iterator->container;
    lx_assert(list->element.comp);
    return list->element.comp(litem, ritem);
}

static lx_void_t lx_list_iterator_remove(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_assert(iterator && iterator->container);
    lx_list_remove((lx_list_ref_t)iterator->container, itor);
}

static lx_void_t lx_list_iterator_of(lx_iterator_ref_t iterator, lx_cpointer_t container) {
    static lx_iterator_op_t op = {
        lx_list_iterator_head,
        lx_list_iterator_tail,
        lx_list_iterator_prev,
        lx_list_iterator_next,
        lx_list_iterator_item,
        lx_list_iterator_size,
        lx_list_iterator_comp,
        lx_list_iterator_remove
    };
    iterator->container = container;
    iterator->mode      = LX_ITERATOR_MODE_FORWARD | LX_ITERATOR_MODE_REVERSE;
    iterator->op        = &op;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_list_ref_t lx_list_init(lx_size_t grow, lx_element_t element) {
    lx_assert_and_check_return_val(element.size, lx_null);

    lx_bool_t   ok = lx_false;
    lx_list_t*  list = lx_null;
    do {

        // init list
        list = lx_malloc0_type(lx_list_t);
        lx_assert_and_check_break(list);

        list->element = element;
        list->base.iterator_of = lx_list_iterator_of;
        lx_list_entry_init_(&list->head, 0, sizeof(lx_list_entry_t) + element.size);

        // init pool, item = entry + data
        list->pool = lx_fixed_pool_init(grow? grow : LX_LIST_GROW, sizeof(lx_list_entry_t) + element.size, lx_list_item_exit, (lx_cpointer_t)list);
        lx_assert_and_check_break(list->pool);

        ok = lx_true;

    } while (0);

    if (!ok && list) {
        lx_list_exit((lx_list_ref_t)list);
        list = lx_null;
    }
    return (lx_list_ref_t)list;
}

lx_void_t lx_list_exit(lx_list_ref_t self) {
    lx_list_t* list = (lx_list_t*)self;
    if (list) {
        lx_list_clear(self);
        if (list->pool) {
            lx_fixed_pool_exit(list->pool);
        }
        lx_free(list);
    }
}

lx_void_t lx_list_clear(lx_list_ref_t self) {
    lx_list_t* list = (lx_list_t*)self;
    if (list) {
        if (list->pool) {
            lx_fixed_pool_clear(list->pool);
        }
        lx_list_entry_clear(&list->head);
    }
}

lx_pointer_t lx_list_head(lx_list_ref_t self) {
    lx_list_t* list = (lx_list_t*)self;
    lx_assert(list);
    lx_size_t itor = lx_list_iterator_head_(list);
    return lx_list_iterator_item_(itor);
}

lx_size_t lx_list_size(lx_list_ref_t self) {
    lx_list_t* list = (lx_list_t*)self;
    lx_assert_and_check_return_val(list && list->pool, 0);
    lx_assert(lx_list_entry_size(&list->head) == lx_fixed_pool_size(list->pool));
    return lx_list_entry_size(&list->head);
}

lx_size_t lx_list_insert_prev(lx_list_ref_t self, lx_size_t itor, lx_cpointer_t data) {
    lx_list_t* list = (lx_list_t*)self;
    lx_assert_and_check_return_val(list && list->pool, 0);
    lx_assert_and_check_return_val(lx_list_size(self) < LX_LIST_MAXN, (lx_size_t)lx_list_entry_tail(&list->head));

    lx_list_entry_ref_t node = (lx_list_entry_ref_t)itor;
    lx_assert_and_check_return_val(node, lx_list_iterator_tail_(list));

    lx_list_entry_ref_t entry = (lx_list_entry_ref_t)lx_fixed_pool_malloc(list->pool);
    lx_assert_and_check_return_val(entry, lx_list_iterator_tail_(list));

    lx_memcpy((lx_pointer_t)(((lx_list_entry_t*)entry) + 1), data, list->element.size);
    lx_list_entry_insert_prev(&list->head, node, entry);
    return (lx_size_t)entry;
}

lx_size_t lx_list_insert_next(lx_list_ref_t self, lx_size_t itor, lx_cpointer_t data) {
    return lx_list_insert_prev(self, lx_list_iterator_next_(itor), data);
}

lx_size_t lx_list_insert_head(lx_list_ref_t self, lx_cpointer_t data) {
    lx_list_t* list = (lx_list_t*)self;
    lx_assert(list);
    return lx_list_insert_prev(self, lx_list_iterator_head_(list), data);
}

lx_size_t lx_list_insert_tail(lx_list_ref_t self, lx_cpointer_t data) {
    lx_list_t* list = (lx_list_t*)self;
    lx_assert(list);
    return lx_list_insert_prev(self, lx_list_iterator_tail_(list), data);
}

lx_void_t lx_list_replace(lx_list_ref_t self, lx_size_t itor, lx_cpointer_t data) {
    lx_list_t* list = (lx_list_t*)self;
    lx_assert_and_check_return(list && itor);

    lx_list_entry_ref_t node = (lx_list_entry_ref_t)itor;
    lx_assert_and_check_return(node);

    lx_pointer_t item = (lx_pointer_t)(((lx_list_entry_t*)node) + 1);
    if (list->element.free) {
        list->element.free(item);
    }
    lx_memcpy((lx_pointer_t)item, data, list->element.size);
}

lx_size_t lx_list_remove(lx_list_ref_t self, lx_size_t itor) {
    lx_list_t* list = (lx_list_t*)self;
    lx_assert_and_check_return_val(list && list->pool && itor, 0);

    lx_list_entry_ref_t node = (lx_list_entry_ref_t)itor;
    lx_assert_and_check_return_val(node, lx_list_iterator_tail_(list));

    lx_list_entry_ref_t next = lx_list_entry_next(node);
    lx_list_entry_remove(&list->head, node);
    lx_fixed_pool_free(list->pool, node);
    return (lx_size_t)next;
}

