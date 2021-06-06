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
 * @file        float.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "float.h"
#include "../libm/libm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_int_t lx_unitdiv(lx_float_t numer, lx_float_t denom, lx_float_t* result) {
    lx_assert(result);

    // negate it
    if (numer < 0) {
        numer = -numer;
        denom = -denom;
    }

    // must be valid numerator and denominator
    if (0 == denom || 0 == numer || numer >= denom) {
        return 0;
    }

    // must be finite value
    lx_float_t r = numer / denom;
    lx_assert_and_check_return_val(lx_isfinf(r), 0);

    // must be in range: [0, 1)
    lx_assert_and_check_return_val(r >= 0 && r < 1.0f, 0);

    // too smaller? not save result
    lx_check_return_val(r != 0, 0);
    *result = r;
    return 1;
}

