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
 * @file        profiler.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "profiler.h"

#if 0
/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the profiler
static lx_stream_ref_t  g_profiler = lx_null;

// the head
static lx_char_t const* g_head = "\
<!DOCTYPE html>\n\
<html>\n\
    <head>\n\
        <meta charset=\"utf-8\">\n\
        <title>PolygonTessellationProfiler</title>\n\
        <script src=\"raphael.js\"></script>\n\
        <script>\n\
        \n\
            // size\n\
            var width = %{float};\n\
            var height = %{float};\n\
\n\
            // translate\n\
            var tx = %{float};\n\
            var ty = %{float};\n\
\n\
            function add_edge(canvas, id, vb, ve, x1, y1, x2, y2)\n\
            {\n\
                var edge = canvas.path(\"M\" + x1 + \", \" + y1 + \"L\" + x2 + \", \" + y2);\n\
                edge.translate(tx, ty);\n\
                edge.attr(\"stroke\", \"blue\");\n\
\n\
                var text = canvas.text((x1 + x2) / 2, (y1 + y2) / 2, id);\n\
                text.translate(tx, ty).scale(0.7);\n\
                text.attr(\"fill\", \"red\");\n\
\n\
                canvas.text(x1, y1, vb).translate(tx, ty).scale(0.7);\n\
                canvas.text(x2, y2, ve).translate(tx, ty).scale(0.7);\n\
\n\
                return edge;\n\
            }\n\
            function add_split(canvas, id, vb, ve, x1, y1, x2, y2)\n\
            {\n\
                var edge = canvas.path(\"M\" + x1 + \", \" + y1 + \"L\" + x2 + \", \" + y2);\n\
                edge.translate(tx, ty);\n\
                edge.attr(\"stroke\", \"pink\");\n\
\n\
                var text = canvas.text((x1 + x2) / 2, (y1 + y2) / 2, id);\n\
                text.translate(tx, ty).scale(0.7);\n\
                text.attr(\"fill\", \"green\");\n\
\n\
                canvas.text(x1, y1, vb).translate(tx, ty).scale(0.7);\n\
                canvas.text(x2, y2, ve).translate(tx, ty).scale(0.7);\n\
\n\
                return edge;\n\
            }\n\
            function add_patch(canvas, id, vb, ve, x1, y1, x2, y2)\n\
            {\n\
                var edge = canvas.path(\"M\" + x1 + \", \" + y1 + \"L\" + x2 + \", \" + y2);\n\
                edge.translate(tx, ty);\n\
                edge.attr(\"stroke\", \"lightgrey\");\n\
\n\
                var text = canvas.text((x1 + x2) / 2, (y1 + y2) / 2, id);\n\
                text.translate(tx, ty).scale(0.7);\n\
                text.attr(\"fill\", \"lightgrey\");\n\
\n\
                canvas.text(x1, y1, vb).translate(tx, ty).scale(0.7);\n\
                canvas.text(x2, y2, ve).translate(tx, ty).scale(0.7);\n\
\n\
                return edge;\n\
            }\n\
            function add_inter(canvas, vi, x, y)\n\
            {\n\
                var text = canvas.text(x, y, vi);\n\
                text.translate(tx, ty).scale(0.7);\n\
            }\n\
            function finish_face(canvas, x, y)\n\
            {\n\
                var text = canvas.text(x, y, \"*\");\n\
                text.translate(tx, ty).scale(0.7);\n\
                text.attr(\"fill\", \"red\");\n\
            }\n\
            function finish_region(canvas, x1, y1, x2, y2)\n\
            {\n\
                var text = canvas.text((x1 + x2) / 2, (y1 + y2) / 2, \"__\");\n\
                text.translate(tx, ty).scale(0.7);\n\
                text.attr(\"fill\", \"red\");\n\
            }\n\
            window.onload = function ()\n\
            {\n\
                // canvas\n\
                var canvas = Raphael(0, 0, width, height); \n\
\n\
                add_edge(canvas, \"ebl\", \"\", \"\", %{float}, %{float}, %{float}, %{float});\n\
                add_edge(canvas, \"ebr\", \"\", \"\", %{float}, %{float}, %{float}, %{float});\n\
\n\
";

// the tail
static lx_char_t const* g_tail = "\
            };\n\
        </script>\n\
    </head>\n\
    <body>\n\
        <div id=\"holder\"></div>\n\
    </body>\n\
</html>\n\
";

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bool_t lx_tessellator_profiler_init(lx_rect_ref_t bounds) {
    lx_assert_and_check_return_val(bounds, lx_false);

    lx_bool_t   ok = lx_false;
    lx_char_t*  head = lx_null;
    lx_size_t   head_maxn = lx_strlen(g_head) + 256;
    do {
        lx_assert(!g_profiler);

        // init sequence
        static lx_size_t s_sequence = 0;

        // get the temporary directory
        lx_char_t temp[LX_PATH_MAXN];
        lx_size_t size = lx_directory_temporary(temp, sizeof(temp));
        lx_assert_and_check_break(size);

        // make the profiler file path
        lx_snprintf(temp + size, sizeof(temp) - size, "/profiler_%lu.html", s_sequence++);

        // make profiler
        g_profiler = lx_stream_init_from_file(temp, LX_FILE_MODE_RW | LX_FILE_MODE_CREAT | LX_FILE_MODE_TRUNC);
        lx_assert_and_check_break(g_profiler);

        // open profiler
        if (!lx_stream_open(g_profiler)) break;

        // make head
        head = (lx_char_t*)lx_malloc0(head_maxn);
        lx_assert_and_check_break(head);

        // the width and height
        lx_float_t width    = bounds->w + 64.0f;
        lx_float_t height   = bounds->h + 64.0f;

        // the tx and ty
        lx_float_t tx = -bounds->x + 32.0f;
        lx_float_t ty = -bounds->y + 32.0f;

        // the left vertical line
        lx_float_t lx1 = bounds->x;
        lx_float_t ly1 = bounds->y;
        lx_float_t lx2 = bounds->x;
        lx_float_t ly2 = bounds->y + bounds->h;

        // the right vertical line
        lx_float_t rx1 = bounds->x + bounds->w;
        lx_float_t ry1 = bounds->y;
        lx_float_t rx2 = bounds->x + bounds->w;
        lx_float_t ry2 = bounds->y + bounds->h;

        // format head
        lx_size_t head_size = lx_snprintf(  head, head_maxn, g_head
                                        ,   &width, &height, &tx, &ty
                                        ,   &lx1, &ly1, &lx2, &ly2
                                        ,   &rx1, &ry1, &rx2, &ry2);

        // write head
        if (!lx_stream_bwrit(g_profiler, (lx_byte_t const*)head, head_size)) {
            break;
        }

        // ok
        ok = lx_true;

    } while (0);

    if (!ok && g_profiler) {
        lx_stream_exit(g_profiler);
        g_profiler = lx_null;
    }
    if (head) {
        lx_free(head);
        head = lx_null;
    }
    return ok;
}

lx_void_t lx_tessellator_profiler_exit() {
    if (g_profiler) {
        lx_stream_bwrit(g_profiler, (lx_byte_t const*)g_tail, lx_strlen(g_tail));
        lx_stream_exit(g_profiler);
        g_profiler = lx_null;
    }
}

lx_void_t lx_tessellator_profiler_add_edge(lx_mesh_edge_ref_t edge) {
    lx_assert_and_check_return(g_profiler && edge);

    lx_mesh_vertex_ref_t org = lx_mesh_edge_org(edge);
    lx_mesh_vertex_ref_t dst = lx_mesh_edge_dst(edge);
    lx_point_ref_t point_org = lx_tessellator_vertex_point(org);
    lx_point_ref_t point_dst = lx_tessellator_vertex_point(dst);

    lx_char_t line[256] = {0};
    lx_long_t size = lx_snprintf(line, sizeof(line), "                add_edge(canvas, \"e%lu\", \"v%lu\", \"v%lu\", %{float}, %{float}, %{float}, %{float});\n", edge->id, org->id, dst->id, &point_org->x, &point_org->y, &point_dst->x, &point_dst->y);
    lx_assert_and_check_return(size > 0);
    lx_stream_bwrit(g_profiler, (lx_byte_t const*)line, size);
}

lx_void_t lx_tessellator_profiler_add_split(lx_mesh_edge_ref_t edge) {
    lx_assert_and_check_return(g_profiler && edge);

    lx_mesh_vertex_ref_t org = lx_mesh_edge_org(edge);
    lx_mesh_vertex_ref_t dst = lx_mesh_edge_dst(edge);
    lx_point_ref_t point_org = lx_tessellator_vertex_point(org);
    lx_point_ref_t point_dst = lx_tessellator_vertex_point(dst);

    lx_char_t line[256] = {0};
    lx_long_t size = lx_snprintf(line, sizeof(line), "                add_split(canvas, \"e%lu\", \"v%lu\", \"v%lu\", %{float}, %{float}, %{float}, %{float});\n", edge->id, org->id, dst->id, &point_org->x, &point_org->y, &point_dst->x, &point_dst->y);
    lx_assert_and_check_return(size > 0);
    lx_stream_bwrit(g_profiler, (lx_byte_t const*)line, size);
}

lx_void_t lx_tessellator_profiler_add_patch(lx_mesh_edge_ref_t edge) {
    lx_assert_and_check_return(g_profiler && edge);

    lx_mesh_vertex_ref_t org = lx_mesh_edge_org(edge);
    lx_mesh_vertex_ref_t dst = lx_mesh_edge_dst(edge);
    lx_point_ref_t point_org = lx_tessellator_vertex_point(org);
    lx_point_ref_t point_dst = lx_tessellator_vertex_point(dst);

    lx_char_t line[256] = {0};
    lx_long_t size = lx_snprintf(line, sizeof(line), "                add_patch(canvas, \"e%lu\", \"v%lu\", \"v%lu\", %{float}, %{float}, %{float}, %{float});\n", edge->id, org->id, dst->id, &point_org->x, &point_org->y, &point_dst->x, &point_dst->y);
    lx_assert_and_check_return(size > 0);
    lx_stream_bwrit(g_profiler, (lx_byte_t const*)line, size);
}

lx_void_t lx_tessellator_profiler_add_inter(lx_mesh_vertex_ref_t inter) {
    lx_assert_and_check_return(g_profiler && inter);

    lx_point_ref_t point = lx_tessellator_vertex_point(inter);

    lx_char_t line[256] = {0};
    lx_long_t size = lx_snprintf(line, sizeof(line), "                add_inter(canvas, \"v%lu\", %{float}, %{float});\n", inter->id, &point->x, &point->y);
    lx_assert_and_check_return(size > 0);
    lx_stream_bwrit(g_profiler, (lx_byte_t const*)line, size);
}

lx_void_t lx_tessellator_profiler_finish_region(lx_tessellator_active_region_ref_t region) {
    lx_assert_and_check_return(g_profiler && region);

    // the region is inside?
    lx_check_return(region->inside);

    // get the edge of this region
    lx_mesh_edge_ref_t edge = region->edge;
    lx_assert_and_check_return(edge);

    lx_mesh_vertex_ref_t org = lx_mesh_edge_org(edge);
    lx_mesh_vertex_ref_t dst = lx_mesh_edge_dst(edge);
    lx_point_ref_t point_org = lx_tessellator_vertex_point(org);
    lx_point_ref_t point_dst = lx_tessellator_vertex_point(dst);

    lx_char_t line[256] = {0};
    lx_long_t size = lx_snprintf(line, sizeof(line), "                finish_region(canvas, %{float}, %{float}, %{float}, %{float});\n", &point_org->x, &point_org->y, &point_dst->x, &point_dst->y);
    lx_assert_and_check_return(size > 0);
    lx_stream_bwrit(g_profiler, (lx_byte_t const*)line, size);
}
#endif
