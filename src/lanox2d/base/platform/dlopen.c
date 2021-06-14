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
 * @file        dlopen.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "dlopen.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if defined(LX_CONFIG_OS_WINDOWS)
#   include "windows/dlopen.c"
#elif defined(LX_CONFIG_OS_ANDROID)
#   include "android/dlopen.c"
#elif defined(LX_CONFIG_OS_IOS)
#   include "mach/dlopen.c"
#elif defined(LX_CONFIG_POSIX_HAVE_DLOPEN)
#   include "posix/dlopen.c"
#else
lx_dlimage_ref_t lx_dlopen(lx_char_t const* filename, lx_int_t flag) {
    lx_trace_noimpl();
    return lx_null;
}
lx_pointer_t lx_dlsym(lx_dlimage_ref_t dlimage, lx_char_t const* symbol) {
    lx_trace_noimpl();
    return lx_null;
}

lx_int_t lx_dlclose(lx_dlimage_ref_t dlimage) {
    lx_trace_noimpl();
    return 0;
}

#endif
