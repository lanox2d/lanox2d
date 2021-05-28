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
 * @file        stack.h
 *
 */
#ifndef LX_BASE_CONTAINER_STACK_H
#define LX_BASE_CONTAINER_STACK_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the stack ref type
typedef lx_typeref(stack);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init stack
 *
 * @param grow      the item grow
 * @param itemsize  the item size
 * @param free      the free function of element item
 * @param udata     the user data
 *
 * @return          the stack
 */
lx_stack_ref_t      lx_stack_init(lx_size_t grow, lx_size_t itemsize, lx_element_free_t free, lx_cpointer_t udata);

/*! exit stack
 *
 * @param stack     the stack
 */
lx_void_t           lx_stack_exit(lx_stack_ref_t stack);

/*! get the stack head item
 *
 * @param stack     the stack
 *
 * @return          the head item
 */
lx_pointer_t        lx_stack_head(lx_stack_ref_t stack);

/*! get the stack last item
 *
 * @param stack     the stack
 *
 * @return          the last item
 */
lx_pointer_t        lx_stack_last(lx_stack_ref_t stack);

/*! clear the stack
 *
 * @param stack     the stack
 */
lx_void_t           lx_stack_clear(lx_stack_ref_t stack);

/*! copy the stack
 *
 * @param stack     the stack
 * @param copy      the copied stack
 */
lx_void_t           lx_stack_copy(lx_stack_ref_t stack, lx_stack_ref_t copy);

/*! put the stack item
 *
 * @param stack     the stack
 * @param data      the item data
 */
lx_void_t           lx_stack_put(lx_stack_ref_t stack, lx_cpointer_t data);

/*! pop the stack item
 *
 * @param stack     the stack
 */
lx_void_t           lx_stack_pop(lx_stack_ref_t stack);

/*! get the stack top item
 *
 * @param stack     the stack
 *
 * @return          the stack top item
 */
lx_pointer_t        lx_stack_top(lx_stack_ref_t stack);

/*! get the stack size
 *
 * @param stack     the stack
 *
 * @return          the stack size
 */
lx_size_t           lx_stack_size(lx_stack_ref_t stack);

/*! foreach all elements
 *
 * @param stack     the stack
 * @param foreach   the foreach function
 * @param udata     the user data
 */
lx_void_t           lx_stack_foreach(lx_stack_ref_t stack, lx_element_foreach_t foreach, lx_cpointer_t udata);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


