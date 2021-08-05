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
 * @file        gl_macosx.h
 *
 */
#ifndef LX_CORE_DEVICE_OPENGL_GL_MACOSX_H
#define LX_CORE_DEVICE_OPENGL_GL_MACOSX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "gl.h"
#if LX_GL_API_VERSION >= 30
#    include <OpenGL/gl3.h>
#else
#    include <OpenGL/gl.h>
#    include <OpenGL/glu.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interface
 */

#if LX_GL_API_VERSION < 20
static lx_GLvoid_t LX_GL_APICALL lx_gl_api_glOrthof(lx_GLfloat_t left, lx_GLfloat_t right, lx_GLfloat_t bottom, lx_GLfloat_t top, lx_GLfloat_t nearp, lx_GLfloat_t farp) {
    lx_glOrtho(left, right, bottom, top, nearp, farp);
}
#endif

static lx_bool_t lx_gl_api_init() {
    lx_bool_t ok = lx_false;
    do {
        // load interfaces for common
        LX_GL_API_LOAD_S(glActiveTexture);
#if LX_GL_API_VERSION < 30
        LX_GL_API_LOAD_S(glAlphaFunc);
#endif
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
        LX_GL_API_LOAD_S(glGenVertexArrays);
        LX_GL_API_LOAD_S(glBindVertexArray);
        LX_GL_API_LOAD_S(glDeleteVertexArrays);
#endif
        ok = lx_true;

    } while (0);
    return ok;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif
