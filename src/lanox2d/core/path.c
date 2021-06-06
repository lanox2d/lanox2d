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
 * @file        path.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "path.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the points grow count
#ifdef LX_CONFIG_SMALL
#   define LX_PATH_POINTS_GROW      (32)
#else
#   define LX_PATH_POINTS_GROW      (64)
#endif

// the point step for code
#define lx_path_point_step(code)    ((code) < 1? 1 : (code) - 1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the path flag enum
typedef enum lx_path_flag_e_ {
    LX_PATH_FLAG_DIRTY_HINT    = 1
,   LX_PATH_FLAG_DIRTY_BOUNDS  = 2
,   LX_PATH_FLAG_DIRTY_POLYGON = 4
,   LX_PATH_FLAG_DIRTY_CONVEX  = 8
,   LX_PATH_FLAG_DIRTY_ALL     = LX_PATH_FLAG_DIRTY_HINT | LX_PATH_FLAG_DIRTY_BOUNDS | LX_PATH_FLAG_DIRTY_POLYGON | LX_PATH_FLAG_DIRTY_CONVEX
,   LX_PATH_FLAG_CURVE         = 16    //< have curve contour?
,   LX_PATH_FLAG_CONVEX        = 32    //< all contours are convex polygon?
,   LX_PATH_FLAG_CLOSED        = 64    //< the contour is closed now?
,   LX_PATH_FLAG_SINGLE        = 128   //< single contour?
}lx_path_flag_e;
