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
 * @file        printf.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "libc.h"
#include <stdio.h>
#if defined(LX_CONFIG_OS_ANDROID)
#   include <jni.h>
#   include <android/log.h>
#elif defined(LX_CONFIG_OS_IOS)
#   include <os/log.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

lx_int_t lx_puts(lx_char_t const* s) {
#if defined(LX_CONFIG_OS_ANDROID)
    return __android_log_print(ANDROID_LOG_INFO, "lanox2d", "%s", s);
#elif defined(LX_CONFIG_OS_IOS)
    os_log(OS_LOG_DEFAULT, "[lanox2d]: %s", s);
    return 1;
#else
    return fputs(s, stdout);
#endif
}

lx_int_t lx_printf(lx_char_t const* fmt, ...) {
    lx_int_t  ret = 0;
    lx_char_t line[8192] = {0};
    lx_vsnprintf_fmt(line, sizeof(line) - 1, fmt, &ret);
    lx_puts(line);
    return ret;
}

lx_int_t lx_sprintf(lx_char_t* s, lx_char_t const* fmt, ...) {
    lx_int_t ret = 0;
    lx_vsnprintf_fmt(s, LX_MAXU16, fmt, &ret);
    return ret;
}

lx_int_t lx_snprintf(lx_char_t* s, lx_size_t n, lx_char_t const* fmt, ...) {
    lx_int_t ret = 0;
    lx_vsnprintf_fmt(s, n, fmt, &ret);
    return ret;
}


