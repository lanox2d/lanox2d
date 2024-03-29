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
 * @file        monotone.h
 */
#ifndef LX_UTILS_IMPL_TESSELLATOR_MONOTONE_H
#define LX_UTILS_IMPL_TESSELLATOR_MONOTONE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* make horizontal monotone region for mesh
 *
 * monotone:
 *
 *   . . .
 *  .     .
 * . ----- .
 * .      .
 * . -----  .
 * .           .
 * . . . . . . . .
 *
 * not monotone:
 *
 *   . . .
 *  .     .
 * . ----- .
 * .      .    ........
 * . -----  .  . ---- .  <= two segments
 * .           .      .
 * .                  .
 * . . . . . . . . . ..
 *
 * @param tessellator   the tessellator
 * @param bounds        the polygon bounds
 */
lx_void_t               lx_tessellator_monotone_make(lx_tessellator_t* tessellator, lx_rect_ref_t bounds);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


