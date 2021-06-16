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
 * @file        edge_list.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "edge_list.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the mesh edge list grow
#ifdef LX_CONFIG_SMALL
#   define LX_MESH_EDGE_LIST_GROW               (128)
#else
#   define LX_MESH_EDGE_LIST_GROW               (256)
#endif

// the mesh edge list maxn
#ifdef LX_CONFIG_SMALL
#   define LX_MESH_EDGE_LIST_MAXN               (1 << 16)
#else
#   define LX_MESH_EDGE_LIST_MAXN               (1 << 30)
#endif

// the edge user data pointer
#define lx_mesh_edge_user(edge)                 ((lx_mesh_edge_ref_t)(edge) + 1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the mesh edge list list type
typedef struct lx_mesh_edge_list_t {
    lx_iterator_base_t      base;
    lx_mesh_edge_t          head[2];
    lx_size_t               edge_size;
    lx_size_t               order;
    lx_size_t               itemsize;
    lx_element_free_t       itemfree;
#ifdef LX_DEBUG
    lx_size_t               id;
#endif
}lx_mesh_edge_list_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_void_t lx_mesh_edge_exit(lx_pointer_t data, lx_cpointer_t udata)
{
    // check
    lx_mesh_edge_list_t* list = (lx_mesh_edge_list_t*)udata;
    lx_assert_and_check_return(list && data);

    // exit the user data
    list->element.free(&list->element, (lx_pointer_t)((lx_mesh_edge_ref_t)data + 1));
    list->element.free(&list->element, (lx_pointer_t)((lx_mesh_edge_ref_t)((lx_byte_t*)data + list->edge_size) + 1));
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_inline lx_void_t lx_mesh_edge_init(lx_mesh_edge_ref_t edge)
{
    // check
    lx_assert(edge);

    // the sym edge
    lx_mesh_edge_ref_t edge_sym = edge->sym;
    lx_assert(edge_sym && edge < edge_sym);

    /* init edge and make self-loop
     *
     *  edge: ..............e.............. : edge_sym
     *    |                                     /|\
     *   \|/                                     |
     *  edge: ..............e.............. : edge_sym
     *
     */
    edge->next     = edge;
    edge_sym->next = edge_sym;
}
static lx_inline lx_void_t lx_mesh_edge_insert_prev(lx_mesh_edge_ref_t edge, lx_mesh_edge_ref_t edge_next)
{
    // check
    lx_assert(edge && edge_next);

    // the sym edge
    lx_mesh_edge_ref_t edge_sym = edge->sym;
    lx_assert(edge_sym && edge < edge_sym);

    // the next sym edge
    lx_mesh_edge_ref_t edge_next_sym = edge_next->sym;
    lx_assert(edge_next_sym && edge_next < edge_next_sym);

    // the prev sym edge
    lx_mesh_edge_ref_t edge_prev_sym = edge_next_sym->next;
    lx_assert(edge_prev_sym && edge_prev_sym->sym);

    /* insert edge before the next edge
     *
     *  edge_prev : ..............e.............. : edge_prev_sym
     *       |    |                             /|\    /|\
     *       |   \|/                             |      |
     *          edge : ...........e..........: edge_sym
     *       |    |                             /|\     |
     *      \|/  \|/                             |      |
     *  edge_next : ..............e.............. : edge_next_sym
     *
     */
    edge_sym->next              = edge_prev_sym;
    edge_prev_sym->sym->next    = edge;
    edge->next                  = edge_next;
    edge_next_sym->next         = edge_sym;
}
static lx_inline lx_void_t lx_mesh_edge_remove_done(lx_mesh_edge_ref_t edge)
{
    // check
    lx_assert(edge);

    // the sym edge
    lx_mesh_edge_ref_t edge_sym = edge->sym;
    lx_assert(edge_sym && edge < edge_sym);

    // the next edge
    lx_mesh_edge_ref_t edge_next = edge->next;
    lx_assert(edge_next && edge_next->sym);

    // the prev sym edge
    lx_mesh_edge_ref_t edge_prev_sym = edge_sym->next;
    lx_assert(edge_prev_sym && edge_prev_sym->sym);

    /* remove edge
     *
     *  edge_prev : ..............e.............. : edge_prev_sym
     *       |    |                             /|\    /|\
     *       |   \|/                             |      |
     *       |  edge : ...........e..........: edge_sym |
     *       |    |                             /|\     |
     *      \|/  \|/                             |      |
     *  edge_next : ..............e.............. : edge_next_sym
     *
     */
    edge_next->sym->next = edge_prev_sym;
    edge_prev_sym->sym->next = edge_next;
}
static lx_size_t lx_mesh_edge_itor_size(lx_iterator_ref_t iterator)
{
    // the size
    return lx_mesh_edge_list_size((lx_mesh_edge_list_ref_t)iterator);
}
static lx_size_t lx_mesh_edge_itor_head(lx_iterator_ref_t iterator)
{
    // check
    lx_mesh_edge_list_t* list = (lx_mesh_edge_list_t*)iterator;
    lx_assert(list);

    // head
    return (lx_size_t)list->head[0].next;
}
static lx_size_t lx_mesh_edge_itor_last(lx_iterator_ref_t iterator)
{
    // check
    lx_mesh_edge_list_t* list = (lx_mesh_edge_list_t*)iterator;
    lx_assert(list && list->head[1].next);

    // last
    return (lx_size_t)(list->head[1].next->sym);
}
static lx_size_t lx_mesh_edge_itor_tail(lx_iterator_ref_t iterator)
{
    // check
    lx_mesh_edge_list_t* list = (lx_mesh_edge_list_t*)iterator;
    lx_assert(list);

    // tail
    return (lx_size_t)list->head;
}
static lx_size_t lx_mesh_edge_itor_next(lx_iterator_ref_t iterator, lx_size_t itor)
{
    // check
    lx_assert(itor);

    // next
    return (lx_size_t)(((lx_mesh_edge_ref_t)itor)->next);
}
static lx_size_t lx_mesh_edge_itor_prev(lx_iterator_ref_t iterator, lx_size_t itor)
{
    // check
    lx_assert(itor && ((lx_mesh_edge_ref_t)itor)->sym && ((lx_mesh_edge_ref_t)itor)->sym->next);

    // prev
    return (lx_size_t)(((lx_mesh_edge_ref_t)itor)->sym->next->sym);
}
static lx_pointer_t lx_mesh_edge_itor_item(lx_iterator_ref_t iterator, lx_size_t itor)
{
    // check
    lx_assert(itor);

    // data
    return (lx_pointer_t)itor;
}

static lx_void_t lx_mesh_edge_iterator_of(lx_iterator_ref_t iterator, lx_cpointer_t container) {
    static lx_iterator_op_t op = {
        lx_mesh_edge_iterator_head,
        lx_mesh_edge_iterator_tail,
        lx_mesh_edge_iterator_prev,
        lx_mesh_edge_iterator_next,
        lx_mesh_edge_iterator_item
    };
    iterator->container = container;
    iterator->mode      = LX_ITERATOR_MODE_FORWARD | LX_ITERATOR_MODE_REVERSE | LX_ITERATOR_MODE_READONLY;
    iterator->op        = &op;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_mesh_edge_list_ref_t lx_mesh_edge_list_init(lx_size_t itemsize, lx_element_free_t itemfree) {
    lx_assert_and_check_return_val(itemsize, lx_null);

    lx_bool_t            ok = lx_false;
    lx_mesh_edge_list_t* list = lx_null;
    do {
        // init list
        list = lx_malloc0_type(lx_mesh_edge_list_t);
        lx_assert_and_check_break(list);

        list->itemsize  = itemsize;
        list->itemfree  = itemfree;
        list->order     = LX_MESH_ORDER_INSERT_TAIL;
        list->edge_size = lx_align_cpu(sizeof(lx_mesh_edge_t) + itemsize);
        list->base.iterator_of = lx_mesh_edge_iterator_of;

        // init pool, item = (edge + data) + (edge->sym + data)
        list->pool = lx_fixed_pool_init(lx_null, LX_MESH_EDGE_LIST_GROW, list->edge_size << 1, lx_null, lx_mesh_edge_exit, (lx_cpointer_t)list);
        lx_assert_and_check_break(list->pool);

        // init head edge
        list->head[0].sym = &list->head[1];
        list->head[1].sym = &list->head[0];

        // init edge list
        lx_mesh_edge_init(list->head);
        ok = lx_true;

    } while (0);

    if (!ok && list) {
        lx_mesh_edge_list_exit((lx_mesh_edge_list_ref_t)list);
        list = lx_null;
    }
    return (lx_mesh_edge_list_ref_t)list;
}
lx_void_t lx_mesh_edge_list_exit(lx_mesh_edge_list_ref_t self)
{
    // check
    lx_mesh_edge_list_t* list = (lx_mesh_edge_list_t*)self;
    lx_assert_and_check_return(list);

    // clear it first
    lx_mesh_edge_list_clear(list);

    // exit pool
    if (list->pool) lx_fixed_pool_exit(list->pool);
    list->pool = lx_null;

    // exit it
    lx_free(list);
}
lx_void_t lx_mesh_edge_list_clear(lx_mesh_edge_list_ref_t self)
{
    // check
    lx_mesh_edge_list_t* list = (lx_mesh_edge_list_t*)self;
    lx_assert_and_check_return(list);

    // clear pool
    if (list->pool) lx_fixed_pool_clear(list->pool);

    // clear list
    lx_mesh_edge_init(list->head);

#ifdef LX_DEBUG
    // clear id
    list->id = 0;
#endif
}
lx_iterator_ref_t lx_mesh_edge_list_itor(lx_mesh_edge_list_ref_t self)
{
    // the iterator
    return (lx_iterator_ref_t)list;
}
lx_size_t lx_mesh_edge_list_size(lx_mesh_edge_list_ref_t self)
{
    // check
    lx_mesh_edge_list_t* list = (lx_mesh_edge_list_t*)self;
    lx_assert_and_check_return_val(list && list->pool, 0);

    // the size
    return lx_fixed_pool_size(list->pool);
}
lx_size_t lx_mesh_edge_list_maxn(lx_mesh_edge_list_ref_t self)
{
    // the edge maxn
    return LX_MESH_EDGE_LIST_MAXN;
}
lx_mesh_edge_ref_t lx_mesh_edge_list_make(lx_mesh_edge_list_ref_t self)
{
    // check
    lx_mesh_edge_list_t* list = (lx_mesh_edge_list_t*)self;
    lx_assert_and_check_return_val(list && list->pool, lx_null);

    // make it
    lx_mesh_edge_ref_t edge = (lx_mesh_edge_ref_t)lx_fixed_pool_malloc0(list->pool);
    lx_assert_and_check_return_val(edge, lx_null);

    // the sym edge
    lx_mesh_edge_ref_t edge_sym = (lx_mesh_edge_ref_t)((lx_byte_t*)edge + list->edge_size);

    // init edge
    edge->sym       = edge_sym;
    edge->onext     = edge;
    edge->lnext     = edge_sym;

    // init sym edge
    edge_sym->sym   = edge;
    edge_sym->onext = edge_sym;
    edge_sym->lnext = edge;

#ifdef LX_DEBUG
    // init id
    edge->id        = ++list->id;
    edge_sym->id    = edge->id;

    // save list
    edge->list      = (lx_pointer_t)list;
    edge_sym->list  = (lx_pointer_t)list;
#endif

    // insert to the edge list
    switch (list->order)
    {
    case LX_MESH_ORDER_INSERT_HEAD:
        lx_mesh_edge_insert_prev(edge, list->head->next);
        break;
    case LX_MESH_ORDER_INSERT_TAIL:
    default:
        lx_mesh_edge_insert_prev(edge, list->head);
        break;
    }

    // ok
    return edge;
}
lx_mesh_edge_ref_t lx_mesh_edge_list_make_loop(lx_mesh_edge_list_ref_t self, lx_bool_t is_ccw)
{
    // check
    lx_mesh_edge_list_t* list = (lx_mesh_edge_list_t*)self;
    lx_assert_and_check_return_val(list && list->pool, lx_null);

    // make it
    lx_mesh_edge_ref_t edge = (lx_mesh_edge_ref_t)lx_fixed_pool_malloc0(list->pool);
    lx_assert_and_check_return_val(edge, lx_null);

    // the sym edge
    lx_mesh_edge_ref_t edge_sym = (lx_mesh_edge_ref_t)((lx_byte_t*)edge + list->edge_size);

    // init edge
    edge->sym       = edge_sym;
    edge->onext     = edge_sym;
    edge->lnext     = edge;

    // init sym edge
    edge_sym->sym   = edge;
    edge_sym->onext = edge;
    edge_sym->lnext = edge_sym;

#ifdef LX_DEBUG
    // init id
    edge->id        = ++list->id;
    edge_sym->id    = edge->id;

    // save list
    edge->list      = (lx_pointer_t)list;
    edge_sym->list  = (lx_pointer_t)list;
#endif

    // insert to the edge list
    switch (list->order)
    {
    case LX_MESH_ORDER_INSERT_HEAD:
        lx_mesh_edge_insert_prev(edge, list->head->next);
        break;
    case LX_MESH_ORDER_INSERT_TAIL:
    default:
        lx_mesh_edge_insert_prev(edge, list->head);
        break;
    }

    // clockwise? reverse it
    return is_ccw? edge : edge_sym;
}
#ifdef LX_DEBUG
lx_long_t lx_mesh_edge_list_cstr(lx_mesh_edge_list_ref_t self, lx_mesh_edge_ref_t edge, lx_char_t* data, lx_size_t maxn)
{
    // check
    lx_mesh_edge_list_t* list = (lx_mesh_edge_list_t*)self;
    lx_assert_and_check_return_val(list && list->element.cstr && edge && maxn, -1);

    // make the edge info
    lx_char_t           edge_info[4096] = {0};
    lx_char_t const*    pedge_info = list->element.cstr(&list->element, lx_mesh_edge_list_data(list, edge), edge_info, sizeof(edge_info));
    if (!pedge_info) pedge_info = list->element.cstr(&list->element, lx_mesh_edge_list_data(list, edge->sym), edge_info, sizeof(edge_info));
    if (!pedge_info)
    {
        lx_snprintf(edge_info, sizeof(edge_info) - 1, "<e%lu>", edge->id);
        pedge_info = edge_info;
    }

    // make it, @note only for the debug mode
    return lx_snprintf(data, maxn, "(%s: %{mesh_vertex} => %{mesh_vertex})", pedge_info, edge->org, edge->sym->org);
}
#endif
lx_void_t lx_mesh_edge_list_kill(lx_mesh_edge_list_ref_t self, lx_mesh_edge_ref_t edge)
{
    // check
    lx_mesh_edge_list_t* list = (lx_mesh_edge_list_t*)self;
    lx_assert_and_check_return(list && list->pool && edge);

    // make sure the edge points to the first half-edge
    if (edge->sym < edge) edge = edge->sym;

#ifdef LX_DEBUG
    // check
    lx_assert(edge->id && edge->sym->id);

    // clear id
    edge->id        = 0;
    edge->sym->id   = 0;
#endif

    // remove it from the list
    lx_mesh_edge_remove_done(edge);

    // exit it
    lx_fixed_pool_free(list->pool, edge);
}
lx_cpointer_t lx_mesh_edge_list_data(lx_mesh_edge_list_ref_t self, lx_mesh_edge_ref_t edge)
{
    // check
    lx_mesh_edge_list_t* list = (lx_mesh_edge_list_t*)self;
    lx_assert_and_check_return_val(list && list->element.data && edge, lx_null);

    // the user data
    return list->element.data(&list->element, lx_mesh_edge_user(edge));
}
lx_void_t lx_mesh_edge_list_data_set(lx_mesh_edge_list_ref_t self, lx_mesh_edge_ref_t edge, lx_cpointer_t data)
{
    // check
    lx_mesh_edge_list_t* list = (lx_mesh_edge_list_t*)self;
    lx_assert_and_check_return(list && list->element.repl && edge);

    // set the user data
    list->element.repl(&list->element, lx_mesh_edge_user(edge), data);
}
lx_size_t lx_mesh_edge_list_order(lx_mesh_edge_list_ref_t self)
{
    // check
    lx_mesh_edge_list_t* list = (lx_mesh_edge_list_t*)self;
    lx_assert_and_check_return_val(list, LX_MESH_ORDER_INSERT_TAIL);

    // the order
    return list->order;
}
lx_void_t lx_mesh_edge_list_order_set(lx_mesh_edge_list_ref_t self, lx_size_t order)
{
    // check
    lx_mesh_edge_list_t* list = (lx_mesh_edge_list_t*)self;
    lx_assert_and_check_return(list);

    // set the order
    list->order = order;
}


