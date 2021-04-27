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
 * @file        abort.h
 *
 */
#ifndef LX_PREFIX_ABORT_H
#define LX_PREFIX_ABORT_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"
#include "arch.h"
#include "trace.h"
#include "assembler.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#if (defined(LX_ARCH_x86) || defined(LX_ARCH_x64)) && \
        (!defined(LX_CONFIG_EXCEPTION_ENABLE) || defined(LX_CONFIG_OS_WINDOWS))
#   if defined(LX_ASSEMBLER_IS_MASM) && !defined(LX_ARCH_x64)
#       define lx_abort_impl()                          do { lx_asm { int 3 } } while (0)
#   elif defined(LX_ASSEMBLER_IS_GAS)
#     define lx_abort_impl()                            do { lx_asm lx_volatile ("int3"); } while (0)
#   endif
#endif

#ifndef lx_abort_impl
#   define lx_abort_impl()                              do { *((lx_volatile lx_int_t*)0) = 0; } while (0)
#endif

#define lx_abort()                                      do { lx_trace_e("abort"); lx_abort_impl(); } while(0)

#endif


