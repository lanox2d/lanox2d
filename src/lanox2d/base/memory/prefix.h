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
 * @file        prefix.h
 *
 */
#ifndef LX_BASE_MEMORY_PREFIX_H
#define LX_BASE_MEMORY_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the pool data empty magic number
#define LX_POOL_DATA_EMPTY_MAGIC            (0xdeaf)

// the pool data patch value
#define LX_POOL_DATA_PATCH                  (0xcc)

// the pool data size maximum
#define LX_POOL_DATA_SIZE_MAXN              (LX_MAXU32)

// the pool data address alignment
#define LX_POOL_DATA_ALIGN                  LX_CPU_BITBYTE

// the pool data alignment keyword
#define lx_pool_data_aligned                lx_cpu_aligned

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

#ifdef LX_DEBUG
// the pool data debug head type
typedef lx_pool_data_aligned struct lx_pool_data_debug_head_t_ {
    /* the magic
     *
     * @note the address may be not accessed if we place the magic to head.
     */
    lx_uint16_t                 magic;
}lx_pool_data_aligned lx_pool_data_debug_head_t;
#endif

// the pool data empty head type
typedef struct lx_pool_data_empty_head_t_ {
#ifdef LX_DEBUG
    lx_pool_data_debug_head_t   debug;
#endif
}lx_pool_data_empty_head_t;

#endif


