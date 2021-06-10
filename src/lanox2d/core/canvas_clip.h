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
 * @file        canvas_clip.h
 *
 */
#ifndef LX_CORE_CANVAS_CLIP_H
#define LX_CORE_CANVAS_CLIP_H

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

/*! get the clipper
 *
 * @param canvas    the canvas
 *
 * @return          the clipper
 */
lx_clipper_ref_t    lx_canvas_clipper(lx_canvas_ref_t canvas);

/*! save clipper
 *
 * @param canvas    the canvas
 *
 * @return          the current clipper
 */
lx_clipper_ref_t    lx_canvas_clipper_save(lx_canvas_ref_t canvas);

/*! load clipper
 *
 * @param canvas    the canvas
 */
lx_void_t           lx_canvas_clipper_load(lx_canvas_ref_t canvas);


/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


