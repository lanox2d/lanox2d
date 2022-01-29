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
 * @file        bitmap_shader.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "bitmap_shader.h"
#include "../../shader.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

static lx_inline lx_void_t lx_shader_texture_apply_wrap(lx_size_t tile_mode_s, lx_size_t tile_mode_t) {
    static lx_GLuint_t wrap[] = {
        LX_GL_CLAMP_TO_BORDER,
        LX_GL_CLAMP_TO_BORDER,
        LX_GL_CLAMP_TO_EDGE,
        LX_GL_REPEAT,
        LX_GL_MIRRORED_REPEAT
    };
    lx_assert(tile_mode_s < lx_arrayn(wrap) && tile_mode_t < lx_arrayn(wrap));
    lx_glTexParameteri(LX_GL_TEXTURE_2D, LX_GL_TEXTURE_WRAP_S, wrap[tile_mode_s]);
    lx_glTexParameteri(LX_GL_TEXTURE_2D, LX_GL_TEXTURE_WRAP_T, wrap[tile_mode_t]);
}

static lx_void_t lx_bitmap_shader_devdata_free(lx_shader_ref_t self) {
    lx_shader_t* shader = (lx_shader_t*)self;
    lx_assert(shader);

    lx_bitmap_shader_devdata_t* devdata = (lx_bitmap_shader_devdata_t*)shader->devdata;
    if (devdata) {
        if (devdata->texture) {
            lx_glDeleteTextures(1, &devdata->texture);
            devdata->texture = 0;
        }
        shader->devdata = lx_null;
    }
}

static lx_bitmap_shader_devdata_t* lx_bitmap_shader_init_devdata(lx_bitmap_shader_t* shader) {
    lx_assert(shader);

    // get bitmap
    lx_bitmap_ref_t bitmap = shader->bitmap;
    lx_assert(bitmap);

    // generate texture
    lx_GLuint_t texture = 0;
    lx_glGenTextures(1, &texture);
    lx_assert_and_check_return_val(texture, lx_null);

    // init bitmap shader devdata
    lx_bitmap_shader_devdata_t* devdata = lx_malloc0_type(lx_bitmap_shader_devdata_t);
    lx_assert_and_check_return_val(devdata, lx_null);

    // init texture
    devdata->texture = texture;
    lx_glBindTexture(LX_GL_TEXTURE_2D, texture);
    lx_glPixelStorei(LX_GL_UNPACK_ALIGNMENT, 1);

    // apply wrap
    lx_size_t tile_mode = lx_shader_tile_mode((lx_shader_ref_t)shader);
    lx_shader_texture_apply_wrap(tile_mode, tile_mode);

    // apply texture data
    lx_pointer_t data   = lx_bitmap_data(bitmap);
    lx_size_t    pixfmt = lx_bitmap_pixfmt(bitmap);
    lx_size_t    width  = lx_bitmap_width(bitmap);
    lx_size_t    height = lx_bitmap_height(bitmap);
    switch (pixfmt) {
    case LX_PIXFMT_RGBA8888 | LX_PIXFMT_BENDIAN:
    case LX_PIXFMT_RGBX8888 | LX_PIXFMT_BENDIAN:
        lx_glTexImage2D(LX_GL_TEXTURE_2D, 0, LX_GL_RGBA, (lx_GLsizei_t)width, (lx_GLsizei_t)height, 0, LX_GL_RGBA, LX_GL_UNSIGNED_BYTE, data);
        break;
    case LX_PIXFMT_ARGB8888:
    case LX_PIXFMT_XRGB8888:
        lx_glTexImage2D(LX_GL_TEXTURE_2D, 0, LX_GL_RGBA, (lx_GLsizei_t)width, (lx_GLsizei_t)height, 0, LX_GL_BGRA, LX_GL_UNSIGNED_BYTE, data);
        break;
    case LX_PIXFMT_RGB565:
        lx_glTexImage2D(LX_GL_TEXTURE_2D, 0, LX_GL_RGB, (lx_GLsizei_t)width, (lx_GLsizei_t)height, 0, LX_GL_RGB, LX_GL_UNSIGNED_SHORT_5_6_5, data);
        break;
    case LX_PIXFMT_RGB888:
        lx_glTexImage2D(LX_GL_TEXTURE_2D, 0, LX_GL_RGB, (lx_GLsizei_t)width, (lx_GLsizei_t)height, 0, LX_GL_BGR, LX_GL_UNSIGNED_BYTE, data);
        break;
    case LX_PIXFMT_RGBA4444:
    case LX_PIXFMT_RGBX4444:
        lx_glTexImage2D(LX_GL_TEXTURE_2D, 0, LX_GL_RGBA, (lx_GLsizei_t)width, (lx_GLsizei_t)height, 0, LX_GL_RGBA, LX_GL_UNSIGNED_SHORT_4_4_4_4, data);
        break;
    case LX_PIXFMT_RGBA5551:
    case LX_PIXFMT_RGBX5551:
        lx_glTexImage2D(LX_GL_TEXTURE_2D, 0, LX_GL_RGBA, (lx_GLsizei_t)width, (lx_GLsizei_t)height, 0, LX_GL_RGBA, LX_GL_UNSIGNED_SHORT_5_5_5_1, data);
        break;
    default:
        lx_trace_e("unsupported pixfmt for texture!");
        break;
    }

    /* convert world coordinate to camera coordinate
     *
     * before:
     *
     *
     *       bx        bounds of vertices
     *      -------V7---------------------V6------
     *  by |     /                          \     |
     *     |   /              |               \   |
     *     | /    bitmap  sw  |                 \ |
     *    V8          -----------------           V5
     *     |      sh |        |        |          |
     *     |         |        |        |          | bh
     *     |---------|--------O--------|----------|------> (bitmap matrix in world coordinate)
     *     |         |        |        |          |
     *     |         |        |        |          |
     *    V1          -----------------           V4
     *     | \                |                 / |
     *     |   \             \|/              /   |
     *     |     \                          /     |
     *      -------V2--------------------V3-------
     *                       bw
     *
     * after:
     *
     *       bx        bounds of vertices
     *      -------V7---------------------V6------
     *  by |     /                          \     |
     *     |   /              |               \   |
     *     | /    camera  sw  |                 \ |
     *    V8         O--------------------------- V5-----> (matrix in camera coordinate)
     *     |      sh |||||||| | ||||||||          |
     *     |         |||||||| | ||||||||          | bh
     *     |    -----|--------.--------|------    |
     *     |         |||||||| | ||||||||          |
     *     |         |||||||| | ||||||||          |
     *    V1         |-----------------           V4
     *     | \      \|/       |                 / |
     *     |   \              |               /   |
     *     |     \                          /     |
     *      -------V2--------------------V3-------
     *                       bw
     */
    lx_float_t sw = (lx_float_t)width;
    lx_float_t sh = (lx_float_t)height;
    devdata->matrix = shader->base.matrix;
    if (lx_matrix_invert(&devdata->matrix)) {
        devdata->matrix.tx /= sw;
        devdata->matrix.ty /= sh;
    }
    return devdata;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bitmap_shader_devdata_t* lx_bitmap_shader_devdata(lx_bitmap_shader_t* shader) {
    lx_bitmap_shader_devdata_t* devdata = (lx_bitmap_shader_devdata_t*)shader->base.devdata;
    if (!devdata) {
        devdata = lx_bitmap_shader_init_devdata(shader);
        if (devdata) {
            shader->base.devdata_free = lx_bitmap_shader_devdata_free;
            shader->base.devdata = devdata;
        }
    }
    return devdata;
}
