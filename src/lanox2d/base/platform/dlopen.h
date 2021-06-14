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
 * @file        dlopen.h
 *
 */
#ifndef LX_BASE_PLATFORM_DLOPEN_H
#define LX_BASE_PLATFORM_DLOPEN_H

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

/// the dlopen flag enum
typedef enum lx_dlopen_flag_e {
    LX_RTLD_LAZY    = 1
,   LX_RTLD_NOW     = 2
}lx_dlopen_flag_e;

/// the dlimage ref type
typedef lx_typeref(dlimage);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! The function dlopen() loads the dynamic library and returns an opaque "handle".
 *
 * @param filename  the dynamic library file named by the null-terminated string filename
 * @param flag      the load flag
 *
 * @return          the dynamic library handle
 */
lx_dlimage_ref_t    lx_dlopen(lx_char_t const* filename, lx_int_t flag);

/*! get the address where that symbol is loaded into memory
 *
 * @param handle    the dynamic library handle
 *
 * @return          the symbol address
 */
lx_pointer_t        lx_dlsym(lx_dlimage_ref_t dlimage, lx_char_t const* symbol);

/*! It decrements the reference count on the dynamic library handle handle.
 * If the reference count drops to zero and no other loaded libraries use symbols in it, then the dynamic library is unloaded.
 *
 * @param handle    the dynamic library handle
 *
 * @return          0 on success, and nonzero on error
 */
lx_int_t            lx_dlclose(lx_dlimage_ref_t dlimage);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


