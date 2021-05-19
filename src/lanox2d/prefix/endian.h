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
 * @file        endian.h
 *
 */
#ifndef LX_PREFIX_ENDIAN_H
#define LX_PREFIX_ENDIAN_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// words endian
#ifdef __BYTE_ORDER__
#   if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#       define LX_WORDS_BIGENDIAN
#   endif
#endif
#if defined(MIPSEB) || defined(_MIPSEB) || defined(__MIPSEB__)
#   define LX_WORDS_BIGENDIAN
#endif

// float endian
#ifdef __FLOAT_WORD_ORDER__
#   if __FLOAT_WORD_ORDER__ != __ORDER_LITTLE_ENDIAN__
#       define LX_FLOAT_BIGENDIAN
#   endif
#endif

#endif


