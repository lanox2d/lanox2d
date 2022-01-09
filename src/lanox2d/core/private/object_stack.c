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
 * @file        object_stack.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "object_stack.h"
#include "../path.h"
#include "../paint.h"
#include "../clipper.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the object stack type
typedef struct lx_object_stack_t_ {
    lx_size_t       type;
    lx_stack_ref_t  stack;
    lx_stack_ref_t  cache;
    lx_handle_t     object;
    lx_size_t       cache_size;
}lx_object_stack_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_handle_t lx_object_stack_object_init(lx_size_t type) {
    lx_handle_t object = lx_null;
    switch (type) {
    case LX_OBJECT_STACK_TYPE_PATH:
        object = (lx_handle_t)lx_path_init();
        break;
    case LX_OBJECT_STACK_TYPE_PAINT:
        object = (lx_handle_t)lx_paint_init();
        break;
    case LX_OBJECT_STACK_TYPE_CLIPPER:
        object = (lx_handle_t)lx_clipper_init();
        break;
    default:
        break;
    }
    return object;
}

static lx_void_t lx_object_stack_object_exit(lx_size_t type, lx_handle_t object) {
    lx_assert_and_check_return(object);
    switch (type) {
    case LX_OBJECT_STACK_TYPE_PATH:
        //lx_path_exit((lx_path_ref_t)object);
        break;
    case LX_OBJECT_STACK_TYPE_PAINT:
        lx_paint_exit((lx_paint_ref_t)object);
        break;
    case LX_OBJECT_STACK_TYPE_CLIPPER:
        //lx_clipper_exit((lx_clipper_ref_t)object);
        break;
    default:
        break;
    }
}

static lx_void_t lx_object_stack_object_copy(lx_size_t type, lx_handle_t object, lx_handle_t copied) {
    lx_assert_and_check_return(object);
    switch (type) {
    case LX_OBJECT_STACK_TYPE_PATH:
        //lx_path_copy((lx_path_ref_t)object, (lx_path_ref_t)copied);
        break;
    case LX_OBJECT_STACK_TYPE_PAINT:
        lx_paint_copy((lx_paint_ref_t)object, (lx_paint_ref_t)copied);
        break;
    case LX_OBJECT_STACK_TYPE_CLIPPER:
        //lx_clipper_copy((lx_clipper_ref_t)object, (lx_clipper_ref_t)copied);
        break;
    default:
        break;
    }
}

static lx_bool_t lx_object_stack_object_free(lx_iterator_ref_t iterator, lx_pointer_t item, lx_cpointer_t udata) {
    lx_object_stack_t* stack = (lx_object_stack_t*)udata;
    if (stack) {
        lx_handle_t* pobject = (lx_handle_t*)item;
        if (pobject && *pobject) {
            lx_object_stack_object_exit(stack->type, *pobject);
        }
    }
    return lx_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_object_stack_ref_t lx_object_stack_init(lx_size_t grow, lx_size_t type) {
    lx_bool_t           ok = lx_false;
    lx_object_stack_t*  stack = lx_null;
    do {
        // check
        lx_assert_and_check_break(type && type < LX_OBJECT_STACK_TYPE_MAXN);

        // init stack
        stack = lx_malloc0_type(lx_object_stack_t);
        lx_assert_and_check_break(stack);

        stack->type       = type;
        stack->cache_size = grow? grow : 8;

        // init cache
        stack->cache = lx_stack_init(grow, lx_element_mem(sizeof(lx_handle_t), lx_null, lx_null));
        lx_assert_and_check_break(stack->cache);

        // init stack
        stack->stack = lx_stack_init(grow, lx_element_mem(sizeof(lx_handle_t), lx_null, lx_null));
        lx_assert_and_check_break(stack->stack);

        // ok
        ok = lx_true;

    } while (0);

    if (!ok && stack) {
        lx_object_stack_exit((lx_object_stack_ref_t)stack);
        stack = lx_null;
    }
    return (lx_object_stack_ref_t)stack;
}

lx_void_t lx_object_stack_exit(lx_object_stack_ref_t self) {
    lx_object_stack_t* stack = (lx_object_stack_t*)self;
    if (stack) {
        if (stack->object) {
            lx_object_stack_object_exit(stack->type, stack->object);
            stack->object = lx_null;
        }
        if (stack->stack) {
            lx_iterator_t iterator;
            lx_iterator_of(&iterator, stack->stack);
            lx_foreach_all(&iterator, lx_object_stack_object_free, stack);
            lx_stack_exit(stack->stack);
            stack->stack = lx_null;
        }
        if (stack->cache) {
            lx_iterator_t iterator;
            lx_iterator_of(&iterator, stack->cache);
            lx_foreach_all(&iterator, lx_object_stack_object_free, stack);
            lx_stack_exit(stack->cache);
            stack->cache = lx_null;
        }
        lx_free(stack);
    }
}

lx_handle_t lx_object_stack_save(lx_object_stack_ref_t self) {
    // check
    lx_object_stack_t* stack = (lx_object_stack_t*)self;
    lx_assert_and_check_return_val(stack && stack->cache && stack->stack, lx_null);

    // init object first if be null
    if (!stack->object) {
        stack->object = lx_object_stack_object_init(stack->type);
    }
    lx_assert_and_check_return_val(stack->object, lx_null);

    // get a new object from cache first
    lx_handle_t object = lx_null;
    if (lx_stack_size(stack->cache)) {
        lx_handle_t* pobject = (lx_handle_t*)lx_stack_top(stack->cache);
        if (pobject) {
            object = *pobject;
        }
        lx_assert_and_check_return_val(stack->cache, lx_null);
        lx_stack_pop(stack->cache);
    } else {
        // make a new object
        object = lx_object_stack_object_init(stack->type);
    }
    lx_assert_and_check_return_val(object, lx_null);

    // init the new object using the old object
    lx_object_stack_object_copy(stack->type, object, stack->object);

    // save the old object
    lx_stack_push(stack->stack, &stack->object);
    stack->object = object;
    return stack->object;
}

lx_void_t lx_object_stack_load(lx_object_stack_ref_t self) {
    // check
    lx_object_stack_t* stack = (lx_object_stack_t*)self;
    lx_assert_and_check_return(stack && stack->cache && stack->stack);

    // init object first if be null
    if (!stack->object) {
        stack->object = lx_object_stack_object_init(stack->type);
    }
    lx_assert_and_check_return(stack->object);

    // load new object from the stack top
    lx_handle_t  object = lx_null;
    lx_handle_t* pobject = (lx_handle_t*)lx_stack_top(stack->stack);
    if (pobject) {
        object = *pobject;
    }
    lx_assert_and_check_return(object);

    // pop the top object
    lx_stack_pop(stack->stack);

    // put current object to the cache if the cache be not full
    if (stack->object && lx_stack_size(stack->cache) < stack->cache_size) {
        lx_stack_push(stack->cache, &stack->object);
    } else {
        lx_object_stack_object_exit(stack->type, stack->object);
    }

    // update the current object
    stack->object = object;
}

lx_handle_t lx_object_stack_object(lx_object_stack_ref_t self) {
    lx_object_stack_t* stack = (lx_object_stack_t*)self;
    if (stack) {
        if (!stack->object) {
            stack->object = lx_object_stack_object_init(stack->type);
        }
        return stack->object;
    }
    return lx_null;
}
