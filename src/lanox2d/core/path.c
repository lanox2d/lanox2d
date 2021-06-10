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

// the path type
typedef struct lx_path_t_ {
    lx_iterator_base_t  base;
    lx_shape_t          hint;
    lx_uint8_t          flags;
    lx_rect_t           bounds;
    lx_point_t          head;
    lx_path_item_t      item;
    lx_array_ref_t      codes;
    lx_array_ref_t      points;
    lx_polygon_t        polygon;
    lx_array_ref_t      polygon_points;
    lx_array_ref_t      polygon_counts;
}lx_path_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

static lx_inline lx_bool_t lx_path_is_last_code(lx_path_t* path, lx_uint8_t code) {
    lx_assert(path->codes);
    lx_uint8_t* pcode = lx_array_last(path->codes);
    return pcode? *pcode == code : lx_false;
}

static lx_inline lx_void_t lx_path_insert_code(lx_path_t* path, lx_uint8_t code) {
    lx_assert(path->codes);
    lx_array_insert_tail(path->codes, &code);
}

static lx_bool_t lx_path_make_hint(lx_path_t* path) {
    lx_assert_and_check_return_val(path && path->codes && path->points, lx_false);

    // clear hint first
    path->hint.type = LX_SHAPE_TYPE_NONE;

    // no curve? make bounds
    if (!(path->flags & LX_PATH_FLAG_CURVE)) {

        // the codes
        lx_uint8_t const* codes = (lx_uint8_t const*)lx_array_data(path->codes);
        lx_assert_and_check_return_val(codes, lx_false);

        // the points
        lx_point_ref_t points = (lx_point_ref_t)lx_array_data(path->points);
        lx_assert_and_check_return_val(points, lx_false);

        // is rect?
        lx_size_t count = lx_array_size(path->points);
        if (    count == 5
            &&  points[0].x == points[4].x && points[0].y == points[4].y
            &&  codes[0] == LX_PATH_CODE_MOVE
            &&  codes[1] == LX_PATH_CODE_LINE
            &&  codes[2] == LX_PATH_CODE_LINE
            &&  codes[3] == LX_PATH_CODE_LINE
            &&  codes[4] == LX_PATH_CODE_LINE
            &&  (   (   points[0].x != points[1].x && points[0].y == points[1].y
                    &&  points[1].x == points[2].x && points[1].y != points[2].y
                    &&  points[2].x != points[3].x && points[2].y == points[3].y
                    &&  points[3].x == points[4].x && points[3].y != points[4].y)
                ||
                    (   points[0].x == points[1].x && points[0].y != points[1].y
                    &&  points[1].x != points[2].x && points[1].y == points[2].y
                    &&  points[2].x == points[3].x && points[2].y != points[3].y
                    &&  points[3].x != points[4].x && points[3].y == points[4].y))) {
            path->hint.type = LX_SHAPE_TYPE_RECT;
            lx_bounds_make(&path->hint.u.rect, points, 4);
        } else if ( count == 4 // is triangle?
                &&  points[0].x == points[3].x && points[0].y == points[3].y
                &&  codes[0] == LX_PATH_CODE_MOVE
                &&  codes[1] == LX_PATH_CODE_LINE
                &&  codes[2] == LX_PATH_CODE_LINE
                &&  codes[3] == LX_PATH_CODE_LINE
                &&  points[0].x != points[1].x && points[0].y != points[1].y
                &&  points[0].x != points[2].x && points[0].y != points[2].y
                &&  points[1].x != points[2].x && points[1].y != points[2].y) {
            path->hint.type          = LX_SHAPE_TYPE_TRIANGLE;
            path->hint.u.triangle.p0 = points[0];
            path->hint.u.triangle.p1 = points[1];
            path->hint.u.triangle.p2 = points[2];
        } else if ( count == 2 // is line?
                &&  codes[0] == LX_PATH_CODE_MOVE
                &&  codes[1] == LX_PATH_CODE_LINE
                &&  points[0].x != points[1].x && points[0].y != points[1].y) {
            path->hint.type      = LX_SHAPE_TYPE_LINE;
            path->hint.u.line.p0 = points[0];
            path->hint.u.line.p1 = points[1];
        } else if (count == 1 && codes[0] == LX_PATH_CODE_MOVE) { // is point?
            path->hint.type     = LX_SHAPE_TYPE_POINT;
            path->hint.u.point  = points[0];
        }
    }
    return lx_true;
}

static lx_bool_t lx_path_make_convex(lx_path_t* path) {
    lx_assert_and_check_return_val(path && path->codes && path->points, lx_false);

    // clear convex first
    path->flags &= ~LX_PATH_FLAG_CONVEX;

    // attempt to analyze convex from the hint shape first
    lx_shape_ref_t hint = lx_path_hint((lx_path_ref_t)path);
    if (hint && hint->type) {
        switch (hint->type) {
        case LX_SHAPE_TYPE_RECT:
        case LX_SHAPE_TYPE_CIRCLE:
        case LX_SHAPE_TYPE_ELLIPSE:
        case LX_SHAPE_TYPE_TRIANGLE:
        case LX_SHAPE_TYPE_ROUND_RECT:
            path->flags |= LX_PATH_FLAG_CONVEX;
            break;
        default:
            break;
        }
    }

    // analyze convex from the single closed contour
    if (   !(path->flags & LX_PATH_FLAG_CONVEX)
        &&  (path->flags & LX_PATH_FLAG_SINGLE)
        &&  (path->flags & LX_PATH_FLAG_CLOSED)
        &&  lx_array_size(path->codes) > 3) {

        // init flag first
        path->flags |= LX_PATH_FLAG_CONVEX;

        // analyze it
        lx_fixed_t x0 = 0;
        lx_fixed_t y0 = 0;
        lx_fixed_t x1 = 0;
        lx_fixed_t y1 = 0;
        lx_fixed_t x2 = 0;
        lx_fixed_t y2 = 0;
        lx_long_t  cross = 0;
        lx_long_t  cross_prev = 0;
        lx_hong_t  cross_value = 0;
        lx_size_t  point_count = 0;
        lx_size_t  contour_count = 0;
        lx_bool_t  finished = lx_false;
        lx_for_all_if (lx_path_item_ref_t, item, path, item && contour_count < 2) {
            switch (item->code) {
            case LX_PATH_CODE_MOVE: {
                x0 = x1;
                y0 = y1;
                x1 = x2;
                y1 = y2;
                x2 = lx_float_to_fixed(item->points[0].x);
                y2 = lx_float_to_fixed(item->points[0].y);
                contour_count++;
                point_count++;
                break;
            }
            case LX_PATH_CODE_LINE: {
                x0 = x1;
                y0 = y1;
                x1 = x2;
                y1 = y2;
                x2 = lx_float_to_fixed(item->points[0].x);
                y2 = lx_float_to_fixed(item->points[0].y);
                point_count++;
                break;
            }
            case LX_PATH_CODE_QUAD: {
                x0 = x1;
                y0 = y1;
                x1 = x2;
                y1 = y2;
                x2 = lx_float_to_fixed(item->points[1].x);
                y2 = lx_float_to_fixed(item->points[1].y);

                // points enough?
                if (point_count > 1) {

                    // compute the cross value of the vectors (p1, p0) and (p1, p2)
                    cross_value = (lx_hong_t)(x0 - x1) * (y2 - y1) - (lx_hong_t)(y0 - y1) * (x2 - x1);

                    // compute cross = sign(cross_value)
                    cross = cross_value < 0? -1 : cross_value > 0;

                    // concave contour?
                    if ((cross * cross_prev) < 0) {
                        path->flags &= ~LX_PATH_FLAG_CONVEX;
                        finished = lx_true;
                        break;
                    }
                    cross_prev = cross;
                }

                x0 = x1;
                y0 = y1;
                x1 = x2;
                y1 = y2;
                x2 = lx_float_to_fixed(item->points[2].x);
                y2 = lx_float_to_fixed(item->points[2].y);
                point_count += 2;
                break;
            }
            case LX_PATH_CODE_CUBIC: {
                x0 = x1;
                y0 = y1;
                x1 = x2;
                y1 = y2;
                x2 = lx_float_to_fixed(item->points[1].x);
                y2 = lx_float_to_fixed(item->points[1].y);

                // points enough?
                if (point_count > 1) {

                    // compute the cross value of the vectors (p1, p0) and (p1, p2)
                    cross_value = (lx_hong_t)(x0 - x1) * (y2 - y1) - (lx_hong_t)(y0 - y1) * (x2 - x1);

                    // compute cross = sign(cross_value)
                    cross = cross_value < 0? -1 : cross_value > 0;

                    // concave contour?
                    if ((cross * cross_prev) < 0) {
                        path->flags  &= ~LX_PATH_FLAG_CONVEX;
                        finished = lx_true;
                        break;
                    }
                    cross_prev = cross;
                }

                x0 = x1;
                y0 = y1;
                x1 = x2;
                y1 = y2;
                x2 = lx_float_to_fixed(item->points[2].x);
                y2 = lx_float_to_fixed(item->points[2].y);
                point_count += 3;

                // compute the cross value of the vectors (p1, p0) and (p1, p2)
                cross_value = (lx_hong_t)(x0 - x1) * (y2 - y1) - (lx_hong_t)(y0 - y1) * (x2 - x1);

                // compute cross = sign(cross_value)
                cross = cross_value < 0? -1 : cross_value > 0;

                // concave contour?
                if ((cross * cross_prev) < 0) {
                    path->flags &= ~LX_PATH_FLAG_CONVEX;
                    finished = lx_true;
                    break;
                }

                x0 = x1;
                y0 = y1;
                x1 = x2;
                y1 = y2;
                x2 = lx_float_to_fixed(item->points[3].x);
                y2 = lx_float_to_fixed(item->points[3].y);
                cross_prev = cross;
                break;
            }
            case LX_PATH_CODE_CLOSE: {
                lx_point_ref_t points = (lx_point_ref_t)lx_array_data(path->points);
                lx_assert(points && lx_array_size(path->points) > 1);
                lx_assert(points[0].x == item->points[0].x && points[0].y == item->points[0].y);

                x0 = x1;
                y0 = y1;
                x1 = x2;
                y1 = y2;
                x2 = lx_float_to_fixed(points[1].x);
                y2 = lx_float_to_fixed(points[1].y);
                break;
            }
            default:
                lx_assert(0);
                break;
            }

            // finished?
            lx_check_break(!finished);

            // the points are enough?
            lx_check_continue(point_count > 2);

            // compute the cross value of the vectors (p1, p0) and (p1, p2)
            cross_value = (lx_hong_t)(x0 - x1) * (y2 - y1) - (lx_hong_t)(y0 - y1) * (x2 - x1);

            // compute cross = sign(cross_value)
            cross = cross_value < 0? -1 : cross_value > 0;

            // concave contour?
            if ((cross * cross_prev) < 0)
            {
                path->flags &= ~LX_PATH_FLAG_CONVEX;
                break;
            }

            // update the previous cross
            cross_prev = cross;
        }

        // check
        lx_assert(point_count > 2);
        lx_assert(contour_count == 1);
    }

    // trace
    lx_trace_d("make: convex: %s", (path->flags & LX_PATH_FLAG_CONVEX)? "ok" : "no");

    // ok
    return lx_true;
}

static lx_void_t lx_path_make_quad_for_arc_to(lx_point_ref_t ctrl, lx_point_ref_t point, lx_cpointer_t udata) {
    // append point and skip the first point which the ctrl point is empty
    lx_assert(udata && point);
    if (ctrl) {
        lx_path_quad_to((lx_path_ref_t)udata, ctrl, point);
    }
}

static lx_void_t lx_path_make_quad_for_add_arc(lx_point_ref_t ctrl, lx_point_ref_t point, lx_cpointer_t udata) {
    lx_assert(udata && point);
    if (ctrl) {
        lx_path_quad_to((lx_path_ref_t)udata, ctrl, point);
    } else {
        lx_path_move_to((lx_path_ref_t)udata, point);
    }
}

static lx_void_t lx_path_make_line_for_curve_to(lx_point_ref_t point, lx_cpointer_t udata) {
    lx_value_t* values = (lx_value_t*)udata;
    lx_assert(values && point);

    // the polygon points
    lx_array_ref_t polygon_points = (lx_array_ref_t)values[0].ptr;
    lx_assert(polygon_points);

    // append point
    lx_array_insert_tail(polygon_points, point);
    values[1].u16++;
}

static lx_bool_t lx_path_make_polygon(lx_path_t* path) {
    lx_assert_and_check_return_val(path && path->codes && path->points, lx_false);

    // init polygon counts
    if (!path->polygon_counts) {
        path->polygon_counts = lx_array_init(8, sizeof(lx_uint16_t), lx_null);
    }
    lx_assert_and_check_return_val(path->polygon_counts, lx_false);

    // have curve?
    if (path->flags & LX_PATH_FLAG_CURVE) {

        // init polygon points
        if (!path->polygon_points) {
            path->polygon_points = lx_array_init(lx_array_size(path->points), sizeof(lx_point_t), lx_null);
        }
        lx_assert_and_check_return_val(path->polygon_points, lx_false);

        // clear polygon points and counts
        lx_array_clear(path->polygon_points);
        lx_array_clear(path->polygon_counts);

        // init values
        lx_value_t values[2];
        values[0].ptr = (lx_pointer_t)path->polygon_points;
        values[1].u16 = 0;
        lx_for_all (lx_path_item_ref_t, item, path) {
            switch (item->code) {
            case LX_PATH_CODE_MOVE: {
                if (values[1].u16) lx_array_insert_tail(path->polygon_counts, &values[1].u16);
                lx_array_insert_tail(path->polygon_points, &item->points[0]);
                values[1].u16 = 1;
                break;
            }
            case LX_PATH_CODE_LINE: {
                lx_array_insert_tail(path->polygon_points, &item->points[1]);
                values[1].u16++;
                break;
            }
            case LX_PATH_CODE_QUAD: {
                lx_bezier2_make_line(item->points, lx_path_make_line_for_curve_to, values);
                break;
            }
            case LX_PATH_CODE_CUBIC: {
                lx_bezier3_make_line(item->points, lx_path_make_line_for_curve_to, values);
                break;
            }
            case LX_PATH_CODE_CLOSE:
            default:
                break;
            }
        }

        // append the last count
        if (values[1].u16) {
            lx_array_insert_tail(path->polygon_counts, &values[1].u16);
            values[1].u16 = 0;
        }

        // append the tail count
        lx_uint16_t zero = 0;
        lx_array_insert_tail(path->polygon_counts, &zero);

        // init polygon
        path->polygon.points = (lx_point_ref_t)lx_array_data(path->polygon_points);
        path->polygon.counts = (lx_uint16_t*)lx_array_data(path->polygon_counts);
    }
    // only move-to and line-to? using the points directly
    else {
        // init polygon counts
        lx_uint16_t count = 0;
        lx_array_clear(path->polygon_counts);
        lx_for_all (lx_long_t, code, path->codes) {
            lx_assert(code >= 0 && code < LX_PATH_CODE_MAXN);
            if (code == LX_PATH_CODE_MOVE) {
                if (count) lx_array_insert_tail(path->polygon_counts, &count);
                count = 0;
            }
            count += (lx_uint16_t)lx_path_point_step(code);
        }

        // append the last count
        if (count) {
            lx_array_insert_tail(path->polygon_counts, &count);
            count = 0;
        }

        // append the tail count
        count = 0;
        lx_array_insert_tail(path->polygon_counts, &count);

        // init polygon
        path->polygon.points = (lx_point_ref_t)lx_array_data(path->points);
        path->polygon.counts = (lx_uint16_t*)lx_array_data(path->polygon_counts);
    }

    // check
    lx_assert_and_check_return_val(path->polygon.points && path->polygon.counts, lx_false);

    // is convex polygon?
    path->polygon.convex = lx_path_convex((lx_path_ref_t)path);
    return lx_true;
}

static lx_size_t lx_path_iterator_head(lx_iterator_ref_t iterator) {
    return 0;
}

static lx_size_t lx_path_iterator_tail(lx_iterator_ref_t iterator) {
    lx_path_t* path = (lx_path_t*)iterator->container;
    lx_assert(path && path->codes && path->points);

    lx_size_t code_tail  = lx_array_size(path->codes);
    lx_size_t point_tail = lx_array_size(path->points);
    lx_assert(code_tail <= LX_MAXU16 && point_tail <= LX_MAXU16);
    return ((code_tail << 16) | point_tail);
}

static lx_size_t lx_path_iterator_next(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_path_t* path = (lx_path_t*)iterator->container;
    lx_assert(path && path->codes);

    // the code
    lx_uint8_t* pcode = (lx_uint8_t*)lx_array_item(path->codes, itor >> 16);
    lx_uint8_t  code = *pcode;
    lx_assert(code < LX_PATH_CODE_MAXN);

    /* code_index++
     * point_index += point_step
     */
    return itor + (0x10000 | lx_path_point_step(code));
}

static lx_size_t lx_path_iterator_prev(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_path_t* path = (lx_path_t*)iterator->container;
    lx_assert(path && path->codes);

    // check the code index
    lx_assert(itor >> 16);

    // the code
    lx_uint8_t* pcode = (lx_uint8_t*)lx_array_item(path->codes, (itor >> 16) - 1);
    lx_uint8_t  code = *pcode;
    lx_assert(code < LX_PATH_CODE_MAXN);

    // check the point index
    lx_assert((itor & 0xffff) >= lx_path_point_step(code));

    /* code_index--
     * point_index -= point_step
     */
    return itor - (0x10000 | lx_path_point_step(code));
}

static lx_pointer_t lx_path_iterator_item(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_path_t* path = (lx_path_t*)iterator->container;
    lx_assert(path && path->codes && path->points);

    // get code
    lx_size_t   code_index  = itor >> 16;
    lx_size_t   point_index = itor & 0xffff;
    lx_uint8_t* pcode       = (lx_uint8_t*)lx_array_item(path->codes, code_index);
    lx_uint8_t  code        = *pcode;
    lx_assert(code < 1 || point_index);

    // get item
    path->item.code   = code;
    path->item.points = (lx_point_ref_t)lx_array_item(path->points, code < 1? point_index : point_index - 1);
    lx_assert(path->item.points);
    return &path->item;
}

static lx_void_t lx_path_iterator_of(lx_iterator_ref_t iterator, lx_cpointer_t container) {
    static lx_iterator_op_t op = {
        lx_path_iterator_head,
        lx_path_iterator_tail,
        lx_path_iterator_prev,
        lx_path_iterator_next,
        lx_path_iterator_item
    };
    iterator->container = container;
    iterator->mode      = LX_ITERATOR_MODE_FORWARD | LX_ITERATOR_MODE_REVERSE | LX_ITERATOR_MODE_READONLY;
    iterator->op        = &op;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_path_ref_t lx_path_init() {
    lx_bool_t  ok = lx_false;
    lx_path_t* path = lx_null;
    do {
        // init path
        path = lx_malloc0_type(lx_path_t);
        lx_assert_and_check_break(path);

        path->hint.type        = LX_SHAPE_TYPE_NONE;
        path->flags            = LX_PATH_FLAG_DIRTY_ALL | LX_PATH_FLAG_CLOSED | LX_PATH_FLAG_SINGLE;
        path->base.iterator_of = lx_path_iterator_of;

        // init codes
        path->codes = lx_array_init(LX_PATH_POINTS_GROW >> 1, sizeof(lx_uint8_t), lx_null);
        lx_assert_and_check_break(path->codes);

        // init points
        path->points = lx_array_init(LX_PATH_POINTS_GROW, sizeof(lx_point_t), lx_null);
        lx_assert_and_check_break(path->points);

        // ok
        ok = lx_true;

    } while (0);

    if (!ok && path) {
        lx_path_exit((lx_path_ref_t)path);
        path = lx_null;
    }
    return (lx_path_ref_t)path;
}

lx_void_t lx_path_exit(lx_path_ref_t self) {
    lx_path_t* path = (lx_path_t*)self;
    if (path) {
        if (path->polygon_points) {
            lx_array_exit(path->polygon_points);
            path->polygon_points = lx_null;
        }
        if (path->polygon_counts) {
            lx_array_exit(path->polygon_counts);
            path->polygon_counts = lx_null;
        }
        if (path->points) {
            lx_array_exit(path->points);
            path->points = lx_null;
        }
        if (path->codes) {
            lx_array_exit(path->codes);
            path->codes = lx_null;
        }
        lx_free(path);
    }
}

lx_void_t lx_path_clear(lx_path_ref_t self) {
    lx_path_t* path = (lx_path_t*)self;
    if (path) {
        path->flags = LX_PATH_FLAG_DIRTY_ALL | LX_PATH_FLAG_SINGLE;
        if (path->codes) {
            lx_array_clear(path->codes);
        }
        if (path->points) {
            lx_array_clear(path->points);
        }
    }
}

lx_void_t lx_path_copy(lx_path_ref_t self, lx_path_ref_t copied) {
    lx_path_t* path        = (lx_path_t*)self;
    lx_path_t* path_copied = (lx_path_t*)copied;
    lx_assert_and_check_return(path && path->codes && path->points);
    lx_assert_and_check_return(path_copied && path_copied->codes && path_copied->points);

    // empty? clear it
    if (lx_path_empty(copied)) {
        lx_path_clear(self);
        return ;
    }

    // copy path
    path->flags  = path_copied->flags | LX_PATH_FLAG_DIRTY_POLYGON;
    path->hint   = path_copied->hint;
    path->head   = path_copied->head;
    path->bounds = path_copied->bounds;
    lx_array_copy(path->codes, path_copied->codes);
    lx_array_copy(path->points, path_copied->points);
}

lx_bool_t lx_path_empty(lx_path_ref_t self) {
    lx_path_t* path = (lx_path_t*)self;
    lx_assert_and_check_return_val(path && path->codes, lx_true);
    return lx_array_size(path->codes) == 0;
}

lx_rect_ref_t lx_path_bounds(lx_path_ref_t self) {
    lx_path_t* path = (lx_path_t*)self;
    lx_assert_and_check_return_val(path && path->points, lx_null);

    // empty?
    if (lx_path_empty(self)) {
        return lx_null;
    }

    // dirty? make bounds
    if (path->flags & LX_PATH_FLAG_DIRTY_BOUNDS) {

        // attempt to make bounds from the hint shape first
        if (!(path->flags & LX_PATH_FLAG_DIRTY_HINT) && path->hint.type) {
            switch (path->hint.type) {
            case LX_SHAPE_TYPE_RECT: {
                    path->bounds    = path->hint.u.rect;
                    path->flags      &= ~LX_PATH_FLAG_DIRTY_BOUNDS;
                }
                break;
            case LX_SHAPE_TYPE_CIRCLE: {
                    path->bounds.x  = path->hint.u.circle.c.x - path->hint.u.circle.r;
                    path->bounds.y  = path->hint.u.circle.c.y - path->hint.u.circle.r;
                    path->bounds.w  = path->hint.u.circle.r * 2.0f;
                    path->bounds.h  = path->bounds.w;
                    path->flags      &= ~LX_PATH_FLAG_DIRTY_BOUNDS;
                }
                break;
            case LX_SHAPE_TYPE_ELLIPSE: {
                    path->bounds.x  = path->hint.u.ellipse.c.x - path->hint.u.ellipse.rx;
                    path->bounds.y  = path->hint.u.ellipse.c.y - path->hint.u.ellipse.ry;
                    path->bounds.w  = path->hint.u.ellipse.rx * 2.0f;
                    path->bounds.h  = path->hint.u.ellipse.ry * 2.0f;
                    path->flags      &= ~LX_PATH_FLAG_DIRTY_BOUNDS;
                }
                break;
            default:
                break;
            }
        }

        // make bounds from points
        if (path->flags & LX_PATH_FLAG_DIRTY_BOUNDS) {
            lx_point_ref_t points = (lx_point_ref_t)lx_array_data(path->points);
            if (points) {
                lx_bounds_make(&path->bounds, points, lx_array_size(path->points));
                path->flags &= ~LX_PATH_FLAG_DIRTY_BOUNDS;
            }
        }
    }
    lx_assert_and_check_return_val(!(path->flags & LX_PATH_FLAG_DIRTY_BOUNDS), lx_null);
    return &path->bounds;
}

lx_bool_t lx_path_convex(lx_path_ref_t self) {
    lx_path_t* path = (lx_path_t*)self;
    if (path) {
        if (lx_path_empty(self)) {
            return lx_true;
        }
        // convex dirty? remake it
        if (path->flags & LX_PATH_FLAG_DIRTY_CONVEX) {
            if (lx_path_make_convex(path)) {
                path->flags &= ~LX_PATH_FLAG_DIRTY_CONVEX;
            }
        }
        return path->flags & LX_PATH_FLAG_CONVEX;
    }
    return lx_false;
}

lx_void_t lx_path_convex_set(lx_path_ref_t self, lx_bool_t convex) {
    lx_path_t* path = (lx_path_t*)self;
    if (path) {
        if (convex) path->flags |= LX_PATH_FLAG_CONVEX;
        else path->flags &= ~LX_PATH_FLAG_CONVEX;
        path->flags &= ~LX_PATH_FLAG_DIRTY_CONVEX;
    }
}

lx_bool_t lx_path_last(lx_path_ref_t self, lx_point_ref_t point) {
    lx_path_t* path = (lx_path_t*)self;
    lx_assert_and_check_return_val(path && point, lx_false);

    lx_point_ref_t last = (lx_point_ref_t)lx_array_last(path->points);
    if (last) {
        *point = *last;
        return lx_true;
    }
    return lx_false;
}

lx_void_t lx_path_last_set(lx_path_ref_t self, lx_point_ref_t point) {
    lx_path_t* path = (lx_path_t*)self;
    lx_assert_and_check_return(path && point);

    lx_point_ref_t last = (lx_point_ref_t)lx_array_last(path->points);
    lx_assert_and_check_return(last);

    *last = *point;
}

lx_shape_ref_t lx_path_hint(lx_path_ref_t self) {
    lx_path_t* path = (lx_path_t*)self;
    lx_assert_and_check_return_val(path, lx_null);

    if (lx_path_empty(self)) {
        return lx_null;
    }

    // hint dirty? remake it
    if (path->flags & LX_PATH_FLAG_DIRTY_HINT) {
        if (lx_path_make_hint(path)) {
            path->flags &= ~LX_PATH_FLAG_DIRTY_HINT;
        }
    }
    return path->hint.type != LX_SHAPE_TYPE_NONE? &path->hint : lx_null;
}

lx_polygon_ref_t lx_path_polygon(lx_path_ref_t self) {
    lx_path_t* path = (lx_path_t*)self;
    lx_assert_and_check_return_val(path, lx_null);

    if (lx_path_empty(self)) {
        return lx_null;
    }

    // polygon dirty? remake it
    if (path->flags & LX_PATH_FLAG_DIRTY_POLYGON) {
        if (lx_path_make_polygon(path)) {
            path->flags &= ~LX_PATH_FLAG_DIRTY_POLYGON;
        }
    }
    return &path->polygon;
}

lx_void_t lx_path_apply(lx_path_ref_t self, lx_matrix_ref_t matrix) {
    lx_path_t* path = (lx_path_t*)self;
    lx_assert_and_check_return(path && path->points && matrix);

    if (!lx_path_empty(self)) {
        lx_for_all(lx_point_ref_t, point, path->points) {
            lx_point_apply(point, matrix);
        }
    }
}

lx_void_t lx_path_close(lx_path_ref_t self) {
    lx_path_t* path = (lx_path_t*)self;
    lx_assert_and_check_return(path && path->codes && path->points);

    // close path and avoid double close
    if (lx_array_size(path->points) > 2 && !lx_path_is_last_code(path, LX_PATH_CODE_CLOSE)) {
        // patch a line segment if the current point is not equal to the first point of the contour
        lx_point_t last = {0};
        if (lx_path_last(self, &last) && (last.x != path->head.x || last.y != path->head.y))
            lx_path_line_to(self, &path->head);

        // append code
        lx_path_insert_code(path, LX_PATH_CODE_CLOSE);
    }
    path->flags |= LX_PATH_FLAG_CLOSED;
}

lx_void_t lx_path_move_to(lx_path_ref_t self, lx_point_ref_t point)
{
    // check
    lx_path_t* path = (lx_path_t*)self;
    lx_assert_and_check_return(path && path->codes && path->points && point);

    // replace the last point to avoid one alone move-to point
    if (lx_path_is_last_code(path, LX_PATH_CODE_MOVE)) {
        lx_array_replace_last(path->points, point);
    } else {
        // move-to
        lx_path_insert_code(path, LX_PATH_CODE_MOVE);
        lx_array_insert_tail(path->points, point);

        // clear single if the contour count > 1
        if (lx_array_size(path->codes) > 1) {
            path->flags &= ~LX_PATH_FLAG_SINGLE;
        }
    }

    path->head = *point;
    path->flags &= ~LX_PATH_FLAG_CLOSED;
    path->flags |= LX_PATH_FLAG_DIRTY_ALL;
}

lx_void_t lx_path_move2_to(lx_path_ref_t self, lx_float_t x, lx_float_t y) {
    lx_point_t point;
    lx_point_make(&point, x, y);
    lx_path_move_to(self, &point);
}

lx_void_t lx_path_move2i_to(lx_path_ref_t self, lx_long_t x, lx_long_t y) {
    lx_point_t point;
    lx_point_imake(&point, x, y);
    lx_path_move_to(self, &point);
}

lx_void_t lx_path_line_to(lx_path_ref_t self, lx_point_ref_t point) {
    lx_path_t* path = (lx_path_t*)self;
    lx_assert_and_check_return(path && path->codes && path->points && point);

    // closed? patch one move-to point first using the last point
    if (path->flags & LX_PATH_FLAG_CLOSED) {
        // move-to the last point if exists
        lx_point_t last = {0};
        lx_path_last(self, &last);
        lx_path_move_to(self, &last);
    }

    // line-to
    lx_path_insert_code(path, LX_PATH_CODE_LINE);
    lx_array_insert_tail(path->points, point);
    path->flags |= LX_PATH_FLAG_DIRTY_ALL;
}

lx_void_t lx_path_line2_to(lx_path_ref_t self, lx_float_t x, lx_float_t y) {
    lx_point_t point;
    lx_point_make(&point, x, y);
    lx_path_line_to(self, &point);
}

lx_void_t lx_path_line2i_to(lx_path_ref_t self, lx_long_t x, lx_long_t y) {
    lx_point_t point;
    lx_point_imake(&point, x, y);
    lx_path_line_to(self, &point);
}

lx_void_t lx_path_quad_to(lx_path_ref_t self, lx_point_ref_t ctrl, lx_point_ref_t point) {
    lx_path_t* path = (lx_path_t*)self;
    lx_assert_and_check_return(path && path->codes && path->points && ctrl && point);

    // closed? patch one move-to point first using the last point
    if (path->flags & LX_PATH_FLAG_CLOSED) {
        // move-to the last point if exists
        lx_point_t last = {0};
        lx_path_last(self, &last);
        lx_path_move_to(self, &last);
    }

    // quad-to
    lx_path_insert_code(path, LX_PATH_CODE_QUAD);
    lx_array_insert_tail(path->points, ctrl);
    lx_array_insert_tail(path->points, point);
    path->flags |= LX_PATH_FLAG_DIRTY_ALL | LX_PATH_FLAG_CURVE;
}

lx_void_t lx_path_quad2_to(lx_path_ref_t self, lx_float_t cx, lx_float_t cy, lx_float_t x, lx_float_t y) {
    lx_point_t ctrl, point;
    lx_point_make(&ctrl, cx, cy);
    lx_point_make(&point, x, y);
    lx_path_quad_to(self, &ctrl, &point);
}

lx_void_t lx_path_quad2i_to(lx_path_ref_t self, lx_long_t cx, lx_long_t cy, lx_long_t x, lx_long_t y) {
    lx_point_t ctrl, point;
    lx_point_imake(&ctrl, cx, cy);
    lx_point_imake(&point, x, y);
    lx_path_quad_to(self, &ctrl, &point);
}

lx_void_t lx_path_cubic_to(lx_path_ref_t self, lx_point_ref_t ctrl0, lx_point_ref_t ctrl1, lx_point_ref_t point) {
    lx_path_t* path = (lx_path_t*)self;
    lx_assert_and_check_return(path && path->codes && path->points && ctrl0 && ctrl1 && point);

    // closed? patch one move-to point first using the last point
    if (path->flags & LX_PATH_FLAG_CLOSED) {
        // move-to the last point if exists
        lx_point_t last = {0};
        lx_path_last(self, &last);
        lx_path_move_to(self, &last);
    }

    // cubic-to
    lx_path_insert_code(path, LX_PATH_CODE_CUBIC);
    lx_array_insert_tail(path->points, ctrl0);
    lx_array_insert_tail(path->points, ctrl1);
    lx_array_insert_tail(path->points, point);
    path->flags |= LX_PATH_FLAG_DIRTY_ALL | LX_PATH_FLAG_CURVE;
}

lx_void_t lx_path_cubic2_to(lx_path_ref_t self, lx_float_t cx0, lx_float_t cy0, lx_float_t cx1, lx_float_t cy1, lx_float_t x, lx_float_t y) {
    lx_point_t ctrl0, ctrl1, point;
    lx_point_make(&ctrl0, cx0, cy0);
    lx_point_make(&ctrl1, cx1, cy1);
    lx_point_make(&point, x, y);
    lx_path_cubic_to(self, &ctrl0, &ctrl1, &point);
}

lx_void_t lx_path_cubic2i_to(lx_path_ref_t self, lx_long_t cx0, lx_long_t cy0, lx_long_t cx1, lx_long_t cy1, lx_long_t x, lx_long_t y) {
    lx_point_t ctrl0, ctrl1, point;
    lx_point_imake(&ctrl0, cx0, cy0);
    lx_point_imake(&ctrl1, cx1, cy1);
    lx_point_imake(&point, x, y);
    lx_path_cubic_to(self, &ctrl0, &ctrl1, &point);
}

lx_void_t lx_path_arc_to(lx_path_ref_t self, lx_arc_ref_t arc) {
    lx_path_t* path = (lx_path_t*)self;
    lx_assert_and_check_return(path && arc);

    // empty and dirty? make hint
    lx_bool_t hint_maked = lx_false;
    if (lx_path_empty(self) && (path->flags & LX_PATH_FLAG_DIRTY_HINT)) {
        path->hint.type  = LX_SHAPE_TYPE_ARC;
        path->hint.u.arc = *arc;
        hint_maked       = lx_true;
    }

    // make quad points for arc
    lx_arc_make_quad(arc, lx_path_make_quad_for_arc_to, path);

    // hint have been maked? remove dirty
    if (hint_maked) {
        path->flags &= ~LX_PATH_FLAG_DIRTY_HINT;
    }
}

lx_void_t lx_path_arc2_to(lx_path_ref_t self, lx_float_t x0, lx_float_t y0, lx_float_t rx, lx_float_t ry, lx_float_t ab, lx_float_t an) {
    lx_arc_t arc;
    lx_arc_make(&arc, x0, y0, rx, ry, ab, an);
    lx_path_arc_to(self, &arc);
}

lx_void_t lx_path_arc2i_to(lx_path_ref_t self, lx_long_t x0, lx_long_t y0, lx_size_t rx, lx_size_t ry, lx_long_t ab, lx_long_t an) {
    lx_arc_t arc;
    lx_arc_imake(&arc, x0, y0, rx, ry, ab, an);
    lx_path_arc_to(self, &arc);
}

lx_void_t lx_path_path_to(lx_path_ref_t self, lx_path_ref_t added) {
    lx_for_all (lx_path_item_ref_t, item, added) {
        switch (item->code) {
        case LX_PATH_CODE_MOVE: {
            // ignore the first point
            if (item_itor != item_head)
            {
                lx_path_move_to(self, &item->points[0]);
            }
            break;
        }
        case LX_PATH_CODE_LINE:
            lx_path_line_to(self, &item->points[1]);
            break;
        case LX_PATH_CODE_QUAD:
            lx_path_quad_to(self, &item->points[1], &item->points[2]);
            break;
        case LX_PATH_CODE_CUBIC:
            lx_path_cubic_to(self, &item->points[1], &item->points[2], &item->points[3]);
            break;
        case LX_PATH_CODE_CLOSE:
            lx_path_close(self);
            break;
        default:
            lx_trace_e("invalid code: %u", item->code);
            break;
        }
    }
}

lx_void_t lx_path_rpath_to(lx_path_ref_t self, lx_path_ref_t added) {
    lx_bool_t need_move = lx_false;
    lx_bool_t need_close = lx_false;
    lx_rfor_all (lx_path_item_ref_t, item, added) {
        if (need_move) {
            lx_path_move_to(self, &item->points[0]);
            need_move = lx_false;
        }
        switch (item->code) {
        case LX_PATH_CODE_MOVE: {
            if (need_close) {
                lx_path_close(self);
                need_close = lx_false;
            }
            need_move = lx_true;
            break;
        }
        case LX_PATH_CODE_LINE:
            lx_path_line_to(self, &item->points[0]);
            break;
        case LX_PATH_CODE_QUAD:
            lx_path_quad_to(self, &item->points[1], &item->points[0]);
            break;
        case LX_PATH_CODE_CUBIC:
            lx_path_cubic_to(self, &item->points[2], &item->points[1], &item->points[0]);
            break;
        case LX_PATH_CODE_CLOSE:
            // need close path after makeing contour
            need_close = lx_true;
            break;
        default:
            lx_trace_e("invalid code: %u", item->code);
            break;
        }
    }
}

lx_void_t lx_path_add_path(lx_path_ref_t self, lx_path_ref_t added) {
    if (lx_path_empty(self)) {
        lx_path_copy(self, added);
    } else {
        lx_for_all (lx_path_item_ref_t, item, added) {
            switch (item->code) {
            case LX_PATH_CODE_MOVE:
                lx_path_move_to(self, &item->points[0]);
                break;
            case LX_PATH_CODE_LINE:
                lx_path_line_to(self, &item->points[1]);
                break;
            case LX_PATH_CODE_QUAD:
                lx_path_quad_to(self, &item->points[1], &item->points[2]);
                break;
            case LX_PATH_CODE_CUBIC:
                lx_path_cubic_to(self, &item->points[1], &item->points[2], &item->points[3]);
                break;
            case LX_PATH_CODE_CLOSE:
                lx_path_close(self);
                break;
            default:
                lx_trace_e("invalid code: %u", item->code);
                break;
            }
        }
    }
}

lx_void_t lx_path_add_rpath(lx_path_ref_t self, lx_path_ref_t added) {
    lx_bool_t need_move = lx_true;
    lx_bool_t need_close = lx_false;
    lx_rfor_all (lx_path_item_ref_t, item, added) {
        if (need_move) {
            lx_path_move_to(self, &item->points[0]);
            need_move = lx_false;
        }
        switch (item->code) {
        case LX_PATH_CODE_MOVE: {
            if (need_close) {
                lx_path_close(self);
                need_close = lx_false;
            }
            need_move = lx_true;
            break;
        }
        case LX_PATH_CODE_LINE:
            lx_path_line_to(self, &item->points[0]);
            break;
        case LX_PATH_CODE_QUAD:
            lx_path_quad_to(self, &item->points[1], &item->points[0]);
            break;
        case LX_PATH_CODE_CUBIC:
            lx_path_cubic_to(self, &item->points[2], &item->points[1], &item->points[0]);
            break;
        case LX_PATH_CODE_CLOSE:
            // need close path after makeing contour
            need_close = lx_true;
            break;
        default:
            lx_trace_e("invalid code: %lu", item->code);
            break;
        }
    }
}

lx_void_t lx_path_add_line(lx_path_ref_t self, lx_line_ref_t line) {
    lx_path_t* path = (lx_path_t*)self;
    lx_assert_and_check_return(path && line);

    // empty and dirty? make hint
    lx_bool_t hint_maked = lx_false;
    if (lx_path_empty(self) && (path->flags & LX_PATH_FLAG_DIRTY_HINT)) {
        path->hint.type   = LX_SHAPE_TYPE_LINE;
        path->hint.u.line = *line;
        path->flags       &= ~LX_PATH_FLAG_DIRTY_HINT;
        hint_maked        = lx_true;
    }

    // add line
    lx_path_move_to(self, &line->p0);
    lx_path_line_to(self, &line->p1);

    // hint have been maked? remove dirty
    if (hint_maked) {
        path->flags &= ~LX_PATH_FLAG_DIRTY_HINT;
    }
}

lx_void_t lx_path_add_line2(lx_path_ref_t self, lx_float_t x0, lx_float_t y0, lx_float_t x1, lx_float_t y1) {
    lx_line_t line;
    lx_line_make(&line, x0, y0, x1, y1);
    lx_path_add_line(self, &line);
}

lx_void_t lx_path_add_line2i(lx_path_ref_t self, lx_long_t x0, lx_long_t y0, lx_long_t x1, lx_long_t y1) {
    lx_line_t line;
    lx_line_imake(&line, x0, y0, x1, y1);
    lx_path_add_line(self, &line);
}

lx_void_t lx_path_add_arc(lx_path_ref_t self, lx_arc_ref_t arc) {
    lx_path_t* path = (lx_path_t*)self;
    lx_assert_and_check_return(path && path->codes && path->points && arc);

    // ellipse? add it
    if (arc->an >= LX_DEGREE_360 || arc->an <= -LX_DEGREE_360) {
        lx_ellipse_t ellipse;
        lx_ellipse_make(&ellipse, arc->c.x, arc->c.y, arc->rx, arc->ry);
        lx_path_add_ellipse(self, &ellipse, (arc->an > 0)? LX_ROTATE_DIRECTION_CW : LX_ROTATE_DIRECTION_CCW);
        return ;
    }

    // empty and dirty? make hint
    lx_bool_t hint_maked = lx_false;
    if (lx_path_empty(self) && (path->flags & LX_PATH_FLAG_DIRTY_HINT)) {
        path->hint.type  = LX_SHAPE_TYPE_ARC;
        path->hint.u.arc = *arc;
        hint_maked       = lx_true;
    }

    // make quad curves for arc
    lx_arc_make_quad(arc, lx_path_make_quad_for_add_arc, path);

    // hint have been maked? remove dirty
    if (hint_maked) {
        path->flags &= ~LX_PATH_FLAG_DIRTY_HINT;
    }
}

lx_void_t lx_path_add_arc2(lx_path_ref_t self, lx_float_t x0, lx_float_t y0, lx_float_t rx, lx_float_t ry, lx_float_t ab, lx_float_t an) {
    lx_arc_t arc;
    lx_arc_make(&arc, x0, y0, rx, ry, ab, an);
    lx_path_add_arc(self, &arc);
}

lx_void_t lx_path_add_arc2i(lx_path_ref_t self, lx_long_t x0, lx_long_t y0, lx_size_t rx, lx_size_t ry, lx_long_t ab, lx_long_t an) {
    lx_arc_t arc;
    lx_arc_imake(&arc, x0, y0, rx, ry, ab, an);
    lx_path_add_arc(self, &arc);
}

lx_void_t lx_path_add_triangle(lx_path_ref_t self, lx_triangle_ref_t triangle) {
    lx_path_t* path = (lx_path_t*)self;
    lx_assert_and_check_return(path && triangle);

    // empty and dirty? make hint
    lx_bool_t hint_maked = lx_false;
    if (lx_path_empty(self) && (path->flags & LX_PATH_FLAG_DIRTY_HINT)) {
        path->hint.type       = LX_SHAPE_TYPE_TRIANGLE;
        path->hint.u.triangle = *triangle;
        hint_maked            = lx_true;
    }

    // add triangle
    lx_path_move_to(self, &triangle->p0);
    lx_path_line_to(self, &triangle->p1);
    lx_path_line_to(self, &triangle->p2);
    lx_path_close(self);

    // hint have been maked? remove dirty
    if (hint_maked) {
        path->flags &= ~LX_PATH_FLAG_DIRTY_HINT;
    }
}

lx_void_t lx_path_add_triangle2(lx_path_ref_t self, lx_float_t x0, lx_float_t y0, lx_float_t x1, lx_float_t y1, lx_float_t x2, lx_float_t y2) {
    lx_triangle_t triangle;
    lx_triangle_make(&triangle, x0, y0, x1, y1, x2, y2);
    lx_path_add_triangle(self, &triangle);
}

lx_void_t lx_path_add_triangle2i(lx_path_ref_t self, lx_long_t x0, lx_long_t y0, lx_long_t x1, lx_long_t y1, lx_long_t x2, lx_long_t y2) {
    lx_triangle_t triangle;
    lx_triangle_imake(&triangle, x0, y0, x1, y1, x2, y2);
    lx_path_add_triangle(self, &triangle);
}

lx_void_t lx_path_add_rect(lx_path_ref_t self, lx_rect_ref_t rect, lx_size_t direction) {
    lx_path_t* path = (lx_path_t*)self;
    lx_assert_and_check_return(path && rect);

    // empty and dirty? make hint
    lx_bool_t hint_maked = lx_false;
    if (lx_path_empty(self) && (path->flags & LX_PATH_FLAG_DIRTY_HINT)) {
        path->hint.type   = LX_SHAPE_TYPE_RECT;
        path->hint.u.rect = *rect;
        hint_maked        = lx_true;
    }

    // add rect
    lx_path_move2_to(self, rect->x, rect->y);
    if (direction == LX_ROTATE_DIRECTION_CW) {
        lx_path_line2_to(self, rect->x + rect->w, rect->y);
        lx_path_line2_to(self, rect->x + rect->w, rect->y + rect->h);
        lx_path_line2_to(self, rect->x, rect->y + rect->h);
    } else {
        lx_path_line2_to(self, rect->x, rect->y + rect->h);
        lx_path_line2_to(self, rect->x + rect->w, rect->y + rect->h);
        lx_path_line2_to(self, rect->x + rect->w, rect->y);
    }
    lx_path_close(self);

    // hint have been maked? remove dirty
    if (hint_maked) {
        path->flags &= ~LX_PATH_FLAG_DIRTY_HINT;
    }
}

lx_void_t lx_path_add_rect2(lx_path_ref_t self, lx_float_t x, lx_float_t y, lx_float_t w, lx_float_t h, lx_size_t direction) {
    lx_rect_t rect;
    lx_rect_make(&rect, x, y, w, h);
    lx_path_add_rect(self, &rect, direction);
}

lx_void_t lx_path_add_rect2i(lx_path_ref_t self, lx_long_t x, lx_long_t y, lx_size_t w, lx_size_t h, lx_size_t direction) {
    lx_rect_t rect;
    lx_rect_imake(&rect, x, y, w, h);
    lx_path_add_rect(self, &rect, direction);
}

lx_void_t lx_path_add_round_rect(lx_path_ref_t self, lx_round_rect_ref_t rect, lx_size_t direction) {
    lx_path_t* path = (lx_path_t*)self;
    lx_assert_and_check_return(path && rect);

    // is rect or ellipse?
    if (lx_round_rect_is_rect(rect)) {
        lx_path_add_rect(self, &rect->bounds, direction);
        return ;
    } else if (lx_round_rect_is_ellipse(rect)) {
        lx_ellipse_t ellipse;
        lx_ellipse_make_from_rect(&ellipse, &rect->bounds);
        lx_path_add_ellipse(self, &ellipse, direction);
        return ;
    }

    // empty and dirty? make hint
    lx_bool_t hint_maked = lx_false;
    if (lx_path_empty(self) && (path->flags & LX_PATH_FLAG_DIRTY_HINT)) {
        path->hint.type         = LX_SHAPE_TYPE_ROUND_RECT;
        path->hint.u.round_rect = *rect;
        hint_maked              = lx_true;
    }

    // the bounds
    lx_float_t xl = rect->bounds.x;
    lx_float_t yt = rect->bounds.y;
    lx_float_t xr = xl + rect->bounds.w;
    lx_float_t yb = yt + rect->bounds.h;

    // the radius
    lx_float_t rx1 = rect->radius[LX_RECT_CORNER_LT].x;
    lx_float_t ry1 = rect->radius[LX_RECT_CORNER_LT].y;
    lx_float_t rx2 = rect->radius[LX_RECT_CORNER_RT].x;
    lx_float_t ry2 = rect->radius[LX_RECT_CORNER_RT].y;
    lx_float_t rx3 = rect->radius[LX_RECT_CORNER_RB].x;
    lx_float_t ry3 = rect->radius[LX_RECT_CORNER_RB].y;
    lx_float_t rx4 = rect->radius[LX_RECT_CORNER_LB].x;
    lx_float_t ry4 = rect->radius[LX_RECT_CORNER_LB].y;

    // the centers of the corner
    lx_float_t cx1 = xl + rx1;
    lx_float_t cy1 = yt + ry1;
    lx_float_t cx2 = xr - rx2;
    lx_float_t cy2 = yt + ry2;
    lx_float_t cx3 = xr - rx3;
    lx_float_t cy3 = yb - ry3;
    lx_float_t cx4 = xl + rx4;
    lx_float_t cy4 = yb - ry4;

    // the factors of the left-top corner
    lx_float_t sx1 = rx1 * LX_TAN_PIOVER8;
    lx_float_t sy1 = ry1 * LX_TAN_PIOVER8;
    lx_float_t mx1 = rx1 * LX_SQRT2_OVER2;
    lx_float_t my1 = ry1 * LX_SQRT2_OVER2;

    // the factors of the right-top corner
    lx_float_t sx2 = rx2 * LX_TAN_PIOVER8;
    lx_float_t sy2 = ry2 * LX_TAN_PIOVER8;
    lx_float_t mx2 = rx2 * LX_SQRT2_OVER2;
    lx_float_t my2 = ry2 * LX_SQRT2_OVER2;

    // the factors of the right-bottom corner
    lx_float_t sx3 = rx3 * LX_TAN_PIOVER8;
    lx_float_t sy3 = ry3 * LX_TAN_PIOVER8;
    lx_float_t mx3 = rx3 * LX_SQRT2_OVER2;
    lx_float_t my3 = ry3 * LX_SQRT2_OVER2;

    // the factors of the left-bottom corner
    lx_float_t sx4 = rx4 * LX_TAN_PIOVER8;
    lx_float_t sy4 = ry4 * LX_TAN_PIOVER8;
    lx_float_t mx4 = rx4 * LX_SQRT2_OVER2;
    lx_float_t my4 = ry4 * LX_SQRT2_OVER2;

    // move to the first point
    lx_path_move2_to(self, xl, yt + ry1);

#if 0
    // add the round rect
    if (direction == LX_ROTATE_DIRECTION_CW) {
        lx_path_arc2_to(self,   xl + rx1,   yt + ry1, rx1, ry1, -LX_DEGREE_180, LX_DEGREE_90    );
        lx_path_line2_to(self,  xr - rx2,   yt                                                  );
        lx_path_arc2_to(self,   xr - rx2,   yt + ry2, rx2, ry2, -LX_DEGREE_90,  LX_DEGREE_90    );
        lx_path_line2_to(self,  xr,         yb - ry3                                            );
        lx_path_arc2_to(self,   xr - rx2,   yb - ry3, rx3, ry3, 0,              LX_DEGREE_90    );
        lx_path_line2_to(self,  xl + rx4,   yb                                                  );
        lx_path_arc2_to(self,   xl + rx4,   yb - ry4, rx4, ry4, LX_DEGREE_90,   LX_DEGREE_90    );
        lx_path_line2_to(self,  xl,         yt + ry1                                            );
    } else {
        lx_path_line2_to(self,  xl,         yb - ry4                                            );
        lx_path_arc2_to(self,   xl + rx4,   yb - ry4, rx4, ry4, LX_DEGREE_180,  -LX_DEGREE_90   );
        lx_path_line2_to(self,  xr - rx3,   yb                                                  );
        lx_path_arc2_to(self,   xr - rx2,   yb - ry3, rx3, ry3, LX_DEGREE_90,   -LX_DEGREE_90   );
        lx_path_line2_to(self,  xr,         yt + ry3                                            );
        lx_path_arc2_to(self,   xr - rx2,   yt + ry2, rx2, ry2, 0,              -LX_DEGREE_90   );
        lx_path_line2_to(self,  xl + rx1,   yt                                                  );
        lx_path_arc2_to(self,   xl + rx1,   yt + ry1, rx1, ry1, -LX_DEGREE_90,  -LX_DEGREE_90   );
    }
#else
    /* add the round rect
     *
     * see lx_path_add_ellipse
     */
    if (direction == LX_ROTATE_DIRECTION_CW) {
        // the left-top corner
        lx_path_quad2_to(self, xl,          cy1 - sy1,  cx1 - mx1,  cy1 - my1   );
        lx_path_quad2_to(self, cx1 - sx1,   yt,         cx1,        yt          );
        lx_path_line2_to(self, cx2,         yt                                  );

        // the right-top corner
        lx_path_quad2_to(self, cx2 + sx2,   yt,         cx2 + mx2,  cy2 - my2   );
        lx_path_quad2_to(self, xr,          cy2 - sy2,  xr,         cy2         );
        lx_path_line2_to(self, xr,          cy3                                 );

        // the right-bottom corner
        lx_path_quad2_to(self, xr,          cy3 + sy3,  cx3 + mx3,  cy3 + my3   );
        lx_path_quad2_to(self, cx3 + sx3,   yb,         cx3,        yb          );
        lx_path_line2_to(self, cx4,         yb                                  );

        // the left-bottom corner
        lx_path_quad2_to(self, cx4 - sx4,   yb,         cx4 - mx4,  cy4 + my4   );
        lx_path_quad2_to(self, xl,          cy4 + sy4,  xl,         cy4         );
        lx_path_line2_to(self, xl,          cy1                                 );
    } else {
        // the left-bottom corner
        lx_path_line2_to(self, xl,          cy4                                 );
        lx_path_quad2_to(self, cx4 - rx4,   cy4 + sy4,  cx4 - mx4,  cy4 + my4   );
        lx_path_quad2_to(self, cx4 - sx4,   yb,         cx4,        yb          );

        // the right-bottom corner
        lx_path_line2_to(self, cx3,         yb                                  );
        lx_path_quad2_to(self, cx3 + sx3,   yb,         cx3 + mx3,  cy3 + my3   );
        lx_path_quad2_to(self, xr,          cy3 + sy3,  xr,         cy3         );

        // the right-top corner
        lx_path_line2_to(self, xr,          cy2                                 );
        lx_path_quad2_to(self, xr,          cy2 - sy2,  cx2 + mx2,  cy2 - my2   );
        lx_path_quad2_to(self, cx2 + sx2,   yt,         cx2,        yt          );

        // the left-top corner
        lx_path_line2_to(self, cx1,         yt                                  );
        lx_path_quad2_to(self, cx1 - sx1,   yt,         cx1 - mx1,  cy1 - my1   );
        lx_path_quad2_to(self, xl,          cy1 - sy1,  xl,         cy1         );
    }
#endif

    // close path
    lx_path_close(self);

    // hint have been maked? remove dirty
    if (hint_maked) {
        path->flags &= ~LX_PATH_FLAG_DIRTY_HINT;
    }
}

lx_void_t lx_path_add_round_rect2(lx_path_ref_t self, lx_rect_ref_t bounds, lx_float_t rx, lx_float_t ry, lx_size_t direction) {
    lx_round_rect_t rect;
    lx_round_rect_make_same(&rect, bounds, rx, ry);
    lx_path_add_round_rect(self, &rect, direction);
}

lx_void_t lx_path_add_round_rect2i(lx_path_ref_t self, lx_rect_ref_t bounds, lx_size_t rx, lx_size_t ry, lx_size_t direction) {
    lx_round_rect_t rect;
    lx_round_rect_imake_same(&rect, bounds, rx, ry);
    lx_path_add_round_rect(self, &rect, direction);
}

lx_void_t lx_path_add_circle(lx_path_ref_t self, lx_circle_ref_t circle, lx_size_t direction) {
    lx_path_t* path = (lx_path_t*)self;
    lx_assert_and_check_return(path && circle);

    // empty and dirty? make hint
    lx_bool_t hint_maked = lx_false;
    if (lx_path_empty(self) && (path->flags & LX_PATH_FLAG_DIRTY_HINT)) {
        path->hint.type     = LX_SHAPE_TYPE_CIRCLE;
        path->hint.u.circle = *circle;
        hint_maked          = lx_true;

        // @note remove dirty first before adding ellipse
        path->flags         &= ~LX_PATH_FLAG_DIRTY_HINT;
    }

    // add ellipse
    lx_ellipse_t ellipse;
    lx_ellipse_make(&ellipse, circle->c.x, circle->c.y, circle->r, circle->r);
    lx_path_add_ellipse(self, &ellipse, direction);

    // hint have been maked? remove dirty
    if (hint_maked) {
        path->flags &= ~LX_PATH_FLAG_DIRTY_HINT;
    }
}

lx_void_t lx_path_add_circle2(lx_path_ref_t self, lx_float_t x0, lx_float_t y0, lx_float_t r, lx_size_t direction) {
    lx_circle_t circle;
    lx_circle_make(&circle, x0, y0, r);
    lx_path_add_circle(self, &circle, direction);
}

lx_void_t lx_path_add_circle2i(lx_path_ref_t self, lx_long_t x0, lx_long_t y0, lx_size_t r, lx_size_t direction) {
    lx_circle_t circle;
    lx_circle_imake(&circle, x0, y0, r);
    lx_path_add_circle(self, &circle, direction);
}

lx_void_t lx_path_add_ellipse(lx_path_ref_t self, lx_ellipse_ref_t ellipse, lx_size_t direction) {
    lx_path_t* path = (lx_path_t*)self;
    lx_assert_and_check_return(path && ellipse);

    // empty and dirty? make hint
    lx_bool_t hint_maked = lx_false;
    if (lx_path_empty(self) && (path->flags & LX_PATH_FLAG_DIRTY_HINT)) {
        path->hint.type      = LX_SHAPE_TYPE_ELLIPSE;
        path->hint.u.ellipse = *ellipse;
        hint_maked           = lx_true;
    }

    // init center and radius
    lx_float_t rx = ellipse->rx;
    lx_float_t ry = ellipse->ry;
    lx_float_t x0 = ellipse->c.x;
    lx_float_t y0 = ellipse->c.y;

    // init factor
    lx_float_t sx = rx * LX_TAN_PIOVER8; //< tan(pi/8)
    lx_float_t sy = ry * LX_TAN_PIOVER8;
    lx_float_t mx = rx * LX_SQRT2_OVER2; //< sqrt(2)/2
    lx_float_t my = ry * LX_SQRT2_OVER2;

    // init bounds
    lx_float_t x1 = x0 - rx;
    lx_float_t y1 = y0 - ry;
    lx_float_t x2 = x0 + rx;
    lx_float_t y2 = y0 + ry;

    /* make ellipse path
     *
     * <pre>
     * circle:
     * - quad_to(c, p)
     * - p(x0 + r * sqrt(2) / 2, y0 - r * sqrt(2)/2)
     * - c(x2, y0 - r * tan(pi/8))
     *
     * (x1, y1)
     *  ---------------------------
     * |             |           . |
     * |             |      p  .   |
     * |             |      .      . c
     * |             |   .         |
     * |             |. 45         |
     * |- - - - - - - - - - - - - -| (move-to)
     * |          (x0, y0)         |
     * |             |             |
     * |             |             |
     * |             |             |
     * |             |             |
     *  ---------------------------
     *                      (x2, y2)
     *
     *
     * ellipse:
     * - quad_to(c, p)
     * - p(x0 + rx * sqrt(2) / 2, y0 - ry * sqrt(2)/2)
     * - c(x2, y0 - ry * tan(pi/8))
     *
     * (x1, y1)
     *  ------------------------------------------
     * |                     |            .       |
     * |                     |      p  .          |
     * |                     |      .             . c
     * |                     |   .                |
     * |                     |. 45                |
     * |- - - - - - - - - - - - - - - - - - - - - | (move-to)
     * |                  (x0, y0)                |
     * |                     |                    |
     * |                     |                    |
     * |                     |                    |
     * |                     |                    |
     *  ------------------------------------------
     *                                     (x2, y2)
     *
     *
     * </pre>
     */
    lx_path_move2_to(self, x2, y0);
    if (direction == LX_ROTATE_DIRECTION_CW) {
        // the right-bottom corner
        lx_path_quad2_to(self, x2,          y0 + sy,    x0 + mx,    y0 + my );
        lx_path_quad2_to(self, x0 + sx,     y2,         x0,         y2      );

        // the left-bottom corner
        lx_path_quad2_to(self, x0 - sx,     y2,         x0 - mx,    y0 + my );
        lx_path_quad2_to(self, x1,          y0 + sy,    x1,         y0      );

        // the left-top corner
        lx_path_quad2_to(self, x1,          y0 - sy,    x0 - mx,    y0 - my );
        lx_path_quad2_to(self, x0 - sx,     y1,         x0,         y1      );

        // the right-top corner
        lx_path_quad2_to(self, x0 + sx,     y1,         x0 + mx,    y0 - my );
        lx_path_quad2_to(self, x2,          y0 - sy,    x2,         y0      );
    } else {
        // the right-top corner
        lx_path_quad2_to(self, x2,          y0 - sy,    x0 + mx,    y0 - my );
        lx_path_quad2_to(self, x0 + sx,     y1,         x0,         y1      );

        // the left-top corner
        lx_path_quad2_to(self, x0 - sx,     y1,         x0 - mx,    y0 - my );
        lx_path_quad2_to(self, x1,          y0 - sy,    x1,         y0      );

        // the left-bottom corner
        lx_path_quad2_to(self, x1,          y0 + sy,    x0 - mx,    y0 + my );
        lx_path_quad2_to(self, x0 - sx,     y2,         x0,         y2      );

        // the right-bottom corner
        lx_path_quad2_to(self, x0 + sx,     y2,         x0 + mx,    y0 + my );
        lx_path_quad2_to(self, x2,          y0 + sy,    x2,         y0      );
    }

    // close path
    lx_path_close(self);

    // hint have been maked? remove dirty
    if (hint_maked) {
        path->flags &= ~LX_PATH_FLAG_DIRTY_HINT;
    }
}

lx_void_t lx_path_add_ellipse2(lx_path_ref_t self, lx_float_t x0, lx_float_t y0, lx_float_t rx, lx_float_t ry, lx_size_t direction) {
    lx_ellipse_t ellipse;
    lx_ellipse_make(&ellipse, x0, y0, rx, ry);
    lx_path_add_ellipse(self, &ellipse, direction);
}

lx_void_t lx_path_add_ellipse2i(lx_path_ref_t self, lx_long_t x0, lx_long_t y0, lx_size_t rx, lx_size_t ry, lx_size_t direction) {
    lx_ellipse_t ellipse;
    lx_ellipse_imake(&ellipse, x0, y0, rx, ry);
    lx_path_add_ellipse(self, &ellipse, direction);
}

#ifdef LX_DEBUG
lx_void_t lx_path_dump(lx_path_ref_t self) {
    lx_assert_and_check_return(self);

    lx_trace_i("");
    lx_point_t last = {0};
    if (lx_path_last(self, &last)) {
        lx_trace_i("last: %{point}", &last);
    }

    lx_trace_i("bounds: %{rect}", lx_path_bounds(self));
    lx_for_all_if (lx_path_item_ref_t, item, self, item) {
        switch (item->code) {
        case LX_PATH_CODE_MOVE:
            lx_trace_i("move_to: %{point}", &item->points[0]);
            break;
        case LX_PATH_CODE_LINE:
            lx_trace_i("line_to: %{point}", &item->points[1]);
            break;
        case LX_PATH_CODE_QUAD:
            lx_trace_i("quad_to: %{point}, %{point}", &item->points[1], &item->points[2]);
            break;
        case LX_PATH_CODE_CUBIC:
            lx_trace_i("cubic_to: %{point}, %{point}, %{point}", &item->points[1], &item->points[2], &item->points[3]);
            break;
        case LX_PATH_CODE_CLOSE:
            lx_trace_i("closed");
            break;
        default:
            break;
        }
    }
}
#endif
