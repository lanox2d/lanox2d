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
 * @file        prefix.h
 *
 */
#ifndef LX_BASE_STREAM_PREFIX_H
#define LX_BASE_STREAM_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the stream type
typedef struct lx_stream_t_ {
    lx_long_t (*peek)(struct lx_stream_t_* stream, lx_byte_t const** pdata, lx_size_t size);
    lx_bool_t (*seek)(struct lx_stream_t_* stream, lx_size_t offset);
    lx_bool_t (*skip)(struct lx_stream_t_* stream, lx_size_t size);
    lx_bool_t (*write)(struct lx_stream_t_* stream, lx_byte_t const* data, lx_size_t size);
    lx_bool_t (*pwrite)(struct lx_stream_t_* stream, lx_byte_t const* data, lx_size_t size, lx_size_t offset);
    lx_bool_t (*flush)(struct lx_stream_t_* stream);
    lx_size_t (*size)(struct lx_stream_t_* stream);
    lx_size_t (*offset)(struct lx_stream_t_* stream);
    lx_void_t (*exit)(struct lx_stream_t_* stream);
}lx_stream_t;

#endif
