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
 * @file        gl.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "gl.h"
#if defined(LX_CONFIG_OS_MACOSX)
#   include "gl_macosx.h"
#elif defined(LX_CONFIG_OS_IOS)
#   include "gl_ios.h"
#elif defined(LX_CONFIG_OS_WINDOWS)
#   include "gl_windows.h"
#elif defined(LX_CONFIG_OS_ANDROID)
#   include "gl_android.h"
#else
#   include "gl_linux.h"
#endif
#include "matrix.h"
#include "program.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// define opengl api function
#define LX_GL_API_DEFINE(func)            lx_##func##_t lx_##func = lx_null

// init opengl api extension
#define LX_GL_API_EXT_CHECK_INIT(ext)           do {\
    if (!lx_strncmp(p, "GL_" #ext, l)) { \
        g_gl_context.extensions[LX_GL_EXT_##ext] = 1; \
        lx_trace_d("GL_" #ext ": 1"); \
    } \
} while (0)

#define LX_GL_API_EXT_INIT(ext)           do {\
    g_gl_context.extensions[LX_GL_EXT_##ext] = 1; \
    lx_trace_d("GL_" #ext ": 1"); \
} while (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the opengl context type
typedef struct lx_gl_context_t_ {
    lx_gl_matrix_t          modelview;
    lx_gl_matrix_t          projection;
    lx_gl_program_ref_t     program;
    lx_byte_t               extensions[LX_GL_EXT_ARB_MAXN];
}lx_gl_context_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * functions
 */
LX_GL_API_DEFINE(glActiveTexture);
LX_GL_API_DEFINE(glAlphaFunc);
LX_GL_API_DEFINE(glAttachShader);
LX_GL_API_DEFINE(glBindTexture);
LX_GL_API_DEFINE(glBlendFunc);
LX_GL_API_DEFINE(glClear);
LX_GL_API_DEFINE(glClearColor);
LX_GL_API_DEFINE(glClearStencil);
LX_GL_API_DEFINE(glColor4f);
LX_GL_API_DEFINE(glColorMask);
LX_GL_API_DEFINE(glColorPointer);
LX_GL_API_DEFINE(glCompileShader);
LX_GL_API_DEFINE(glCreateProgram);
LX_GL_API_DEFINE(glCreateShader);
LX_GL_API_DEFINE(glDeleteProgram);
LX_GL_API_DEFINE(glDeleteShader);
LX_GL_API_DEFINE(glDeleteTextures);
LX_GL_API_DEFINE(glDisable);
LX_GL_API_DEFINE(glDisableClientState);
LX_GL_API_DEFINE(glDisableVertexAttribArray);
LX_GL_API_DEFINE(glDrawArrays);
LX_GL_API_DEFINE(glEnable);
LX_GL_API_DEFINE(glEnableClientState);
LX_GL_API_DEFINE(glEnableVertexAttribArray);
LX_GL_API_DEFINE(glGenTextures);
LX_GL_API_DEFINE(glGetAttribLocation);
LX_GL_API_DEFINE(glGetProgramiv);
LX_GL_API_DEFINE(glGetProgramInfoLog);
LX_GL_API_DEFINE(glGetShaderiv);
LX_GL_API_DEFINE(glGetShaderInfoLog);
LX_GL_API_DEFINE(glGetString);
LX_GL_API_DEFINE(glGetUniformLocation);
LX_GL_API_DEFINE(glHint);
LX_GL_API_DEFINE(glIsTexture);
LX_GL_API_DEFINE(glLineWidth);
LX_GL_API_DEFINE(glLinkProgram);
LX_GL_API_DEFINE(glLoadIdentity);
LX_GL_API_DEFINE(glLoadMatrixf);
LX_GL_API_DEFINE(glMatrixMode);
LX_GL_API_DEFINE(glMultMatrixf);
LX_GL_API_DEFINE(glOrtho);
LX_GL_API_DEFINE(glOrthof);
LX_GL_API_DEFINE(glPixelStorei);
LX_GL_API_DEFINE(glPopMatrix);
LX_GL_API_DEFINE(glPushMatrix);
LX_GL_API_DEFINE(glRotatef);
LX_GL_API_DEFINE(glScalef);
LX_GL_API_DEFINE(glScissor);
LX_GL_API_DEFINE(glShaderSource);
LX_GL_API_DEFINE(glStencilFunc);
LX_GL_API_DEFINE(glStencilMask);
LX_GL_API_DEFINE(glStencilOp);
LX_GL_API_DEFINE(glTexCoordPointer);
LX_GL_API_DEFINE(glTexEnvi);
LX_GL_API_DEFINE(glTexImage2D);
LX_GL_API_DEFINE(glTexParameterf);
LX_GL_API_DEFINE(glTexParameteri);
LX_GL_API_DEFINE(glTranslatef);
LX_GL_API_DEFINE(glUniform1i);
LX_GL_API_DEFINE(glUniformMatrix4fv);
LX_GL_API_DEFINE(glUseProgram);
LX_GL_API_DEFINE(glVertexAttrib4f);
LX_GL_API_DEFINE(glVertexAttribPointer);
LX_GL_API_DEFINE(glVertexPointer);
LX_GL_API_DEFINE(glViewport);
LX_GL_API_DEFINE(glGetError);
LX_GL_API_DEFINE(glGenVertexArrays);
LX_GL_API_DEFINE(glGenBuffers);
LX_GL_API_DEFINE(glBindVertexArray);
LX_GL_API_DEFINE(glBindBuffer);
LX_GL_API_DEFINE(glBufferData);
LX_GL_API_DEFINE(glDeleteVertexArrays);
LX_GL_API_DEFINE(glDeleteBuffers);

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the opengl context
static lx_gl_context_t g_gl_context;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

// init opengl api extensions
static lx_void_t lx_gl_extensions_init() {

#if LX_GL_API_VERSION < 30
    lx_char_t const* p = (lx_char_t const*)lx_glGetString(LX_GL_EXTENSIONS);
    if (p) {
        lx_size_t        n = lx_strlen(p);
        lx_char_t const* e = p + n;
        while (p < e) {
            lx_char_t const* q = lx_strchr(p, ' ');
            lx_size_t        l = q? q - p : lx_strlen(p);
            LX_GL_API_EXT_CHECK_INIT(ARB_vertex_array_object);
            LX_GL_API_EXT_CHECK_INIT(ARB_texture_non_power_of_two);
            p += l + 1;
        }
    }
#endif

#if LX_GL_API_VERSION >= 33
    // opengl 3.3 extensions supported by default (core)
    LX_GL_API_EXT_INIT(ARB_vertex_array_object);
#endif
}


/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bool_t lx_gl_context_init(lx_size_t width, lx_size_t height, lx_size_t framewidth, lx_size_t frameheight) {

    // trace
    lx_trace_d("init opengl %lux%lu, frame: %lux%lu", width, height, framewidth, frameheight);

    // init api
    if (!lx_gl_api_init()) {
        lx_trace_e("init opengl api failed!");
        return lx_false;
    }

    // init context
    lx_memset(&g_gl_context, 0, sizeof(g_gl_context));

    // init extensions
    lx_gl_extensions_init();

    // init viewport
    lx_glViewport(0, 0, (lx_GLsizei_t)framewidth, (lx_GLsizei_t)frameheight);

    /* init the projection matrix
     *
     * opengl (origin):
     *
     *  y
     * /|\
     *  |
     *  |
     *   ----------> x
     *
     * to (world)
     *
     *   ----------> x
     *  |
     *  |
     * \|/
     *  y
     */
#if LX_GL_API_VERSION >= 20
    lx_gl_matrix_orthof(g_gl_context.projection, 0.0f, (lx_GLfloat_t)width, (lx_GLfloat_t)height, 0.0f, -1.0f, 1.0f);
#else
    lx_glMatrixMode(LX_GL_PROJECTION);
    lx_glLoadIdentity();
    lx_glOrthof(0.0f, (lx_GLfloat_t)width, (lx_GLfloat_t)height, 0.0f, -1.0f, 1.0f);
#endif

    // init the modelview matrix
    lx_gl_matrix_clear(g_gl_context.modelview);
#if LX_GL_API_VERSION < 20
    lx_glMatrixMode(LX_GL_MODELVIEW);
    lx_glLoadIdentity();
#endif
    return lx_true;
}

lx_bool_t lx_gl_has_extension(lx_size_t ext) {
    return ext < lx_arrayn(g_gl_context.extensions)? (lx_bool_t)g_gl_context.extensions[ext] : lx_false;
}

lx_gl_matrix_ref_t lx_gl_matrix_modelview(lx_noarg_t) {
    return g_gl_context.modelview;
}

lx_gl_matrix_ref_t lx_gl_matrix_projection() {
#if LX_GL_API_VERSION >= 20
    return g_gl_context.projection;
#else
    // we need not it now
    return lx_null;
#endif
}

lx_void_t lx_gl_matrix_uniform_set(lx_size_t index, lx_gl_matrix_ref_t matrix) {
    lx_assert(matrix);
#if LX_GL_API_VERSION >= 20
    lx_assert(g_gl_context.program);
    lx_glUniformMatrix4fv(lx_gl_program_location(g_gl_context.program, index), 1, LX_GL_FALSE, matrix);
#else
    if (index == LX_GL_PROGRAM_LOCATION_MATRIX_MODEL) {
        lx_glMatrixMode(LX_GL_MODELVIEW);
    } else if (index == LX_GL_PROGRAM_LOCATION_MATRIX_PROJECT) {
        lx_glMatrixMode(LX_GL_PROJECTION);
    } else if (index == LX_GL_PROGRAM_LOCATION_MATRIX_TEXCOORD) {
        lx_glMatrixMode(LX_GL_TEXTURE);
    }
    lx_glLoadIdentity();
    lx_glMultMatrixf(matrix);
#endif
}

lx_void_t lx_gl_program_enable(lx_gl_program_ref_t program) {
#if LX_GL_API_VERSION >= 20
    lx_assert(program);
    g_gl_context.program = program;
    lx_gl_program_bind(program);
#endif
}

lx_void_t lx_gl_program_disable() {
    g_gl_context.program = lx_null;
}

lx_GLuint_t lx_gl_vertex_array_init() {
    lx_GLuint_t id = 0;
#if LX_GL_API_VERSION >= 20
    if (lx_gl_has_extension(LX_GL_EXT_ARB_vertex_array_object)) {
        lx_glGenVertexArrays(1, &id);
    }
#endif
    return id;
}

lx_void_t lx_gl_vertex_array_exit(lx_GLuint_t id) {
#if LX_GL_API_VERSION >= 20
    if (lx_gl_has_extension(LX_GL_EXT_ARB_vertex_array_object)) {
        lx_glDeleteVertexArrays(1, &id);
    }
#endif
}

lx_bool_t lx_gl_vertex_array_enable(lx_GLuint_t id) {
#if LX_GL_API_VERSION >= 20
    if (lx_gl_has_extension(LX_GL_EXT_ARB_vertex_array_object)) {
        lx_glBindVertexArray(id);
        return lx_true;
    }
#endif
    return lx_false;
}

lx_void_t lx_gl_vertex_array_disable() {
#if LX_GL_API_VERSION >= 20
    if (lx_gl_has_extension(LX_GL_EXT_ARB_vertex_array_object)) {
        lx_glBindVertexArray(0);
    }
#endif
}

lx_GLuint_t lx_gl_vertex_buffer_init(lx_noarg_t) {
    lx_GLuint_t id = 0;
#if LX_GL_API_VERSION >= 20
    lx_glGenBuffers(1, &id);
#endif
    return id;
}

lx_void_t lx_gl_vertex_buffer_exit(lx_GLuint_t id) {
#if LX_GL_API_VERSION >= 20
    lx_glDeleteBuffers(1, &id);
#endif
}

lx_void_t lx_gl_vertex_buffer_data_set(lx_cpointer_t buffer, lx_size_t size, lx_bool_t dynamic) {
#if LX_GL_API_VERSION >= 20
    lx_glBufferData(LX_GL_ARRAY_BUFFER, (lx_GLsizeiptr_t)size, buffer, dynamic? LX_GL_DYNAMIC_DRAW : LX_GL_STATIC_DRAW);
#endif
}

lx_void_t lx_gl_vertex_buffer_enable(lx_GLuint_t id) {
#if LX_GL_API_VERSION >= 20
    lx_glBindBuffer(LX_GL_ARRAY_BUFFER, id);
#endif
}

lx_void_t lx_gl_vertex_buffer_disable() {
#if LX_GL_API_VERSION >= 20
    lx_glBindBuffer(LX_GL_ARRAY_BUFFER, 0);
#endif
}

lx_void_t lx_gl_vertex_attribute_enable(lx_size_t index) {
#if LX_GL_API_VERSION >= 20
    lx_assert(g_gl_context.program);
    lx_glEnableVertexAttribArray(lx_gl_program_location(g_gl_context.program, index));
#else
    if (index == LX_GL_PROGRAM_LOCATION_VERTICES) {
        lx_glEnableClientState(LX_GL_VERTEX_ARRAY);
    } else if (index == LX_GL_PROGRAM_LOCATION_TEXCOORDS) {
        lx_glEnableClientState(LX_GL_TEXTURE_COORD_ARRAY);
    }
#endif
}

lx_void_t lx_gl_vertex_attribute_disable(lx_size_t index) {
#if LX_GL_API_VERSION >= 20
    lx_assert(g_gl_context.program);
    lx_glDisableVertexAttribArray(lx_gl_program_location(g_gl_context.program, index));
#else
    if (index == LX_GL_PROGRAM_LOCATION_VERTICES) {
        lx_glDisableClientState(LX_GL_VERTEX_ARRAY);
    } else if (index == LX_GL_PROGRAM_LOCATION_TEXCOORDS) {
        lx_glDisableClientState(LX_GL_TEXTURE_COORD_ARRAY);
    }
#endif
}

lx_void_t lx_gl_vertex_attribute_set(lx_size_t index, lx_point_ref_t pointer) {
#if LX_GL_API_VERSION >= 20
    lx_assert(g_gl_context.program);
    lx_glVertexAttribPointer(lx_gl_program_location(g_gl_context.program, index), 2, LX_GL_FLOAT, LX_GL_FALSE, 0, pointer);
#else
    if (index == LX_GL_PROGRAM_LOCATION_VERTICES) {
        lx_glVertexPointer(2, LX_GL_FLOAT, 0, pointer);
    } else if (index == LX_GL_PROGRAM_LOCATION_TEXCOORDS) {
        lx_glTexCoordPointer(2, LX_GL_FLOAT, 0, pointer);
    }
#endif
}

lx_void_t lx_gl_vertex_color_set(lx_size_t index, lx_color_t color) {
#if LX_GL_API_VERSION >= 20
    lx_assert(g_gl_context.program);
    lx_glVertexAttrib4f(lx_gl_program_location(g_gl_context.program, index), (lx_float_t)color.r / 0xff, (lx_float_t)color.g / 0xff, (lx_float_t)color.b / 0xff, (lx_float_t)color.a / 0xff);
#else
    lx_glColor4f((lx_float_t)color.r / 0xff, (lx_float_t)color.g / 0xff, (lx_float_t)color.b / 0xff, (lx_float_t)color.a / 0xff);
#endif
}

