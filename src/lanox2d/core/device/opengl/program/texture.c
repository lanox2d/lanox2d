/*!The Graphic Box Library
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
 * Copyright (C) 2009 - 2017, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        texture.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_gl_program_ref_t lx_gl_program_init_texture_2x() {
    static lx_char_t const* vshader =
#if defined(LX_CONFIG_OS_IOS) || defined(LX_CONFIG_OS_ANDROID)
        "precision mediump float;\n"
#endif
        "\n"
        "attribute vec4 aColor;\n"
        "attribute vec4 aTexcoords;\n"
        "attribute vec4 aVertices;\n"
        "\n"
        "varying vec4 vColors;\n"
        "varying vec4 vTexcoords;\n"
        "uniform mat4 uMatrixModel;\n"
        "uniform mat4 uMatrixProject;\n"
        "uniform mat4 uMatrixTexcoord;\n"
        "\n"
        "void main() {\n"
        "   vColors = aColor;\n"
        "   vTexcoords = uMatrixTexcoord * aTexcoords;\n"
        "   gl_Position = uMatrixProject * uMatrixModel * aVertices;\n"
        "}\n";

    static lx_char_t const* fshader =
#if defined(LX_CONFIG_OS_IOS) || defined(LX_CONFIG_OS_ANDROID)
        "precision mediump float;\n"
#endif
        "\n"
        "varying vec4 vColors;\n"
        "varying vec4 vTexcoords;\n"
        "uniform sampler2D uSampler;\n"
        "\n"
        "void main() {\n"
        "   gl_FragColor = vColors * texture2D(uSampler, vec2(vTexcoords.x, vTexcoords.y));\n"
        "}\n";

    lx_gl_program_ref_t program = lx_gl_program_init(LX_GL_PROGRAM_TYPE_TEXTURE, vshader, fshader);
    if (program) {
        lx_gl_program_location_set(program, LX_GL_PROGRAM_LOCATION_COLORS,          lx_gl_program_attr(program, "aColor"));
        lx_gl_program_location_set(program, LX_GL_PROGRAM_LOCATION_VERTICES,        lx_gl_program_attr(program, "aVertices"));
        lx_gl_program_location_set(program, LX_GL_PROGRAM_LOCATION_TEXCOORDS,       lx_gl_program_attr(program, "aTexcoords"));
        lx_gl_program_location_set(program, LX_GL_PROGRAM_LOCATION_MATRIX_MODEL,    lx_gl_program_unif(program, "uMatrixModel"));
        lx_gl_program_location_set(program, LX_GL_PROGRAM_LOCATION_MATRIX_PROJECT,  lx_gl_program_unif(program, "uMatrixProject"));
        lx_gl_program_location_set(program, LX_GL_PROGRAM_LOCATION_MATRIX_TEXCOORD, lx_gl_program_unif(program, "uMatrixTexcoord"));
    }
    return program;
}

static lx_gl_program_ref_t lx_gl_program_init_texture_3x() {
    static lx_char_t const* vshader =
        "#version 330\n"
        "precision mediump float;\n"
        "\n"
        "in vec4 aColor;\n"
        "in vec4 aTexcoords;\n"
        "in vec4 aVertices;\n"
        "\n"
        "out vec4 vColors;\n"
        "out vec4 vTexcoords;\n"
        "uniform mat4 uMatrixModel;\n"
        "uniform mat4 uMatrixProject;\n"
        "uniform mat4 uMatrixTexcoord;\n"
        "\n"
        "void main() {\n"
        "   vColors = aColor;\n"
        "   vTexcoords = uMatrixTexcoord * aTexcoords;\n"
        "   gl_Position = uMatrixProject * uMatrixModel * aVertices;\n"
        "}\n";

    static lx_char_t const* fshader =
        "#version 330\n"
        "precision mediump float;\n"
        "\n"
        "in vec4 vColors;\n"
        "in vec4 vTexcoords;\n"
        "uniform sampler2D uSampler;\n"
        "out vec4 finalColor;\n"
        "\n"
        "void main() {\n"
        "   finalColor = vColors * texture(uSampler, vec2(vTexcoords.x, vTexcoords.y));\n"
        "}\n";

    lx_gl_program_ref_t program = lx_gl_program_init(LX_GL_PROGRAM_TYPE_TEXTURE, vshader, fshader);
    if (program) {
        lx_gl_program_location_set(program, LX_GL_PROGRAM_LOCATION_COLORS,          lx_gl_program_attr(program, "aColor"));
        lx_gl_program_location_set(program, LX_GL_PROGRAM_LOCATION_VERTICES,        lx_gl_program_attr(program, "aVertices"));
        lx_gl_program_location_set(program, LX_GL_PROGRAM_LOCATION_TEXCOORDS,       lx_gl_program_attr(program, "aTexcoords"));
        lx_gl_program_location_set(program, LX_GL_PROGRAM_LOCATION_MATRIX_MODEL,    lx_gl_program_unif(program, "uMatrixModel"));
        lx_gl_program_location_set(program, LX_GL_PROGRAM_LOCATION_MATRIX_PROJECT,  lx_gl_program_unif(program, "uMatrixProject"));
        lx_gl_program_location_set(program, LX_GL_PROGRAM_LOCATION_MATRIX_TEXCOORD, lx_gl_program_unif(program, "uMatrixTexcoord"));
    }
    return program;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_gl_program_ref_t lx_gl_program_init_texture(lx_size_t glversion) {
    if (glversion > 0x30) {
        return lx_gl_program_init_texture_3x();
    }
    return lx_gl_program_init_texture_2x();
}

