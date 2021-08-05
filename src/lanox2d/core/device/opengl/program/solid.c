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
 * @file        solid.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_gl_program_ref_t lx_gl_program_init_solid() {
    static lx_char_t const vshader[] = {
#if LX_GL_API_VERSION > 30
#   include "solid_33.vs.h"
#elif defined(LX_GL_API_ES)
#   include "solid_es20.vs.h"
#else
#   include "solid_21.vs.h"
#endif
    };

    static lx_char_t const fshader[] = {
#if LX_GL_API_VERSION > 30
#   include "solid_33.fs.h"
#elif defined(LX_GL_API_ES)
#   include "solid_es20.fs.h"
#else
#   include "solid_21.fs.h"
#endif
    };

    lx_gl_program_ref_t program = lx_gl_program_init(LX_GL_PROGRAM_TYPE_SOLID, vshader, fshader);
    if (program) {
        lx_gl_program_location_set(program, LX_GL_PROGRAM_LOCATION_COLORS,          lx_gl_program_attr(program, "aColor"));
        lx_gl_program_location_set(program, LX_GL_PROGRAM_LOCATION_VERTICES,        lx_gl_program_attr(program, "aVertices"));
        lx_gl_program_location_set(program, LX_GL_PROGRAM_LOCATION_MATRIX_MODEL,    lx_gl_program_unif(program, "uMatrixModel"));
        lx_gl_program_location_set(program, LX_GL_PROGRAM_LOCATION_MATRIX_PROJECT,  lx_gl_program_unif(program, "uMatrixProject"));
    }
    return program;
}
