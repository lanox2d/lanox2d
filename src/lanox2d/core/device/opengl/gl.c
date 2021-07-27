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
// define func
#define LX_GL_API_DEFINE(func)            lx_##func##_t lx_##func = lx_null

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
 * private implementation
 */
static lx_GLvoid_t LX_GL_APICALL lx_gl_api_glOrthof(lx_GLfloat_t left, lx_GLfloat_t right, lx_GLfloat_t bottom, lx_GLfloat_t top, lx_GLfloat_t nearp, lx_GLfloat_t farp) {
    lx_glOrtho(left, right, bottom, top, nearp, farp);
}

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

            // load interfaces for gl >= 2.0
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

            // load interfaces for gl >= 3.0
            LX_GL_API_LOAD_D(library, glGenVertexArrays);
            LX_GL_API_LOAD_D(library, glBindVertexArray);
            LX_GL_API_LOAD_D(library, glDeleteVertexArrays);

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

            // load interfaces for gl 1.x
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

        // load interfaces for gl 1.x
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

        // load interfaces for gl >= 2.0
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

        // load interfaces for gl >= 3.0
#ifdef LX_CONFIG_OS_MACOSX
        LX_GL_API_LOAD_S_(glGenVertexArrays, glGenVertexArraysAPPLE);
        LX_GL_API_LOAD_S_(glBindVertexArray, glBindVertexArrayAPPLE);
        LX_GL_API_LOAD_S_(glDeleteVertexArrays, glDeleteVertexArraysAPPLE);
#else
        LX_GL_API_LOAD_S(glGenVertexArrays);
        LX_GL_API_LOAD_S(glBindVertexArray);
        LX_GL_API_LOAD_S(glDeleteVertexArrays);
#endif
        ok = lx_true;

    } while (0);
    return ok;
}
#endif


/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bool_t lx_gl_context_init() {
    if (!lx_gl_api_init()) {
        lx_trace_e("init opengl api failed!");
        return lx_false;
    }
    return lx_true;
}
