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
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "shader.h"
#include "bitmap.h"
#include "private/shader.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_shader_ref_t lx_shader_init_linear_gradient(lx_size_t tile_mode, lx_gradient_ref_t gradient, lx_line_ref_t line) {
    lx_assert(gradient && gradient->colors && gradient->count && line);
    lx_linear_gradient_shader_t* shader = lx_malloc0_type(lx_linear_gradient_shader_t);
    if (shader) {
        shader->base.type      = LX_SHADER_TYPE_LINEAR_GRADIENT;
        shader->base.tile_mode = tile_mode;
        shader->gradient       = *gradient;
        shader->line           = *line;
        lx_shader_matrix_set((lx_shader_ref_t)shader, lx_null);
    }
    return (lx_shader_ref_t)shader;
}

lx_shader_ref_t lx_shader_init2_linear_gradient(lx_size_t tile_mode, lx_gradient_ref_t gradient, lx_float_t xb, lx_float_t yb, lx_float_t xe, lx_float_t ye) {
    lx_line_t line;
    lx_line_make(&line, xb, yb, xe, ye);
    return lx_shader_init_linear_gradient(tile_mode, gradient, &line);
}

lx_shader_ref_t lx_shader_init2i_linear_gradient(lx_size_t tile_mode, lx_gradient_ref_t gradient, lx_long_t xb, lx_long_t yb, lx_long_t xe, lx_long_t ye) {
    lx_line_t line;
    lx_line_imake(&line, xb, yb, xe, ye);
    return lx_shader_init_linear_gradient(tile_mode, gradient, &line);
}

lx_shader_ref_t lx_shader_init_radial_gradient(lx_size_t tile_mode, lx_gradient_ref_t gradient, lx_circle_ref_t circle) {
    lx_assert(gradient && gradient->colors && gradient->count && circle);
    lx_radial_gradient_shader_t* shader = lx_malloc0_type(lx_radial_gradient_shader_t);
    if (shader) {
        shader->base.type      = LX_SHADER_TYPE_RADIAL_GRADIENT;
        shader->base.tile_mode = tile_mode;
        shader->gradient       = *gradient;
        shader->circle         = *circle;
        lx_shader_matrix_set((lx_shader_ref_t)shader, lx_null);
    }
    return (lx_shader_ref_t)shader;
}

lx_shader_ref_t lx_shader_init2_radial_gradient(lx_size_t tile_mode, lx_gradient_ref_t gradient, lx_float_t x0, lx_float_t y0, lx_float_t r) {
    lx_circle_t circle;
    lx_circle_make(&circle, x0, y0, r);
    return lx_shader_init_radial_gradient(tile_mode, gradient, &circle);
}

lx_shader_ref_t lx_shader_init2i_radial_gradient(lx_size_t tile_mode, lx_gradient_ref_t gradient, lx_long_t x0, lx_long_t y0, lx_size_t r) {
    lx_circle_t circle;
    lx_circle_imake(&circle, x0, y0, r);
    return lx_shader_init_radial_gradient(tile_mode, gradient, &circle);
}

lx_shader_ref_t lx_shader_init_bitmap(lx_size_t tile_mode, lx_bitmap_ref_t bitmap) {
    lx_assert_and_check_return_val(bitmap, lx_null);
    lx_bitmap_shader_t* shader = lx_malloc0_type(lx_bitmap_shader_t);
    if (shader) {
        shader->base.type      = LX_SHADER_TYPE_BITMAP;
        shader->base.tile_mode = tile_mode;
        shader->bitmap         = bitmap;
        lx_shader_matrix_set((lx_shader_ref_t)shader, lx_null);
    }
    return (lx_shader_ref_t)shader;
}

lx_void_t lx_shader_exit(lx_shader_ref_t self) {
    lx_shader_t* shader = (lx_shader_t*)self;
    if (shader) {
        if (shader->exit) {
            shader->exit(self);
        }
        lx_free(shader);
    }
}

lx_size_t lx_shader_type(lx_shader_ref_t self) {
    lx_shader_t* shader = (lx_shader_t*)self;
    return shader? shader->type : LX_SHADER_TYPE_NONE;
}

lx_size_t lx_shader_tile_mode(lx_shader_ref_t self) {
    lx_shader_t* shader = (lx_shader_t*)self;
    return shader? shader->tile_mode : LX_SHADER_TILE_MODE_NONE;
}

lx_matrix_ref_t lx_shader_matrix(lx_shader_ref_t self) {
    lx_shader_t* shader = (lx_shader_t*)self;
    return shader? &shader->matrix : lx_null;
}

lx_void_t lx_shader_matrix_set(lx_shader_ref_t self, lx_matrix_ref_t matrix) {
    lx_shader_t* shader = (lx_shader_t*)self;
    if (shader) {
        if (matrix) {
            shader->matrix = *matrix;
        } else {
            lx_matrix_clear(&shader->matrix);
        }
    }
}
