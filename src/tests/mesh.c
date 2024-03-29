#include "lanox2d/lanox2d.h"

#define lx_test_mesh_edge_cstr(edge)            *((lx_char_t const**)lx_mesh_edge_data(edge))
#define lx_test_mesh_edge_cstr_set(edge, s)     (*((lx_char_t const**)lx_mesh_edge_data(edge)) = (s), \
                                                *((lx_char_t const**)lx_mesh_edge_data(lx_mesh_edge_sym(edge))) = (s "_sym"))

#define lx_test_mesh_face_cstr(face)            *((lx_char_t const**)lx_mesh_face_data(face))
#define lx_test_mesh_face_cstr_set(face, s)     *((lx_char_t const**)lx_mesh_face_data(face)) = (s)

#define lx_test_mesh_vertex_cstr(vertex)        *((lx_char_t const**)lx_mesh_vertex_data(vertex))
#define lx_test_mesh_vertex_cstr_set(vertex, s) *((lx_char_t const**)lx_mesh_vertex_data(vertex)) = (s)

#ifdef LX_DEBUG
static lx_void_t lx_test_mesh_dump(lx_mesh_ref_t self) {
    lx_trace_i("");
    lx_trace_i("edges:");
    lx_for_all (lx_mesh_edge_ref_t, edge, lx_mesh_edge_list(self)) {
        lx_trace_i("    %s, %s", lx_test_mesh_edge_cstr(edge), lx_test_mesh_edge_cstr(lx_mesh_edge_sym(edge)));
    }

    lx_trace_i("faces:");
    lx_for_all (lx_mesh_face_ref_t, face, lx_mesh_face_list(self)) {
        lx_trace_i("    %s:", lx_test_mesh_face_cstr(face));
        lx_mesh_edge_ref_t head = lx_mesh_face_edge(face);
        lx_mesh_edge_ref_t edge = head;
        do {
            lx_trace_i("        %s, %s", lx_test_mesh_edge_cstr(edge), lx_test_mesh_edge_cstr(lx_mesh_edge_sym(edge)));
            edge = lx_mesh_edge_lnext(edge);
        } while (edge != head);
    }

    lx_trace_i("vertices:");
    lx_for_all (lx_mesh_vertex_ref_t, vertex, lx_mesh_vertex_list(self)) {
        lx_trace_i("    %s:", lx_test_mesh_vertex_cstr(vertex));
        lx_mesh_edge_ref_t head = lx_mesh_vertex_edge(vertex);
        lx_mesh_edge_ref_t edge = head;
        do {
            lx_trace_i("        %s, %s", lx_test_mesh_edge_cstr(edge), lx_test_mesh_edge_cstr(lx_mesh_edge_sym(edge)));
            edge = lx_mesh_edge_onext(edge);
        } while (edge != head);
    }
}
#endif

static lx_void_t lx_test_mesh_listener(lx_mesh_event_ref_t event) {
    lx_assert(event);
    switch (event->type) {
    case LX_MESH_EVENT_FACE_MERGE:
        lx_trace_d("face.merge(%s, %s) => %s",
            lx_test_mesh_face_cstr(event->org), lx_test_mesh_face_cstr(event->dst), lx_test_mesh_face_cstr(event->dst));
        break;
    case LX_MESH_EVENT_FACE_SPLIT:
        lx_trace_d("face.split(%s) => (%s, %s)",
            lx_test_mesh_face_cstr(event->org), lx_test_mesh_face_cstr(event->org), lx_test_mesh_face_cstr(event->dst));
        break;
    case LX_MESH_EVENT_EDGE_MERGE:
        lx_trace_d("edge.merge(%s, %s) => %s",
            lx_test_mesh_edge_cstr(event->org), lx_test_mesh_edge_cstr(event->dst), lx_test_mesh_edge_cstr(event->dst));
        break;
    case LX_MESH_EVENT_EDGE_SPLIT:
        lx_trace_d("edge.split(%s) => (%s, %s)",
            lx_test_mesh_edge_cstr(event->org), lx_test_mesh_edge_cstr(event->org), lx_test_mesh_edge_cstr(event->dst));
        break;
    default:
        lx_assert(0);
        break;
    }
}

static lx_void_t lx_test_mesh_split() {
    lx_trace_i("==========================================================================");

    lx_element_t  element = lx_element_mem(sizeof(lx_char_t const*), lx_null);
    lx_mesh_ref_t mesh = lx_mesh_init(element, element, element);
    if (mesh) {
        lx_mesh_listener_set(mesh, lx_test_mesh_listener, mesh);
        lx_mesh_listener_event_add(mesh, LX_MESH_EVENT_FACE_MERGE | LX_MESH_EVENT_FACE_SPLIT | LX_MESH_EVENT_EDGE_MERGE | LX_MESH_EVENT_EDGE_SPLIT);

        // make a clockwise self-loop edge
        lx_mesh_edge_ref_t edge0 = lx_mesh_edge_make_loop(mesh, lx_false);
        if (edge0) {
            /* make a quadrangle
             *
             *                     e1
             *           v0 --------------> v1
             *            |                 |
             *         e0 |      rface      | e2     lface
             *            |                 |
             *           v3 <-------------- v2
             *                     e3
             *
             */
            lx_mesh_edge_ref_t edge1 = lx_mesh_edge_insert(mesh, edge0, edge0);
            lx_mesh_edge_ref_t edge2 = lx_mesh_edge_insert(mesh, edge1, edge0);
            lx_mesh_edge_ref_t edge3 = lx_mesh_edge_insert(mesh, edge2, edge0);

            // save face name
            lx_test_mesh_face_cstr_set(lx_mesh_edge_lface(edge0), "lface");
            lx_test_mesh_face_cstr_set(lx_mesh_edge_rface(edge0), "rface");

            // save edge name
            lx_test_mesh_edge_cstr_set(edge0, "e0");
            lx_test_mesh_edge_cstr_set(edge1, "e1");
            lx_test_mesh_edge_cstr_set(edge2, "e2");
            lx_test_mesh_edge_cstr_set(edge3, "e3");

            // save vertex name
            lx_test_mesh_vertex_cstr_set(lx_mesh_edge_dst(edge0), "v0");
            lx_test_mesh_vertex_cstr_set(lx_mesh_edge_dst(edge1), "v1");
            lx_test_mesh_vertex_cstr_set(lx_mesh_edge_dst(edge2), "v2");
            lx_test_mesh_vertex_cstr_set(lx_mesh_edge_dst(edge3), "v3");

#ifdef LX_DEBUG
            // trace
            lx_trace_i("");
            lx_trace_i("split: make");

            // check mesh
            lx_mesh_check(mesh);

            // dump mesh
            lx_test_mesh_dump(mesh);
#endif
            /* split a quadrangle
             *
             *                e1       e4
             *           v0 ------ v4 -----> v1
             *            |                 |
             *         e0 |      rface      | e2     lface
             *            |                 |
             *           v3 <----- v5 <---- v2
             *                 e5       e3
             *
             */
            lx_mesh_edge_ref_t edge4 = lx_mesh_edge_split(mesh, edge1);
            lx_mesh_edge_ref_t edge5 = lx_mesh_edge_split(mesh, edge3);

            // save edge name
            lx_test_mesh_edge_cstr_set(edge4, "e4");
            lx_test_mesh_edge_cstr_set(edge5, "e5");

            // save vertex name
            lx_test_mesh_vertex_cstr_set(lx_mesh_edge_org(edge4), "v4");
            lx_test_mesh_vertex_cstr_set(lx_mesh_edge_org(edge5), "v5");

#ifdef LX_DEBUG
            lx_trace_i("");
            lx_trace_i("split: done");
            lx_mesh_check(mesh);
            lx_test_mesh_dump(mesh);
#endif
        }

        lx_mesh_exit(mesh);
    }
}

static lx_void_t lx_test_mesh_splice() {
    lx_trace_i("==========================================================================");

    lx_element_t  element = lx_element_mem(sizeof(lx_char_t const*), lx_null);
    lx_mesh_ref_t mesh = lx_mesh_init(element, element, element);
    if (mesh) {
        lx_mesh_listener_set(mesh, lx_test_mesh_listener, mesh);
        lx_mesh_listener_event_add(mesh, LX_MESH_EVENT_FACE_MERGE | LX_MESH_EVENT_FACE_SPLIT | LX_MESH_EVENT_EDGE_MERGE | LX_MESH_EVENT_EDGE_SPLIT);

        /* make a edge
         *
         *           lface
         *
         *         O -----> D
         *
         *           lface
         */
        lx_mesh_edge_ref_t edge = lx_mesh_edge_make(mesh);
        if (edge) {

            // save face name
            lx_test_mesh_face_cstr_set(lx_mesh_edge_lface(edge), "lface");

            // save vertex name
            lx_test_mesh_vertex_cstr_set(lx_mesh_edge_org(edge), "org");
            lx_test_mesh_vertex_cstr_set(lx_mesh_edge_dst(edge), "dst");

#ifdef LX_DEBUG
            // trace
            lx_trace_i("");
            lx_trace_i("splice: make");

            lx_mesh_check(mesh);
            lx_test_mesh_dump(mesh);
#endif

            /* splice
             *
             *          -------
             *         |       |
             *         | rface |
             *         |       |
             *         O/D <---
             *
             *           lface
             *
             */
            lx_mesh_edge_splice(mesh, edge, lx_mesh_edge_sym(edge));

#ifdef LX_DEBUG
            // trace
            lx_trace_i("");
            lx_trace_i("splice: done");

            lx_mesh_check(mesh);
            lx_test_mesh_dump(mesh);
#endif

            /* splice
             *
             *           lface
             *
             *         O -----> D
             *
             *           lface
             *
             */
            lx_mesh_edge_splice(mesh, edge, lx_mesh_edge_sym(edge));

#ifdef LX_DEBUG
            // trace
            lx_trace_i("");
            lx_trace_i("splice: done");

            lx_mesh_check(mesh);
            lx_test_mesh_dump(mesh);
#endif
        }
        lx_mesh_exit(mesh);
    }
}

static lx_void_t lx_test_mesh_radiation() {
    lx_trace_i("==========================================================================");

    lx_element_t  element = lx_element_mem(sizeof(lx_char_t const*), lx_null);
    lx_mesh_ref_t mesh = lx_mesh_init(element, element, element);
    if (mesh) {
        lx_mesh_listener_set(mesh, lx_test_mesh_listener, mesh);
        lx_mesh_listener_event_add(mesh, LX_MESH_EVENT_FACE_MERGE | LX_MESH_EVENT_FACE_SPLIT | LX_MESH_EVENT_EDGE_MERGE | LX_MESH_EVENT_EDGE_SPLIT);

        // make a edge
        lx_mesh_edge_ref_t edge1 = lx_mesh_edge_make(mesh);
        if (edge1) {

            // the face
            lx_mesh_face_ref_t face = lx_mesh_edge_lface(edge1);
            lx_assert(face == lx_mesh_edge_rface(edge1));

            // save face name
            lx_test_mesh_face_cstr_set(face, "face");

            /* make a radiation
             *
             *                    v4
             *                   / \
             *                    |
             *                    |
             *                    | e4
             *                    |
             *                    |
             *                    |
             * <---------------- v0 ---------------->
             * v1       e1        |        e3       v3
             *                    |
             *                    |
             *                    | e2
             *                    |
             *                    |
             *                   \ /
             *                    v2
             */
            lx_mesh_edge_ref_t edge2 = lx_mesh_edge_append(mesh, lx_mesh_edge_sym(edge1));
            lx_mesh_edge_ref_t edge3 = lx_mesh_edge_append(mesh, lx_mesh_edge_sym(edge1));
            lx_mesh_edge_ref_t edge4 = lx_mesh_edge_append(mesh, lx_mesh_edge_sym(edge1));

            // save edge name
            lx_test_mesh_edge_cstr_set(edge1, "e1");
            lx_test_mesh_edge_cstr_set(edge2, "e2");
            lx_test_mesh_edge_cstr_set(edge3, "e3");
            lx_test_mesh_edge_cstr_set(edge4, "e4");

            // save vertex name
            lx_test_mesh_vertex_cstr_set(lx_mesh_edge_org(edge1), "v0");
            lx_test_mesh_vertex_cstr_set(lx_mesh_edge_dst(edge1), "v1");
            lx_test_mesh_vertex_cstr_set(lx_mesh_edge_dst(edge2), "v2");
            lx_test_mesh_vertex_cstr_set(lx_mesh_edge_dst(edge3), "v3");
            lx_test_mesh_vertex_cstr_set(lx_mesh_edge_dst(edge4), "v4");

#ifdef LX_DEBUG
            // trace
            lx_trace_i("");
            lx_trace_i("radiation: make");

            lx_mesh_check(mesh);
            lx_test_mesh_dump(mesh);
#endif

            /* remove one
             *
             *                    v4
             *                   / \
             *                    |
             *                    |
             *                    | e4
             *                    |
             *                    |
             *                    |
             *                   v0 ---------------->
             *                    |        e3       v3
             *                    |
             *                    |
             *                    | e2
             *                    |
             *                    |
             *                   \ /
             *                    v2
             */
            lx_mesh_edge_remove(mesh, edge1);

#ifdef LX_DEBUG
            lx_trace_i("");
            lx_trace_i("radiation: kill");
            lx_mesh_check(mesh);
            lx_test_mesh_dump(mesh);
#endif

            // remove all
            lx_mesh_edge_remove(mesh, edge2);
            lx_mesh_edge_remove(mesh, edge3);
            lx_mesh_edge_remove(mesh, edge4);

            // check
            lx_assert(lx_mesh_is_empty(mesh));
        }
        lx_mesh_exit(mesh);
    }
}

static lx_void_t lx_test_mesh_quadrangle() {
    lx_trace_i("==========================================================================");

    lx_element_t  element = lx_element_mem(sizeof(lx_char_t const*), lx_null);
    lx_mesh_ref_t mesh = lx_mesh_init(element, element, element);
    if (mesh) {
        lx_mesh_listener_set(mesh, lx_test_mesh_listener, mesh);
        lx_mesh_listener_event_add(mesh, LX_MESH_EVENT_FACE_MERGE | LX_MESH_EVENT_FACE_SPLIT | LX_MESH_EVENT_EDGE_MERGE | LX_MESH_EVENT_EDGE_SPLIT);

        // make a counter-clockwise self-loop edge
        lx_mesh_edge_ref_t edge0 = lx_mesh_edge_make_loop(mesh, lx_true);
        if (edge0) {
            /* make a quadrangle
             *
             *                     e0
             *           v0 <-------------- v3
             *            |                 |
             *         e1 |      lface      | e3     rface
             *            |                 |
             *           v1 --------------> v2
             *                     e2
             *
             */
            lx_mesh_edge_ref_t edge1 = lx_mesh_edge_insert(mesh, edge0, edge0);
            lx_mesh_edge_ref_t edge2 = lx_mesh_edge_insert(mesh, edge1, edge0);
            lx_mesh_edge_ref_t edge3 = lx_mesh_edge_insert(mesh, edge2, edge0);

            // save face name
            lx_test_mesh_face_cstr_set(lx_mesh_edge_lface(edge0), "lface");
            lx_test_mesh_face_cstr_set(lx_mesh_edge_rface(edge0), "rface");

            // save edge name
            lx_test_mesh_edge_cstr_set(edge0, "e0");
            lx_test_mesh_edge_cstr_set(edge1, "e1");
            lx_test_mesh_edge_cstr_set(edge2, "e2");
            lx_test_mesh_edge_cstr_set(edge3, "e3");

            // save vertex name
            lx_test_mesh_vertex_cstr_set(lx_mesh_edge_dst(edge0), "v0");
            lx_test_mesh_vertex_cstr_set(lx_mesh_edge_dst(edge1), "v1");
            lx_test_mesh_vertex_cstr_set(lx_mesh_edge_dst(edge2), "v2");
            lx_test_mesh_vertex_cstr_set(lx_mesh_edge_dst(edge3), "v3");

#ifdef LX_DEBUG
            // trace
            lx_trace_i("");
            lx_trace_i("quadrangle: make");

            // check mesh
            lx_mesh_check(mesh);

            // dump mesh
            lx_test_mesh_dump(mesh);
#endif

            /* remove one
             *
             *                              v3
             *                   e1   .     |
             *                   .          | e3     rface
             *              .      lface    |
             *           v1 --------------> v2
             *                     e2

             */
            lx_mesh_edge_remove(mesh, edge0);

#ifdef LX_DEBUG
            lx_trace_i("");
            lx_trace_i("quadrangle: kill");
            lx_mesh_check(mesh);
            lx_test_mesh_dump(mesh);
#endif

            // remove all
            lx_mesh_edge_remove(mesh, edge2);
            lx_mesh_edge_remove(mesh, edge3);
            lx_mesh_edge_remove(mesh, edge1);

            // check
            lx_assert(lx_mesh_is_empty(mesh));
        }
        lx_mesh_exit(mesh);
    }
}

static lx_void_t lx_test_mesh_tetrahedron() {
    lx_trace_i("==========================================================================");

    lx_element_t  element = lx_element_mem(sizeof(lx_char_t const*), lx_null);
    lx_mesh_ref_t mesh = lx_mesh_init(element, element, element);
    if (mesh) {
        lx_mesh_listener_set(mesh, lx_test_mesh_listener, mesh);
        lx_mesh_listener_event_add(mesh, LX_MESH_EVENT_FACE_MERGE | LX_MESH_EVENT_FACE_SPLIT | LX_MESH_EVENT_EDGE_MERGE | LX_MESH_EVENT_EDGE_SPLIT);

        // make a clockwise self-loop edge
        lx_mesh_edge_ref_t edge0 = lx_mesh_edge_make_loop(mesh, lx_false);
        if (edge0) {
            /* make a tetrahedron
             *
             *                     e1
             *           v0 --------------> v1-----------------
             *            | .         rface |                  |
             *         e0 |        .        | e2     lface     |
             *            | face1     e5 .  |                  | e4
             *           v3 <-------------- v2                 |
             *            |        e3                          |
             *            |                                    |   face0
             *             <-----------------------------------
             *
             */
            lx_mesh_edge_ref_t edge1 = lx_mesh_edge_insert(mesh, edge0, edge0);
            lx_mesh_edge_ref_t edge2 = lx_mesh_edge_insert(mesh, edge1, edge0);
            lx_mesh_edge_ref_t edge3 = lx_mesh_edge_insert(mesh, edge2, edge0);

            // save face name
            lx_test_mesh_face_cstr_set(lx_mesh_edge_lface(edge0), "lface");
            lx_test_mesh_face_cstr_set(lx_mesh_edge_rface(edge0), "rface");

            lx_mesh_edge_ref_t edge4 = lx_mesh_edge_connect(mesh, edge1, edge0);
            lx_mesh_edge_ref_t edge5 = lx_mesh_edge_connect(mesh, lx_mesh_edge_sym(edge3), lx_mesh_edge_sym(edge0));

            // save face name
            lx_test_mesh_face_cstr_set(lx_mesh_edge_lface(edge4), "face0");
            lx_test_mesh_face_cstr_set(lx_mesh_edge_lface(edge5), "face1");

            // save edge name
            lx_test_mesh_edge_cstr_set(edge0, "e0");
            lx_test_mesh_edge_cstr_set(edge1, "e1");
            lx_test_mesh_edge_cstr_set(edge2, "e2");
            lx_test_mesh_edge_cstr_set(edge3, "e3");
            lx_test_mesh_edge_cstr_set(edge4, "e4");
            lx_test_mesh_edge_cstr_set(edge5, "e5");

            // save vertex name
            lx_test_mesh_vertex_cstr_set(lx_mesh_edge_dst(edge0), "v0");
            lx_test_mesh_vertex_cstr_set(lx_mesh_edge_dst(edge1), "v1");
            lx_test_mesh_vertex_cstr_set(lx_mesh_edge_dst(edge2), "v2");
            lx_test_mesh_vertex_cstr_set(lx_mesh_edge_dst(edge3), "v3");

#ifdef LX_DEBUG
            lx_trace_i("");
            lx_trace_i("tetrahedron: make");
            lx_mesh_check(mesh);
            lx_test_mesh_dump(mesh);
#endif

            // disconnect two
            lx_mesh_edge_disconnect(mesh, edge4);
            lx_mesh_edge_disconnect(mesh, edge5);

#ifdef LX_DEBUG
            // trace
            lx_trace_i("");
            lx_trace_i("tetrahedron: kill");

            // check mesh
            lx_mesh_check(mesh);

            // dump mesh
            lx_test_mesh_dump(mesh);
#endif

            // remove all
            lx_mesh_edge_remove(mesh, edge2);
            lx_mesh_edge_remove(mesh, edge3);
            lx_mesh_edge_remove(mesh, edge0);
            lx_mesh_edge_remove(mesh, edge1);

            // check
            lx_assert(lx_mesh_is_empty(mesh));
        }
        lx_mesh_exit(mesh);
    }
}

int main(int argc, char** argv) {
    lx_test_mesh_split();
    lx_test_mesh_splice();
    lx_test_mesh_radiation();
    lx_test_mesh_quadrangle();
    lx_test_mesh_tetrahedron();
    return 0;
}
