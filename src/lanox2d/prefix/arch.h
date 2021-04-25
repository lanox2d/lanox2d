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
 * @file        arch.h
 *
 */
#ifndef LX_PREFIX_ARCH_H
#define LX_PREFIX_ARCH_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"
#include "keyword.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/* arch
 *
 * gcc builtin macros for gcc -dM -E - < /dev/null
 *
 * .e.g gcc -m64 -dM -E - < /dev/null | grep 64
 * .e.g gcc -m32 -dM -E - < /dev/null | grep 86
 * .e.g gcc -march=armv6 -dM -E - < /dev/null | grep ARM
 */
#if defined(__i386) \
    || defined(__i686) \
    || defined(__i386__) \
    || defined(__i686__) \
    || defined(_M_IX86)
#   define LX_ARCH_x86
#elif defined(__x86_64) \
    || defined(__amd64__) \
    || defined(__amd64) \
    || defined(_M_IA64) \
    || defined(_M_X64)
#   define LX_ARCH_x64
#elif defined(__arm__) || defined(__arm64) || defined(__arm64__) || (defined(__aarch64__) && __aarch64__)
#   define LX_ARCH_ARM
#   if defined(__ARM64_ARCH_8__)
#       define LX_ARCH_ARM64
#       define LX_ARCH_ARM_VERSION          (8)
#       define LX_ARCH_ARM_v8
#   elif defined(__ARM_ARCH_7A__)
#       define LX_ARCH_ARM_VERSION          (7)
#       define LX_ARCH_ARM_v7A
#   elif defined(__ARM_ARCH_7__)
#       define LX_ARCH_ARM_VERSION          (7)
#       define LX_ARCH_ARM_v7
#   elif defined(__ARM_ARCH_6__)
#       define LX_ARCH_ARM_VERSION          (6)
#       define LX_ARCH_ARM_v6
#   elif defined(__ARM_ARCH_5TE__)
#       define LX_ARCH_ARM_VERSION          (5)
#       define LX_ARCH_ARM_v5te
#   elif defined(__ARM_ARCH_5__)
#       define LX_ARCH_ARM_VERSION          (5)
#       define LX_ARCH_ARM_v5
#   elif defined(__ARM_ARCH_4T__)
#       define LX_ARCH_ARM_VERSION          (4)
#       define LX_ARCH_ARM_v4t
#   elif defined(__ARM_ARCH_3__)
#       define LX_ARCH_ARM_VERSION          (3)
#       define LX_ARCH_ARM_v3
#   elif defined(__ARM_ARCH)
#       define LX_ARCH_ARM_VERSION          __ARM_ARCH
#       if __ARM_ARCH >= 8
#           define LX_ARCH_ARM_v8
#           if defined(__arm64) || defined(__arm64__)
#               define LX_ARCH_ARM64
#           elif (defined(__aarch64__) && __aarch64__)
#               define LX_ARCH_ARM64
#           endif
#       elif __ARM_ARCH >= 7
#           define LX_ARCH_ARM_v7
#       elif __ARM_ARCH >= 6
#           define LX_ARCH_ARM_v6
#       else
#           define LX_ARCH_ARM_v5
#       endif
#   elif defined(__aarch64__) && __aarch64__
#       define LX_ARCH_ARM_v8
#       define LX_ARCH_ARM64
#   else
#       error unknown arm arch version
#   endif
#   if !defined(LX_ARCH_ARM64) && (defined(__arm64) || defined(__arm64__) || (defined(__aarch64__) && __aarch64__))
#       define LX_ARCH_ARM64
#   endif
#   if defined(__thumb__)
#       define LX_ARCH_ARM_THUMB
#   endif
#   if defined(__ARM_NEON__)
#       define LX_ARCH_ARM_NEON
#   endif
#elif defined(mips) \
    || defined(_mips) \
    || defined(__mips__)
#   define LX_ARCH_MIPS
#elif defined(LX_COMPILER_IS_TINYC)
#   if defined(TCC_TARGET_I386)
#       define LX_ARCH_x86
#   elif defined(__x86_64__) || defined(TCC_TARGET_X86_64)
#       define LX_ARCH_x64
#   elif defined(TCC_TARGET_ARM)
#       define LX_ARCH_ARM
#   else
#       error unknown arch for tiny c, please define target like -DTCC_TARGET_I386
#   endif
#else
//#     define LX_ARCH_SPARC
//#     define LX_ARCH_PPC
//#     define LX_ARCH_SH4
#   error unknown arch
#endif

// sse
#if defined(LX_ARCH_x86) || defined(LX_ARCH_x64)
#   if defined(__SSE__)
#       define LX_ARCH_SSE
#   endif
#   if defined(__SSE2__)
#       define LX_ARCH_SSE2
#   endif
#   if defined(__SSE3__)
#       define LX_ARCH_SSE3
#   endif
#endif

// vfp
#if defined(__VFP_FP__) || (defined(LX_COMPILER_IS_TINYC) && defined(TCC_ARM_VFP))
#   define LX_ARCH_VFP
#endif

// elf
#if defined(__ELF__) || (defined(LX_COMPILER_IS_TINYC) && !defined(TCC_ARM_PE))
#   define LX_ARCH_ELF
#endif

// mach
#if defined(__MACH__)
#   define LX_ARCH_MACH
#endif

#endif


