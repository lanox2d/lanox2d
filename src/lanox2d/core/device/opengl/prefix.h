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
 * @file        prefix.h
 *
 */
#ifndef LX_CORE_DEVICE_OPENGL_PREFIX_H
#define LX_CORE_DEVICE_OPENGL_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../../path.h"
#include "../../paint.h"
#include "../../private/stroker.h"
#include "../../../platform/window.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// basic types
typedef lx_uint_t       lx_GLenum_t;
typedef lx_char_t       lx_GLchar_t;
typedef lx_byte_t       lx_GLboolean_t;
typedef lx_uint_t       lx_GLbitfield_t;
typedef lx_void_t       lx_GLvoid_t;
typedef lx_char_t       lx_GLbyte_t;
typedef lx_ushort_t     lx_GLushort_t;
typedef lx_short_t      lx_GLshort_t;
typedef lx_int_t        lx_GLint_t;
typedef lx_byte_t       lx_GLubyte_t;
typedef lx_uint_t       lx_GLuint_t;
typedef lx_int_t        lx_GLsizei_t;
typedef lx_intptr_t     lx_GLsizeiptr_t;
typedef lx_float_t      lx_GLfloat_t;
typedef lx_float_t      lx_GLclampf_t;
typedef lx_double_t     lx_GLdouble_t;
typedef lx_double_t     lx_GLclampd_t;

// the gl program ref type
typedef lx_typeref(gl_program);

// the gl matrix type
typedef lx_GLfloat_t    lx_gl_matrix_t[16];

// the gl matrix ref type
typedef lx_GLfloat_t*   lx_gl_matrix_ref_t;

#endif


