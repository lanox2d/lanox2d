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
 * @file        page.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "page.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef LX_CONFIG_OS_WINDOWS
#include <windows.h>
lx_size_t lx_page_size() {
    static lx_size_t g_page_size = 0;
    if (!g_page_size) {
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        g_page_size = info.dwPageSize;
    }
    return g_page_size;
}
#elif defined(LX_CONFIG_POSIX_HAVE_GETPAGESIZE) || \
		defined(LX_CONFIG_POSIX_HAVE_SYSCONF)
#include <unistd.h>
lx_size_t lx_page_size() {
    static lx_size_t g_page_size = 0;
    if (!g_page_size) {
#if defined(LX_CONFIG_POSIX_HAVE_SYSCONF) && defined(_SC_PAGESIZE)
        g_page_size = (lx_size_t)sysconf(_SC_PAGESIZE);
#elif defined(LX_CONFIG_POSIX_HAVE_GETPAGESIZE)
        g_page_size = (lx_size_t)getpagesize();
#endif
    }
    return g_page_size;
}
#else
lx_size_t lx_page_size() {
    // default: 4KB
    return 4096;
}
#endif
