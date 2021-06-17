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
 * @file        fixed_pool.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "fixed_pool.h"
#include "static_fixed_pool.h"
#include "../libc/libc.h"
#include "../container/container.h"
#include "../algorithm/algorithm.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the item belong to this slot?
#define lx_fixed_pool_slot_exists(slot, item)   (((lx_byte_t*)(item) > (lx_byte_t*)(slot)) && ((lx_byte_t*)(item) < (lx_byte_t*)slot + (slot)->size))

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the fixed pool slot type
typedef struct lx_fixed_pool_slot_t_ {
    // the size: sizeof(slot) + data
    lx_size_t                       size;
    lx_static_fixed_pool_ref_t      pool;
    lx_list_entry_t                 entry;
}lx_fixed_pool_slot_t;

// the fixed pool type
typedef struct lx_fixed_pool_t_ {
    lx_size_t                       slot_size;
    lx_size_t                       item_size;
    lx_size_t                       item_count;
    lx_fixed_pool_item_init_cb_t    func_init;
    lx_fixed_pool_item_exit_cb_t    func_exit;
    lx_cpointer_t                   func_udata;
    lx_fixed_pool_slot_t*           current_slot;
    lx_list_entry_head_t            partial_slots;
    lx_list_entry_head_t            full_slots;
    lx_fixed_pool_slot_t**          slot_list;
    lx_size_t                       slot_count;
    lx_size_t                       slot_space;
}lx_fixed_pool_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_bool_t lx_fixed_pool_item_exit(lx_pointer_t data, lx_cpointer_t udata) {
    lx_fixed_pool_t* pool = (lx_fixed_pool_t*)udata;
    if (pool && pool->func_exit) {
        pool->func_exit(data, pool->func_udata);
    }
    return lx_true;
}

static lx_void_t lx_fixed_pool_slot_exit(lx_fixed_pool_t* pool, lx_fixed_pool_slot_t* slot) {
    lx_assert(pool && slot && pool->slot_list && pool->slot_count);

    // trace
    //lx_trace_d("slot[%lu]: exit: size: %lu", pool->item_size, slot->size);

    // make array and iterator
    lx_iterator_t    iterator;
    lx_fixed_array_t fixed_array;
    lx_fixed_array_init_ptr(&fixed_array, (lx_pointer_t*)pool->slot_list, pool->slot_count);
    lx_iterator_of(&iterator, &fixed_array);

    // find the slot from the slot list
    lx_size_t itor = lx_binary_find_all(&iterator, (lx_cpointer_t)slot);
    lx_assert(itor != lx_iterator_tail(&iterator) && itor < pool->slot_count && pool->slot_list[itor]);
    lx_check_return(itor != lx_iterator_tail(&iterator) && itor < pool->slot_count && pool->slot_list[itor]);

    // remove the slot
    if (itor + 1 < pool->slot_count) lx_memmov(pool->slot_list + itor, pool->slot_list + itor + 1, (pool->slot_count - itor - 1) * sizeof(lx_fixed_pool_slot_t*));
    pool->slot_count--;
    lx_free(slot);
}

static lx_fixed_pool_slot_t* lx_fixed_pool_slot_init(lx_fixed_pool_t* pool) {
    lx_assert(pool && pool->slot_size && pool->item_size);

    lx_bool_t               ok = lx_false;
    lx_fixed_pool_slot_t*   slot = lx_null;
    do {
#ifdef LX_DEBUG
        // init patch for checking underflow
        lx_size_t patch = 1;
#else
        lx_size_t patch = 0;
#endif

        // the item space
        lx_size_t item_space = sizeof(lx_pool_data_empty_head_t) + pool->item_size + patch;

        // the need space
        lx_size_t need_space = sizeof(lx_fixed_pool_slot_t) + pool->slot_size * item_space;

        // make slot
        slot = (lx_fixed_pool_slot_t*)lx_malloc(need_space);
        lx_assert_and_check_break(slot);
        lx_assert_and_check_break(need_space > sizeof(lx_fixed_pool_slot_t) + item_space);

        // init slot
        slot->size = need_space;
        slot->pool = lx_static_fixed_pool_init((lx_byte_t*)&slot[1], need_space - sizeof(lx_fixed_pool_slot_t), pool->item_size);
        lx_assert_and_check_break(slot->pool);

        if (!pool->slot_list) {
            pool->slot_count = 0;
            pool->slot_space = 64;
            pool->slot_list = (lx_fixed_pool_slot_t**)lx_nalloc(pool->slot_space, sizeof(lx_fixed_pool_slot_t*));
            lx_assert_and_check_break(pool->slot_list);
        } else if (pool->slot_count == pool->slot_space) { // no enough space?
            pool->slot_space <<= 1;
            pool->slot_list = (lx_fixed_pool_slot_t**)lx_ralloc(pool->slot_list, pool->slot_space * sizeof(lx_fixed_pool_slot_t*));
            lx_assert_and_check_break(pool->slot_list);
        }
        lx_assert_and_check_break(pool->slot_count < pool->slot_space);

        // insert the slot to the slot list in the increasing order (TODO binary search)
        lx_size_t i = 0;
        lx_size_t n = pool->slot_count;
        for (i = 0; i < n; i++) if (slot <= pool->slot_list[i]) break;
        if (i < n) lx_memmov(pool->slot_list + i + 1, pool->slot_list + i, (n - i) * sizeof(lx_fixed_pool_slot_t*));
        pool->slot_list[i] = slot;

        // update the slot count
        pool->slot_count++;

        // trace
        //lx_trace_d("slot[%lu]: init: size: %lu, item: %lu => %lu", pool->item_size, need_space, pool->slot_size, lx_static_fixed_pool_maxn(slot->pool));

        // ok
        ok = lx_true;

    } while (0);

    if (!ok && slot) {
        lx_fixed_pool_slot_exit(pool, slot);
        slot = lx_null;
    }
    return slot;
}

static lx_long_t lx_fixed_pool_slot_comp(lx_iterator_ref_t iterator, lx_cpointer_t item, lx_cpointer_t data) {
    lx_fixed_pool_slot_t* slot = (lx_fixed_pool_slot_t*)item;
    lx_assert(slot);
    return (lx_byte_t*)data < (lx_byte_t*)slot? 1 : ((lx_byte_t*)data >= (lx_byte_t*)slot + slot->size? -1 : 0);
}

static lx_fixed_pool_slot_t* lx_fixed_pool_slot_find(lx_fixed_pool_t* pool, lx_pointer_t data) {
    lx_assert_and_check_return_val(pool && data, lx_null);

    // make array and iterator
    lx_iterator_t    iterator;
    lx_fixed_array_t fixed_array;
    lx_fixed_array_init_ptr(&fixed_array, (lx_pointer_t*)pool->slot_list, pool->slot_count);
    lx_iterator_of(&iterator, &fixed_array);

    // find it
    lx_size_t itor = lx_binary_find_all_if(&iterator, lx_fixed_pool_slot_comp, data);
    lx_check_return_val(itor != lx_iterator_tail(&iterator), lx_null);

    // get slot
    lx_fixed_pool_slot_t* slot = pool->slot_list[itor];
    lx_assert_and_check_return_val(slot, lx_null);
    lx_assert(lx_fixed_pool_slot_exists(slot, data));
    return slot;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_fixed_pool_ref_t lx_fixed_pool_init(lx_size_t slot_size, lx_size_t item_size, lx_fixed_pool_item_init_cb_t item_init, lx_fixed_pool_item_exit_cb_t item_exit, lx_cpointer_t udata) {
    lx_assert_and_check_return_val(item_size, lx_null);

    lx_bool_t           ok = lx_false;
    lx_fixed_pool_t*    pool = lx_null;
    do {

        // init pool
        pool = (lx_fixed_pool_t*)lx_malloc0_type(lx_fixed_pool_t);
        lx_assert_and_check_break(pool);

        pool->slot_size         = slot_size? slot_size : (lx_page_size() >> 4);
        pool->item_size         = item_size;
        pool->func_init         = item_init;
        pool->func_exit         = item_exit;
        pool->func_udata        = udata;
        lx_assert_and_check_break(pool->slot_size);

        lx_list_entry_init(&pool->partial_slots, lx_fixed_pool_slot_t, entry);
        lx_list_entry_init(&pool->full_slots, lx_fixed_pool_slot_t, entry);

        // ok
        ok = lx_true;

    } while (0);

    if (!ok && pool) {
        lx_fixed_pool_exit((lx_fixed_pool_ref_t)pool);
        pool = lx_null;
    }
    return (lx_fixed_pool_ref_t)pool;
}

lx_void_t lx_fixed_pool_exit(lx_fixed_pool_ref_t self) {
    lx_fixed_pool_t* pool = (lx_fixed_pool_t*)self;
    if (pool) {
        lx_fixed_pool_clear(self);
        if (pool->current_slot) {
            lx_fixed_pool_slot_exit(pool, pool->current_slot);
            pool->current_slot = lx_null;
        }
        if (pool->slot_list) {
            lx_free(pool->slot_list);
            pool->slot_list = lx_null;
        }
        pool->slot_count = 0;
        pool->slot_space = 0;
        lx_free(pool);
    }
}

lx_size_t lx_fixed_pool_size(lx_fixed_pool_ref_t self) {
    lx_fixed_pool_t* pool = (lx_fixed_pool_t*)self;
    lx_assert_and_check_return_val(pool, 0);

    return pool? pool->item_count : 0;
}

lx_void_t lx_fixed_pool_clear(lx_fixed_pool_ref_t self) {
    lx_fixed_pool_t* pool = (lx_fixed_pool_t*)self;
    lx_assert_and_check_return(pool);

    // exit items
    if (pool->func_exit) {
        lx_fixed_pool_foreach(self, lx_fixed_pool_item_exit, (lx_pointer_t)pool);
    }

    // exit the partial slots
    lx_iterator_t partial_iterator;
    lx_iterator_of(&partial_iterator, lx_list_entry_itor(&pool->partial_slots));
    lx_size_t itor = lx_iterator_head(&partial_iterator);
    while (itor != lx_iterator_tail(&partial_iterator)) {
        lx_fixed_pool_slot_t* slot = (lx_fixed_pool_slot_t*)lx_iterator_item(&partial_iterator, itor);
        lx_assert_and_check_break(slot);
        lx_assert(slot != pool->current_slot);
        lx_size_t next = lx_iterator_next(&partial_iterator, itor);
        lx_fixed_pool_slot_exit(pool, slot);
        itor = next;
    }

    // exit the full slots
    lx_iterator_t full_iterator;
    lx_iterator_of(&full_iterator, lx_list_entry_itor(&pool->full_slots));
    itor = lx_iterator_head(&full_iterator);
    while (itor != lx_iterator_tail(&full_iterator)) {
        lx_fixed_pool_slot_t* slot = (lx_fixed_pool_slot_t*)lx_iterator_item(&full_iterator, itor);
        lx_assert_and_check_break(slot);
        lx_assert(slot != pool->current_slot);
        lx_size_t next = lx_iterator_next(&full_iterator, itor);
        lx_fixed_pool_slot_exit(pool, slot);
        itor = next;
    }

    // clear slots
    if (pool->current_slot && pool->current_slot->pool) {
        lx_static_fixed_pool_clear(pool->current_slot->pool);
    }
    pool->item_count = 0;
    lx_list_entry_clear(&pool->partial_slots);
    lx_list_entry_clear(&pool->full_slots);
}

lx_pointer_t lx_fixed_pool_malloc(lx_fixed_pool_ref_t self) {
    lx_fixed_pool_t* pool = (lx_fixed_pool_t*)self;
    lx_assert_and_check_return_val(pool, lx_null);

    lx_bool_t       ok = lx_false;
    lx_pointer_t    data = lx_null;
    do {
        // no current slot or the current slot is full? update the current slot
        if (!pool->current_slot || lx_static_fixed_pool_full(pool->current_slot->pool)) {

            // move the current slot to the full slots if exists
            if (pool->current_slot) lx_list_entry_insert_tail(&pool->full_slots, &pool->current_slot->entry);

            // clear the current slot
            pool->current_slot = lx_null;

            // attempt to get a slot from the partial slots
            if (!lx_list_entry_is_empty(&pool->partial_slots)) {
                lx_list_entry_ref_t entry = lx_list_entry_head(&pool->partial_slots);
                lx_assert_and_check_break(entry);

                pool->current_slot = (lx_fixed_pool_slot_t*)lx_list_entry(&pool->partial_slots, entry);
                lx_assert_and_check_break(pool->current_slot);

                lx_list_entry_remove(&pool->partial_slots, entry);
            } else {
                pool->current_slot = lx_fixed_pool_slot_init(pool);
            }
        }

        // check
        lx_assert_and_check_break(pool->current_slot && pool->current_slot->pool);
        lx_assert_and_check_break(!lx_static_fixed_pool_full(pool->current_slot->pool));

        // make data from the current slot
        data = lx_static_fixed_pool_malloc(pool->current_slot->pool);
        lx_assert_and_check_break(data);

        if (pool->func_init && !pool->func_init(data, pool->func_udata)) {
            break;
        }
        pool->item_count++;

        // ok
        ok = lx_true;

    } while (0);

    if (!ok) {
        if (data && pool->current_slot && pool->current_slot->pool)
            lx_static_fixed_pool_free(pool->current_slot->pool, data);
        data = lx_null;
    }

    // check
    lx_assertf(data, "malloc(%lu) failed!", pool->item_size);
    return data;
}

lx_pointer_t lx_fixed_pool_malloc0(lx_fixed_pool_ref_t self) {
    lx_fixed_pool_t* pool = (lx_fixed_pool_t*)self;
    lx_assert_and_check_return_val(pool, lx_null);

    lx_pointer_t data = lx_fixed_pool_malloc(self);
    if (data) {
        lx_memset(data, 0, pool->item_size);
    }
    return data;
}

lx_bool_t lx_fixed_pool_free(lx_fixed_pool_ref_t self, lx_pointer_t data) {
    lx_fixed_pool_t* pool = (lx_fixed_pool_t*)self;
    lx_assert_and_check_return_val(pool, lx_false);

    lx_bool_t ok = lx_false;
    do {
        lx_assertf(pool->item_count, "double free data: %p", data);

        // find the slot
        lx_fixed_pool_slot_t* slot = lx_fixed_pool_slot_find(pool, data);
        lx_assertf(slot, "the data: %p not belong to pool: %p", data, self);
        lx_assert_and_check_break(slot->pool);

        // the slot is full?
        lx_bool_t full = lx_static_fixed_pool_full(slot->pool);

        // free it
        if (pool->func_exit) {
            pool->func_exit(data, pool->func_udata);
        }
        if (!lx_static_fixed_pool_free(slot->pool, data)) {
            break;
        }

        // not the current slot?
        if (slot != pool->current_slot) {

            // is full? move the slot to the partial slots
            if (full) {
                lx_list_entry_remove(&pool->full_slots, &slot->entry);
                lx_list_entry_insert_tail(&pool->partial_slots, &slot->entry);
            } else if (lx_static_fixed_pool_empty(slot->pool)) {
                // is empty? exit the slot
                lx_list_entry_remove(&pool->partial_slots, &slot->entry);
                lx_fixed_pool_slot_exit(pool, slot);
            }
        }

        // update the item count
        pool->item_count--;

        // ok
        ok = lx_true;

    } while (0);

#ifdef LX_DEBUG
    if (!ok) {
        lx_abort();
    }
#endif
    return ok;
}

lx_void_t lx_fixed_pool_foreach(lx_fixed_pool_ref_t self, lx_fixed_pool_item_foreach_cb_t callback, lx_cpointer_t udata) {
    lx_fixed_pool_t* pool = (lx_fixed_pool_t*)self;
    lx_assert_and_check_return(pool && callback);

    // foreach the current slot first
    if (pool->current_slot && pool->current_slot->pool) {
        lx_static_fixed_pool_foreach(pool->current_slot->pool, callback, udata);
    }

    // foreach the partial slots
    lx_for_all_if(lx_fixed_pool_slot_t*, partial_slot, lx_list_entry_itor(&pool->partial_slots), partial_slot && partial_slot->pool) {
        lx_assert(!lx_static_fixed_pool_full(partial_slot->pool));
        lx_static_fixed_pool_foreach(partial_slot->pool, callback, udata);
    }

    // foreach the full slots
    lx_for_all_if(lx_fixed_pool_slot_t*, full_slot, lx_list_entry_itor(&pool->full_slots), full_slot && full_slot->pool) {
        lx_assert(lx_static_fixed_pool_full(full_slot->pool));
        lx_static_fixed_pool_foreach(full_slot->pool, callback, udata);
    }
}

#ifdef LX_DEBUG
lx_void_t lx_fixed_pool_dump(lx_fixed_pool_ref_t self) {
    lx_fixed_pool_t* pool = (lx_fixed_pool_t*)self;
    lx_assert_and_check_return(pool);

    // dump the current slot first
    if (pool->current_slot && pool->current_slot->pool) {
        lx_static_fixed_pool_dump(pool->current_slot->pool);
    }

    // dump the partial slots
    lx_for_all_if(lx_fixed_pool_slot_t*, partial_slot, lx_list_entry_itor(&pool->partial_slots), partial_slot && partial_slot->pool) {
        lx_assert(!lx_static_fixed_pool_full(partial_slot->pool));
        lx_static_fixed_pool_dump(partial_slot->pool);
    }

    // dump the full slots
    lx_for_all_if(lx_fixed_pool_slot_t*, full_slot, lx_list_entry_itor(&pool->full_slots), full_slot && full_slot->pool) {
        lx_assert(lx_static_fixed_pool_full(full_slot->pool));
        lx_static_fixed_pool_dump(full_slot->pool);
    }
}
#endif
