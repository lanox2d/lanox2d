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
 * @file        version.h
 *
 */
#ifndef LX_PREFIX_VERSION_H
#define LX_PREFIX_VERSION_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"
#include "keyword.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/// the major version
#define LX_VERSION_MAJOR            LX_CONFIG_VERSION_MAJOR

/// the minor version
#define LX_VERSION_MINOR            LX_CONFIG_VERSION_MINOR

/// the alter version
#define LX_VERSION_ALTER            LX_CONFIG_VERSION_ALTER

/// the build version
#ifndef LX_CONFIG_VERSION_BUILD
#   define LX_CONFIG_VERSION_BUILD  0
#endif
#define LX_VERSION_BUILD            LX_CONFIG_VERSION_BUILD

/// the build version string
#define LX_VERSION_BUILD_STRING     __lx_mstring_ex__(LX_CONFIG_VERSION_BUILD)

/// the version string
#define LX_VERSION_STRING           __lx_mstrcat6__("tbox_", __lx_mstring_ex__(__lx_mconcat8_ex__(v, LX_VERSION_MAJOR, _, LX_VERSION_MINOR, _, LX_VERSION_ALTER, _, LX_CONFIG_VERSION_BUILD)), "_", LX_ARCH_VERSION_STRING, " by ", LX_COMPILER_VERSION_STRING)

/// the short version string
#define LX_VERSION_SHORT_STRING     __lx_mstrcat__("tbox_", __lx_mstring_ex__(__lx_mconcat8_ex__(v, LX_VERSION_MAJOR, _, LX_VERSION_MINOR, _, LX_VERSION_ALTER, _, LX_CONFIG_VERSION_BUILD)))

#endif


