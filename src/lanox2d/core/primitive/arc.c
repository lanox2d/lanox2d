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
 * @file        arc.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "arc.h"
#include "point.h"
#include "../../base/base.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_void_t lx_arc_make(lx_arc_ref_t arc, lx_float_t x0, lx_float_t y0, lx_float_t rx, lx_float_t ry, lx_float_t ab, lx_float_t an) {
    arc->c.x    = x0;
    arc->c.y    = y0;
    arc->rx     = rx;
    arc->ry     = ry;
    arc->ab     = ab;
    arc->an     = an;
}

lx_void_t lx_arc_imake(lx_arc_ref_t arc, lx_long_t x0, lx_long_t y0, lx_size_t rx, lx_size_t ry, lx_long_t ab, lx_long_t an) {
    lx_arc_make(arc, (lx_float_t)x0, (lx_float_t)y0, (lx_float_t)rx, (lx_float_t)ry, (lx_float_t)ab, (lx_float_t)an);
}
