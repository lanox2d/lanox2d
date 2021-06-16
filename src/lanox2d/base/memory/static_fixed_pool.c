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
 * @file        static_fixed_pool.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "static_fixed_pool.h"
#include "../libc/libc.h"
#include "../utils/utils.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#ifdef LX_WORDS_BIGENDIAN

// allocate the index
#   define lx_static_fixed_pool_used_set1(used, i)          do {(used)[(i) >> 3] |= (0x1 << (7 - ((i) & 7)));} while (0)

// free the index
#   define lx_static_fixed_pool_used_set0(used, i)          do {(used)[(i) >> 3] &= ~(0x1 << (7 - ((i) & 7)));} while (0)

// the index have been allocated?
#   define lx_static_fixed_pool_used_bset(used, i)          ((used)[(i) >> 3] & (0x1 << (7 - ((i) & 7))))

// find the first free index
#   define lx_static_fixed_pool_find_free(v)                lx_bits_fb0_be(v)

#else

// allocate the index
#   define lx_static_fixed_pool_used_set1(used, i)          do {(used)[(i) >> 3] |= (0x1 << ((i) & 7));} while (0)

// free the index
#   define lx_static_fixed_pool_used_set0(used, i)          do {(used)[(i) >> 3] &= ~(0x1 << ((i) & 7));} while (0)

// the index have been allocated?
#   define lx_static_fixed_pool_used_bset(used, i)          ((used)[(i) >> 3] & (0x1 << ((i) & 7)))

// find the first free index
#   define lx_static_fixed_pool_find_free(v)                lx_bits_fb0_le(v)

#endif

// cache the predicted index
#define lx_static_fixed_pool_cache_pred(pool, i)            do { (pool)->pred_index = ((i) >> LX_CPU_SHIFT) + 1; } while (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the static fixed pool type
typedef lx_pool_data_aligned struct __lx_static_fixed_pool_t
{
    // the real data address after the used_info info
    lx_byte_t*                  data;

    // the data tail
    lx_byte_t*                  tail;

    // the used info
    lx_byte_t*                  used_info;

    // the used info size
    lx_size_t                   info_size;

    // the predict index
    lx_size_t                   pred_index;

    // the item size
    lx_size_t                   item_size;

    // the item space: head + item_size
    lx_size_t                   item_space;

    // the item count
    lx_size_t                   item_count;

    // the item maximum count
    lx_size_t                   item_maxn;

    // the data head size
    lx_uint16_t                 data_head_size;

    // for small allocator?
    lx_uint16_t                 for_small;

#ifdef LX_DEBUG

    // the peak size
    lx_size_t                   peak_size;

    // the total size
    lx_size_t                   total_size;

    // the real size
    lx_size_t                   real_size;

    // the occupied size
    lx_size_t                   occupied_size;

    // the malloc count
    lx_size_t                   malloc_count;

    // the free count
    lx_size_t                   free_count;

    // the predict failed count
    lx_size_t                   pred_failed;

#endif

}lx_pool_data_aligned lx_static_fixed_pool_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_pool_data_empty_head_t* lx_static_fixed_pool_malloc_pred(lx_static_fixed_pool_t* pool)
{
    // check
    lx_assert_and_check_return_val(pool, lx_null);

    // done
    lx_pool_data_empty_head_t* data_head = lx_null;
    do
    {
        // exists the predict index?
        lx_check_break(pool->pred_index);

        // the predict index
        lx_size_t pred_index = pool->pred_index - 1;
        lx_assert((pred_index << LX_CPU_SHIFT) < pool->item_maxn);

        // the predict data
        lx_size_t* data = (lx_size_t*)pool->used_info + pred_index;

        // full?
        lx_check_break((*data) + 1);

        // the free bit index
        lx_size_t index = (pred_index << LX_CPU_SHIFT) + lx_static_fixed_pool_find_free(*data);

        // out of range?
        if (index >= pool->item_maxn)
        {
            // clear the pred index
            pool->pred_index = 0;
            break;
        }

        // check
        lx_assert(!lx_static_fixed_pool_used_bset(pool->used_info, index));

        // the data head
        data_head = (lx_pool_data_empty_head_t*)(pool->data + index * pool->item_space);

        // allocate it
        lx_static_fixed_pool_used_set1(pool->used_info, index);

        // the predict data is full
        if (!((*data) + 1))
        {
            // clear the predict index
            pool->pred_index = 0;

            // predict the next index
            if (index + 1 < pool->item_maxn && !lx_static_fixed_pool_used_bset(pool->used_info, index + 1))
                lx_static_fixed_pool_cache_pred(pool, index + 1);
        }

    } while (0);

#ifdef LX_DEBUG
    // update the predict failed count
    if (!data_head) pool->pred_failed++;
#endif

    // ok?
    return data_head;
}

#if 1
static lx_pool_data_empty_head_t* lx_static_fixed_pool_malloc_find(lx_static_fixed_pool_t* pool)
{
    // check
    lx_assert_and_check_return_val(pool, lx_null);

    // init
    lx_size_t   i = 0;
    lx_size_t*  p = (lx_size_t*)pool->used_info;
    lx_size_t*  e = (lx_size_t*)(pool->used_info + pool->info_size);
    lx_byte_t*  d = lx_null;

    // check align
    lx_assert_and_check_return_val(!(((lx_size_t)p) & (LX_CPU_BITBYTE - 1)), lx_null);

    // find the free chunk, item_space * 32|64 items
#ifdef __lx_small__
//  while (p < e && *p == 0xffffffff) p++;
//  while (p < e && *p == 0xffffffffffffffffL) p++;
    while (p < e && !((*p) + 1)) p++;
#else
    while (p + 7 < e)
    {
        if (p[0] + 1) { p += 0; break; }
        if (p[1] + 1) { p += 1; break; }
        if (p[2] + 1) { p += 2; break; }
        if (p[3] + 1) { p += 3; break; }
        if (p[4] + 1) { p += 4; break; }
        if (p[5] + 1) { p += 5; break; }
        if (p[6] + 1) { p += 6; break; }
        if (p[7] + 1) { p += 7; break; }
        p += 8;
    }
    while (p < e && !(*p + 1)) p++;
#endif
    lx_check_return_val(p < e, lx_null);

    // find the free bit index
    lx_size_t m = pool->item_maxn;
    i = (((lx_byte_t*)p - pool->used_info) << 3) + lx_static_fixed_pool_find_free(*p);
    lx_check_return_val(i < m, lx_null);

    // allocate it
    d = pool->data + i * pool->item_space;
    lx_static_fixed_pool_used_set1(pool->used_info, i);

    // predict this index if no full?
    if ((*p) + 1) lx_static_fixed_pool_cache_pred(pool, i);

    // ok?
    return (lx_pool_data_empty_head_t*)d;
}
#else
static lx_pool_data_empty_head_t* lx_static_fixed_pool_malloc_find(lx_static_fixed_pool_t* pool)
{
    // check
    lx_assert_and_check_return_val(pool, lx_null);

    // done
    lx_size_t   i = 0;
    lx_size_t   m = pool->item_maxn;
    lx_byte_t*  p = pool->used_info;
    lx_byte_t   u = *p;
    lx_byte_t   b = 0;
    lx_byte_t*  d = lx_null;
    for (i = 0; i < m; ++i)
    {
        // bit
        b = i & 0x07;

        // u++
        if (!b)
        {
            u = *p++;

            // skip the non-free byte
            //if (u == 0xff)
            if (!(u + 1))
            {
                i += 7;
                continue ;
            }
        }

        // is free?
        // if (!lx_static_fixed_pool_used_bset(pool->used_info, i))
        if (!(u & (0x01 << b)))
        {
            // ok
            d = pool->data + i * pool->item_space;
            // lx_static_fixed_pool_used_set1(pool->used_info, i);
            *(p - 1) |= (0x01 << b);

            // predict the next block
            if (i + 1 < m && !lx_static_fixed_pool_used_bset(pool->used_info, i + 1))
                lx_static_fixed_pool_cache_pred(pool, i + 1);

            break;
        }
    }

    // ok?
    return (lx_pool_data_empty_head_t*)d;
}
#endif

#ifdef LX_DEBUG
static lx_void_t lx_static_fixed_pool_check_data(lx_static_fixed_pool_t* pool, lx_pool_data_empty_head_t const* data_head) {
    lx_assert_and_check_return(pool && data_head);

    lx_byte_t const*    data = (lx_byte_t const*)data_head + pool->data_head_size;
    lx_size_t index = ((lx_byte_t*)data_head - pool->data) / pool->item_space;

    // check
    lx_assertf(!(((lx_byte_t*)data_head - pool->data) % pool->item_space), "the invalid data: %p", data);
    lx_assertf(lx_static_fixed_pool_used_bset(pool->used_info, index), "data have been freed: %p", data);
    lx_assertf(data_head->debug.magic == (pool->for_small? LX_POOL_DATA_MAGIC : LX_POOL_DATA_EMPTY_MAGIC), "the invalid data: %p", data);
    lx_assertf(((lx_byte_t*)data)[pool->item_size] == LX_POOL_DATA_PATCH, "data underflow");
}
static lx_void_t lx_static_fixed_pool_check_next(lx_static_fixed_pool_t* pool, lx_pool_data_empty_head_t const* data_head)
{
    // check
    lx_assert_and_check_return(pool && data_head);

    // the index
    lx_size_t index = ((lx_byte_t*)data_head - pool->data) / pool->item_space;

    // check the next data
    if (index + 1 < pool->item_maxn && lx_static_fixed_pool_used_bset(pool->used_info, index + 1))
        lx_static_fixed_pool_check_data(pool, (lx_pool_data_empty_head_t*)((lx_byte_t*)data_head + pool->item_space));
}
static lx_void_t lx_static_fixed_pool_check_prev(lx_static_fixed_pool_t* pool, lx_pool_data_empty_head_t const* data_head)
{
    // check
    lx_assert_and_check_return(pool && data_head);

    // the index
    lx_size_t index = ((lx_byte_t*)data_head - pool->data) / pool->item_space;

    // check the prev data
    if (index && lx_static_fixed_pool_used_bset(pool->used_info, index - 1))
        lx_static_fixed_pool_check_data(pool, (lx_pool_data_empty_head_t*)((lx_byte_t*)data_head - pool->item_space));
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_static_fixed_pool_ref_t lx_static_fixed_pool_init(lx_byte_t* data, lx_size_t size, lx_size_t item_size, lx_bool_t for_small)
{
    // check
    lx_assert_and_check_return_val(data && size && item_size, lx_null);

    // align data and size
    lx_size_t diff = lx_align((lx_size_t)data, LX_POOL_DATA_ALIGN) - (lx_size_t)data;
    lx_assert_and_check_return_val(size > diff + sizeof(lx_static_fixed_pool_t), lx_null);
    size -= diff;
    data += diff;

    // init pool
    lx_static_fixed_pool_t* pool = (lx_static_fixed_pool_t*)data;
    lx_memset(pool, 0, sizeof(lx_static_fixed_pool_t));

    // for small allocator?
    pool->for_small = !!for_small;
    pool->data_head_size = for_small? sizeof(lx_pool_data_head_t) : sizeof(lx_pool_data_empty_head_t);
#ifndef LX_DEBUG
    // fix data alignment, because sizeof(lx_pool_data_empty_head_t) == 1 now.
    if (!for_small) pool->data_head_size = 0;
#endif
    lx_assert_and_check_return_val(!(pool->data_head_size & (LX_POOL_DATA_ALIGN - 1)), lx_null);

#ifdef LX_DEBUG
    // init patch for checking underflow
    lx_size_t patch = 1;
#else
    lx_size_t patch = 0;
#endif

    // init the item space
    pool->item_space = pool->data_head_size + item_size + patch;
    pool->item_space = lx_align(pool->item_space, LX_POOL_DATA_ALIGN);
    lx_assert_and_check_return_val(pool->item_space > pool->data_head_size, lx_null);

    // init the used info
    pool->used_info = (lx_byte_t*)lx_align((lx_size_t)&pool[1], LX_POOL_DATA_ALIGN);
    lx_assert_and_check_return_val(data + size > pool->used_info, lx_null);

    /* init the item maxn
     *
     * used_info + maxn * item_space < left
     * align8(maxn) / 8 + maxn * item_space < left
     * (maxn + 7) / 8 + maxn * item_space < left
     * (maxn / 8) + (7 / 8) + maxn * item_space < left
     * maxn * (1 / 8 + item_space) < left - (7 / 8)
     * maxn < (left - (7 / 8)) / (1 / 8 + item_space)
     * maxn < (left * 8 - 7) / (1 + item_space * 8)
     */
    pool->item_maxn = (((data + size - pool->used_info) << 3) - 7) / (1 + (pool->item_space << 3));
    lx_assert_and_check_return_val(pool->item_maxn, lx_null);

    // init the used info size
    pool->info_size = lx_align(pool->item_maxn, LX_CPU_BITSIZE) >> 3;
    lx_assert_and_check_return_val(pool->info_size && !(pool->info_size & (LX_CPU_BITBYTE - 1)), lx_null);

    // clear the used info
    lx_memset(pool->used_info, 0, pool->info_size);

    // init data
    pool->data = (lx_byte_t*)lx_align((lx_size_t)pool->used_info + pool->info_size, LX_POOL_DATA_ALIGN);
    lx_assert_and_check_return_val(data + size > pool->data, lx_null);
    lx_assert_and_check_return_val(pool->item_maxn * pool->item_space <= (lx_size_t)(data + size - pool->data + 1), lx_null);

    // init data tail
    pool->tail = pool->data + pool->item_maxn * pool->item_space;

    // init the item size
    pool->item_size = item_size;

    // init the item count
    pool->item_count = 0;

    // init the predict index
    pool->pred_index = 1;

    // trace
    lx_trace_d("init: data: %p, size: %lu, head_size: %lu, item_size: %lu, item_maxn: %lu, item_space: %lu", pool->data, size, pool->data - (lx_byte_t*)pool, item_size, pool->item_maxn, pool->item_space);

    // ok
    return (lx_static_fixed_pool_ref_t)pool;
}
lx_void_t lx_static_fixed_pool_exit(lx_static_fixed_pool_ref_t self)
{
    // check
    lx_static_fixed_pool_t* pool = (lx_static_fixed_pool_t*)self;
    lx_assert_and_check_return(pool);

    // clear it
    lx_static_fixed_pool_clear(self);

    // exit it
    lx_memset(pool, 0, sizeof(lx_static_fixed_pool_t));
}
lx_size_t lx_static_fixed_pool_size(lx_static_fixed_pool_ref_t self)
{
    // check
    lx_static_fixed_pool_t* pool = (lx_static_fixed_pool_t*)self;
    lx_assert_and_check_return_val(pool, 0);

    // the item count
    return pool->item_count;
}
lx_size_t lx_static_fixed_pool_maxn(lx_static_fixed_pool_ref_t self)
{
    // check
    lx_static_fixed_pool_t* pool = (lx_static_fixed_pool_t*)self;
    lx_assert_and_check_return_val(pool, 0);

    // the item maximum count
    return pool->item_maxn;
}
lx_bool_t lx_static_fixed_pool_full(lx_static_fixed_pool_ref_t self)
{
    // check
    lx_static_fixed_pool_t* pool = (lx_static_fixed_pool_t*)self;
    lx_assert_and_check_return_val(pool, 0);

    // full?
    return pool->item_count == pool->item_maxn? lx_true : lx_false;
}
lx_bool_t lx_static_fixed_pool_null(lx_static_fixed_pool_ref_t self)
{
    // check
    lx_static_fixed_pool_t* pool = (lx_static_fixed_pool_t*)self;
    lx_assert_and_check_return_val(pool, 0);

    // null?
    return !pool->item_count? lx_true : lx_false;
}
lx_void_t lx_static_fixed_pool_clear(lx_static_fixed_pool_ref_t self)
{
    // check
    lx_static_fixed_pool_t* pool = (lx_static_fixed_pool_t*)self;
    lx_assert_and_check_return(pool);

    // clear used_info
    if (pool->used_info) lx_memset(pool->used_info, 0, pool->info_size);

    // clear size
    pool->item_count = 0;

    // init the predict index
    pool->pred_index = 1;

    // clear info
#ifdef LX_DEBUG
    pool->peak_size     = 0;
    pool->total_size    = 0;
    pool->real_size     = 0;
    pool->occupied_size = 0;
    pool->malloc_count  = 0;
    pool->free_count    = 0;
    pool->pred_failed   = 0;
#endif
}
lx_pointer_t lx_static_fixed_pool_malloc(lx_static_fixed_pool_ref_t self)
{
    // check
    lx_static_fixed_pool_t* pool = (lx_static_fixed_pool_t*)self;
    lx_assert_and_check_return_val(pool && pool->item_space, lx_null);

    // done
    lx_pointer_t                data = lx_null;
    lx_pool_data_empty_head_t*  data_head = lx_null;
    do
    {
        // full?
        lx_check_break(pool->item_count < pool->item_maxn);

        // predict it?
        data_head = lx_static_fixed_pool_malloc_pred(pool);

        // find it
        if (!data_head) data_head = lx_static_fixed_pool_malloc_find(pool);
        lx_check_break(data_head);

        // the real data
        data = (lx_byte_t*)data_head + pool->data_head_size;

        // save the real size
        if (pool->for_small) ((lx_pool_data_head_t*)data_head)->size = pool->item_size;

        // count++
        pool->item_count++;

#ifdef LX_DEBUG

        // init the debug info
        data_head->debug.magic     = pool->for_small? LX_POOL_DATA_MAGIC : LX_POOL_DATA_EMPTY_MAGIC;

        // make the dirty data and patch 0xcc for checking underflow
        lx_memset(data, LX_POOL_DATA_PATCH, pool->item_space - pool->data_head_size);

        // update the real size
        pool->real_size     += pool->item_size;

        // update the occupied size
        pool->occupied_size += pool->item_space - LX_POOL_DATA_HEAD_DIFF_SIZE - 1;

        // update the total size
        pool->total_size    += pool->item_size;

        // update the peak size
        if (pool->total_size > pool->peak_size) pool->peak_size = pool->total_size;

        // update the malloc count
        pool->malloc_count++;

        // check the prev data
        lx_static_fixed_pool_check_prev(pool, data_head);

        // check the next data
        lx_static_fixed_pool_check_next(pool, data_head);
#endif

    } while (0);

    // check
    lx_assertf(data, "malloc(%lu) failed!", pool->item_size);
    lx_assertf(!(((lx_size_t)data) & (LX_POOL_DATA_ALIGN - 1)), "malloc(%lu): unaligned data: %p", pool->item_size, data);

    // ok?
    return data;
}
lx_bool_t lx_static_fixed_pool_free(lx_static_fixed_pool_ref_t self, lx_pointer_t data)
{
    // check
    lx_static_fixed_pool_t* pool = (lx_static_fixed_pool_t*)self;
    lx_assert_and_check_return_val(pool && pool->item_space, lx_false);

    // done
    lx_bool_t                   ok = lx_false;
    lx_pool_data_empty_head_t*  data_head = (lx_pool_data_empty_head_t*)((lx_byte_t*)data - pool->data_head_size);
    do
    {
        // the index
        lx_size_t index = ((lx_byte_t*)data_head - pool->data) / pool->item_space;

        // check
        lx_assertf_and_check_break((lx_byte_t*)data_head >= pool->data && (lx_byte_t*)data_head + pool->item_space <= pool->tail, "the data: %p not belong to pool: %p", data, pool);
        lx_assertf(!(((lx_byte_t*)data_head - pool->data) % pool->item_space), "free the invalid data: %p", data);
        lx_assertf_and_check_break(pool->item_count, "double free data: %p", data);
        lx_assertf_and_check_break(lx_static_fixed_pool_used_bset(pool->used_info, index), "double free data: %p", data);
        lx_assertf(data_head->debug.magic == (pool->for_small? LX_POOL_DATA_MAGIC : LX_POOL_DATA_EMPTY_MAGIC), "the invalid data: %p", data);
        lx_assertf(((lx_byte_t*)data)[pool->item_size] == LX_POOL_DATA_PATCH, "data underflow");

#ifdef LX_DEBUG
        // check the prev data
        lx_static_fixed_pool_check_prev(pool, data_head);

        // check the next data
        lx_static_fixed_pool_check_next(pool, data_head);

        // update the total size
        pool->total_size -= pool->item_size;

        // update the free count
        pool->free_count++;
#endif

        // free it
        lx_static_fixed_pool_used_set0(pool->used_info, index);

        // predict it if no cache
        if (!pool->pred_index) lx_static_fixed_pool_cache_pred(pool, index);

        // size--
        pool->item_count--;

        // ok
        ok = lx_true;

    } while (0);

    return ok;
}
lx_void_t lx_static_fixed_pool_walk(lx_static_fixed_pool_ref_t self, lx_fixed_pool_item_walk_func_t func, lx_cpointer_t priv)
{
    // check
    lx_static_fixed_pool_t* pool = (lx_static_fixed_pool_t*)self;
    lx_assert_and_check_return(pool && pool->item_maxn && pool->item_space && func);

    // walk
    lx_size_t   i = 0;
    lx_size_t   m = pool->item_maxn;
    lx_byte_t*  p = pool->used_info;
    lx_byte_t*  d = pool->data + pool->data_head_size;
    lx_byte_t   u = *p;
    lx_byte_t   b = 0;
    for (i = 0; i < m; ++i)
    {
        // bit
        b = i & 0x07;

        // u++
        if (!b)
        {
            u = *p++;

            // this byte is all occupied?
            //if (u == 0xff)
            if (!(u + 1))
            {
                // done func
                func(d + (i + 0) * pool->item_space, priv);
                func(d + (i + 1) * pool->item_space, priv);
                func(d + (i + 2) * pool->item_space, priv);
                func(d + (i + 3) * pool->item_space, priv);
                func(d + (i + 4) * pool->item_space, priv);
                func(d + (i + 5) * pool->item_space, priv);
                func(d + (i + 6) * pool->item_space, priv);
                func(d + (i + 7) * pool->item_space, priv);

                // skip this byte and continue it
                i += 7;
                continue ;
            }
        }

        // is occupied?
        // if (lx_static_fixed_pool_used_bset(pool->used_info, i))
        if ((u & (0x01 << b)))
        {
            // done func
            func(d + i * pool->item_space, priv);
        }
    }
}
#ifdef LX_DEBUG
lx_void_t lx_static_fixed_pool_dump(lx_static_fixed_pool_ref_t self)
{
    // check
    lx_static_fixed_pool_t* pool = (lx_static_fixed_pool_t*)self;
    lx_assert_and_check_return(pool && pool->used_info);

    // dump
    lx_size_t index = 0;
    for (index = 0; index < pool->item_maxn; ++index)
    {
        // leak?
        if (lx_static_fixed_pool_used_bset(pool->used_info, index))
        {
            // the data head
            lx_pool_data_empty_head_t* data_head = (lx_pool_data_empty_head_t*)(pool->data + index * pool->item_space);

            // check it
            lx_static_fixed_pool_check_data(pool, data_head);

            // the data
            lx_byte_t const* data = (lx_byte_t const*)data_head + pool->data_head_size;

            // trace
            lx_trace_e("leak: %p", data);
        }
    }

    // trace debug info
    lx_trace_i("[%lu]: peak_size: %lu, wast_rate: %llu/10000, pred_failed: %lu, item_maxn: %lu, free_count: %lu, malloc_count: %lu"
            ,   pool->item_size
            ,   pool->peak_size
            ,   pool->occupied_size? (((lx_hize_t)pool->occupied_size - pool->real_size) * 10000) / (lx_hize_t)pool->occupied_size : 0
            ,   pool->pred_failed
            ,   pool->item_maxn
            ,   pool->free_count
            ,   pool->malloc_count);

}
#endif
