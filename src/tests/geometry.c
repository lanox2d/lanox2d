#include "lanox2d/lanox2d.h"

static lx_void_t lx_test_utils_geometry_is_ccw() {
    /* make points
     *
     * (-10, -50)       (500, -50)
     *    . <-------------- . p1
     *   p2                 |
     *                      |
     *                      |
     *                      . p0
     *                  (500, 100)
     */
    lx_point_t p0;
    lx_point_t p1;
    lx_point_t p2;
    lx_point_imake(&p0, 500, 100);
    lx_point_imake(&p1, 500, -50);
    lx_point_imake(&p2, -10, -50);

    // check
    if (!lx_points_is_ccw(&p0, &p1, &p2)) lx_abort();
    if (!lx_points_is_ccw(&p1, &p2, &p0)) lx_abort();
    if (!lx_points_is_ccw(&p2, &p0, &p1)) lx_abort();

    if (lx_points_is_ccw(&p0, &p2, &p1)) lx_abort();
    if (lx_points_is_ccw(&p1, &p0, &p2)) lx_abort();
    if (lx_points_is_ccw(&p2, &p1, &p0)) lx_abort();
}

static lx_void_t lx_test_utils_geometry_in_point() {
    /* make points
     *
     * (-10, -50)       (500, -50)
     *    . <-------------- . p1
     *   p2                 |
     *                      |
     *                      |
     *                      . p0
     *                  (500, 100)
     */
    lx_point_t p0;
    lx_point_t p1;
    lx_point_t p2;
    lx_point_imake(&p0, 500, 100);
    lx_point_imake(&p1, 500, -50);
    lx_point_imake(&p2, -10, -50);

    // check
    if (!lx_point_in_top_or_horizontal(&p1, &p0)) lx_abort();
    if (!lx_point_in_top_or_horizontal(&p2, &p0)) lx_abort();
    if (!lx_point_in_top_or_horizontal(&p2, &p1)) lx_abort();

    if (!lx_point_in_left_or_vertical(&p2, &p1)) lx_abort();
    if (!lx_point_in_left_or_vertical(&p2, &p0)) lx_abort();
    if (!lx_point_in_left_or_vertical(&p1, &p0)) lx_abort();
}

static lx_void_t lx_test_utils_geometry_in_segment() {
    /* make points
     *
     * (-10, -50)       (500, -50)
     *    . <-------------- . p1
     *   p2                 |
     *                      |
     *                      |
     *                      . p0
     *                  (500, 100)
     */
    lx_point_t p0;
    lx_point_t p1;
    lx_point_t p2;
    lx_point_imake(&p0, 500, 100);
    lx_point_imake(&p1, 500, -50);
    lx_point_imake(&p2, -10, -50);

    // check
    if (!lx_point_in_segment_left(&p2, &p1, &p0)) lx_abort();
    if (!lx_point_in_segment_right(&p1, &p2, &p0)) lx_abort();
    if (!lx_point_in_segment_top(&p1, &p2, &p0)) lx_abort();
    if (!lx_point_in_segment_bottom(&p0, &p2, &p1)) lx_abort();
}

static lx_void_t lx_test_utils_geometry_intersection() {
    // the points
    lx_point_t org1;
    lx_point_t dst1;
    lx_point_t org2;
    lx_point_t dst2;
    lx_point_t intersection;

    // test results
    lx_volatile lx_size_t count = 256;
    lx_volatile lx_size_t rand = 0xbeaf;
    while (count--) {
        rand = (rand * 10807 + 1) & 0xffffffff;
        lx_point_imake(&org1, rand % LX_WIDTH_MAX, rand % LX_HEIGHT_MAX);
        rand = (rand * 10807 + 1) & 0xffffffff;
        lx_point_imake(&dst1, rand % LX_WIDTH_MAX, rand % LX_HEIGHT_MAX);
        rand = (rand * 10807 + 1) & 0xffffffff;
        lx_point_imake(&org2, -rand % LX_WIDTH_MAX, rand % LX_HEIGHT_MAX);
        rand = (rand * 10807 + 1) & 0xffffffff;
        lx_point_imake(&dst2, rand % LX_WIDTH_MAX, -rand % LX_HEIGHT_MAX);

        // calculate intersection
        if (lx_segment_intersection(&org1, &dst1, &org2, &dst2, &intersection)) {
            lx_trace_i("(%{point} => %{point}) x (%{point} => %{point})): %{point}", &org1, &dst1, &org2, &dst2, &intersection);
        }
    }

    // test performance
    count = 1000000;
    lx_size_t in = 0;
    lx_hong_t dt = lx_mclock();
    while (count--) {
        rand = (rand * 10807 + 1) & 0xffffffff;
        lx_point_imake(&org1, rand % LX_WIDTH_MAX, rand % LX_HEIGHT_MAX);
        rand = (rand * 10807 + 1) & 0xffffffff;
        lx_point_imake(&dst1, rand % LX_WIDTH_MAX, rand % LX_HEIGHT_MAX);
        rand = (rand * 10807 + 1) & 0xffffffff;
        lx_point_imake(&org2, -rand % LX_WIDTH_MAX, rand % LX_HEIGHT_MAX);
        rand = (rand * 10807 + 1) & 0xffffffff;
        lx_point_imake(&dst2, rand % LX_WIDTH_MAX, -rand % LX_HEIGHT_MAX);

        // calculate intersection
        if (lx_segment_intersection(&org1, &dst1, &org2, &dst2, &intersection) > 0) {
            // update the intersection number
            in++;
        }
    }
    dt = lx_mclock() - dt;

    // trace
    lx_trace_i("intersection: count: %lu, time: %lld ms", in, dt);
}

int main(int argc, char** argv) {
    lx_test_utils_geometry_is_ccw();
    lx_test_utils_geometry_in_point();
    lx_test_utils_geometry_in_segment();
    lx_test_utils_geometry_intersection();
    return 0;
}
