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
 * @file        libc.h
 *
 */
#ifndef LX_BASE_LIBC_H
#define LX_BASE_LIBC_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// vsnprintf fmt
#define lx_vsnprintf_fmt(s, n, fmt, r) \
do { \
    lx_int_t __lx_ret = 0; \
    va_list __varg_list; \
    va_start(__varg_list, fmt); \
    __lx_ret = lx_vsnprintf(s, (n), fmt, __varg_list); \
    va_end(__varg_list); \
    if (__lx_ret >= 0) s[__lx_ret] = '\0'; \
    *r = __lx_ret > 0? __lx_ret : 0; \
 \
} while (0)

/// print string with newline
#if defined(LX_COMPILER_IS_GCC) || defined(LX_COMPILER_IS_TINYC)
#   define lx_print(fmt, arg ...)           do { lx_printf(fmt lx_newline, ## arg); } while (0)
#elif defined(LX_COMPILER_IS_MSVC) && LX_COMPILER_VERSION_BE(13, 0)
#   define lx_print(fmt, ...)               do { lx_printf(fmt lx_newline, __VA_ARGS__); } while (0)
#else
#   define lx_print
#endif

// ctype
#define lx_isspace(x)               (((x) == 0x20) || ((x) > 0x8 && (x) < 0xe))
#define lx_isgraph(x)               ((x) > 0x1f && (x) < 0x7f)
#define lx_isalpha(x)               (((x) > 0x40 && (x) < 0x5b) || ((x) > 0x60 && (x) < 0x7b))
#define lx_isupper(x)               ((x) > 0x40 && (x) < 0x5b)
#define lx_islower(x)               ((x) > 0x60 && (x) < 0x7b)
#define lx_isascii(x)               ((x) >= 0x0 && (x) < 0x80)
#define lx_isdigit(x)               ((x) > 0x2f && (x) < 0x3a)
#define lx_isdigit2(x)              ((x) == '0' || (x) == '1')
#define lx_isdigit8(x)              (((x) > 0x2f && (x) < 0x38))
#define lx_isdigit10(x)             (lx_isdigit(x))
#define lx_isdigit16(x)             (((x) > 0x2f && (x) < 0x3a) || ((x) > 0x40 && (x) < 0x47) || ((x) > 0x60 && (x) < 0x67))

#define lx_tolower(x)               (lx_isupper(x)? (x) + 0x20 : (x))
#define lx_toupper(x)               (lx_islower(x)? (x) - 0x20 : (x))

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! puts
 *
 * @param s         the c-string
 *
 * @return          the real size
 */
lx_int_t            lx_puts(lx_char_t const* s);

/*! printf
 *
 * @param fmt       the format string
 *
 * @return          the real size
 */
lx_int_t            lx_printf(lx_char_t const* fmt, ...);

/*! sprintf
 *
 * @param s         the string data
 * @param fmt       the format string
 *
 * @return          the real size
 */
lx_int_t            lx_sprintf(lx_char_t* s, lx_char_t const* fmt, ...);

/*! snprintf
 *
 * @param s         the string data
 * @param n         the string size
 * @param fmt       the format string
 *
 * @return          the real size
 */
lx_int_t            lx_snprintf(lx_char_t* s, lx_size_t n, lx_char_t const* fmt, ...);

/*! vsnprintf
 *
 * @param s         the string data
 * @param n         the string size
 * @param fmt       the format string
 * @param args      the arguments
 *
 * @return          the real size
 */
lx_int_t            lx_vsnprintf(lx_char_t* s, lx_size_t n, lx_char_t const* fmt, va_list args);

/*! memset
 *
 * @param s         the memory data
 * @param c         the filled byte
 * @param n         the data size
 *
 * @return          the memory data
 */
lx_pointer_t        lx_memset(lx_pointer_t s, lx_byte_t c, lx_size_t n);

/*! memset16
 *
 * @param s         the memory data
 * @param c         the filled byte
 * @param n         the data size
 *
 * @return          the memory data
 */
lx_pointer_t        lx_memset16(lx_pointer_t s, lx_uint16_t c, lx_size_t n);

/*! memset24
 *
 * @param s         the memory data
 * @param c         the filled byte
 * @param n         the data size
 *
 * @return          the memory data
 */
lx_pointer_t        lx_memset24(lx_pointer_t s, lx_uint32_t c, lx_size_t n);

/*! memset32
 *
 * @param s         the memory data
 * @param c         the filled byte
 * @param n         the data size
 *
 * @return          the memory data
 */
lx_pointer_t        lx_memset32(lx_pointer_t s, lx_uint32_t c, lx_size_t n);

/*! memcpy
 *
 * @param s1        the destinate data
 * @param s1        the source data
 * @param n         the data size
 *
 * @return          the destinate data
 */
lx_pointer_t        lx_memcpy(lx_pointer_t s1, lx_cpointer_t s2, lx_size_t n);

/*! memmov
 *
 * @param s1        the destinate data
 * @param s1        the source data
 * @param n         the data size
 *
 * @return          the destinate data
 */
lx_pointer_t        lx_memmov(lx_pointer_t s1, lx_cpointer_t s2, lx_size_t n);

/*! strlen
 *
 * @param s         the c-string
 *
 * @return          the string length
 */
lx_size_t           lx_strlen(lx_char_t const* s);

/*! strnlen
 *
 * @param s         the c-string
 * @param n         the max length
 *
 * @return          the string length
 */
lx_size_t           lx_strnlen(lx_char_t const* s, lx_size_t n);

/*! strcmp
 *
 * @param s1        the c-string
 * @param s2        the c-string
 *
 * @return          >0, 0, <0
 */
lx_int_t            lx_strcmp(lx_char_t const* s1, lx_char_t const* s2);

/*! strncmp
 *
 * @param s1        the c-string
 * @param s2        the c-string
 * @param n         the max length
 *
 * @return          >0, 0, <0
 */
lx_int_t            lx_strncmp(lx_char_t const* s1, lx_char_t const* s2, lx_size_t n);

/*! strnicmp
 *
 * @param s1        the c-string
 * @param s2        the c-string
 * @param n         the max length
 *
 * @return          >0, 0, <0
 */
lx_int_t            lx_strnicmp(lx_char_t const* s1, lx_char_t const* s2, lx_size_t n);

/*! strtol
 *
 * @param s         the c-string
 * @param endptr    the end pointer
 * @param base      the base, 10, 16, ..
 *
 * @return          the long value
 */
lx_long_t           lx_strtol(lx_char_t const* s, lx_char_t** endptr, lx_int_t base);

/*! strchr
 *
 * @param s         the c-string
 * @param c         the charactor
 *
 * @return          the result positon
 */
lx_char_t*          lx_strchr(lx_char_t const* s, lx_char_t c);

/* strlcpy
 *
 * @param s1        the c-string
 * @param s2        the c-string
 * @param n         the s1 max buffer length
 *
 * @return          the result length
 */
lx_size_t           lx_strlcpy(lx_char_t* s1, lx_char_t const* s2, lx_size_t n);

/*! get random value
 *
 * @return          the random value
 */
lx_int_t            lx_rand(lx_noarg_t);

/*! set random seed
 *
 * @param seed      the random seed
 */
lx_void_t           lx_srand(lx_uint_t seed);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


