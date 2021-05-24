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
 * @file        prefix.h
 *
 */
#ifndef LX_CORE_BASICTYPE_PREFIX_H
#define LX_CORE_BASICTYPE_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/// the max width
#define LX_WIDTH_MAX           (8192)

/// the max height
#define LX_HEIGHT_MAX          (8192)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the pixel type
typedef lx_uint32_t                 lx_pixel_t;

/// the bitmap ref type
typedef lx_typeref(bitmap);

/// the canvas ref type
typedef lx_typeref(canvas);

/// the window ref type
typedef lx_typeref(window);

/// the device ref type
typedef lx_typeref(device);

/*! @def the color type
 *
 * the color type
 *
 * @code
 * union lx_c2p_t_ {
 *      lx_color_t c;
 *      lx_pixel_t p;
 * }c2p;
 * c2p.c = c;
 * return c2p.p;
 * @endcode
 *
 */
#ifdef LX_WORDS_BIGENDIAN
typedef struct lx_color_t_ {
    lx_byte_t           a;
    lx_byte_t           r;
    lx_byte_t           g;
    lx_byte_t           b;
}lx_color_t, *lx_color_ref_t;
#else
typedef struct lx_color_t_ {
    lx_byte_t           b;
    lx_byte_t           g;
    lx_byte_t           r;
    lx_byte_t           a;
}lx_color_t, *lx_color_ref_t;
#endif

/*! the matrix type
 *
 * <pre>
 * sx: the x-scale
 * sy: the y-scale
 * kx: the x-skew
 * ky: the y-skew
 * tx: the x-translate
 * ty: the y-translate
 *
 * x' = x * sx + y * kx + tx
 * y' = x * ky + y * sy + ty
 *
 * x'           sx kx tx    x * sx + y * kx + tx
 * y' = x y 1 * ky sy ty =  x * ky + y * sy + ty
 * 1             0  0  1                       1
 *
 * the following table describes how the members of the matrix are used for each type of
 * operation:
 *                    sx                kx               ky              sy                tx     ty
 * rotation           sx * cos          sx * -sin        sy * sin        sy * cos          0      0
 * scaling            sx                0                0               sy                0      0
 * translation        0                 0                0               0                 tx     ty
 * ...
 *
 * </pre>
 *
 */
typedef struct lx_matrix_t_ {
    lx_float_t          sx, kx, tx;
    lx_float_t          ky, sy, ty;
}lx_matrix_t, *lx_matrix_ref_t;

/// the point type
typedef struct lx_point_t_ {
    lx_float_t          x;
    lx_float_t          y;
}lx_point_t, *lx_point_ref_t;

/// the rotate direction enum
typedef enum lx_rotate_direction_e_ {
    LX_ROTATE_DIRECTION_CW    = 0 //!< the clockwise direction
,   LX_ROTATE_DIRECTION_CCW   = 1 //!< the counter-clockwise direction
}lx_rotate_direction_e;

/// the vector type
typedef lx_point_t      lx_vector_t;

/// the vector ref type
typedef lx_point_ref_t  lx_vector_ref_t;

/// the line type
typedef struct lx_line_t_ {
    lx_point_t          p0;
    lx_point_t          p1;
}lx_line_t, *lx_line_ref_t;

/// the triangle type
typedef struct lx_triangle_t_ {
    lx_point_t          p0;
    lx_point_t          p1;
    lx_point_t          p2;
}lx_triangle_t, *lx_triangle_ref_t;

/// the rect type
typedef struct lx_rect_t_ {
    lx_float_t          x;
    lx_float_t          y;
    lx_float_t          w;
    lx_float_t          h;
}lx_rect_t, *lx_rect_ref_t;

/// the rect corner enum
typedef enum lx_rect_corner_e_ {
    LX_RECT_CORNER_LT   = 0     //<! the left-top corner
,   LX_RECT_CORNER_RT   = 1     //<! the right-top corner
,   LX_RECT_CORNER_RB   = 2     //<! the right-bottom corner
,   LX_RECT_CORNER_LB   = 3     //<! the left-bottom corner
,   LX_RECT_CORNER_MAXN = 4     //<! the corner max count
}lx_rect_corner_e;

/// the round rect type
typedef struct lx_round_rect_t_ {
    lx_rect_t           bounds;
    /*! the radius of the four corners
     *
     * <pre>
     *
     *  lt                     rt
     *   --------------------->
     * /|\                     |
     *  |                      |
     *  |                      |
     *  |                      |
     *  |                     \|/
     *  <----------------------
     *  lb                     rb
     *
     * </pre>
     */
    lx_vector_t         radius[LX_RECT_CORNER_MAXN];
}lx_round_rect_t, *lx_round_rect_ref_t;

/// the circle type
typedef struct lx_circle_t_ {
    lx_point_t          c; //!< the center point
    lx_float_t          r; //!< the radius
}lx_circle_t, *lx_circle_ref_t;

/// the ellipse type
typedef struct lx_ellipse_t_ {
    lx_point_t          c;
    lx_float_t          rx; //!< the x-radius
    lx_float_t          ry; //!< the y-radius
}lx_ellipse_t, *lx_ellipse_ref_t;

/// the arc type
typedef struct lx_arc_t_ {
    lx_point_t          c;
    lx_float_t          rx;
    lx_float_t          ry;
    lx_float_t          ab; //!< the start angle
    lx_float_t          an; //!< the sweep angle, clockwise: > 0, counter-clockwise: < 0

}lx_arc_t, *lx_arc_ref_t;

/*! the polygon type
 *
 * @code
    lx_point_t      points[] = {    {x0, y0}, {x1, y1}, {x2, y2}
                                ,   {x3, y3}, {x4, y4}, {x5, y5}, {x3, y3}};
    lx_size_t       counts[] = {3, 4, 0};
    lx_polygon_t    polygon = {points, counts};
 * @endcode
 */
typedef struct lx_polygon_t_ {
    lx_point_ref_t      points;
    lx_uint16_t*        counts;
    lx_bool_t           convex;
}lx_polygon_t, *lx_polygon_ref_t;

/// the gradient type
typedef struct lx_gradient_t_ {
    lx_color_t*         colors;
    lx_float_t*         radios;
    lx_size_t           count;
}lx_gradient_t, *lx_gradient_ref_t;

/// the shape type enum
typedef enum lx_shape_type_e_ {
    LX_SHAPE_TYPE_NONE          = 0x0000 //!< none
,   LX_SHAPE_TYPE_ARC           = 0x0001 //!< arc
,   LX_SHAPE_TYPE_PATH          = 0x0002 //!< path
,   LX_SHAPE_TYPE_LINE          = 0x0003 //!< line
,   LX_SHAPE_TYPE_RECT          = 0x0004 //!< rect
,   LX_SHAPE_TYPE_POINT         = 0x0005 //!< point
,   LX_SHAPE_TYPE_CIRCLE        = 0x0006 //!< circle
,   LX_SHAPE_TYPE_ELLIPSE       = 0x0007 //!< ellipse
,   LX_SHAPE_TYPE_POLYGON       = 0x0008 //!< polygon
,   LX_SHAPE_TYPE_TRIANGLE      = 0x0009 //!< triangle
,   LX_SHAPE_TYPE_ROUND_RECT    = 0x0010 //!< round rect
}lx_shape_type_e;

/// the shape type
typedef struct lx_shape_t_ {
    lx_size_t           type;
    union
    {
        lx_arc_t        arc;
        lx_line_t       line;
        lx_rect_t       rect;
        lx_point_t      point;
        lx_circle_t     circle;
        lx_ellipse_t    ellipse;
        lx_polygon_t    polygon;
        lx_triangle_t   triangle;
        lx_round_rect_t round_rect;
        // TODO path
    }u;

}lx_shape_t, *lx_shape_ref_t;

#endif


