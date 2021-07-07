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
 * @file        stream.h
 *
 */
#ifndef LX_BASE_STREAM_H
#define LX_BASE_STREAM_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#define LX_STREAM_BUFFER_MAXN   (8192)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */
typedef lx_typeref(stream);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init data stream
 *
 * @return              the stream
 */
lx_stream_ref_t         lx_stream_init_data(lx_void_t);

/*! init file stream
 *
 * @param path          the file path
 * @param mode          the file mode, e.g. "r", "w"
 *
 * @return              the stream
 */
lx_stream_ref_t         lx_stream_init_file(lx_char_t const* path, lx_char_t const* mode);

/* exit stream
 *
 * @param stream        the stream
 */
lx_void_t               lx_stream_exit(lx_stream_ref_t stream);

/* peek stream data
 *
 * @endcode
 *
 * @param stream        the stream
 * @param pdata         the data pointer
 * @param size          the size
 *
 * @return              the real data size or -1
 */
lx_long_t               lx_stream_peek(lx_stream_ref_t stream, lx_byte_t const** pdata, lx_size_t size);

/* peek stream value (u1)
 *
 * @endcode
 *
 * @param stream        the stream
 * @param offset        the offset
 *
 * @return              the value
 */
lx_uint8_t              lx_stream_peek_u1(lx_stream_ref_t stream, lx_size_t offset);

/* peek stream value (u2/be)
 *
 * @endcode
 *
 * @param stream        the stream
 * @param offset        the offset
 *
 * @return              the value
 */
lx_uint16_t             lx_stream_peek_u2be(lx_stream_ref_t stream, lx_size_t offset);

/* peek stream value (u4/be)
 *
 * @endcode
 *
 * @param stream        the stream
 * @param offset        the offset
 *
 * @return              the value
 */
lx_uint32_t             lx_stream_peek_u4be(lx_stream_ref_t stream, lx_size_t offset);

/* skip stream data size
 *
 * @endcode
 *
 * @param stream        the stream
 * @param size          the size
 *
 * @return              lx_true or lx_false
 */
lx_bool_t               lx_stream_skip(lx_stream_ref_t stream, lx_size_t size);

/*! write stream data
 *
 * @param stream        the stream
 * @param data          the data
 * @param size          the size
 *
 * @return              lx_true or lx_false
 */
lx_bool_t               lx_stream_write(lx_stream_ref_t stream, lx_byte_t const* data, lx_size_t size);

/*! write stream value (u1)
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              lx_true or lx_false
 */
lx_bool_t               lx_stream_write_u1(lx_stream_ref_t stream, lx_uint8_t value);

/*! write stream value (u4/be)
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              lx_true or lx_false
 */
lx_bool_t               lx_stream_write_u4(lx_stream_ref_t stream, lx_uint32_t value);

/*! write stream data to the given position
 *
 * @param stream        the stream
 * @param data          the data
 * @param size          the size
 * @param offset        the offset
 *
 * @return              lx_true or lx_false
 */
lx_bool_t               lx_stream_pwrite(lx_stream_ref_t stream, lx_byte_t const* data, lx_size_t size, lx_size_t offset);

/*! write stream value (u1) to given position
 *
 * @param stream        the stream
 * @param value         the value
 * @param offset        the offset
 *
 * @return              lx_true or lx_false
 */
lx_bool_t               lx_stream_pwrite_u1(lx_stream_ref_t stream, lx_uint8_t value, lx_size_t offset);

/*! write stream value (u4/be) to given position
 *
 * @param stream        the stream
 * @param value         the value
 * @param offset        the offset
 *
 * @return              lx_true or lx_false
 */
lx_bool_t               lx_stream_pwrite_u4(lx_stream_ref_t stream, lx_uint32_t value, lx_size_t offset);

/*! flush stream data
 *
 * @param stream        the stream
 *
 * @return              lx_true or lx_false
 */
lx_bool_t               lx_stream_flush(lx_stream_ref_t stream);

/*! get stream size
 *
 * @param stream        the stream
 *
 * @return              the data size
 */
lx_size_t               lx_stream_size(lx_stream_ref_t stream);

/*! get stream offset
 *
 * @param stream        the stream
 *
 * @return              the data offset
 */
lx_size_t               lx_stream_offset(lx_stream_ref_t stream);

/*! get left stream data size, only for reader
 *
 * @param stream        the stream
 *
 * @return              the data size
 */
lx_size_t               lx_stream_left(lx_stream_ref_t stream);

/*! save stream data from the given stream
 *
 * @param stream        the stream
 * @param reader        the reader
 * @param size          the size
 *
 * @return              lx_true or lx_false
 */
lx_bool_t               lx_stream_save(lx_stream_ref_t stream, lx_stream_ref_t reader, lx_size_t size);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif
