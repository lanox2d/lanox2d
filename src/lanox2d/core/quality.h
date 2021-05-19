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
 * @file        quality.h
 *
 */
#ifndef LX_CORE_QUALITY_H
#define LX_CORE_QUALITY_H

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

/// the quality enum
typedef enum lx_quality_e_ {
    LX_QUALITY_LOW  = 0
,   LX_QUALITY_MID  = 1
,   LX_QUALITY_TOP  = 2
} lx_quality_e;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! get quality
 *
 * @return          the quality
 */
lx_size_t           lx_quality(lx_noarg_t);

/*! set quality
 *
 * @param quality   the quality
 */
lx_void_t           lx_quality_set(lx_size_t quality);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


