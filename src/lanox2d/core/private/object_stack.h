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
 * @file        object_stack.h
 *
 */
#ifndef LX_CORE_IMPL_OBJECT_STACK_H
#define LX_CORE_IMPL_OBJECT_STACK_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the object stack type enum
typedef enum __lx_object_stack_type_e
{
    LX_OBJECT_STACK_TYPE_NONE        = 0
,   LX_OBJECT_STACK_TYPE_PATH        = 1
,   LX_OBJECT_STACK_TYPE_PAINT       = 2
,   LX_OBJECT_STACK_TYPE_CLIPPER     = 3
,   LX_OBJECT_STACK_TYPE_MAXN        = 4

}lx_object_stack_type_e;

// the object stack ref type
typedef lx_typeref(object_stack);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* init the object stack for path, paint and clipper
 *
 * @param grow          the grow size
 * @param type          the object type
 *
 * @return              the stack
 */
lx_object_stack_ref_t   lx_object_stack_init(lx_size_t grow, lx_size_t type);

/* exit the object stack
 *
 * @param stack         the stack
 */
lx_void_t               lx_object_stack_exit(lx_object_stack_ref_t stack);

/* save the current object to the top object
 *
 * @param stack         the stack
 *
 * @return              the current object
 */
lx_handle_t             lx_object_stack_save(lx_object_stack_ref_t stack);

/* load the top object to the current object
 *
 * @param stack         the stack
 */
lx_void_t               lx_object_stack_load(lx_object_stack_ref_t stack);

/* get the current object
 *
 * @param stack         the stack
 *
 * @return              the top object
 */
lx_handle_t             lx_object_stack_object(lx_object_stack_ref_t stack);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


