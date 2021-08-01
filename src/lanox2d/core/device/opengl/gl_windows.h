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
 * @file        gl_windows.h
 *
 */
#ifndef LX_CORE_DEVICE_OPENGL_GL_WINDOWS_H
#define LX_CORE_DEVICE_OPENGL_GL_WINDOWS_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "gl.h"
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

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

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif
