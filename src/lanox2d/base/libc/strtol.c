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
 * @file        strtol.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "libc.h"
#include <stdlib.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef LX_CONFIG_LIBC_HAVE_STRTOL
lx_long_t lx_strtol(lx_char_t const* s, lx_char_t** endptr, lx_int_t base) {
    lx_assert_and_check_return_val(s, 0);
#   ifdef LX_COMPILER_IS_MSVC
    return _strtol(s, endptr, base);
#   else
    return strtol(s, endptr, base);
#   endif
}
#else
lx_long_t lx_strtol(lx_char_t const* s, lx_char_t** endptr, lx_int_t base) {
    lx_trace_noimpl();
    return -1;
}
#endif
