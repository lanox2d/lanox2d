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
 * @file        allocator.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "allocator.h"
#include <stdlib.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * malloc allocator
 */
static lx_pointer_t lx_malloc_allocator_malloc(lx_allocator_ref_t allocator, lx_size_t size) {
    return malloc(size);
}

static lx_pointer_t lx_malloc_allocator_malloc0(lx_allocator_ref_t allocator, lx_size_t size) {
    return calloc(1, size);
}

static lx_pointer_t lx_malloc_allocator_ralloc(lx_allocator_ref_t allocator, lx_pointer_t data, lx_size_t size) {
    return realloc(data, size);
}

static lx_void_t lx_malloc_allocator_free(lx_allocator_ref_t allocator, lx_pointer_t data) {
    free(data);
}


/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the malloc allocator
static lx_allocator_t     g_allocator_malloc = {
    lx_malloc_allocator_malloc,
    lx_malloc_allocator_malloc0,
    lx_malloc_allocator_ralloc,
    lx_malloc_allocator_free
};

// the global allocator
static lx_allocator_ref_t g_allocator = &g_allocator_malloc;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

lx_allocator_ref_t lx_allocator() {
    return g_allocator;
}

lx_void_t lx_allocator_set(lx_allocator_ref_t allocator) {
    g_allocator = allocator;
}

lx_pointer_t lx_allocator_malloc(lx_allocator_ref_t allocator, lx_size_t size) {
    return allocator->malloc(allocator, size);
}

lx_pointer_t lx_allocator_malloc0(lx_allocator_ref_t allocator, lx_size_t size) {
    return allocator->malloc0(allocator, size);
}

lx_pointer_t lx_allocator_nalloc(lx_allocator_ref_t allocator, lx_size_t item, lx_size_t size) {
    return lx_allocator_malloc(allocator, item * size);
}

lx_pointer_t lx_allocator_nalloc0(lx_allocator_ref_t allocator, lx_size_t item, lx_size_t size) {
    return lx_allocator_malloc0(allocator, item * size);
}

lx_pointer_t lx_allocator_ralloc(lx_allocator_ref_t allocator, lx_pointer_t data, lx_size_t size) {
    return allocator->ralloc(allocator, data, size);
}

lx_void_t lx_allocator_free(lx_allocator_ref_t allocator, lx_pointer_t data) {
    allocator->free(allocator, data);
}


