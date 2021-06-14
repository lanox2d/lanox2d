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
 * @file        list_entry.h
 *
 */
#ifndef LX_BASE_CONTAINER_LIST_ENTRY_H
#define LX_BASE_CONTAINER_LIST_ENTRY_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "iterator.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/// get the list entry
#define lx_list_entry(head, entry)      ((((lx_byte_t*)(entry)) - (head)->eoff))

/*! get the list entry with zero offset
 *
 * @code
 *
    // the xxxx entry type
    typedef struct lx_xxxx_entry_t_ {
        // the list entry (be placed in header with zero offset)
        lx_list_entry_t     entry;
        // ..
    }lx_xxxx_entry_t;
 *
 * @endcode
 */
#define lx_list_entry0(entry)           (entry)

/*! init the list entry
 *
 * @code
 *
    // the xxxx entry type
    typedef struct lx_xxxx_entry_t_ {
        lx_list_entry_t     entry;
        lx_size_t           data;
    }lx_xxxx_entry_t;

    // init the list
    lx_list_entry_head_t list;
    lx_list_entry_init(&list, lx_xxxx_entry_t, entry);

 * @endcode
 */
#define lx_list_entry_init(list, type, entry)     lx_list_entry_init_(list, lx_offsetof(type, entry), sizeof(type))

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the doubly-linked list entry type
 *
 * <pre>
 * list: list => ... => last
 *        |               |
 *        <---------------
 *
 * </pre>
 */
typedef struct lx_list_entry_t_ {
    struct lx_list_entry_t_*   next;
    struct lx_list_entry_t_*   prev;
}lx_list_entry_t, *lx_list_entry_ref_t;

/// the list entry head type
typedef struct lx_list_entry_head_t_ {
    struct lx_list_entry_t_*    next;
    struct lx_list_entry_t_*    prev;
    lx_iterator_base_t          itor;
    lx_size_t                   size;
    lx_size_t                   eoff;
}lx_list_entry_head_t, *lx_list_entry_head_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init list
 *
 * @param list                          the list
 * @param entry_offset                  the entry offset
 * @param entry_size                    the entry size
 */
lx_void_t                               lx_list_entry_init_(lx_list_entry_head_ref_t list, lx_size_t entry_offset, lx_size_t entry_size);

/*! exit list
 *
 * @param list                          the list
 */
lx_void_t                               lx_list_entry_exit(lx_list_entry_head_ref_t list);

/*! get the list iterator
 *
 * @param list                          the list
 *
 * @return                              the list iterator
 */
static lx_inline lx_iterator_base_t*    lx_list_entry_itor(lx_list_entry_head_ref_t list) {
    lx_assert(list);
    return &list->itor;
}

/*! clear list
 *
 * @param list                          the list
 */
static lx_inline lx_void_t              lx_list_entry_clear(lx_list_entry_head_ref_t list) {
    lx_assert(list);
    list->next = (lx_list_entry_ref_t)list;
    list->prev = (lx_list_entry_ref_t)list;
    list->size = 0;
}

/*! the list entry count
 *
 * @param list                          the list
 *
 * @return                              the list entry count
 */
static lx_inline lx_size_t              lx_list_entry_size(lx_list_entry_head_ref_t list) {
    lx_assert(list);
    return list->size;
}

/*! the list next entry
 *
 * @param entry                         the entry
 *
 * @return                              the next entry
 */
static lx_inline lx_list_entry_ref_t    lx_list_entry_next(lx_list_entry_ref_t entry) {
    lx_assert(entry);
    return entry->next;
}

/*! the list prev entry
 *
 * @param entry                         the entry
 *
 * @return                              the prev entry
 */
static lx_inline lx_list_entry_ref_t    lx_list_entry_prev(lx_list_entry_ref_t entry) {
    lx_assert(entry);
    return entry->prev;
}

/*! the list head entry
 *
 * @param list                         the list
 *
 * @return                             the head entry
 */
static lx_inline lx_list_entry_ref_t    lx_list_entry_head(lx_list_entry_head_ref_t list) {
    lx_assert(list);
    return list->next;
}

/*! the list last entry
 *
 * @param list                          the list
 *
 * @return                              the last entry
 */
static lx_inline lx_list_entry_ref_t    lx_list_entry_last(lx_list_entry_head_ref_t list) {
    lx_assert(list);
    return list->prev;
}

/*! the list tail entry
 *
 * @param list                          the list
 *
 * @return                              the tail entry
 */
static lx_inline lx_list_entry_ref_t    lx_list_entry_tail(lx_list_entry_head_ref_t list) {
    return (lx_list_entry_ref_t)list;
}

/*! the list is empty?
 *
 * @param list                          the list
 *
 * @return                              lx_true or lx_false
 */
static lx_inline lx_bool_t              lx_list_entry_is_empty(lx_list_entry_head_ref_t list) {
    lx_assert(list);
    return !list->size;
}

/*! is the list head entry?
 *
 * @param list                          the list
 * @param entry                         the entry
 *
 * @return                              lx_true or lx_false
 */
static lx_inline lx_bool_t              lx_list_entry_is_head(lx_list_entry_head_ref_t list, lx_list_entry_ref_t entry) {
    lx_assert(list);
    return list->next == entry;
}

/*! is the list last entry?
 *
 * @param list                          the list
 * @param entry                         the entry
 *
 * @return                              lx_true or lx_false
 */
static lx_inline lx_bool_t              lx_list_entry_is_last(lx_list_entry_head_ref_t list, lx_list_entry_ref_t entry) {
    lx_assert(list);
    return list->prev == entry;
}

/*! the list is valid?
 *
 * @param list                          the list
 *
 * @return                              lx_true or lx_false
 */
static lx_inline lx_bool_t              lx_list_entry_is_valid(lx_list_entry_head_ref_t list) {
    lx_assert(list);
    return (list->next && list->next->prev == (lx_list_entry_ref_t)list) && (list->prev && list->prev->next == (lx_list_entry_ref_t)list);
}

/*! splice the spliced_list to list[prev, next]
 *
 * @param list                          the list
 * @param prev                          the prev
 * @param next                          the next
 * @param spliced_list                  the spliced list
 */
static lx_inline lx_void_t              lx_list_entry_splice(lx_list_entry_head_ref_t list, lx_list_entry_ref_t prev, lx_list_entry_ref_t next, lx_list_entry_head_ref_t spliced_list) {
    lx_assert(list && prev && next);
    lx_assert(spliced_list && spliced_list->next && spliced_list->prev);

    // valid?
    lx_assert(lx_list_entry_is_valid(list));
    lx_assert(lx_list_entry_is_valid(spliced_list));

    // empty?
    lx_check_return(!lx_list_entry_is_empty(spliced_list));

    // done
    spliced_list->next->prev    = prev;
    prev->next                  = spliced_list->next;
    spliced_list->prev->next    = next;
    next->prev                  = spliced_list->prev;

    // update size
    list->size += spliced_list->size;

    // clear the spliced list
    lx_list_entry_clear(spliced_list);
}

/*! splice the spliced_list at the list head
 *
 * @param list                          the list
 * @param spliced_list                  the spliced list
 */
static lx_inline lx_void_t              lx_list_entry_splice_head(lx_list_entry_head_ref_t list, lx_list_entry_head_ref_t spliced_list) {
    lx_assert(list);
    lx_list_entry_splice(list, (lx_list_entry_ref_t)list, list->next, spliced_list);
}

/*! splice the spliced_list at the list tail
 *
 * @param list                          the list
 * @param spliced_list                  the spliced list
 */
static lx_inline lx_void_t              lx_list_entry_splice_tail(lx_list_entry_head_ref_t list, lx_list_entry_head_ref_t spliced_list) {
    lx_assert(list);
    lx_list_entry_splice(list, list->prev, (lx_list_entry_ref_t)list, spliced_list);
}

/*! insert entry to the next
 *
 * @param list                          the list
 * @param node                          the list node
 * @param entry                         the inserted list entry
 */
static lx_inline lx_void_t              lx_list_entry_insert_next(lx_list_entry_head_ref_t list, lx_list_entry_ref_t node, lx_list_entry_ref_t entry) {
    lx_assert(list && node && node->next && entry);
    lx_assert(node != entry);
    lx_assert(lx_list_entry_is_valid(list));

    node->next->prev    = entry;
    entry->next         = node->next;
    entry->prev         = node;
    node->next          = entry;
    list->size++;
}

/*! insert entry to the prev
 *
 * @param list                          the list
 * @param node                          the list node
 * @param entry                         the inserted list entry
 */
static lx_inline lx_void_t              lx_list_entry_insert_prev(lx_list_entry_head_ref_t list, lx_list_entry_ref_t node, lx_list_entry_ref_t entry) {
    lx_assert(list && node);
    lx_list_entry_insert_next(list, node->prev, entry);
}

/*! insert entry to the head
 *
 * @param list                          the list
 * @param entry                         the inserted list entry
 */
static lx_inline lx_void_t              lx_list_entry_insert_head(lx_list_entry_head_ref_t list, lx_list_entry_ref_t entry) {
    lx_list_entry_insert_next(list, (lx_list_entry_ref_t)list, entry);
}

/*! insert entry to the tail
 *
 * @param list                          the list
 * @param entry                         the inserted list entry
 */
static lx_inline lx_void_t              lx_list_entry_insert_tail(lx_list_entry_head_ref_t list, lx_list_entry_ref_t entry) {
    lx_assert(list && entry);
    lx_list_entry_insert_next(list, list->prev, entry);
}

/*! replace the entry
 *
 * @param list                          the list
 * @param node                          the replaced list node
 * @param entry                         the new list entry
 */
static lx_inline lx_void_t              lx_list_entry_replace(lx_list_entry_head_ref_t list, lx_list_entry_ref_t node, lx_list_entry_ref_t entry) {
    lx_assert(node && node->next && node->prev && entry);
    lx_assert(node != entry);
    lx_assert(lx_list_entry_is_valid(list));

    entry->next         = node->next;
    entry->next->prev   = entry;
    entry->prev         = node->prev;
    entry->prev->next   = entry;
}

/*! replace the next entry
 *
 * @param list                          the list
 * @param node                          the list node
 * @param entry                         the new list entry
 */
static lx_inline lx_void_t              lx_list_entry_replace_next(lx_list_entry_head_ref_t list, lx_list_entry_ref_t node, lx_list_entry_ref_t entry) {
    lx_assert(node);
    lx_list_entry_replace(list, node->next, entry);
}

/*! replace the prev entry
 *
 * @param list                          the list
 * @param node                          the list node
 * @param entry                         the new list entry
 */
static lx_inline lx_void_t              lx_list_entry_replace_prev(lx_list_entry_head_ref_t list, lx_list_entry_ref_t node, lx_list_entry_ref_t entry) {
    lx_assert(node);
    lx_list_entry_replace(list, node->prev, entry);
}

/*! replace the head entry
 *
 * @param list                          the list
 * @param entry                         the new list entry
 */
static lx_inline lx_void_t              lx_list_entry_replace_head(lx_list_entry_head_ref_t list, lx_list_entry_ref_t entry) {
    lx_assert(list);
    lx_list_entry_replace(list, list->next, entry);
}

/*! replace the last entry
 *
 * @param list                          the list
 * @param entry                         the new list entry
 */
static lx_inline lx_void_t              lx_list_entry_replace_last(lx_list_entry_head_ref_t list, lx_list_entry_ref_t entry) {
    lx_assert(list);
    lx_list_entry_replace(list, list->prev, entry);
}

/*! remove the entry (private interface)
 *
 * @param list                          the list
 * @param prev                          the prev entry
 * @param next                          the next entry
 */
static lx_inline lx_void_t              lx_list_entry_remove_(lx_list_entry_head_ref_t list, lx_list_entry_ref_t prev, lx_list_entry_ref_t next) {
    lx_assert(list && list->size && prev && next);
    lx_assert(lx_list_entry_is_valid(list));

    prev->next = next;
    next->prev = prev;
    list->size--;
}

/*! remove the entry
 *
 * @param list                          the list
 * @param entry                         the removed list entry
 */
static lx_inline lx_void_t              lx_list_entry_remove(lx_list_entry_head_ref_t list, lx_list_entry_ref_t entry) {
    lx_assert(entry);
    lx_list_entry_remove_(list, entry->prev, entry->next);
}

/*! remove the next entry
 *
 * @param list                          the list
 * @param prev                          the prev entry
 */
static lx_inline lx_void_t              lx_list_entry_remove_next(lx_list_entry_head_ref_t list, lx_list_entry_ref_t prev) {
    lx_assert(prev && prev->next);
    lx_list_entry_remove_(list, prev, prev->next->next);
}

/*! remove the prev entry
 *
 * @param list                          the list
 * @param next                          the next entry
 */
static lx_inline lx_void_t              lx_list_entry_remove_prev(lx_list_entry_head_ref_t list, lx_list_entry_ref_t next) {
    lx_assert(next && next->prev);
    lx_list_entry_remove_(list, next->prev->prev, next);
}

/*! remove the head entry
 *
 * @param list                          the list
 */
static lx_inline lx_void_t              lx_list_entry_remove_head(lx_list_entry_head_ref_t list) {
    lx_assert(list && list->next);
    lx_list_entry_remove_(list, (lx_list_entry_ref_t)list, list->next->next);
}

/*! remove the last entry
 *
 * @param list                          the list
 */
static lx_inline lx_void_t              lx_list_entry_remove_last(lx_list_entry_head_ref_t list) {
    lx_assert(list && list->prev);
    lx_list_entry_remove_(list, list->prev->prev, (lx_list_entry_ref_t)list);
}

/*! moveto the next entry
 *
 * @param list                          the list
 * @param node                          the list node
 * @param entry                         the moved list entry
 */
static lx_inline lx_void_t              lx_list_entry_moveto_next(lx_list_entry_head_ref_t list, lx_list_entry_ref_t node, lx_list_entry_ref_t entry) {
    lx_check_return(node != entry);

    lx_list_entry_remove(list, entry);
    lx_list_entry_insert_next(list, node, entry);
}

/*! moveto the prev entry
 *
 * @param list                          the list
 * @param node                          the list node
 * @param entry                         the moved list entry
 */
static lx_inline lx_void_t              lx_list_entry_moveto_prev(lx_list_entry_head_ref_t list, lx_list_entry_ref_t node, lx_list_entry_ref_t entry) {
    lx_assert(node);
    lx_list_entry_moveto_next(list, node->prev, entry);
}

/*! moveto the head entry
 *
 * @param list                          the list
 * @param entry                         the moved list entry
 */
static lx_inline lx_void_t              lx_list_entry_moveto_head(lx_list_entry_head_ref_t list, lx_list_entry_ref_t entry) {
    lx_list_entry_moveto_next(list, (lx_list_entry_ref_t)list, entry);
}

/*! moveto the tail entry
 *
 * @param list                          the list
 * @param entry                         the moved list entry
 */
static lx_inline lx_void_t              lx_list_entry_moveto_tail(lx_list_entry_head_ref_t list, lx_list_entry_ref_t entry) {
    lx_assert(list);
    lx_list_entry_moveto_next(list, list->prev, entry);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif

