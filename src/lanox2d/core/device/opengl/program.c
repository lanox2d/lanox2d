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
 * @file        program.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "program.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the gl program type
typedef struct lx_gl_program_t {
    lx_size_t           type;
    lx_GLuint_t         program;
    lx_GLuint_t         vshader;
    lx_GLuint_t         fshader;
    lx_GLint_t          location[LX_GL_PROGRAM_LOCATION_MAXN];
}lx_gl_program_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_GLuint_t lx_gl_program_shader(lx_char_t const* data, lx_size_t type) {

    // init shader
    lx_GLuint_t shader = lx_glCreateShader((lx_GLenum_t)type);
    lx_assert_and_check_return_val(shader, 0);

    // init source
    lx_glShaderSource(shader, 1, (lx_GLchar_t const**)&data, lx_null);

    // compile shader
    lx_glCompileShader(shader);
    lx_GLint_t ok = LX_GL_FALSE;
    lx_glGetShaderiv(shader, LX_GL_COMPILE_STATUS, &ok);
    if (ok == LX_GL_FALSE) {
        lx_GLchar_t info[1024] = {0};
        lx_glGetProgramInfoLog(shader, sizeof(info), 0, info);
        lx_trace_e("shader: compile failed: %s", info);
        lx_glDeleteShader(shader);
        return 0;
    }
    return shader;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_gl_program_ref_t lx_gl_program_init(lx_size_t type, lx_char_t const* vshader, lx_char_t const* fshader) {
    lx_assert_and_check_return_val(type && vshader && fshader, lx_null);

    lx_bool_t        ok = lx_false;
    lx_gl_program_t* program = lx_null;
    do {
        // init program
        program = lx_malloc0_type(lx_gl_program_t);
        lx_assert_and_check_break(program);

        program->type = type;

        // load vshader
        program->vshader = lx_gl_program_shader(vshader, LX_GL_VERTEX_SHADER);
        lx_assert_and_check_break(program->vshader);

        // load fshader
        program->fshader = lx_gl_program_shader(fshader, LX_GL_FRAGMENT_SHADER);
        lx_assert_and_check_break(program->fshader);

        // init program
        program->program = lx_glCreateProgram();
        lx_assert_and_check_break(program->program);

        // bind vshader
        lx_glAttachShader(program->program, program->vshader);

        // bind fshader
        lx_glAttachShader(program->program, program->fshader);

        // link program
        lx_glLinkProgram(program->program);

        // get status
        lx_GLint_t status = LX_GL_FALSE;
        lx_glGetProgramiv(program->program, LX_GL_LINK_STATUS, &status);
        if (status == LX_GL_FALSE) {
            lx_GLchar_t info[1024] = {0};
            lx_glGetProgramInfoLog(program->program, sizeof(info), 0, info);
            lx_trace_e("link failed: %s", info);
            break;
        }

        // ok
        ok = lx_true;

    } while (0);

    if (!ok && program) {
        lx_gl_program_exit((lx_gl_program_ref_t)program);
        program = lx_null;
    }
    return (lx_gl_program_ref_t)program;
}

lx_void_t lx_gl_program_exit(lx_gl_program_ref_t self) {
    lx_gl_program_t* program = (lx_gl_program_t*)self;
    if (program) {
        if (program->vshader) {
            lx_glDeleteShader(program->vshader);
            program->vshader = 0;
        }
        if (program->fshader) {
            lx_glDeleteShader(program->fshader);
            program->fshader = 0;
        }
        if (program->program) {
            lx_glDeleteProgram(program->program);
            program->program = 0;
        }
        lx_free(program);
    }
}

lx_void_t lx_gl_program_bind(lx_gl_program_ref_t self) {
    lx_gl_program_t* program = (lx_gl_program_t*)self;
    lx_assert_and_check_return(program && program->program);

    lx_glUseProgram(program->program);
}

lx_GLint_t lx_gl_program_attr(lx_gl_program_ref_t self, lx_char_t const* name) {
    lx_gl_program_t* program = (lx_gl_program_t*)self;
    lx_assert_and_check_return_val(program && program->program && name, -1);

    return lx_glGetAttribLocation(program->program, name);
}

lx_GLint_t lx_gl_program_unif(lx_gl_program_ref_t self, lx_char_t const* name) {
    lx_gl_program_t* program = (lx_gl_program_t*)self;
    lx_assert_and_check_return_val(program && program->program && name, -1);

    return lx_glGetUniformLocation(program->program, name);
}

lx_GLint_t lx_gl_program_location(lx_gl_program_ref_t self, lx_size_t id) {
    lx_gl_program_t* program = (lx_gl_program_t*)self;
    lx_assert_and_check_return_val(program && id < LX_GL_PROGRAM_LOCATION_MAXN, 0);

    return program->location[id];
}

lx_void_t lx_gl_program_location_set(lx_gl_program_ref_t self, lx_size_t id, lx_GLint_t location) {
    lx_gl_program_t* program = (lx_gl_program_t*)self;
    lx_assert_and_check_return(program && id < LX_GL_PROGRAM_LOCATION_MAXN);

    program->location[id] = location;
}

