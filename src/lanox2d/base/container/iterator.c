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
 * @file        iterator.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "iterator.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

lx_void_t lx_iterator_of(lx_iterator_ref_t iterator, lx_cpointer_t container) {
    lx_assert(iterator && container);
    ((lx_iterator_base_t*)container)->iterator_of(iterator, container);
}

lx_size_t lx_iterator_mode(lx_iterator_ref_t iterator) {
    return iterator->mode;
}

lx_size_t lx_iterator_head(lx_iterator_ref_t iterator) {
    lx_assert(iterator && iterator->op && iterator->op->head);
    return iterator->op->head(iterator);
}

lx_size_t lx_iterator_tail(lx_iterator_ref_t iterator) {
    lx_assert(iterator && iterator->op && iterator->op->tail);
    return iterator->op->tail(iterator);
}

lx_size_t lx_iterator_prev(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_assert(iterator && iterator->op && iterator->op->prev);
    return iterator->op->prev(iterator, itor);
}

lx_size_t lx_iterator_next(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_assert(iterator && iterator->op && iterator->op->next);
    return iterator->op->next(iterator, itor);
}

lx_size_t lx_iterator_size(lx_iterator_ref_t iterator) {
    lx_assert(iterator && iterator->op && iterator->op->size);
    return iterator->op->size(iterator);
}

lx_pointer_t lx_iterator_item(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_assert(iterator && iterator->op && iterator->op->item);
    return iterator->op->item(iterator, itor);
}

lx_long_t lx_iterator_comp(lx_iterator_ref_t iterator, lx_cpointer_t litem, lx_cpointer_t ritem) {
    lx_assert(iterator && iterator->op && iterator->op->comp);
    return iterator->op->comp(iterator, litem, ritem);
}

lx_void_t lx_iterator_remove(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_assert(iterator && iterator->op && iterator->op->remove);
    return iterator->op->remove(iterator, itor);
}
