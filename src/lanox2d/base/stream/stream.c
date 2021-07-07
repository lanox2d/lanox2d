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
 * @file        stream.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "stream.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_void_t lx_stream_exit(lx_stream_ref_t self) {
    lx_stream_t* stream = (lx_stream_t*)self;
    if (stream) {
        if (stream->exit) {
            stream->exit(stream);
        }
        lx_free(stream);
    }
}

lx_long_t lx_stream_peek(lx_stream_ref_t self, lx_byte_t const** pdata, lx_size_t size) {
    lx_stream_t* stream = (lx_stream_t*)self;
    lx_assert_and_check_return_val(stream && stream->peek && pdata && size, -1);

    return stream->peek(stream, pdata, size);
}

lx_uint8_t lx_stream_peek_u1(lx_stream_ref_t stream, lx_size_t offset) {
    lx_byte_t const* data = lx_null;
    lx_long_t size = lx_stream_peek(stream, &data, offset + 1);
    return (size == offset + 1 && data)? (lx_uint8_t)data[offset] : 0;
}

lx_uint16_t lx_stream_peek_u2be(lx_stream_ref_t stream, lx_size_t offset) {
    lx_byte_t const* data = lx_null;
    lx_long_t size = lx_stream_peek(stream, &data, offset + 2);
    return (size == offset + 2 && data)? (((lx_uint16_t)data[offset] << 8) + data[offset + 1]) : 0;
}

lx_uint32_t lx_stream_peek_u4be(lx_stream_ref_t stream, lx_size_t offset) {
    lx_byte_t const* data = lx_null;
    lx_long_t size = lx_stream_peek(stream, &data, offset + 4);
    return (size == offset + 4 && data)? (((lx_uint32_t)data[offset] << 24) + (data[offset + 1] << 16) + (data[offset + 2] << 8) + data[offset + 3]) : 0;
}

lx_bool_t lx_stream_skip(lx_stream_ref_t self, lx_size_t size) {
    lx_stream_t* stream = (lx_stream_t*)self;
    lx_assert_and_check_return_val(stream && stream->skip, lx_false);

    return stream->skip(stream, size);
}

lx_bool_t lx_stream_write(lx_stream_ref_t self, lx_byte_t const* data, lx_size_t size) {
    lx_stream_t* stream = (lx_stream_t*)self;
    lx_assert_and_check_return_val(stream && stream->write && data && size, lx_false);

    return stream->write(stream, data, size);
}

lx_bool_t lx_stream_write_u1(lx_stream_ref_t stream, lx_uint8_t value) {
    return lx_stream_write(stream, (lx_byte_t const*)&value, 1);
}

lx_bool_t lx_stream_write_u4(lx_stream_ref_t stream, lx_uint32_t value) {
    lx_byte_t data[4];
    data[0] = (lx_byte_t)(value >> 24);
    data[1] = (lx_byte_t)(value >> 16);
    data[2] = (lx_byte_t)(value >> 8);
    data[3] = (lx_byte_t)value;
    return lx_stream_write(stream, data, sizeof(data));
}

lx_bool_t lx_stream_pwrite(lx_stream_ref_t self, lx_byte_t const* data, lx_size_t size, lx_size_t offset) {
    lx_stream_t* stream = (lx_stream_t*)self;
    lx_assert_and_check_return_val(stream && stream->write && data && size, lx_false);

    return stream->pwrite(stream, data, size, offset);
}

lx_bool_t lx_stream_pwrite_u1(lx_stream_ref_t stream, lx_uint8_t value, lx_size_t offset) {
    return lx_stream_pwrite(stream, (lx_byte_t const*)&value, 1, offset);
}

lx_bool_t lx_stream_pwrite_u4(lx_stream_ref_t stream, lx_uint32_t value, lx_size_t offset) {
    lx_byte_t data[4];
    data[0] = (lx_byte_t)(value >> 24);
    data[1] = (lx_byte_t)(value >> 16);
    data[2] = (lx_byte_t)(value >> 8);
    data[3] = (lx_byte_t)value;
    return lx_stream_pwrite(stream, data, sizeof(data), offset);
}

lx_bool_t lx_stream_flush(lx_stream_ref_t self) {
    lx_stream_t* stream = (lx_stream_t*)self;
    lx_assert_and_check_return_val(stream && stream->flush, lx_false);

    return stream->flush(stream);
}

lx_size_t lx_stream_size(lx_stream_ref_t self) {
    lx_stream_t* stream = (lx_stream_t*)self;
    lx_assert_and_check_return_val(stream && stream->size, 0);

    return stream->size(stream);
}

lx_size_t lx_stream_offset(lx_stream_ref_t self) {
    lx_stream_t* stream = (lx_stream_t*)self;
    lx_assert_and_check_return_val(stream && stream->offset, 0);

    return stream->offset(stream);
}

lx_size_t lx_stream_left(lx_stream_ref_t self) {
    lx_size_t size = lx_stream_size(self);
    lx_size_t offset = lx_stream_offset(self);
    return size > offset? size - offset : 0;
}

lx_bool_t lx_stream_save(lx_stream_ref_t stream, lx_stream_ref_t reader, lx_size_t size) {
    lx_size_t save = 0;
    lx_byte_t const* data = lx_null;
    while (lx_stream_left(reader) && save < size) {
        lx_size_t peeksize = size - save;
        if (peeksize > LX_STREAM_BUFFER_MAXN) peeksize = LX_STREAM_BUFFER_MAXN;
        if (peeksize != lx_stream_peek(reader, &data, peeksize) || !data) return lx_false;
        if (!lx_stream_write(stream, data, peeksize)) return lx_false;
        if (!lx_stream_skip(reader, peeksize)) return lx_false;
        save += peeksize;
    }
    return lx_true;
}

