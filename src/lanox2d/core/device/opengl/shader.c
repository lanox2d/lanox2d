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
 * @file        shader.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "shader.h"
#include "../../shader.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

static lx_inline lx_void_t lx_shader_texture_apply_wrap(lx_size_t tile_mode_s, lx_size_t tile_mode_t) {
    static lx_GLuint_t wrap[] =
    {
        LX_GL_CLAMP_TO_BORDER
    ,   LX_GL_CLAMP_TO_BORDER
    ,   LX_GL_CLAMP_TO_EDGE
    ,   LX_GL_REPEAT
    ,   LX_GL_MIRRORED_REPEAT
    };
    lx_assert(tile_mode_s < lx_arrayn(wrap) && tile_mode_t < lx_arrayn(wrap));
    lx_glTexParameteri(LX_GL_TEXTURE_2D, LX_GL_TEXTURE_WRAP_S, wrap[tile_mode_s]);
    lx_glTexParameteri(LX_GL_TEXTURE_2D, LX_GL_TEXTURE_WRAP_T, wrap[tile_mode_t]);
}

static lx_void_t lx_bitmap_shader_devdata_free(lx_shader_ref_t self) {
    lx_shader_t* shader = (lx_shader_t*)self;
    lx_assert(shader);

    lx_GLuint_t texture = (lx_GLuint_t)(lx_size_t)shader->devdata;
    if (texture) {
        lx_glDeleteTextures(1, &texture);
        shader->devdata = lx_null;
    }
}

static lx_GLuint_t lx_bitmap_shader_init_texture(lx_bitmap_shader_t* shader) {
    lx_assert(shader);

    // get bitmap
    lx_bitmap_ref_t bitmap = shader->bitmap;
    lx_assert(bitmap);

    // generate texture
    lx_GLuint_t texture = 0;
    lx_glGenTextures(1, &texture);
    lx_assert_and_check_return_val(texture, 0);

    // init texture
    lx_glBindTexture(LX_GL_TEXTURE_2D, texture);
    lx_glPixelStorei(LX_GL_UNPACK_ALIGNMENT, 1);

    // apply wrap
    lx_size_t tile_mode = lx_shader_tile_mode((lx_shader_ref_t)shader);
    lx_shader_texture_apply_wrap(tile_mode, tile_mode);

    // apply texture data
    lx_byte_t const* data   = lx_bitmap_data(bitmap);
    lx_size_t        pixfmt = lx_bitmap_pixfmt(bitmap);
    lx_size_t        width  = lx_bitmap_width(bitmap);
    lx_size_t        height = lx_bitmap_height(bitmap);
    switch (LX_PIXFMT(pixfmt)) {
    case LX_PIXFMT_ARGB8888:
    case LX_PIXFMT_XRGB8888:
        lx_glTexImage2D(LX_GL_TEXTURE_2D, 0, LX_GL_RGBA, width, height, 0, LX_GL_BGRA, LX_GL_UNSIGNED_BYTE, data);
        break;
    case LX_PIXFMT_RGB565:
        lx_glTexImage2D(LX_GL_TEXTURE_2D, 0, LX_GL_RGB, width, height, 0, LX_GL_RGB, LX_GL_UNSIGNED_SHORT_5_6_5, data);
        break;
    case LX_PIXFMT_RGB888:
        lx_glTexImage2D(LX_GL_TEXTURE_2D, 0, LX_GL_RGB, width, height, 0, LX_GL_BGR, LX_GL_UNSIGNED_BYTE, data);
        break;
    case LX_PIXFMT_RGBA4444:
    case LX_PIXFMT_RGBX4444:
        lx_glTexImage2D(LX_GL_TEXTURE_2D, 0, LX_GL_RGBA, width, height, 0, LX_GL_RGBA, LX_GL_UNSIGNED_SHORT_4_4_4_4, data);
        break;
    case LX_PIXFMT_RGBA5551:
    case LX_PIXFMT_RGBX5551:
        lx_glTexImage2D(LX_GL_TEXTURE_2D, 0, LX_GL_RGBA, width, height, 0, LX_GL_RGBA, LX_GL_UNSIGNED_SHORT_5_5_5_1, data);
        break;
    default:
        lx_trace_e("unsupported pixfmt for texture!");
        break;
    }
    return texture;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_GLuint_t lx_bitmap_shader_texture(lx_bitmap_shader_t* shader) {
    lx_GLuint_t texture = (lx_GLuint_t)(lx_size_t)shader->base.devdata;
    if (!texture) {
        texture = lx_bitmap_shader_init_texture(shader);
        if (texture) {
            shader->base.devdata_free = lx_bitmap_shader_devdata_free;
            shader->base.devdata = (lx_pointer_t)(lx_size_t)texture;
        }
    }
    return texture;
}
