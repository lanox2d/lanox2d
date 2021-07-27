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
 * @file        gl.h
 *
 */
#ifndef LX_CORE_DEVICE_OPENGL_GL_H
#define LX_CORE_DEVICE_OPENGL_GL_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// api function macros
#define LX_GL_APICALL
#define LX_GL_API_TYPE(func)                LX_GL_APICALL *lx_##func##_t
#define LX_GL_API_EXTERN(func)              extern lx_##func##_t lx_##func
#define LX_GL_API_LOAD_S(func)              lx_##func = func
#define LX_GL_API_LOAD_S_(name, func)       lx_##name = &func
#define LX_GL_API_LOAD_D(library, func)     lx_##func = (lx_##func##_t)lx_dlsym(library, #func); lx_assert_and_check_break(lx_##func)

// opengl api version
#ifdef LX_CONFIG_OPENGL_VERSION
#   define LX_GL_API_VERSION            LX_CONFIG_OPENGL_VERSION
#else
#   error unknown opengl version
#endif
#ifdef LX_CONFIG_OPENGL_ES
#   define LX_GL_API_ES                 LX_CONFIG_OPENGL_ES
#endif

// data type
#define LX_GL_BYTE                      (0x1400)
#define LX_GL_UNSIGNED_BYTE             (0x1401)
#define LX_GL_SHORT                     (0x1402)
#define LX_GL_UNSIGNED_SHORT            (0x1403)
#define LX_GL_INT                       (0x1404)
#define LX_GL_UNSIGNED_INT              (0x1405)
#define LX_GL_FLOAT                     (0x1406)
#if defined(LX_CONFIG_OS_IOS) || defined(LX_CONFIG_OS_ANDROID)
#   define LX_GL_FIXED                  (0x140C)
#endif

// boolean
#define LX_GL_FALSE                     (0)
#define LX_GL_TRUE                      (1)

// string name
#define LX_GL_VENDOR                    (0x1F00)
#define LX_GL_RENDERER                  (0x1F01)
#define LX_GL_VERSION                   (0x1F02)
#define LX_GL_EXTENSIONS                (0x1F03)

// matrix mode
#define LX_GL_MATRIX_MODE               (0x0BA0)
#define LX_GL_MODELVIEW                 (0x1700)
#define LX_GL_PROJECTION                (0x1701)
#define LX_GL_TEXTURE                   (0x1702)

// vertex arrays
#define LX_GL_VERTEX_ARRAY              (0x8074)

// color arrays
#define LX_GL_COLOR_ARRAY               (0x8076)

// texture arrays
#define LX_GL_TEXTURE_COORD_ARRAY       (0x8078)

// texture parameter name
#define LX_GL_TEXTURE_WRAP_S            (0x2802)
#define LX_GL_TEXTURE_WRAP_T            (0x2803)
#define LX_GL_TEXTURE_MAG_FILTER        (0x2800)
#define LX_GL_TEXTURE_MIN_FILTER        (0x2801)

// texture env parameter
#define LX_GL_TEXTURE_ENV_MODE          (0x2200)
#define lx_GL_TEXTURE_ENV_COLOR         (0x2201)

// texture env target
#define LX_GL_TEXTURE_ENV               (0x2300)

// texture env mode
#define LX_GL_MODULATE                  (0x2100)
#define LX_GL_DECAL                     (0x2101)
//#define LX_GL_BLEND
#define LX_GL_ADD                       (0x0104)
//#define LX_GL_REPLACE

// texture mag filter
#define LX_GL_NEAREST                   (0x2600)
#define LX_GL_LINEAR                    (0x2601)

// blending
#define LX_GL_BLEND                     (0x0BE2)
#define LX_GL_BLEND_SRC                 (0x0BE1)
#define LX_GL_BLEND_DST                 (0x0BE0)
#define LX_GL_ZERO                      (0x0)
#define LX_GL_ONE                       (0x1)
#define LX_GL_SRC_COLOR                 (0x0300)
#define LX_GL_ONE_MINUS_SRC_COLOR       (0x0301)
#define LX_GL_SRC_ALPHA                 (0x0302)
#define LX_GL_ONE_MINUS_SRC_ALPHA       (0x0303)
#define LX_GL_DST_ALPHA                 (0x0304)
#define LX_GL_ONE_MINUS_DST_ALPHA       (0x0305)
#define LX_GL_DST_COLOR                 (0x0306)
#define LX_GL_ONE_MINUS_DST_COLOR       (0x0307)
#define LX_GL_SRC_ALPHA_SATURATE        (0x0308)

// antialiasing
#define LX_GL_MULTISAMPLE               (0x809D)
#define LX_GL_LINE_SMOOTH               (0x0B20)
#define LX_GL_LINE_SMOOTH_HINT          (0x0C52)
#define LX_GL_FOG                       (0x0B60)
#define LX_GL_FOG_HINT                  (0x0C54)
#define LX_GL_NICEST                    (0x1102)

// stencil op
//#define LX_GL_ZERO
#define LX_GL_KEEP                      (0x1E00)
#define LX_GL_REPLACE                   (0x1E01)
#define LX_GL_INCR                      (0x1E02)
#define LX_GL_DECR                      (0x1E03)
#define LX_GL_INVERT                    (0x150A)
#define LX_GL_INCR_WRAP                 (0x8507)
#define LX_GL_DECR_WRAP                 (0x8508)

// stencil function
#define LX_GL_NEVER                     (0x0200)
#define LX_GL_LESS                      (0x0201)
#define LX_GL_EQUAL                     (0x0202)
#define LX_GL_LEQUAL                    (0x0203)
#define LX_GL_GREATER                   (0x0204)
#define LX_GL_NOTEQUAL                  (0x0205)
#define LX_GL_GEQUAL                    (0x0206)
#define LX_GL_ALWAYS                    (0x0207)

// enable cap
#define LX_GL_TEXTURE_2D                (0x0DE1)
#define LX_GL_BLEND                     (0x0BE2)
#define LX_GL_DITHER                    (0x0BD0)
#define LX_GL_STENCIL_TEST              (0x0B90)
#define LX_GL_DEPTH_TEST                (0x0B71)
#define LX_GL_SCISSOR_TEST              (0x0C11)

// texture wrap mode
#define LX_GL_REPEAT                    (0x2901)
#define LX_GL_CLAMP_TO_EDGE             (0x812F)
#define LX_GL_MIRRORED_REPEAT           (0x8370)
#define LX_GL_CLAMP_TO_BORDER           (0x812D)

// clear buffer mask
#define LX_GL_DEPTH_BUFFER_BIT          (0x00000100)
#define LX_GL_STENCIL_BUFFER_BIT        (0x00000400)
#define LX_GL_COLOR_BUFFER_BIT          (0x00004000)

// pixel format
#define LX_GL_ALPHA                     (0x1906)
#define LX_GL_RGB                       (0x1907)
#define LX_GL_RGBA                      (0x1908)
#define LX_GL_LUMINANCE                 (0x1909)
#define LX_GL_LUMINANCE_ALPHA           (0x190A)
#define LX_GL_BGR                       (0x80E0)
#define LX_GL_BGRA                      (0x80E1)

// pixel type
//#define LX_GL_UNSIGNED_BYTE
#define LX_GL_UNSIGNED_SHORT_4_4_4_4    (0x8033)
#define LX_GL_UNSIGNED_SHORT_5_5_5_1    (0x8034)
#define LX_GL_UNSIGNED_SHORT_5_6_5      (0x8363)

// pixel store parameter
#define LX_GL_UNPACK_ALIGNMENT          (0x0CF5)
#define LX_GL_PACK_ALIGNMENT            (0x0D05)

// begin mode
#define LX_GL_POINTS                    (0x0000)
#define LX_GL_LINES                     (0x0001)
#define LX_GL_LINE_LOOP                 (0x0002)
#define LX_GL_LINE_STRIP                (0x0003)
#define LX_GL_TRIANGLES                 (0x0004)
#define LX_GL_TRIANGLE_STRIP            (0x0005)
#define LX_GL_TRIANGLE_FAN              (0x0006)

// shaders
#define LX_GL_FRAGMENT_SHADER           (0x8B30)
#define LX_GL_VERTEX_SHADER             (0x8B31)
#define LX_GL_SHADER_TYPE               (0x8B4F)
#define LX_GL_DELETE_STATUS             (0x8B80)
#define LX_GL_LINK_STATUS               (0x8B82)
#define LX_GL_VALIDATE_STATUS           (0x8B83)
#define LX_GL_ATTACHED_SHADERS          (0x8B85)

// shader source
#define LX_GL_COMPILE_STATUS            (0x8B81)
#define LX_GL_INFO_LOG_LENGTH           (0x8B84)
#define LX_GL_SHADER_SOURCE_LENGTH      (0x8B88)
#define LX_GL_SHADER_COMPILER           (0x8DFA)

// texture unit
#define LX_GL_TEXTURE0                  (0x84C0)
#define LX_GL_TEXTURE1                  (0x84C1)
#define LX_GL_TEXTURE2                  (0x84C2)
#define LX_GL_TEXTURE3                  (0x84C3)
#define LX_GL_TEXTURE4                  (0x84C4)
#define LX_GL_TEXTURE5                  (0x84C5)
#define LX_GL_TEXTURE6                  (0x84C6)
#define LX_GL_TEXTURE7                  (0x84C7)
#define LX_GL_TEXTURE8                  (0x84C8)
#define LX_GL_TEXTURE9                  (0x84C9)
#define LX_GL_TEXTURE10                 (0x84CA)
#define LX_GL_TEXTURE11                 (0x84CB)
#define LX_GL_TEXTURE12                 (0x84CC)
#define LX_GL_TEXTURE13                 (0x84CD)
#define LX_GL_TEXTURE14                 (0x84CE)
#define LX_GL_TEXTURE15                 (0x84CF)
#define LX_GL_TEXTURE16                 (0x84D0)
#define LX_GL_TEXTURE17                 (0x84D1)
#define LX_GL_TEXTURE18                 (0x84D2)
#define LX_GL_TEXTURE19                 (0x84D3)
#define LX_GL_TEXTURE20                 (0x84D4)
#define LX_GL_TEXTURE21                 (0x84D5)
#define LX_GL_TEXTURE22                 (0x84D6)
#define LX_GL_TEXTURE23                 (0x84D7)
#define LX_GL_TEXTURE24                 (0x84D8)
#define LX_GL_TEXTURE25                 (0x84D9)
#define LX_GL_TEXTURE26                 (0x84DA)
#define LX_GL_TEXTURE27                 (0x84DB)
#define LX_GL_TEXTURE28                 (0x84DC)
#define LX_GL_TEXTURE29                 (0x84DD)
#define LX_GL_TEXTURE30                 (0x84DE)
#define LX_GL_TEXTURE31                 (0x84DF)
#define LX_GL_ACTIVE_TEXTURE            (0x84E0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

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

// interface types
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glActiveTexture))             (lx_GLenum_t texture);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glAlphaFunc))                 (lx_GLenum_t func, lx_GLclampf_t ref);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glAttachShader))              (lx_GLuint_t program, lx_GLuint_t shader);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glBindTexture))               (lx_GLenum_t target, lx_GLuint_t texture);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glBlendFunc))                 (lx_GLenum_t sfactor, lx_GLenum_t dfactor);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glClear))                     (lx_GLbitfield_t mask);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glClearColor))                (lx_GLclampf_t red, lx_GLclampf_t green, lx_GLclampf_t blue, lx_GLclampf_t alpha);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glClearStencil))              (lx_GLint_t s);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glColor4f))                   (lx_GLfloat_t red, lx_GLfloat_t green, lx_GLfloat_t blue, lx_GLfloat_t alpha);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glColorMask))                 (lx_GLboolean_t red, lx_GLboolean_t green, lx_GLboolean_t blue, lx_GLboolean_t alpha);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glColorPointer))              (lx_GLint_t size, lx_GLenum_t type, lx_GLsizei_t stride, lx_GLvoid_t const* pointer);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glCompileShader))             (lx_GLuint_t shader);
typedef lx_GLuint_t             (LX_GL_API_TYPE(glCreateProgram))             (lx_GLvoid_t);
typedef lx_GLuint_t             (LX_GL_API_TYPE(glCreateShader))              (lx_GLenum_t type);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glDeleteProgram))             (lx_GLuint_t program);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glDeleteShader))              (lx_GLuint_t shader);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glDeleteTextures))            (lx_GLsizei_t n, lx_GLuint_t const* textures);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glDisable))                   (lx_GLenum_t cap);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glDisableClientState))        (lx_GLenum_t cap);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glDisableVertexAttribArray))  (lx_GLuint_t index);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glDrawArrays))                (lx_GLenum_t mode, lx_GLint_t first, lx_GLsizei_t count);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glEnable))                    (lx_GLenum_t cap);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glEnableClientState))         (lx_GLenum_t cap);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glEnableVertexAttribArray))   (lx_GLuint_t index);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glGenTextures))               (lx_GLsizei_t n, lx_GLuint_t* textures);
typedef lx_GLint_t              (LX_GL_API_TYPE(glGetAttribLocation))         (lx_GLuint_t program, lx_GLchar_t const* name);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glGetProgramiv))              (lx_GLuint_t program, lx_GLenum_t pname, lx_GLint_t* params);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glGetProgramInfoLog))         (lx_GLuint_t program, lx_GLsizei_t bufsize, lx_GLsizei_t* length, lx_GLchar_t* infolog);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glGetShaderiv))               (lx_GLuint_t shader, lx_GLenum_t pname, lx_GLint_t* params);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glGetShaderInfoLog))          (lx_GLuint_t shader, lx_GLsizei_t bufsize, lx_GLsizei_t* length, lx_GLchar_t* infolog);
typedef lx_GLubyte_t const*     (LX_GL_API_TYPE(glGetString))                 (lx_GLenum_t name);
typedef lx_GLint_t              (LX_GL_API_TYPE(glGetUniformLocation))        (lx_GLuint_t program, lx_GLchar_t const* name);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glHint))                      (lx_GLenum_t target, lx_GLenum_t mode);
typedef lx_GLboolean_t          (LX_GL_API_TYPE(glIsTexture))                 (lx_GLuint_t texture);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glLineWidth))                 (lx_GLfloat_t width);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glLinkProgram))               (lx_GLuint_t program);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glLoadIdentity))              (lx_GLvoid_t);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glLoadMatrixf))               (lx_GLfloat_t const* matrix);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glMatrixMode))                (lx_GLenum_t mode);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glMultMatrixf))               (lx_GLfloat_t const* matrix);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glOrtho))                     (lx_GLdouble_t left, lx_GLdouble_t right, lx_GLdouble_t bottom, lx_GLdouble_t top, lx_GLdouble_t near, lx_GLdouble_t far);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glOrthof))                    (lx_GLfloat_t left, lx_GLfloat_t right, lx_GLfloat_t bottom, lx_GLfloat_t top, lx_GLfloat_t near, lx_GLfloat_t far);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glPixelStorei))               (lx_GLenum_t pname, lx_GLint_t param);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glPopMatrix))                 (lx_GLvoid_t);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glPushMatrix))                (lx_GLvoid_t);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glRotatef))                   (lx_GLfloat_t angle, lx_GLfloat_t x, lx_GLfloat_t y, lx_GLfloat_t z);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glScalef))                    (lx_GLfloat_t x, lx_GLfloat_t y, lx_GLfloat_t z);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glScissor))                   (lx_GLint_t x, lx_GLint_t y, lx_GLsizei_t width, lx_GLsizei_t height);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glShaderSource))              (lx_GLuint_t shader, lx_GLsizei_t count, lx_GLchar_t const* const* string, const lx_GLint_t* length);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glStencilFunc))               (lx_GLenum_t func, lx_GLint_t ref, lx_GLuint_t mask);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glStencilMask))               (lx_GLuint_t mask);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glStencilOp))                 (lx_GLenum_t fail, lx_GLenum_t zfail, lx_GLenum_t zpass);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glTexCoordPointer))           (lx_GLint_t size, lx_GLenum_t type, lx_GLsizei_t stride, lx_GLvoid_t const* ptr);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glTexEnvi))                   (lx_GLenum_t target, lx_GLenum_t pname, lx_GLint_t param);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glTexImage2D))                (lx_GLenum_t target, lx_GLint_t level, lx_GLint_t internalFormat, lx_GLsizei_t width, lx_GLsizei_t height, lx_GLint_t border, lx_GLenum_t format, lx_GLenum_t type, lx_GLvoid_t const* pixels);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glTexParameterf))             (lx_GLenum_t target, lx_GLenum_t pname, lx_GLfloat_t param);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glTexParameteri))             (lx_GLenum_t target, lx_GLenum_t pname, lx_GLint_t param);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glTranslatef))                (lx_GLfloat_t x, lx_GLfloat_t y, lx_GLfloat_t z);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glUniform1i))                 (lx_GLint_t location, lx_GLint_t x);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glUniformMatrix4fv))          (lx_GLint_t location, lx_GLsizei_t count, lx_GLboolean_t transpose, lx_GLfloat_t const* value);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glUseProgram))                (lx_GLuint_t program);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glVertexAttrib4f))            (lx_GLuint_t indx, lx_GLfloat_t x, lx_GLfloat_t y, lx_GLfloat_t z, lx_GLfloat_t w);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glVertexAttribPointer))       (lx_GLuint_t indx, lx_GLint_t size, lx_GLenum_t type, lx_GLboolean_t normalized, lx_GLsizei_t stride, lx_GLvoid_t const* ptr);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glVertexPointer))             (lx_GLint_t size, lx_GLenum_t type, lx_GLsizei_t stride,  lx_GLvoid_t const* ptr);
typedef lx_GLvoid_t             (LX_GL_API_TYPE(glViewport))                  (lx_GLint_t x, lx_GLint_t y, lx_GLsizei_t width, lx_GLsizei_t height);
typedef lx_GLenum_t             (LX_GL_API_TYPE(glGetError))                  (lx_GLvoid_t);
typedef lx_void_t               (LX_GL_API_TYPE(glGenVertexArrays))           (lx_GLsizei_t n, lx_GLuint_t* arrays);
typedef lx_void_t               (LX_GL_API_TYPE(glGenBuffers))                (lx_GLsizei_t n, lx_GLuint_t* buffers);
typedef lx_void_t               (LX_GL_API_TYPE(glBindVertexArray))           (lx_GLuint_t array);
typedef lx_void_t               (LX_GL_API_TYPE(glBindBuffer))                (lx_GLenum_t target, lx_GLuint_t array);
typedef lx_void_t               (LX_GL_API_TYPE(glBufferData))                (lx_GLenum_t target, lx_GLsizeiptr_t size, lx_GLvoid_t const* data, lx_GLenum_t usage);
typedef lx_void_t               (LX_GL_API_TYPE(glDeleteVertexArrays))        (lx_GLsizei_t n, lx_GLuint_t const* arrays);
typedef lx_void_t               (LX_GL_API_TYPE(glDeleteBuffers))             (lx_GLsizei_t n, lx_GLuint_t const* buffers);

// the opengl extensions enum
typedef enum lx_gl_extensions_e_ {
    LX_GL_EXT_ARB_NONE = 0,
    LX_GL_EXT_ARB_vertex_array_object,          // vao
    LX_GL_EXT_ARB_texture_non_power_of_two,
    LX_GL_EXT_ARB_MAXN
}lx_gl_extensions_e;

/* //////////////////////////////////////////////////////////////////////////////////////
 * functions
 */
LX_GL_API_EXTERN(glActiveTexture);
LX_GL_API_EXTERN(glAlphaFunc);
LX_GL_API_EXTERN(glAttachShader);
LX_GL_API_EXTERN(glBindTexture);
LX_GL_API_EXTERN(glBlendFunc);
LX_GL_API_EXTERN(glClear);
LX_GL_API_EXTERN(glClearColor);
LX_GL_API_EXTERN(glClearStencil);
LX_GL_API_EXTERN(glColor4f);
LX_GL_API_EXTERN(glColorMask);
LX_GL_API_EXTERN(glColorPointer);
LX_GL_API_EXTERN(glCompileShader);
LX_GL_API_EXTERN(glCreateProgram);
LX_GL_API_EXTERN(glCreateShader);
LX_GL_API_EXTERN(glDeleteProgram);
LX_GL_API_EXTERN(glDeleteShader);
LX_GL_API_EXTERN(glDeleteTextures);
LX_GL_API_EXTERN(glDisable);
LX_GL_API_EXTERN(glDisableClientState);
LX_GL_API_EXTERN(glDisableVertexAttribArray);
LX_GL_API_EXTERN(glDrawArrays);
LX_GL_API_EXTERN(glEnable);
LX_GL_API_EXTERN(glEnableClientState);
LX_GL_API_EXTERN(glEnableVertexAttribArray);
LX_GL_API_EXTERN(glGenTextures);
LX_GL_API_EXTERN(glGetAttribLocation);
LX_GL_API_EXTERN(glGetProgramiv);
LX_GL_API_EXTERN(glGetProgramInfoLog);
LX_GL_API_EXTERN(glGetShaderiv);
LX_GL_API_EXTERN(glGetShaderInfoLog);
LX_GL_API_EXTERN(glGetString);
LX_GL_API_EXTERN(glGetUniformLocation);
LX_GL_API_EXTERN(glHint);
LX_GL_API_EXTERN(glIsTexture);
LX_GL_API_EXTERN(glLineWidth);
LX_GL_API_EXTERN(glLinkProgram);
LX_GL_API_EXTERN(glLoadIdentity);
LX_GL_API_EXTERN(glLoadMatrixf);
LX_GL_API_EXTERN(glMatrixMode);
LX_GL_API_EXTERN(glMultMatrixf);
LX_GL_API_EXTERN(glOrtho);
LX_GL_API_EXTERN(glOrthof);
LX_GL_API_EXTERN(glPixelStorei);
LX_GL_API_EXTERN(glPopMatrix);
LX_GL_API_EXTERN(glPushMatrix);
LX_GL_API_EXTERN(glRotatef);
LX_GL_API_EXTERN(glScalef);
LX_GL_API_EXTERN(glScissor);
LX_GL_API_EXTERN(glShaderSource);
LX_GL_API_EXTERN(glStencilFunc);
LX_GL_API_EXTERN(glStencilMask);
LX_GL_API_EXTERN(glStencilOp);
LX_GL_API_EXTERN(glTexCoordPointer);
LX_GL_API_EXTERN(glTexEnvi);
LX_GL_API_EXTERN(glTexImage2D);
LX_GL_API_EXTERN(glTexParameterf);
LX_GL_API_EXTERN(glTexParameteri);
LX_GL_API_EXTERN(glTranslatef);
LX_GL_API_EXTERN(glUniform1i);
LX_GL_API_EXTERN(glUniformMatrix4fv);
LX_GL_API_EXTERN(glUseProgram);
LX_GL_API_EXTERN(glVertexAttrib4f);
LX_GL_API_EXTERN(glVertexAttribPointer);
LX_GL_API_EXTERN(glVertexPointer);
LX_GL_API_EXTERN(glViewport);
LX_GL_API_EXTERN(glGetError);
LX_GL_API_EXTERN(glGenVertexArrays);
LX_GL_API_EXTERN(glGenBuffers);
LX_GL_API_EXTERN(glBindVertexArray);
LX_GL_API_EXTERN(glBindBuffer);
LX_GL_API_EXTERN(glBufferData);
LX_GL_API_EXTERN(glDeleteVertexArrays);
LX_GL_API_EXTERN(glDeleteBuffers);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interface
 */

/* init gl context
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_gl_context_init(lx_noarg_t);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif
