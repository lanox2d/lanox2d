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
 * @file        malloc.h
 *
 */
#ifndef LX_PREFIX_MALLOC_H
#define LX_PREFIX_MALLOC_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define lx_free(data)                               lx_allocator_free(lx_allocator(), (lx_pointer_t)data)
#define lx_malloc(size)                             lx_allocator_malloc(lx_allocator(), size)
#define lx_malloc0(size)                            lx_allocator_malloc0(lx_allocator(), size)
#define lx_nalloc(item, size)                       lx_allocator_nalloc(lx_allocator(), item, size)
#define lx_nalloc0(item, size)                      lx_allocator_nalloc0(lx_allocator(), item, size)
#define lx_ralloc(data, size)                       lx_allocator_ralloc(lx_allocator(), (lx_pointer_t)data, size)

#define lx_malloc_cstr(size)                        (lx_char_t*)lx_allocator_malloc(lx_allocator(), size)
#define lx_malloc0_cstr(size)                       (lx_char_t*)lx_allocator_malloc0(lx_allocator(), size)
#define lx_nalloc_cstr(item, size)                  (lx_char_t*)lx_allocator_nalloc(lx_allocator(), item, size)
#define lx_nalloc0_cstr(item, size)                 (lx_char_t*)lx_allocator_nalloc0(lx_allocator(), item, size)
#define lx_ralloc_cstr(data, size)                  (lx_char_t*)lx_allocator_ralloc(lx_allocator(), (lx_pointer_t)data, size)

#define lx_malloc_bytes(size)                       (lx_byte_t*)lx_allocator_malloc(lx_allocator(), size)
#define lx_malloc0_bytes(size)                      (lx_byte_t*)lx_allocator_malloc0(lx_allocator(), size)
#define lx_nalloc_bytes(item, size)                 (lx_byte_t*)lx_allocator_nalloc(lx_allocator(), item, size)
#define lx_nalloc0_bytes(item, size)                (lx_byte_t*)lx_allocator_nalloc0(lx_allocator(), item, size)
#define lx_ralloc_bytes(data, size)                 (lx_byte_t*)lx_allocator_ralloc(lx_allocator(), (lx_pointer_t)data, size)

#define lx_malloc_type(type)                        (type*)lx_allocator_malloc(lx_allocator(), sizeof(type))
#define lx_malloc0_type(type)                       (type*)lx_allocator_malloc0(lx_allocator(), sizeof(type))
#define lx_nalloc_type(item, type)                  (type*)lx_allocator_nalloc(lx_allocator(), item, sizeof(type))
#define lx_nalloc0_type(item, type)                 (type*)lx_allocator_nalloc0(lx_allocator(), item, sizeof(type))
#define lx_ralloc_type(data, item, type)            (type*)lx_allocator_ralloc(lx_allocator(), (lx_pointer_t)data, ((item) * sizeof(type)))

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */

struct lx_allocator_t_;
struct lx_allocator_t_*     lx_allocator(lx_noarg_t);
lx_pointer_t                lx_allocator_malloc(struct lx_allocator_t_* allocator, lx_size_t size);
lx_pointer_t                lx_allocator_malloc0(struct lx_allocator_t_* allocator, lx_size_t size);
lx_pointer_t                lx_allocator_nalloc(struct lx_allocator_t_* allocator, lx_size_t item, lx_size_t size);
lx_pointer_t                lx_allocator_nalloc0(struct lx_allocator_t_* allocator, lx_size_t item, lx_size_t size);
lx_pointer_t                lx_allocator_ralloc(struct lx_allocator_t_* allocator, lx_pointer_t data, lx_size_t size);
lx_void_t                   lx_allocator_free(struct lx_allocator_t_* allocator, lx_pointer_t data);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


