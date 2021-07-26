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
 * @file        color.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_gl_program_ref_t lx_gl_program_init_solid_2x() {
    static lx_char_t const* vshader =
#if defined(LX_CONFIG_OS_IOS) || defined(LX_CONFIG_OS_ANDROID)
        "precision mediump float;\n"
#endif
        "\n"
        "attribute vec4 aColor;\n"
        "attribute vec4 aVertices;\n"
        "varying vec4 vColors;\n"
        "uniform mat4 uMatrixModel;\n"
        "uniform mat4 uMatrixProject;\n"
        "\n"
        "void main() {\n"
        "   vColors = aColor;\n"
        "   gl_Position = uMatrixProject * uMatrixModel * aVertices;\n"
        "}\n";

    static lx_char_t const* fshader =
#if defined(LX_CONFIG_OS_IOS) || defined(LX_CONFIG_OS_ANDROID)
        "precision mediump float;\n"
#endif
        "\n"
        "varying vec4 vColors;\n"
        "\n"
        "void main() {\n"
        "   gl_FragColor = vColors;\n"
        "}\n";

    lx_gl_program_ref_t program = lx_gl_program_init(LX_GL_PROGRAM_TYPE_SOLID, vshader, fshader);
    if (program) {
        lx_gl_program_location_set(program, LX_GL_PROGRAM_LOCATION_COLORS,          lx_gl_program_attr(program, "aColor"));  //!< FIXME some name can not work for mac, .e.g aColors aColorx ...
        lx_gl_program_location_set(program, LX_GL_PROGRAM_LOCATION_VERTICES,        lx_gl_program_attr(program, "aVertices"));
        lx_gl_program_location_set(program, LX_GL_PROGRAM_LOCATION_MATRIX_MODEL,    lx_gl_program_unif(program, "uMatrixModel"));
        lx_gl_program_location_set(program, LX_GL_PROGRAM_LOCATION_MATRIX_PROJECT,  lx_gl_program_unif(program, "uMatrixProject"));
    }
    return program;
}

static lx_gl_program_ref_t lx_gl_program_init_solid_3x() {
    static lx_char_t const* vshader =
        "#version 330\n"
        "precision mediump float;\n"
        "\n"
        "layout(location = 0) in vec4 aColor;\n"
        "layout(location = 1) in vec4 aVertices;\n"
        "uniform mat4 uMatrixModel;\n"
        "uniform mat4 uMatrixProject;\n"
        "out vec4 vColors;\n"
        "\n"
        "void main() {\n"
        "   vColors = aColor;\n"
        "   gl_Position = uMatrixProject * uMatrixModel * aVertices;\n"
        "}\n";

    static lx_char_t const* fshader =
        "#version 330\n"
        "precision mediump float;\n"
        "\n"
        "in vec4 vColors;\n"
        "out vec4 finalColor;\n"
        "\n"
        "void main() {\n"
        "   finalColor = vColors;\n"
        "}\n";

    lx_gl_program_ref_t program = lx_gl_program_init(LX_GL_PROGRAM_TYPE_SOLID, vshader, fshader);
    if (program) {
        lx_gl_program_location_set(program, LX_GL_PROGRAM_LOCATION_COLORS, 0);
        lx_gl_program_location_set(program, LX_GL_PROGRAM_LOCATION_VERTICES, 1);
        lx_gl_program_location_set(program, LX_GL_PROGRAM_LOCATION_MATRIX_MODEL,    lx_gl_program_unif(program, "uMatrixModel"));
        lx_gl_program_location_set(program, LX_GL_PROGRAM_LOCATION_MATRIX_PROJECT,  lx_gl_program_unif(program, "uMatrixProject"));
    }
    return program;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_gl_program_ref_t lx_gl_program_init_solid() {
    if (lx_gl_version() > 0x30) {
        return lx_gl_program_init_solid_3x();
    }
    return lx_gl_program_init_solid_2x();
}

