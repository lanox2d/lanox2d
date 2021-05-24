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
 * @file        time.h
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "time.h"
#ifdef LX_CONFIG_OS_WINDOWS
#   include <window.h>
#else
#   include <unistd.h>
#   include <sys/time.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef LX_CONFIG_OS_WINDOWS
lx_void_t lx_usleep(lx_size_t us) {
    Sleep(1);
}

lx_void_t lx_msleep(lx_size_t ms) {
    Sleep((DWORD)ms);
}

lx_hong_t lx_mclock() {
    return (lx_hong_t)GetTickCount();
}

lx_hong_t lx_uclock() {
    LARGE_INTEGER f = {{0}};
    LARGE_INTEGER t = {{0}};
    if (QueryPerformanceFrequency(&f) && QueryPerformanceCounter(&t)) {
        return (t.QuadPart * 1000000) / f.QuadPart;
    } else {
        return 0;
    }
}
#else
lx_void_t lx_usleep(lx_size_t us) {
    usleep(us);
}

lx_void_t lx_msleep(lx_size_t ms) {
    lx_usleep(ms * 1000);
}

lx_hong_t lx_mclock() {
    struct timeval tv = {0};
    if (0 != gettimeofday(&tv, lx_null)) {
        return -1;
    }
    return ((lx_hong_t)tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

lx_hong_t lx_uclock() {
    struct timeval tv = {0};
    if (0 != gettimeofday(&tv, lx_null)) {
        return -1;
    }
    return ((lx_hong_t)tv.tv_sec * 1000000 + tv.tv_usec);
}
#endif

