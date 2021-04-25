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
 * @file        compiler.h
 *
 */
#ifndef LX_PREFIX_COMPILER_H
#define LX_PREFIX_COMPILER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// intel c++
#if defined(__INTEL_COMPILER)
#   define LX_COMPILER_IS_INTEL
#   define LX_COMPILER_VERSION_BT(major, minor)     (__INTEL_COMPILER > ((major) * 100 + (minor)))
#   define LX_COMPILER_VERSION_BE(major, minor)     (__INTEL_COMPILER >= ((major) * 100 + (minor)))
#   define LX_COMPILER_VERSION_EQ(major, minor)     (__INTEL_COMPILER == ((major) * 100 + (minor)))
#   define LX_COMPILER_VERSION_LT(major, minor)     (__INTEL_COMPILER < ((major) * 100 + (minor)))
#   define LX_COMPILER_VERSION_LE(major, minor)     (__INTEL_COMPILER <= ((major) * 100 + (minor)))

// tiny c
#elif defined(__TINYC__)
#   define LX_COMPILER_IS_TINYC
#   define LX_COMPILER_VERSION_BT(major, minor)     (__TINYC__ > ((major) * 100 + (minor)))
#   define LX_COMPILER_VERSION_BE(major, minor)     (__TINYC__ >= ((major) * 100 + (minor)))
#   define LX_COMPILER_VERSION_EQ(major, minor)     (__TINYC__ == ((major) * 100 + (minor)))
#   define LX_COMPILER_VERSION_LT(major, minor)     (__TINYC__ < ((major) * 100 + (minor)))
#   define LX_COMPILER_VERSION_LE(major, minor)     (__TINYC__ <= ((major) * 100 + (minor)))

// borland c++
#elif defined(__BORLANDC__)
#   define LX_COMPILER_IS_BORLAND
#   define LX_COMPILER_VERSION_BT(major, minor)     (__BORLANDC__ > ((major) * 100 + (minor)))
#   define LX_COMPILER_VERSION_BE(major, minor)     (__BORLANDC__ >= ((major) * 100 + (minor)))
#   define LX_COMPILER_VERSION_EQ(major, minor)     (__BORLANDC__ == ((major) * 100 + (minor)))
#   define LX_COMPILER_VERSION_LT(major, minor)     (__BORLANDC__ < ((major) * 100 + (minor)))
#   define LX_COMPILER_VERSION_LE(major, minor)     (__BORLANDC__ <= ((major) * 100 + (minor)))

// gnu c/c++
#elif defined(__GNUC__)
#   define LX_COMPILER_IS_GCC
#   if defined(__MINGW32__) || defined(__MINGW64__)
#       define LX_COMPILER_IS_MINGW
#   endif
#   if defined(__MSYS__)
#       define LX_COMPILER_ON_MSYS
#   endif
#   if defined(__CYGWIN__) || defined(__CYGWIN32__)
#       define LX_COMPILER_ON_CYGWIN
#   endif
#   if defined(__unix__) || defined(__unix) || defined(unix) || \
        defined(LX_COMPILER_ON_MSYS) || defined(LX_COMPILER_ON_CYGWIN)
#       define LX_COMPILER_LIKE_UNIX
#   endif
#   define LX_COMPILER_VERSION_BT(major, minor)     ((__GNUC__ * 100 + __GNUC_MINOR__) > ((major) * 100 + (minor)))
#   define LX_COMPILER_VERSION_BE(major, minor)     ((__GNUC__ * 100 + __GNUC_MINOR__) >= ((major) * 100 + (minor)))
#   define LX_COMPILER_VERSION_EQ(major, minor)     ((__GNUC__ * 100 + __GNUC_MINOR__) == ((major) * 100 + (minor)))
#   define LX_COMPILER_VERSION_LT(major, minor)     ((__GNUC__ * 100 + __GNUC_MINOR__) < ((major) * 100 + (minor)))
#   define LX_COMPILER_VERSION_LE(major, minor)     ((__GNUC__ * 100 + __GNUC_MINOR__) <= ((major) * 100 + (minor)))

    // clang
#   if defined(__clang__)
#       define LX_COMPILER_IS_CLANG
#   endif

// watcom c/c++
#elif defined(__WATCOMC__)
#   define LX_COMPILER_IS_WATCOM
#   define LX_COMPILER_VERSION_BT(major, minor)     (__WATCOMC__ > ((major) * 100 + (minor)))
#   define LX_COMPILER_VERSION_BE(major, minor)     (__WATCOMC__ >= ((major) * 100 + (minor)))
#   define LX_COMPILER_VERSION_EQ(major, minor)     (__WATCOMC__ == ((major) * 100 + (minor)))
#   define LX_COMPILER_VERSION_LT(major, minor)     (__WATCOMC__ < ((major) * 100 + (minor)))
#   define LX_COMPILER_VERSION_LE(major, minor)     (__WATCOMC__ <= ((major) * 100 + (minor)))

// digital mars c/c++
#elif defined(__DMC__)
#   define LX_COMPILER_IS_DMC
#   define LX_COMPILER_VERSION_BT(major, minor)     (__DMC__ > ((major) * 256 + (minor)))
#   define LX_COMPILER_VERSION_BE(major, minor)     (__DMC__ >= ((major) * 256 + (minor)))
#   define LX_COMPILER_VERSION_EQ(major, minor)     (__DMC__ == ((major) * 256 + (minor)))
#   define LX_COMPILER_VERSION_LT(major, minor)     (__DMC__ < ((major) * 256 + (minor)))
#   define LX_COMPILER_VERSION_LE(major, minor)     (__DMC__ <= ((major) * 256 + (minor)))

// codeplay vector c/c++
#elif defined(__VECTORC)
#   define LX_COMPILER_IS_VECTORC
#   define LX_COMPILER_VERSION_BT(major, minor)     (__VECTORC > (major))
#   define LX_COMPILER_VERSION_BE(major, minor)     (__VECTORC >= (major))
#   define LX_COMPILER_VERSION_EQ(major, minor)     (__VECTORC == (major))
#   define LX_COMPILER_VERSION_LT(major, minor)     (__VECTORC < (major))
#   define LX_COMPILER_VERSION_LE(major, minor)     (__VECTORC <= (major))

// sun c/c++
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#   define LX_COMPILER_IS_SUNC

// visual c++
#elif defined(_MSC_VER)
#   define LX_COMPILER_IS_MSVC
#   define LX_COMPILER_VERSION_BT(major, minor)     (_MSC_VER > ((major) * 100 + (minor)))
#   define LX_COMPILER_VERSION_BE(major, minor)     (_MSC_VER >= ((major) * 100 + (minor)))
#   define LX_COMPILER_VERSION_EQ(major, minor)     (_MSC_VER == ((major) * 100 + (minor)))
#   define LX_COMPILER_VERSION_LT(major, minor)     (_MSC_VER < ((major) * 100 + (minor)))
#   define LX_COMPILER_VERSION_LE(major, minor)     (_MSC_VER <= ((major) * 100 + (minor)))

// suppress warning
#   pragma warning(disable:4018)
#   pragma warning(disable:4197)
#   pragma warning(disable:4141)
#   pragma warning(disable:4996)
#   pragma warning(disable:4359)
#   pragma warning(disable:4838)

#else
#   define LX_COMPILER_IS_UNKNOWN
#endif


#endif


