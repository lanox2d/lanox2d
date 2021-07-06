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
 * @file        paint.h
 *
 */
#ifndef LX_CORE_PAINT_H
#define LX_CORE_PAINT_H

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

/// the paint mode enum
typedef enum lx_paint_mode_e_ {
    LX_PAINT_MODE_NONE          = 0 //!< none
,   LX_PAINT_MODE_FILL          = 1 //!< fill
,   LX_PAINT_MODE_STROKE        = 2 //!< stroke
,   LX_PAINT_MODE_FILL_STROKE   = 3 //!< fill and stroke
}lx_paint_mode_e;

/// the paint flag enum
typedef enum lx_paint_flag_e_ {
    LX_PAINT_FLAG_NONE          = 0 //!< none
,   LX_PAINT_FLAG_ANTIALIASING  = 1 //!< antialiasing
,   LX_PAINT_FLAG_FILTER_BITMAP = 2 //!< filter bitmap
}lx_paint_flag_e;

/// the paint stroke cap enum
typedef enum lx_paint_stroke_cap_e_ {
    LX_PAINT_STROKE_CAP_BUTT    = 0 //!< no extension
,   LX_PAINT_STROKE_CAP_ROUND   = 1 //!< a semi-circle extension
,   LX_PAINT_STROKE_CAP_SQUARE  = 2 //!< a half square extension
}lx_paint_stroke_cap_e;

/// the paint stroke join enum
typedef enum lx_paint_stroke_join_e_ {
    LX_PAINT_STROKE_JOIN_MITER  = 0 //!< a sharp join
,   LX_PAINT_STROKE_JOIN_ROUND  = 1 //!< a round join
,   LX_PAINT_STROKE_JOIN_BEVEL  = 2 //!< a flat bevel join
}lx_paint_stroke_join_e;

/// the paint fill rule enum
typedef enum lx_paint_fill_rule_e_ {
    LX_PAINT_FILL_RULE_ODD      = 0 //!< odd fill
,   LX_PAINT_FILL_RULE_NONZERO  = 1 //!< non-zero fill
}lx_paint_fill_rule_e;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init paint
 *
 * @return          the paint
 */
lx_paint_ref_t      lx_paint_init(lx_noarg_t);

/*! exit paint
 *
 * @param paint     the paint
 */
lx_void_t           lx_paint_exit(lx_paint_ref_t paint);

/*! clear paint
 *
 * @param paint     the paint
 */
lx_void_t           lx_paint_clear(lx_paint_ref_t paint);

/*! copy paint
 *
 * @param paint     the paint
 * @param copied    the copied paint
 */
lx_void_t           lx_paint_copy(lx_paint_ref_t paint, lx_paint_ref_t copied);

/*! get the paint mode
 *
 * @param paint     the paint
 *
 * @return          the paint mode
 */
lx_size_t           lx_paint_mode(lx_paint_ref_t paint);

/*! set the paint mode
 *
 * @param paint     the paint
 * @param mode      the paint mode
 */
lx_void_t           lx_paint_mode_set(lx_paint_ref_t paint, lx_size_t mode);

/*! get the paint flags
 *
 * @param paint     the paint
 *
 * @return          the paint flags
 */
lx_size_t           lx_paint_flags(lx_paint_ref_t paint);

/*! set the paint flags
 *
 * @param paint     the paint
 * @param flag      the paint flags
 */
lx_void_t           lx_paint_flags_set(lx_paint_ref_t paint, lx_size_t flags);

/*! get the paint color
 *
 * @param paint     the paint
 *
 * @return          the paint color
 */
lx_color_t          lx_paint_color(lx_paint_ref_t paint);

/*! set the paint color
 *
 * @param paint     the paint
 * @param color     the paint color
 */
lx_void_t           lx_paint_color_set(lx_paint_ref_t paint, lx_color_t color);

/*! get the paint alpha
 *
 * @param paint     the paint
 *
 * @return          the paint alpha
 */
lx_byte_t           lx_paint_alpha(lx_paint_ref_t paint);

/*! set the paint alpha
 *
 * @param paint     the paint
 * @param alpha     the paint alpha
 */
lx_void_t           lx_paint_alpha_set(lx_paint_ref_t paint, lx_byte_t alpha);

/*! get the paint width
 *
 * @param paint     the paint
 *
 * @return          the paint width
 */
lx_float_t          lx_paint_stroke_width(lx_paint_ref_t paint);

/*! set the paint width
 *
 * @param paint     the paint
 * @param width     the paint width
 */
lx_void_t           lx_paint_stroke_width_set(lx_paint_ref_t paint, lx_float_t width);

/*! get the paint cap
 *
 * @param paint     the paint
 *
 * @return          the paint cap
 */
lx_size_t           lx_paint_stroke_cap(lx_paint_ref_t paint);

/*! set the paint cap
 *
 * @param paint     the paint
 * @param cap       the paint cap
 */
lx_void_t           lx_paint_stroke_cap_set(lx_paint_ref_t paint, lx_size_t cap);

/*! get the paint join
 *
 * @param paint     the paint
 *
 * @return          the paint join
 */
lx_size_t           lx_paint_stroke_join(lx_paint_ref_t paint);

/*! set the paint join
 *
 * @param paint     the paint
 * @param join      the paint join
 */
lx_void_t           lx_paint_stroke_join_set(lx_paint_ref_t paint, lx_size_t join);

/*! get the limit of the paint miter join
 *
 * <pre>
 *
 * ------------------------------ miter join
 *                    .        . |
 *                      .    L   |
 *            bevel join  .      |
 *                        | .    |  => L / R = sqrt(2)
 *                        |   .  |
 * -----------------|     |      |
 *                  |        R   |
 *                  |            |
 *                  |            |
 *                  |            |
 *                  |            |
 *                  | W = R * 2  |
 *
 *  miter_limit = L / R > 1
 *
 * </pre>
 *
 * @param paint     the paint
 *
 * @return          the miter limit
 */
lx_float_t          lx_paint_stroke_miter(lx_paint_ref_t paint);

/*! set the paint miter limit
 *
 * @param paint     the paint
 * @param miter     the miter limit
 */
lx_void_t           lx_paint_stroke_miter_set(lx_paint_ref_t paint, lx_float_t miter);

/*! get the fill rule
 *
 * @param paint     the paint
 *
 * @return          the fill rule
 */
lx_size_t           lx_paint_fill_rule(lx_paint_ref_t paint);

/*! set the fill rule
 *
 * @param paint     the paint
 * @param rule      the fill rule
 */
lx_void_t           lx_paint_fill_rule_set(lx_paint_ref_t paint, lx_size_t rule);

/*! get the fill texture
 *
 * @param paint     the paint
 *
 * @return          the paint texture
 */
lx_texture_ref_t    lx_paint_fill_texture(lx_paint_ref_t paint);

/*! set the fill texture
 *
 * @param paint     the paint
 * @param texture   the paint texture
 */
lx_void_t           lx_paint_fill_texture_set(lx_paint_ref_t paint, lx_texture_ref_t texture);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


