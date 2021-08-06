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
 * @file        gl_android.h
 *
 */
#ifndef LX_CORE_DEVICE_OPENGL_GL_ANDROID_H
#define LX_CORE_DEVICE_OPENGL_GL_ANDROID_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "gl.h"
#define GL_GLEXT_PROTOTYPES
#include <GLES/gl.h>
#include <GLES/glext.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interface
 */
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

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif
