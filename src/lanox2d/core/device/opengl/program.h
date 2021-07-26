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
 * @file        program.h
 *
 */
#ifndef LX_CORE_DEVICE_OPENGL_PROGRAM_H
#define LX_CORE_DEVICE_OPENGL_PROGRAM_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "gl.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the gl program type enum
typedef enum lx_gl_program_type_e_ {
    LX_GL_PROGRAM_TYPE_NONE    = 0
,   LX_GL_PROGRAM_TYPE_SOLID   = 1
,   LX_GL_PROGRAM_TYPE_TEXTURE = 2
,   LX_GL_PROGRAM_TYPE_MAXN    = 3
}lx_gl_program_type_e;

// the gl program location id enum
typedef enum lx_gl_program_location_e_ {
    LX_GL_PROGRAM_LOCATION_COLORS          = 0
,   LX_GL_PROGRAM_LOCATION_VERTICES        = 1
,   LX_GL_PROGRAM_LOCATION_TEXCOORDS       = 2
,   LX_GL_PROGRAM_LOCATION_SAMPLER         = 3
,   LX_GL_PROGRAM_LOCATION_MATRIX_MODEL    = 4
,   LX_GL_PROGRAM_LOCATION_MATRIX_PROJECT  = 5
,   LX_GL_PROGRAM_LOCATION_MATRIX_TEXCOORD = 6
,   LX_GL_PROGRAM_LOCATION_MAXN            = 7
}lx_gl_program_location_e;

// the gl program ref type
typedef lx_typeref(gl_program);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* init gl program for gl >= 2.x
 *
 * @param type          the program type
 * @param vshader       the vertex shader content
 * @param fshader       the fragment shader content
 *
 * @return              the program
 */
lx_gl_program_ref_t     lx_gl_program_init(lx_size_t type, lx_char_t const* vshader, lx_char_t const* fshader);

/* init sold program
 *
 * @return              the program
 */
lx_gl_program_ref_t     lx_gl_program_init_solid(lx_noarg_t);

/* init texture program
 *
 * @return              the program
 */
lx_gl_program_ref_t     lx_gl_program_init_texture(lx_noarg_t);

/* exit gl program
 *
 * @param program       the program
 */
lx_void_t               lx_gl_program_exit(lx_gl_program_ref_t program);

/* bind the program to the current gl context
 *
 * @param program       the program
 */
lx_void_t               lx_gl_program_bind(lx_gl_program_ref_t program);

/* the attribute location of the given name for the program
 *
 * @param program       the program
 * @param name          the attribute name
 *
 * @return              the attribute location
 */
lx_GLint_t              lx_gl_program_attr(lx_gl_program_ref_t program, lx_char_t const* name);

/* the uniform location of the given name for the program
 *
 * @param program       the program
 * @param name          the uniform name
 *
 * @return              the uniform location
 */
lx_GLint_t              lx_gl_program_unif(lx_gl_program_ref_t program, lx_char_t const* name);

/* the program location for the given id
 *
 * @param program       the program
 * @param index         the location id
 *
 * @return              the program type
 */
lx_GLint_t              lx_gl_program_location(lx_gl_program_ref_t program, lx_size_t id);

/* set the program location from the given id
 *
 * @param program       the program
 * @param index         the location id
 * @param location      the location
 *
 */
lx_void_t               lx_gl_program_location_set(lx_gl_program_ref_t program, lx_size_t id, lx_GLint_t location);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif
