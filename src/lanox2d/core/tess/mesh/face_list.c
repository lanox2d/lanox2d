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
 * @file        face_list.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "face_list.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the mesh face list grow
#ifdef LX_CONFIG_SMALL
#   define LX_MESH_FACE_LIST_GROW               (32)
#else
#   define LX_MESH_FACE_LIST_GROW               (64)
#endif

// the mesh face list maxn
#ifdef LX_CONFIG_SMALL
#   define LX_MESH_FACE_LIST_MAXN               (1 << 16)
#else
#   define LX_MESH_FACE_LIST_MAXN               (1 << 30)
#endif

// the face user data pointer
#define lx_mesh_face_user(face)                 ((lx_mesh_face_ref_t)(face) + 1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the mesh face list type
typedef struct lx_mesh_face_list_t_ {
    lx_iterator_base_t          base;
    lx_list_entry_head_t        head;
    lx_fixed_pool_ref_t         pool;
    lx_element_t                element;
    lx_size_t                   order;
#ifdef LX_DEBUG
    lx_size_t                   id;
#endif
}lx_mesh_face_list_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_void_t lx_mesh_face_exit(lx_pointer_t data, lx_cpointer_t udata) {
    lx_mesh_face_list_t* list = (lx_mesh_face_list_t*)udata;
    if (list && list->element.free && data) {
        list->element.free((lx_pointer_t)((lx_mesh_face_ref_t)data + 1), list->element.udata);
    }
}

static lx_void_t lx_mesh_face_iterator_of(lx_iterator_ref_t iterator, lx_cpointer_t container) {
    lx_mesh_face_list_t* list = (lx_mesh_face_list_t*)container;
    lx_assert(list);
    lx_iterator_of(iterator, lx_list_entry_itor(&list->head));
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_mesh_face_list_ref_t lx_mesh_face_list_init(lx_element_t element) {
    lx_bool_t            ok = lx_false;
    lx_mesh_face_list_t* list = lx_null;
    do {
        // init list
        list = lx_malloc0_type(lx_mesh_face_list_t);
        lx_assert_and_check_break(list);

        list->element = element;
        list->order = LX_MESH_ORDER_INSERT_TAIL;
        lx_list_entry_init_(&list->head, 0, sizeof(lx_mesh_face_t) + element.size);
        list->base.iterator_of = lx_mesh_face_iterator_of;

        // init pool, item = face + data
        list->pool = lx_fixed_pool_init(LX_MESH_FACE_LIST_GROW, sizeof(lx_mesh_face_t) + element.size, lx_mesh_face_exit, (lx_cpointer_t)list);
        lx_assert_and_check_break(list->pool);

        // ok
        ok = lx_true;

    } while (0);

    if (!ok && list) {
        lx_mesh_face_list_exit((lx_mesh_face_list_ref_t)list);
        list = lx_null;
    }
    return (lx_mesh_face_list_ref_t)list;
}

lx_void_t lx_mesh_face_list_exit(lx_mesh_face_list_ref_t self) {
    lx_mesh_face_list_t* list = (lx_mesh_face_list_t*)self;
    if (list) {
        lx_mesh_face_list_clear(self);
        if (list->pool) {
            lx_fixed_pool_exit(list->pool);
            list->pool = lx_null;
        }
        lx_free(list);
    }
}

lx_void_t lx_mesh_face_list_clear(lx_mesh_face_list_ref_t self) {
    lx_mesh_face_list_t* list = (lx_mesh_face_list_t*)self;
    if (list) {
        if (list->pool) {
            lx_fixed_pool_clear(list->pool);
        }
        lx_list_entry_clear(&list->head);
#ifdef LX_DEBUG
        list->id = 0;
#endif
    }
}

lx_size_t lx_mesh_face_list_size(lx_mesh_face_list_ref_t self) {
    lx_mesh_face_list_t* list = (lx_mesh_face_list_t*)self;
    lx_assert_and_check_return_val(list && list->pool, 0);
    lx_assert(lx_list_entry_size(&list->head) == lx_fixed_pool_size(list->pool));

    return lx_list_entry_size(&list->head);
}

lx_mesh_face_ref_t lx_mesh_face_list_head(lx_mesh_face_list_ref_t self) {
    lx_mesh_face_list_t* list = (lx_mesh_face_list_t*)self;
    lx_assert(list);
    return (lx_mesh_face_ref_t)lx_list_entry(&list->head, lx_list_entry_head(&list->head));
}

lx_mesh_face_ref_t lx_mesh_face_list_last(lx_mesh_face_list_ref_t self) {
    lx_mesh_face_list_t* list = (lx_mesh_face_list_t*)self;
    lx_assert(list);
    return (lx_mesh_face_ref_t)lx_list_entry(&list->head, lx_list_entry_last(&list->head));
}

lx_mesh_face_ref_t lx_mesh_face_list_make(lx_mesh_face_list_ref_t self) {
    lx_mesh_face_list_t* list = (lx_mesh_face_list_t*)self;
    lx_assert_and_check_return_val(list && list->pool, lx_null);

    lx_mesh_face_ref_t face = (lx_mesh_face_ref_t)lx_fixed_pool_malloc0(list->pool);
    lx_assert_and_check_return_val(face, lx_null);

#ifdef LX_DEBUG
    face->id = ++list->id;
#endif

    // insert to the face list
    switch (list->order) {
    case LX_MESH_ORDER_INSERT_HEAD:
        lx_list_entry_insert_head(&list->head, &face->entry);
        break;
    case LX_MESH_ORDER_INSERT_TAIL:
    default:
        lx_list_entry_insert_tail(&list->head, &face->entry);
        break;
    }
    return face;
}

lx_void_t lx_mesh_face_list_kill(lx_mesh_face_list_ref_t self, lx_mesh_face_ref_t face) {
    lx_mesh_face_list_t* list = (lx_mesh_face_list_t*)self;
    lx_assert_and_check_return(list && list->pool && face);

#ifdef LX_DEBUG
    lx_assert(face->id);
    face->id = 0;
#endif

    // remove from the face list
    lx_list_entry_remove(&list->head, &face->entry);
    lx_fixed_pool_free(list->pool, face);
}

lx_size_t lx_mesh_face_list_order(lx_mesh_face_list_ref_t self) {
    lx_mesh_face_list_t* list = (lx_mesh_face_list_t*)self;
    lx_assert_and_check_return_val(list, LX_MESH_ORDER_INSERT_TAIL);

    return list->order;
}

lx_void_t lx_mesh_face_list_order_set(lx_mesh_face_list_ref_t self, lx_size_t order) {
    lx_mesh_face_list_t* list = (lx_mesh_face_list_t*)self;
    lx_assert_and_check_return(list);

    list->order = order;
}


