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
 * @file        clipper.h
 *
 */
#ifndef LX_CORE_CLIPPER_H
#define LX_CORE_CLIPPER_H

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

/// the clipper mode enum
typedef enum lx_clipper_mode_e_ {
    LX_CLIPPER_MODE_NONE        = 0x00
,   LX_CLIPPER_MODE_SUBTRACT    = 0x01
,   LX_CLIPPER_MODE_INTERSECT   = 0x02
,   LX_CLIPPER_MODE_UNION       = 0x03
,   LX_CLIPPER_MODE_REPLACE     = 0x04
}lx_clipper_mode_e;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init clipper
 *
 * @return                  the clipper
 */
lx_clipper_ref_t            lx_clipper_init(lx_noarg_t);

/*! exit clipper
 *
 * @param clipper           the clipper
 */
lx_void_t                   lx_clipper_exit(lx_clipper_ref_t clipper);

/*! the clipper shape count
 *
 * @param clipper           the clipper
 *
 * @return                  the item count
 */
lx_size_t                   lx_clipper_size(lx_clipper_ref_t clipper);

/*! clear the clipper
 *
 * @param clipper           the clipper
 */
lx_void_t                   lx_clipper_clear(lx_clipper_ref_t clipper);

/*! copy clipper
 *
 * @param clipper           the clipper
 * @param copied            the copied clipper
 */
lx_void_t                   lx_clipper_copy(lx_clipper_ref_t clipper, lx_clipper_ref_t copied);

/*! get the current clipper matrix
 *
 * @param clipper           the clipper
 *
 * @return                  the matrix
 */
lx_matrix_ref_t             lx_clipper_matrix(lx_clipper_ref_t clipper);

/*! set the current clipper matrix
 *
 * @param clipper           the clipper
 * @param matrix            the matrix
 */
lx_void_t                   lx_clipper_matrix_set(lx_clipper_ref_t clipper, lx_matrix_ref_t matrix);

/*! add path
 *
 * @param clipper           the clipper
 * @param mode              the clipper mode
 * @param path              the path
 */
lx_void_t                   lx_clipper_add_path(lx_clipper_ref_t clipper, lx_size_t mode, lx_path_ref_t path);

/*! add triangle
 *
 * @param clipper           the clipper
 * @param mode              the clipper mode
 * @param triangle          the triangle
 */
lx_void_t                   lx_clipper_add_triangle(lx_clipper_ref_t clipper, lx_size_t mode, lx_triangle_ref_t triangle);

/*! add rect
 *
 * @param clipper           the clipper
 * @param mode              the clipper mode
 * @param rect              the rect
 */
lx_void_t                   lx_clipper_add_rect(lx_clipper_ref_t clipper, lx_size_t mode, lx_rect_ref_t rect);

/*! add round rect
 *
 * @param clipper           the clipper
 * @param mode              the clipper mode
 * @param rect              the round rect
 */
lx_void_t                   lx_clipper_add_round_rect(lx_clipper_ref_t clipper, lx_size_t mode, lx_round_rect_ref_t rect);

/*! add circle
 *
 * @param clipper           the clipper
 * @param mode              the clipper mode
 * @param circle            the circle
 */
lx_void_t                   lx_clipper_add_circle(lx_clipper_ref_t clipper, lx_size_t mode, lx_circle_ref_t circle);

/*! add ellipse
 *
 * @param clipper           the clipper
 * @param mode              the clipper mode
 * @param ellipse           the ellipse
 */
lx_void_t                   lx_clipper_add_ellipse(lx_clipper_ref_t clipper, lx_size_t mode, lx_ellipse_ref_t ellipse);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


