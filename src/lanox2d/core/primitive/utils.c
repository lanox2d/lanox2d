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
 * @file        utils.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "utils.h"
#include "point.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the vsnprintf object entry type
typedef struct lx_vsnprintf_object_entry_t_ {
    lx_char_t const*            name;
    lx_vsnprintf_object_cb_t    callback;
}lx_vsnprintf_object_entry_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */
#ifdef LX_DEBUG
static lx_vsnprintf_object_entry_t* g_vsnprintf_entries = lx_null;
static lx_size_t                    g_vsnprintf_count = 0;
static lx_size_t                    g_vsnprintf_maxn = 16;
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
lx_extern_c_enter
lx_int_t lx_vsnprintf_object(lx_char_t* s, lx_size_t n, lx_char_t const* name, lx_cpointer_t object);
lx_extern_c_leave

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
#ifdef LX_DEBUG
static lx_long_t lx_vsnprintf_object_comp(lx_iterator_ref_t iterator, lx_cpointer_t item, lx_cpointer_t data) {
    lx_vsnprintf_object_entry_t* entry = (lx_vsnprintf_object_entry_t*)item;
    lx_assert(entry && data);
    return lx_strcmp(entry->name, (lx_char_t const*)data);
}

static lx_vsnprintf_object_cb_t lx_vsnprintf_object_find(lx_char_t const* name) {
    lx_assert_and_check_return_val(g_vsnprintf_entries && name, lx_null);

    lx_fixed_array_t fixed_array;
    lx_fixed_array_init_mem(&fixed_array, g_vsnprintf_entries, g_vsnprintf_count, sizeof(lx_vsnprintf_object_entry_t));

    lx_iterator_t iterator;
    lx_iterator_of(&iterator, &fixed_array);
    lx_size_t itor = lx_binary_find_all_if(&iterator, lx_vsnprintf_object_comp, name);
    lx_check_return_val(itor != lx_iterator_tail(&iterator), lx_null);

    return itor < g_vsnprintf_count? g_vsnprintf_entries[itor].callback : lx_null;
}
#endif

static lx_inline lx_int_t lx_vsnprintf_float(lx_char_t* s, lx_size_t n, lx_float_t* value) {
    return lx_snprintf(s, n, "%f", *value);
}

static lx_inline lx_int_t lx_vsnprintf_point(lx_char_t* s, lx_size_t n, lx_point_ref_t point) {
    return lx_snprintf(s, n, "(%{float}, %{float})", &point->x, &point->y);
}

static lx_inline lx_int_t lx_vsnprintf_line(lx_char_t* s, lx_size_t n, lx_line_ref_t line) {
    return lx_snprintf(s, n, "(%{point} => %{point})", &line->p0, &line->p1);
}

static lx_inline lx_int_t lx_vsnprintf_rect(lx_char_t* s, lx_size_t n, lx_rect_ref_t rect) {
    return lx_snprintf(s, n, "(x: %{float}, y: %{float}, w: %{float}, h: %{float})", &rect->x, &rect->y, &rect->w, &rect->h);
}

static lx_inline lx_int_t lx_vsnprintf_round_rect(lx_char_t* s, lx_size_t n, lx_round_rect_ref_t rect) {
    return lx_snprintf(s, n, "(x: %{float}, y: %{float}, w: %{float}, h: %{float}, lt: %{vector}, rt: %{vector}, rb: %{vector}, lb: %{vector})",
                       &rect->bounds.x, &rect->bounds.y, &rect->bounds.w, &rect->bounds.h,
                       &rect->radius[LX_RECT_CORNER_LT],
                       &rect->radius[LX_RECT_CORNER_RT],
                       &rect->radius[LX_RECT_CORNER_RB],
                       &rect->radius[LX_RECT_CORNER_LB]);
}

static lx_inline lx_int_t lx_vsnprintf_triangle(lx_char_t* s, lx_size_t n, lx_triangle_ref_t triangle) {
    return lx_snprintf(s, n, "(%{point}, %{point}, %{point})", &triangle->p0, &triangle->p1, &triangle->p2);
}

static lx_inline lx_int_t lx_vsnprintf_circle(lx_char_t* s, lx_size_t n, lx_circle_ref_t circle) {
    return lx_snprintf(s, n, "(c: %{point}, r: %{float})", &circle->c, &circle->r);
}

static lx_inline lx_int_t lx_vsnprintf_ellipse(lx_char_t* s, lx_size_t n, lx_ellipse_ref_t ellipse) {
    return lx_snprintf(s, n, "(c: %{point}, rx: %{float}, ry: %{float})", &ellipse->c, &ellipse->rx, &ellipse->ry);
}

static lx_inline lx_int_t lx_vsnprintf_arc(lx_char_t* s, lx_size_t n, lx_arc_ref_t arc) {
    return lx_snprintf(s, n, "(c: %{point}, rx: %{float}, ry: %{float}, ab: %{float}, an: %{float})", &arc->c, &arc->rx, &arc->ry, &arc->ab, &arc->an);
}

static lx_inline lx_int_t lx_vsnprintf_color(lx_char_t* s, lx_size_t n, lx_color_ref_t color) {
    return lx_snprintf(s, n, "(a: %u, r: %u, g: %u, b: %u)", color->a, color->r, color->g, color->b);
}

static lx_inline lx_int_t lx_vsnprintf_matrix(lx_char_t* s, lx_size_t n, lx_matrix_ref_t matrix) {
    return lx_snprintf(s, n, "(sx: %{float}, sy: %{float}, kx: %{float}, ky: %{float}, tx: %{float}, ty: %{float})", &matrix->sx, &matrix->sy, &matrix->kx, &matrix->ky, &matrix->tx, &matrix->ty);
}

lx_int_t lx_vsnprintf_object(lx_char_t* s, lx_size_t n, lx_char_t const* name, lx_cpointer_t object) {
    lx_check_return_val(s && n && name && object, -1);
    if (!lx_strcmp(name, "float")) {
        return lx_vsnprintf_float(s, n, (lx_float_t*)object);
    } else if (!lx_strcmp(name, "point") || !lx_strcmp(name, "vector")) {
        return lx_vsnprintf_point(s, n, (lx_point_ref_t)object);
    } else if (!lx_strcmp(name, "line")) {
        return lx_vsnprintf_line(s, n, (lx_line_ref_t)object);
    } else if (!lx_strcmp(name, "rect")) {
        return lx_vsnprintf_rect(s, n, (lx_rect_ref_t)object);
    } else if (!lx_strcmp(name, "round_rect")) {
        return lx_vsnprintf_round_rect(s, n, (lx_round_rect_ref_t)object);
    } else if (!lx_strcmp(name, "triangle")) {
        return lx_vsnprintf_triangle(s, n, (lx_triangle_ref_t)object);
    } else if (!lx_strcmp(name, "circle")) {
        return lx_vsnprintf_circle(s, n, (lx_circle_ref_t)object);
    } else if (!lx_strcmp(name, "ellipse")) {
        return lx_vsnprintf_ellipse(s, n, (lx_ellipse_ref_t)object);
    } else if (!lx_strcmp(name, "arc")) {
        return lx_vsnprintf_arc(s, n, (lx_arc_ref_t)object);
    } else if (!lx_strcmp(name, "color")) {
        return lx_vsnprintf_color(s, n, (lx_color_ref_t)object);
    } else if (!lx_strcmp(name, "matrix")) {
        return lx_vsnprintf_matrix(s, n, (lx_matrix_ref_t)object);
#ifdef LX_DEBUG
    } else if (g_vsnprintf_count) {
        lx_vsnprintf_object_cb_t callback = lx_vsnprintf_object_find(name);
        if (callback) {
            return callback(s, n, object);
        }
#endif
    }
    return -1;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

lx_void_t lx_bounds_make(lx_rect_ref_t bounds, lx_point_ref_t points, lx_size_t count) {
    lx_assert(bounds && points && count);
    lx_point_ref_t point = points++; count--;
    lx_float_t x0 = point->x;
    lx_float_t y0 = point->y;
    lx_float_t x1 = x0;
    lx_float_t y1 = y0;
    lx_float_t x;
    lx_float_t y;
    while (count--) {
        x = points->x;
        y = points->y;
        if (x < x0) x0 = x;
        if (y < y0) y0 = y;
        if (x > x1) x1 = x;
        if (y > y1) y1 = y;
        points++;
    }
    bounds->x = x0;
    bounds->y = y0;
    bounds->w = x1 - x0;
    bounds->h = y1 - y0;
}

lx_void_t lx_matrix_apply_points(lx_matrix_ref_t matrix, lx_point_ref_t points, lx_size_t count) {
    lx_assert(matrix && points && count);
    lx_size_t i = 0;
    for (i = 0; i < count; i++) {
        lx_point_apply(points + i, matrix);
    }
}

#ifdef LX_DEBUG
lx_void_t lx_vsnprintf_object_register(lx_char_t const* name, lx_vsnprintf_object_cb_t callback) {
    lx_assert_and_check_return(name && g_vsnprintf_maxn);

    // init entries
    if (!g_vsnprintf_entries) {
        g_vsnprintf_entries = lx_nalloc_type(g_vsnprintf_maxn, lx_vsnprintf_object_entry_t);
    }
    lx_assert_and_check_return(g_vsnprintf_entries);

    // full? grow it
    if (g_vsnprintf_count >= g_vsnprintf_maxn) {
        g_vsnprintf_maxn = g_vsnprintf_count + 16;
        g_vsnprintf_entries = (lx_vsnprintf_object_entry_t*)lx_ralloc(g_vsnprintf_entries, g_vsnprintf_maxn * sizeof(lx_vsnprintf_object_entry_t));
        lx_assert_and_check_return(g_vsnprintf_entries);
    }

    // find it
    lx_size_t i = 0;
    lx_size_t n = g_vsnprintf_count;
    lx_long_t r = -1;
    for (i = 0; i < n; i++) {
        if ((r = lx_strcmp(name, g_vsnprintf_entries[i].name)) <= 0) {
            break;
        }
    }

    // register it
    if (!r) {
        g_vsnprintf_entries[i].callback = callback;
    } else {
        if (i < n) {
            lx_memmov(g_vsnprintf_entries + i + 1, g_vsnprintf_entries + i, (n - i) * sizeof(lx_vsnprintf_object_entry_t));
        }
        g_vsnprintf_entries[i].name = name;
        g_vsnprintf_entries[i].callback = callback;
        g_vsnprintf_count++;
    }
}

lx_void_t lx_vsnprintf_object_exit() {
    if (g_vsnprintf_entries) {
        lx_free(g_vsnprintf_entries);
        g_vsnprintf_entries = lx_null;
    }
    g_vsnprintf_count = 0;
    g_vsnprintf_maxn = 0;
}
#endif
