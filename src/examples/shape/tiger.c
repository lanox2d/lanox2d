#include "lanox2d/lanox2d.h"
#include "tiger.g"

typedef struct lx_tiger_entry_t_ {
    lx_size_t      is_fill:    1;
    lx_size_t      is_stroke:  1;
    lx_color_t     fill_color;
    lx_color_t     stroke_color;
    lx_float_t     stroke_width;
    lx_path_ref_t  path;
}lx_tiger_entry_t;

static lx_tiger_entry_t* g_tiger_entries = lx_null;
static lx_size_t         g_tiger_entries_count = 0;

static lx_inline lx_char_t const* lx_tiger_entry_skip_separator(lx_char_t const* p) {
    while (*p && (lx_isspace(*p) || *p == ',')) p++;
    return p;
}

static lx_char_t const* lx_tiger_entry_init_float(lx_char_t const* p, lx_float_t* value) {
    while (*p && lx_isspace(*p)) p++;

    // has sign?
    lx_long_t sign = 0;
    if (*p == '-') {
        sign = 1;
        p++;
    }

    // skip '0'
    while (*p == '0') p++;

    // compute double: lhs.rhs
    lx_long_t   dec = 0;
    lx_uint32_t lhs = 0;
    lx_float_t  rhs = 0;
    lx_long_t   zeros = 0;
    lx_int8_t   decimals[256];
    lx_int8_t*  d = decimals;
    lx_int8_t*  e = decimals + 256;
    while (*p)
    {
        lx_char_t ch = *p;

        // is the part of decimal?
        if (ch == '.')
        {
            if (!dec)
            {
                dec = 1;
                p++;
                continue ;
            }
            else break;
        }

        // parse integer and decimal
        if (lx_isdigit10(ch))
        {
            // save decimals
            if (dec)
            {
                if (d < e)
                {
                    if (ch != '0')
                    {
                        // fill '0'
                        while (zeros--) *d++ = 0;
                        zeros = 0;

                        // save decimal
                        *d++ = ch - '0';
                    }
                    else zeros++;
                }
            }
            else lhs = lhs * 10 + (ch - '0');
        }
        else break;

        // next
        p++;
    }

    // check
    lx_assert(d <= decimals + 256);

    // compute decimal
    while (d-- > decimals) rhs = (rhs + *d) / 10;

    // done
    *value = (sign? -(lhs + rhs) : (lhs + rhs));

    // ok
    return p;
}
static lx_char_t const* lx_tiger_entry_init_style_fill(lx_tiger_entry_t* entry, lx_char_t const* p)
{
    // seek to the color
    while (*p && *p != '#') p++;
    p++;

    // get pixel: argb
    lx_pixel_t pixel = lx_strtol(p, lx_null, 16);

    // skip pixel
    lx_size_t n = 0;
    for (; lx_isdigit16(*p); p++, n++) ;

    // only three digits? expand it. e.g. #123 => #112233
    if (n == 3) pixel = (((pixel >> 8) & 0x0f) << 20) | (((pixel >> 8) & 0x0f) << 16) | (((pixel >> 4) & 0x0f) << 12) | (((pixel >> 4) & 0x0f) << 8) | ((pixel & 0x0f) << 4) | (pixel & 0x0f);

    // no alpha? opaque it
    if (!(pixel & 0xff000000)) pixel |= 0xff000000;

    // init fill color
    entry->fill_color   = lx_pixel_color(pixel);
    entry->is_fill      = 1;

    // trace
    lx_trace_d("fill: %{color}", &entry->fill_color);

    // ok
    return p;
}
static lx_char_t const* lx_tiger_entry_init_style_stroke(lx_tiger_entry_t* entry, lx_char_t const* p)
{
    // seek to the color
    while (*p && *p != '#') p++;
    p++;

    // get pixel: argb
    lx_pixel_t pixel = lx_strtol(p, lx_null, 16);

    // skip pixel
    lx_size_t n = 0;
    for (; lx_isdigit16(*p); p++, n++) ;

    // only three digits? expand it. e.g. #123 => #112233
    if (n == 3) pixel = (((pixel >> 8) & 0x0f) << 20) | (((pixel >> 8) & 0x0f) << 16) | (((pixel >> 4) & 0x0f) << 12) | (((pixel >> 4) & 0x0f) << 8) | ((pixel & 0x0f) << 4) | (pixel & 0x0f);

    // no alpha? opaque it
    if (!(pixel & 0xff000000)) pixel |= 0xff000000;

    // init stroke color
    entry->stroke_color     = lx_pixel_color(pixel);
    entry->is_stroke        = 1;

    // init stroke width
    entry->stroke_width     = 1.0f;

    // trace
    lx_trace_d("stroke: %{color}", &entry->stroke_color);

    // ok
    return p;
}
static lx_char_t const* lx_tiger_entry_init_style_stroke_width(lx_tiger_entry_t* entry, lx_char_t const* p)
{
    // seek to the digits
    while (*p && !lx_isdigit(*p)) p++;

    // init the stroke width
    p = lx_tiger_entry_init_float(p, &entry->stroke_width);

    // trace
    lx_trace_d("stroke_width: %{float}", &entry->stroke_width);

    // ok
    return p;
}
static lx_void_t lx_tiger_entry_init_style(lx_tiger_entry_t* entry, lx_char_t const* style)
{
    // check
    lx_assert(entry && style);

    // done
    lx_char_t const* p = style;
    while (*p)
    {
        // done
        if (!lx_strnicmp(p, "fill", 4))
            p = lx_tiger_entry_init_style_fill(entry, p + 4);
        else if (!lx_strnicmp(p, "stroke-width", 12))
            p = lx_tiger_entry_init_style_stroke_width(entry, p + 12);
        else if (!lx_strnicmp(p, "stroke", 6))
            p = lx_tiger_entry_init_style_stroke(entry, p + 6);
        else p++;
    }
}
static lx_char_t const* lx_tiger_entry_init_path_d_xoy(lx_tiger_entry_t* entry, lx_char_t const* p, lx_char_t mode)
{
    // xoy
    lx_float_t xoy = 0; p = lx_tiger_entry_init_float(p, &xoy); p = lx_tiger_entry_skip_separator(p);

    // trace
    lx_trace_d("path: d: %c: %{float}", mode, &(xoy));

    // done path
    if (entry->path)
    {
        // last point
        lx_point_t pt = {0};
        lx_path_last(entry->path, &pt);

        // done
        switch (mode)
        {
            case 'H':
                lx_path_line2_to(entry->path, xoy, pt.y);
                break;
            case 'h':
                lx_path_line2_to(entry->path, pt.x + xoy, pt.y);
                break;
            case 'V':
                lx_path_line2_to(entry->path, pt.x, xoy);
                break;
            case 'v':
                lx_path_line2_to(entry->path, pt.x, pt.y + xoy);
                break;
            default:
                lx_trace_noimpl();
                break;
        }
    }

    // ok
    return p;
}
static lx_char_t const* lx_tiger_entry_init_path_d_xy1(lx_tiger_entry_t* entry, lx_char_t const* p, lx_char_t mode)
{
    // x1
    lx_float_t x1 = 0; p = lx_tiger_entry_init_float(p, &x1); p = lx_tiger_entry_skip_separator(p);

    // y1
    lx_float_t y1 = 0; p = lx_tiger_entry_init_float(p, &y1); p = lx_tiger_entry_skip_separator(p);

    // trace
    lx_trace_d("path: d: %c: %{float}, %{float}", mode, &(x1), &(y1));

    // init path
    if (!entry->path) entry->path = lx_path_init();

    // done path
    if (entry->path)
    {
        // last point
        lx_point_t pt = {0};
        lx_path_last(entry->path, &pt);

        // done
        switch (mode)
        {
            case 'M':
                lx_path_move2_to(entry->path, x1, y1);
                break;
            case 'm':
                lx_path_move2_to(entry->path, pt.x + x1, pt.y + y1);
                break;
            case 'L':
                lx_path_line2_to(entry->path, x1, y1);
                break;
            case 'l':
                lx_path_line2_to(entry->path, pt.x + x1, pt.y + y1);
                break;
            default:
                lx_trace_noimpl();
                break;
        }
    }

    // ok
    return p;
}
static lx_char_t const* lx_tiger_entry_init_path_d_xy2(lx_tiger_entry_t* entry, lx_char_t const* p, lx_char_t mode)
{
    // x1
    lx_float_t x1 = 0; p = lx_tiger_entry_init_float(p, &x1); p = lx_tiger_entry_skip_separator(p);

    // y1
    lx_float_t y1 = 0; p = lx_tiger_entry_init_float(p, &y1); p = lx_tiger_entry_skip_separator(p);

    // x2
    lx_float_t x2 = 0; p = lx_tiger_entry_init_float(p, &x2); p = lx_tiger_entry_skip_separator(p);

    // y2
    lx_float_t y2 = 0; p = lx_tiger_entry_init_float(p, &y2); p = lx_tiger_entry_skip_separator(p);

    // trace
    lx_trace_d("path: d: %c: %{float}, %{float}, %{float}, %{float}", mode, &(x1), &(y1), &(x2), &(y2));

    // init path
    if (!entry->path) entry->path = lx_path_init();

    // done path
    if (entry->path)
    {
        // done
        switch (mode)
        {
            case 'Q':
                lx_path_quad2_to(entry->path, x1, y1, x2, y2);
                break;
            case 'q':
                {
                    lx_point_t pt = {0};
                    lx_path_last(entry->path, &pt);
                    lx_path_quad2_to(entry->path, pt.x + x1, pt.y + y1, pt.x + x2, pt.y + y2);
                }
                break;
            default:
                lx_trace_noimpl();
                break;
        }
    }

    // ok
    return p;
}
static lx_char_t const* lx_tiger_entry_init_path_d_xy3(lx_tiger_entry_t* entry, lx_char_t const* p, lx_char_t mode)
{
    // x1
    lx_float_t x1 = 0; p = lx_tiger_entry_init_float(p, &x1); p = lx_tiger_entry_skip_separator(p);

    // y1
    lx_float_t y1 = 0; p = lx_tiger_entry_init_float(p, &y1); p = lx_tiger_entry_skip_separator(p);

    // x2
    lx_float_t x2 = 0; p = lx_tiger_entry_init_float(p, &x2); p = lx_tiger_entry_skip_separator(p);

    // y2
    lx_float_t y2 = 0; p = lx_tiger_entry_init_float(p, &y2); p = lx_tiger_entry_skip_separator(p);

    // x3
    lx_float_t x3 = 0; p = lx_tiger_entry_init_float(p, &x3); p = lx_tiger_entry_skip_separator(p);

    // y3
    lx_float_t y3 = 0; p = lx_tiger_entry_init_float(p, &y3); p = lx_tiger_entry_skip_separator(p);

    // trace
    lx_trace_d("path: d: %c: %{float}, %{float}, %{float}, %{float}, %{float}, %{float}", mode, &(x1), &(y1), &(x2), &(y2), &(x3), &(y3));

    // init path
    if (!entry->path) entry->path = lx_path_init();

    // done path
    if (entry->path)
    {
        // done
        switch (mode)
        {
            case 'C':
                lx_path_cubic2_to(entry->path, x1, y1, x2, y2, x3, y3);
                break;
            case 'c':
                {
                    lx_point_t pt = {0};
                    lx_path_last(entry->path, &pt);
                    lx_path_cubic2_to(entry->path, pt.x + x1, pt.y + y1, pt.x + x2, pt.y + y2, pt.x + x3, pt.y + y3);
                }
                break;
            default:
                lx_trace_noimpl();
                break;
        }
    }

    // ok
    return p;
}
static lx_char_t const* lx_tiger_entry_init_path_d_a(lx_tiger_entry_t* entry, lx_char_t const* p, lx_char_t mode)
{
    // rx
    lx_float_t rx = 0; p = lx_tiger_entry_init_float(p, &rx); p = lx_tiger_entry_skip_separator(p);

    // ry
    lx_float_t ry = 0; p = lx_tiger_entry_init_float(p, &ry); p = lx_tiger_entry_skip_separator(p);

    // x-axis-rotation
    lx_float_t xr = 0; p = lx_tiger_entry_init_float(p, &xr); p = lx_tiger_entry_skip_separator(p);

    // large-arc-flag
    lx_float_t af = 0; p = lx_tiger_entry_init_float(p, &af); p = lx_tiger_entry_skip_separator(p);

    // sweep-flag
    lx_float_t sf = 0; p = lx_tiger_entry_init_float(p, &sf); p = lx_tiger_entry_skip_separator(p);

    // x
    lx_float_t x = 0; p = lx_tiger_entry_init_float(p, &x); p = lx_tiger_entry_skip_separator(p);

    // y
    lx_float_t y = 0; p = lx_tiger_entry_init_float(p, &y); p = lx_tiger_entry_skip_separator(p);

    // trace
    lx_trace_d("path: a: %c: %{float}, %{float}, %{float}, %{float}, %{float}, %{float}, %{float}", mode, &(rx), &(ry), &(xr), &(af), &(sf), &(x), &(y));

    // init path
    if (!entry->path) entry->path = lx_path_init();

    // done path
    if (entry->path)
    {
        // last point
        lx_point_t pt = {0};
        lx_path_last(entry->path, &pt);

        // absolute x & y
        if (mode == 'a')
        {
            x += pt.x;
            y += pt.y;
        }

        // arc-to
//        lx_path_arc2_to(entry->path, x0, y0, rx, ry, ab, an);

        // noimpl
        lx_trace_noimpl();
    }

    // ok
    return p;
}
static lx_char_t const* lx_tiger_entry_init_path_d_z(lx_tiger_entry_t* entry, lx_char_t const* data, lx_char_t mode)
{
    // trace
    lx_trace_d("path: d: z");

    // close path
    if (entry->path) lx_path_close(entry->path);

    // ok
    return data;
}
static lx_void_t lx_tiger_entry_init_path(lx_tiger_entry_t* entry, lx_char_t const* path)
{
    // check
    lx_assert(entry && path);

    // trace
    lx_trace_d("path: d");

    // done
    lx_char_t const*    p = path;
    lx_char_t           l = '\0';
    lx_char_t           m = *p++;
    while (m)
    {
        lx_size_t d = 0;
        switch (m)
        {
        case 'M':
        case 'm':
        case 'L':
        case 'l':
        case 'T':
        case 't':
            p = lx_tiger_entry_init_path_d_xy1(entry, p, m); l = m;
            break;
        case 'H':
        case 'h':
        case 'V':
        case 'v':
            p = lx_tiger_entry_init_path_d_xoy(entry, p, m); l = m;
            break;
        case 'S':
        case 's':
        case 'Q':
        case 'q':
            p = lx_tiger_entry_init_path_d_xy2(entry, p, m); l = m;
            break;
        case 'C':
        case 'c':
            p = lx_tiger_entry_init_path_d_xy3(entry, p, m); l = m;
            break;
        case 'A':
        case 'a':
            p = lx_tiger_entry_init_path_d_a(entry, p, m); l = m;
            break;
        case 'Z':
        case 'z':
            p = lx_tiger_entry_init_path_d_z(entry, p, m); l = m;
            break;
        default:
            d = 1;
            break;
        }

        // no mode? use the last mode
        if (d && (lx_isdigit(m) || m == '.' || m == '-'))
        {
            m = l;
            p--;
        }
        else m = *p++;
    }
}
static lx_void_t lx_tiger_entry_init(lx_tiger_entry_t* entry, lx_char_t const* style, lx_char_t const* path)
{
    // init style
    lx_tiger_entry_init_style(entry, style);

    // init path
    lx_tiger_entry_init_path(entry, path);
}

static lx_void_t on_init_tiger(lx_window_ref_t window) {
    lx_assert_static(!(lx_arrayn(g_tiger) & 0x1));

    // the width and height
    lx_float_t w = (lx_float_t)lx_window_width(window);
    lx_float_t h = (lx_float_t)lx_window_height(window);

    // init entries
    g_tiger_entries = lx_nalloc0_type(lx_arrayn(g_tiger) >> 1, lx_tiger_entry_t);
    lx_assert(g_tiger_entries);

    // done
    lx_size_t index = 0;
    lx_size_t count = lx_arrayn(g_tiger);
    for (index = 0; index < count; index += 2)
    {
        // the style and path
        lx_char_t const* style  = g_tiger[index];
        lx_char_t const* path   = g_tiger[index + 1];

        // make entry
        lx_tiger_entry_t* entry = &g_tiger_entries[g_tiger_entries_count++];

        // init entry
        lx_tiger_entry_init(entry, style, path);

        // apply matrix to the path
        if (entry->path)
        {
            lx_matrix_t matrix;
            lx_matrix_init_translate(&matrix, -320, -320);
            lx_matrix_scale_lhs(&matrix, w / 640, h / 640);
            lx_path_apply(entry->path, &matrix);
        }
    }
}

static lx_void_t on_exit_tiger(lx_window_ref_t window)
{
    // exit entries
    if (g_tiger_entries)
    {
        // done
        lx_size_t i = 0;
        for (i = 0; i < g_tiger_entries_count; i++)
        {
            // exit path
            if (g_tiger_entries[i].path) lx_path_exit(g_tiger_entries[i].path);
            g_tiger_entries[i].path = lx_null;
        }

        // exit it
        lx_free(g_tiger_entries);
    }
}
static lx_void_t on_draw_tiger(lx_window_ref_t window, lx_canvas_ref_t canvas)
{
    // done
    lx_size_t i = 0;
    for (i = 0; i < g_tiger_entries_count; i++)
    {
        // the entry
        lx_tiger_entry_t* entry = &g_tiger_entries[i];

        // draw path
        if (entry->path)
        {
            // fill it
            if (entry->is_fill)
            {
                lx_canvas_mode_set(canvas, LX_PAINT_MODE_FILL);
                lx_canvas_color_set(canvas, entry->fill_color);
                lx_canvas_draw_path(canvas, entry->path);
            }

            // stroke it
            if (entry->is_stroke)
            {
                lx_canvas_mode_set(canvas, LX_PAINT_MODE_STROKE);
                lx_canvas_color_set(canvas, entry->stroke_color);
                lx_canvas_stroke_width_set(canvas, entry->stroke_width);
                lx_canvas_draw_path(canvas, entry->path);
            }
        }
    }
}
