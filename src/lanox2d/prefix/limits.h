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
 * @file        limits.h
 *
 */
#ifndef LX_PREFIX_LIMITS_H
#define LX_PREFIX_LIMITS_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#define LX_MAXS8                ((lx_sint8_t)(0x7f))
#define LX_MINS8                ((lx_sint8_t)(0x81))
#define LX_MAXU8                ((lx_uint8_t)(0xff))
#define LX_MINU8                ((lx_uint8_t)(0))

#define LX_MAXS16               ((lx_sint16_t)(0x7fff))
#define LX_MINS16               ((lx_sint16_t)(0x8001))
#define LX_MAXU16               ((lx_uint16_t)(0xffff))
#define LX_MINU16               ((lx_uint16_t)(0))

#define LX_MAXS32               ((lx_sint32_t)(0x7fffffff))
#define LX_MINS32               ((lx_sint32_t)(0x80000001))
#define LX_MAXU32               ((lx_uint32_t)(0xffffffff))
#define LX_MINU32               ((lx_uint32_t)(0))

#define LX_MAXS64               ((lx_sint64_t)(0x7fffffffffffffffLL))
#define LX_MINS64               ((lx_sint64_t)(0x8000000000000001LL))
#define LX_MAXU64               ((lx_uint64_t)(0xffffffffffffffffULL))
#define LX_MINU64               ((lx_uint64_t)(0))

#define LX_NAN32                (0x80000000)


#endif


