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
#ifdef LX_CONFIG_OS_MACOSX
#   include <OpenGL/gl.h>
#   include <OpenGL/glu.h>
#else
#define GL_GLEXT_PROTOTYPES
#   include <GL/glut.h>
#   include <GL/glext.h>
#endif

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
    lx_byte_t       extensions[LX_GL_EXT_ARB_MAXN];
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
#if LX_GL_API_VERSION < 20
static lx_GLvoid_t LX_GL_APICALL lx_gl_api_glOrthof(lx_GLfloat_t left, lx_GLfloat_t right, lx_GLfloat_t bottom, lx_GLfloat_t top, lx_GLfloat_t nearp, lx_GLfloat_t farp) {
    lx_glOrtho(left, right, bottom, top, nearp, farp);
}
#endif

#if defined(LX_CONFIG_OS_ANDROID)
static lx_bool_t lx_gl_api_init() {
    lx_bool_t ok = lx_false;
    do {
        // load v2 library first
        lx_dlimage_ref_t library = lx_null;
        if ((library = lx_dlopen("libGLESv2.so", LX_RTLD_LAZY))) {
            // load interfaces for common
            LX_GL_API_LOAD_D(library, glActiveTexture);
            LX_GL_API_LOAD_D(library, glAlphaFunc);
            LX_GL_API_LOAD_D(library, glBindTexture);
            LX_GL_API_LOAD_D(library, glBlendFunc);
            LX_GL_API_LOAD_D(library, glClear);
            LX_GL_API_LOAD_D(library, glClearColor);
            LX_GL_API_LOAD_D(library, glClearStencil);
            LX_GL_API_LOAD_D(library, glColorMask);
            LX_GL_API_LOAD_D(library, glDeleteTextures);
            LX_GL_API_LOAD_D(library, glDisable);
            LX_GL_API_LOAD_D(library, glDrawArrays);
            LX_GL_API_LOAD_D(library, glEnable);
            LX_GL_API_LOAD_D(library, glGenTextures);
            LX_GL_API_LOAD_D(library, glGetString);
            LX_GL_API_LOAD_D(library, glIsTexture);
            LX_GL_API_LOAD_D(library, glPixelStorei);
            LX_GL_API_LOAD_D(library, glScissor);
            LX_GL_API_LOAD_D(library, glStencilFunc);
            LX_GL_API_LOAD_D(library, glStencilMask);
            LX_GL_API_LOAD_D(library, glStencilOp);
            LX_GL_API_LOAD_D(library, glTexImage2D);
            LX_GL_API_LOAD_D(library, glTexParameterf);
            LX_GL_API_LOAD_D(library, glTexParameteri);
            LX_GL_API_LOAD_D(library, glViewport);

#if LX_GL_API_VERSION >= 20
            LX_GL_API_LOAD_D(library, glAttachShader);
            LX_GL_API_LOAD_D(library, glCompileShader);
            LX_GL_API_LOAD_D(library, glCreateProgram);
            LX_GL_API_LOAD_D(library, glCreateShader);
            LX_GL_API_LOAD_D(library, glDeleteProgram);
            LX_GL_API_LOAD_D(library, glDeleteShader);
            LX_GL_API_LOAD_D(library, glDisableVertexAttribArray);
            LX_GL_API_LOAD_D(library, glEnableVertexAttribArray);
            LX_GL_API_LOAD_D(library, glGetAttribLocation);
            LX_GL_API_LOAD_D(library, glGetProgramiv);
            LX_GL_API_LOAD_D(library, glGetProgramInfoLog);
            LX_GL_API_LOAD_D(library, glGetShaderiv);
            LX_GL_API_LOAD_D(library, glGetShaderInfoLog);
            LX_GL_API_LOAD_D(library, glGetUniformLocation);
            LX_GL_API_LOAD_D(library, glLinkProgram);
            LX_GL_API_LOAD_D(library, glShaderSource);
            LX_GL_API_LOAD_D(library, glUniform1i);
            LX_GL_API_LOAD_D(library, glUniformMatrix4fv);
            LX_GL_API_LOAD_D(library, glUseProgram);
            LX_GL_API_LOAD_D(library, glVertexAttrib4f);
            LX_GL_API_LOAD_D(library, glVertexAttribPointer);
            LX_GL_API_LOAD_D(library, glGetError);
            LX_GL_API_LOAD_D(library, glBindBuffer);
            LX_GL_API_LOAD_D(library, glBufferData);
            LX_GL_API_LOAD_D(library, glDeleteBuffers);
#endif
        } else if ((library = lx_dlopen("libGLESv1_CM.so", LX_RTLD_LAZY))) { // load v1 library
            // load interfaces for common
            LX_GL_API_LOAD_D(library, glActiveTexture);
            LX_GL_API_LOAD_D(library, glAlphaFunc);
            LX_GL_API_LOAD_D(library, glBindTexture);
            LX_GL_API_LOAD_D(library, glBlendFunc);
            LX_GL_API_LOAD_D(library, glClear);
            LX_GL_API_LOAD_D(library, glClearColor);
            LX_GL_API_LOAD_D(library, glClearStencil);
            LX_GL_API_LOAD_D(library, glColorMask);
            LX_GL_API_LOAD_D(library, glDeleteTextures);
            LX_GL_API_LOAD_D(library, glDisable);
            LX_GL_API_LOAD_D(library, glDrawArrays);
            LX_GL_API_LOAD_D(library, glEnable);
            LX_GL_API_LOAD_D(library, glGenTextures);
            LX_GL_API_LOAD_D(library, glGetString);
            LX_GL_API_LOAD_D(library, glIsTexture);
            LX_GL_API_LOAD_D(library, glPixelStorei);
            LX_GL_API_LOAD_D(library, glScissor);
            LX_GL_API_LOAD_D(library, glStencilFunc);
            LX_GL_API_LOAD_D(library, glStencilMask);
            LX_GL_API_LOAD_D(library, glStencilOp);
            LX_GL_API_LOAD_D(library, glTexImage2D);
            LX_GL_API_LOAD_D(library, glTexParameterf);
            LX_GL_API_LOAD_D(library, glTexParameteri);
            LX_GL_API_LOAD_D(library, glViewport);

#if LX_GL_API_VERSION < 20
            LX_GL_API_LOAD_D(library, glColor4f);
            LX_GL_API_LOAD_D(library, glColorPointer);
            LX_GL_API_LOAD_D(library, glDisableClientState);
            LX_GL_API_LOAD_D(library, glEnableClientState);
            LX_GL_API_LOAD_D(library, glLoadIdentity);
            LX_GL_API_LOAD_D(library, glLoadMatrixf);
            LX_GL_API_LOAD_D(library, glMatrixMode);
            LX_GL_API_LOAD_D(library, glMultMatrixf);
            LX_GL_API_LOAD_D(library, glOrthof);
            LX_GL_API_LOAD_D(library, glPopMatrix);
            LX_GL_API_LOAD_D(library, glPushMatrix);
            LX_GL_API_LOAD_D(library, glRotatef);
            LX_GL_API_LOAD_D(library, glScalef);
            LX_GL_API_LOAD_D(library, glTexCoordPointer);
            LX_GL_API_LOAD_D(library, glTexEnvi);
            LX_GL_API_LOAD_D(library, glTranslatef);
            LX_GL_API_LOAD_D(library, glVertexPointer);
#endif
        }
        ok = lx_true;
    } while (0);
    return ok;
}
#elif defined(LX_CONFIG_OS_WINDOWS)
static lx_bool_t lx_gl_api_init() {
    lx_bool_t ok = lx_false;
    do {
        LX_GL_API_LOAD_S(glAlphaFunc);
        LX_GL_API_LOAD_S(glBindTexture);
        LX_GL_API_LOAD_S(glBlendFunc);
        LX_GL_API_LOAD_S(glClear);
        LX_GL_API_LOAD_S(glClearColor);
        LX_GL_API_LOAD_S(glClearStencil);
        LX_GL_API_LOAD_S(glColorMask);
        LX_GL_API_LOAD_S(glDeleteTextures);
        LX_GL_API_LOAD_S(glDisable);
        LX_GL_API_LOAD_S(glDrawArrays);
        LX_GL_API_LOAD_S(glEnable);
        LX_GL_API_LOAD_S(glGenTextures);
        LX_GL_API_LOAD_S(glGetString);
        LX_GL_API_LOAD_S(glHint);
        LX_GL_API_LOAD_S(glIsTexture);
        LX_GL_API_LOAD_S(glLineWidth);
        LX_GL_API_LOAD_S(glPixelStorei);
        LX_GL_API_LOAD_S(glScissor);
        LX_GL_API_LOAD_S(glStencilFunc);
        LX_GL_API_LOAD_S(glStencilMask);
        LX_GL_API_LOAD_S(glStencilOp);
        LX_GL_API_LOAD_S(glTexImage2D);
        LX_GL_API_LOAD_S(glTexParameterf);
        LX_GL_API_LOAD_S(glTexParameteri);
        LX_GL_API_LOAD_S(glViewport);

#if LX_GL_API_VERSION < 20
        LX_GL_API_LOAD_S(glColor4f);
        LX_GL_API_LOAD_S(glColorPointer);
        LX_GL_API_LOAD_S(glDisableClientState);
        LX_GL_API_LOAD_S(glEnableClientState);
        LX_GL_API_LOAD_S(glLoadIdentity);
        LX_GL_API_LOAD_S(glLoadMatrixf);
        LX_GL_API_LOAD_S(glMatrixMode);
        LX_GL_API_LOAD_S(glMultMatrixf);
        LX_GL_API_LOAD_S(glOrtho);
        LX_GL_API_LOAD_S_(glOrthof, lx_gl_api_glOrthof);
        LX_GL_API_LOAD_S(glPopMatrix);
        LX_GL_API_LOAD_S(glPushMatrix);
        LX_GL_API_LOAD_S(glRotatef);
        LX_GL_API_LOAD_S(glScalef);
        LX_GL_API_LOAD_S(glTexCoordPointer);
        LX_GL_API_LOAD_S(glTexEnvi);
        LX_GL_API_LOAD_S(glTranslatef);
        LX_GL_API_LOAD_S(glVertexPointer);
#endif
        ok = lx_true;
    } while (0);
    return ok;
}
#else
static lx_bool_t lx_gl_api_init() {
    lx_bool_t ok = lx_false;
    do {
        // load interfaces for common
        LX_GL_API_LOAD_S(glActiveTexture);
        LX_GL_API_LOAD_S(glAlphaFunc);
        LX_GL_API_LOAD_S(glBindTexture);
        LX_GL_API_LOAD_S(glBlendFunc);
        LX_GL_API_LOAD_S(glClear);
        LX_GL_API_LOAD_S(glClearColor);
        LX_GL_API_LOAD_S(glClearStencil);
        LX_GL_API_LOAD_S(glColorMask);
        LX_GL_API_LOAD_S(glDeleteTextures);
        LX_GL_API_LOAD_S(glDisable);
        LX_GL_API_LOAD_S(glDrawArrays);
        LX_GL_API_LOAD_S(glEnable);
        LX_GL_API_LOAD_S(glGenTextures);
        LX_GL_API_LOAD_S(glGetString);
        LX_GL_API_LOAD_S(glHint);
        LX_GL_API_LOAD_S(glIsTexture);
        LX_GL_API_LOAD_S(glLineWidth);
        LX_GL_API_LOAD_S(glPixelStorei);
        LX_GL_API_LOAD_S(glScissor);
        LX_GL_API_LOAD_S(glStencilFunc);
        LX_GL_API_LOAD_S(glStencilMask);
        LX_GL_API_LOAD_S(glStencilOp);
        LX_GL_API_LOAD_S(glTexImage2D);
        LX_GL_API_LOAD_S(glTexParameterf);
        LX_GL_API_LOAD_S(glTexParameteri);
        LX_GL_API_LOAD_S(glViewport);

        // load interfaces for gl 1.x
#if LX_GL_API_VERSION <= 20
        LX_GL_API_LOAD_S(glColor4f);
        LX_GL_API_LOAD_S(glColorPointer);
        LX_GL_API_LOAD_S(glDisableClientState);
        LX_GL_API_LOAD_S(glEnableClientState);
        LX_GL_API_LOAD_S(glLoadIdentity);
        LX_GL_API_LOAD_S(glLoadMatrixf);
        LX_GL_API_LOAD_S(glMatrixMode);
        LX_GL_API_LOAD_S(glMultMatrixf);
#   ifdef LX_CONFIG_OS_IOS
        LX_GL_API_LOAD_S(glOrthof);
#   else
        LX_GL_API_LOAD_S(glOrtho);
        LX_GL_API_LOAD_S_(glOrthof, lx_gl_api_glOrthof);
#   endif
        LX_GL_API_LOAD_S(glPopMatrix);
        LX_GL_API_LOAD_S(glPushMatrix);
        LX_GL_API_LOAD_S(glRotatef);
        LX_GL_API_LOAD_S(glScalef);
        LX_GL_API_LOAD_S(glTexCoordPointer);
        LX_GL_API_LOAD_S(glTexEnvi);
        LX_GL_API_LOAD_S(glTranslatef);
        LX_GL_API_LOAD_S(glVertexPointer);
#endif

#if LX_GL_API_VERSION >= 20
        LX_GL_API_LOAD_S(glAttachShader);
        LX_GL_API_LOAD_S(glCompileShader);
        LX_GL_API_LOAD_S(glCreateProgram);
        LX_GL_API_LOAD_S(glCreateShader);
        LX_GL_API_LOAD_S(glDeleteProgram);
        LX_GL_API_LOAD_S(glDeleteShader);
        LX_GL_API_LOAD_S(glDisableVertexAttribArray);
        LX_GL_API_LOAD_S(glEnableVertexAttribArray);
        LX_GL_API_LOAD_S(glGetAttribLocation);
        LX_GL_API_LOAD_S(glGetProgramiv);
        LX_GL_API_LOAD_S(glGetProgramInfoLog);
        LX_GL_API_LOAD_S(glGetShaderiv);
        LX_GL_API_LOAD_S(glGetShaderInfoLog);
        LX_GL_API_LOAD_S(glGetUniformLocation);
        LX_GL_API_LOAD_S(glLinkProgram);
        LX_GL_API_LOAD_S(glShaderSource);
        LX_GL_API_LOAD_S(glUniform1i);
        LX_GL_API_LOAD_S(glUniformMatrix4fv);
        LX_GL_API_LOAD_S(glUseProgram);
        LX_GL_API_LOAD_S(glVertexAttrib4f);
        LX_GL_API_LOAD_S(glVertexAttribPointer);
        LX_GL_API_LOAD_S(glGetError);
        LX_GL_API_LOAD_S(glGenBuffers);
        LX_GL_API_LOAD_S(glBindBuffer);
        LX_GL_API_LOAD_S(glBufferData);
        LX_GL_API_LOAD_S(glDeleteBuffers);
#endif

#if LX_GL_API_VERSION >= 30
#   ifdef LX_CONFIG_OS_MACOSX
        LX_GL_API_LOAD_S_(glGenVertexArrays, glGenVertexArraysAPPLE);
        LX_GL_API_LOAD_S_(glBindVertexArray, glBindVertexArrayAPPLE);
        LX_GL_API_LOAD_S_(glDeleteVertexArrays, glDeleteVertexArraysAPPLE);
#   else
        LX_GL_API_LOAD_S(glGenVertexArrays);
        LX_GL_API_LOAD_S(glBindVertexArray);
        LX_GL_API_LOAD_S(glDeleteVertexArrays);
#   endif
#endif
        ok = lx_true;

    } while (0);
    return ok;
}
#endif

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
lx_bool_t lx_gl_context_init() {

    // init api
    if (!lx_gl_api_init()) {
        lx_trace_e("init opengl api failed!");
        return lx_false;
    }

    // init context
    lx_memset(&g_gl_context, 0, sizeof(g_gl_context));

    // init extensions
    lx_gl_extensions_init();
    return lx_true;
}

lx_bool_t lx_gl_has_extension(lx_size_t ext) {
    return ext < lx_arrayn(g_gl_context.extensions)? (lx_bool_t)g_gl_context.extensions[ext] : lx_false;
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
        lx_glBindVertexArray(0);
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

lx_GLuint_t lx_gl_vertex_buffer_init(lx_cpointer_t buffer, lx_GLsizeiptr_t size, lx_bool_t dynamic) {
    lx_GLuint_t id = 0;
#if LX_GL_API_VERSION >= 20
    lx_glGenBuffers(1, &id);
    lx_glBindBuffer(LX_GL_ARRAY_BUFFER, id);
    lx_glBufferData(LX_GL_ARRAY_BUFFER, size, buffer, dynamic? LX_GL_DYNAMIC_DRAW : LX_GL_STATIC_DRAW);
#endif
    return id;
}

lx_void_t lx_gl_vertex_buffer_exit(lx_GLuint_t id) {
#if LX_GL_API_VERSION >= 20
    lx_glDeleteBuffers(1, &id);
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
