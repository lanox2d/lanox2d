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
 * @file        trace.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "trace.h"
#include "../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_void_t lx_trace(lx_char_t const* filepath, lx_char_t const* fmt, ...) {
    lx_int_t         ret = 0;
    lx_char_t        line[8192] = {0};
    lx_char_t*       p = line;
    lx_char_t const* e = line + sizeof(line);
    if (filepath) {
        lx_char_t const* sep = filepath;
        while (*sep) {
            if (*sep == '/' || *sep == '\\') filepath = sep + 1;
            sep++;
        }
        p += lx_snprintf(p, e - p - 1, "[lanox2d/%s]: ", filepath);
    } else {
        p += lx_snprintf(p, e - p - 1, "[lanox2d]: ");
    }
    lx_vsnprintf_fmt(p, e - p - 1, fmt, &ret);
    lx_puts(line);
}

