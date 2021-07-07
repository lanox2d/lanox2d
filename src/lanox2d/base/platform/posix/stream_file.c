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
 * @file        stream_file.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../libc/libc.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the file stream type
typedef struct __lx_stream_file_t {
    lx_stream_t         base;
    lx_int_t            file;
    lx_size_t           offset;
    lx_bool_t           readable;

    // only for read mode
    lx_size_t           filesize;
    lx_byte_t const*    filebuff;

    // only for write mode
    lx_byte_t           buff[LX_STREAM_BUFFER_MAXN];
    lx_size_t           buffsize;
}lx_stream_file_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

static lx_bool_t lx_stream_file_flush(lx_stream_t* self) {
    lx_stream_file_t* stream = (lx_stream_file_t*)self;
    lx_assert_and_check_return_val(stream && stream->file, lx_false);

    // only for write mode
    lx_assert_and_check_return_val(!stream->readable, lx_false);

    // flush buffer data to file
    if (stream->buffsize) {
        if (stream->buffsize != write(stream->file, stream->buff, stream->buffsize)) {
            return lx_false;
        }
        stream->buffsize = 0;
    }
    return fsync(stream->file) == 0;
}

static lx_void_t lx_stream_file_exit(lx_stream_t* self) {
    lx_stream_file_t* stream = (lx_stream_file_t*)self;
    lx_assert_and_check_return(stream);

    // flush file first
    if (!stream->readable) {
        lx_stream_file_flush(self);
    }

    // unmap file buffer
    if (stream->filebuff) {
        munmap((lx_pointer_t)stream->filebuff, stream->filesize);
        stream->filebuff = lx_null;
    }

    // close file
    if (stream->file) {
        close(stream->file);
        stream->file = 0;
    }

    // clear status
    stream->offset   = 0;
    stream->filesize = 0;
    stream->buffsize = 0;
    stream->readable = lx_false;
}

static lx_long_t lx_stream_file_peek(lx_stream_t* self, lx_byte_t const** pdata, lx_size_t size) {
    lx_stream_file_t* stream = (lx_stream_file_t*)self;
    lx_assert_and_check_return_val(stream && stream->filebuff && pdata && size, -1);

    // only for read mode
    lx_assert_and_check_return_val(stream->readable, -1);

    // peek data to buffer
    lx_size_t leftsize = stream->filesize - stream->offset;
    *pdata = stream->filebuff + stream->offset;
    return size < leftsize? size : leftsize;
}

static lx_bool_t lx_stream_file_seek(lx_stream_t* self, lx_size_t offset) {
    lx_stream_file_t* stream = (lx_stream_file_t*)self;
    lx_assert_and_check_return_val(stream && stream->file, lx_false);

    // only for read mode
    lx_assert_and_check_return_val(stream->readable, lx_false);

    // update offset
    if (offset <= stream->filesize) {
        stream->offset = offset;
        return lx_true;
    }
    return lx_false;
}

static lx_bool_t lx_stream_file_skip(lx_stream_t* self, lx_size_t size) {
    lx_stream_file_t* stream = (lx_stream_file_t*)self;
    lx_assert_and_check_return_val(stream && stream->file, lx_false);

    // only for read mode
    lx_assert_and_check_return_val(stream->readable, lx_false);

    // update offset
    stream->offset += size;
    if (stream->offset > stream->filesize) {
        stream->offset = stream->filesize;
    }
    return lx_true;
}

static lx_size_t lx_stream_file_size(lx_stream_t* self) {
    lx_stream_file_t* stream = (lx_stream_file_t*)self;
    lx_assert_and_check_return_val(stream && stream->file, 0);

    return stream->filesize;
}

static lx_size_t lx_stream_file_offset(lx_stream_t* self) {
    lx_stream_file_t* stream = (lx_stream_file_t*)self;
    lx_assert_and_check_return_val(stream && stream->file, 0);

    return stream->offset;
}

static lx_bool_t lx_stream_file_write(lx_stream_t* self, lx_byte_t const* data, lx_size_t size) {
    lx_stream_file_t* stream = (lx_stream_file_t*)self;
    lx_assert_and_check_return_val(stream && stream->file && data, lx_false);

    // only for write mode
    lx_assert_and_check_return_val(!stream->readable, lx_false);

    // we need flush it if buffer is full
    if (stream->buffsize && stream->buffsize + size > sizeof(stream->buff)) {
        if (stream->buffsize != write(stream->file, stream->buff, stream->buffsize)) {
            return lx_false;
        }
        stream->buffsize = 0;
    }

    // write data to buffer
    if (stream->buffsize + size <= sizeof(stream->buff)) {
        lx_memcpy(stream->buff + stream->buffsize, data, size);
        stream->buffsize += size;
    } else {
        // write data directly if too large
        lx_assert_and_check_return_val(!stream->buffsize, lx_false);
        if (size != write(stream->file, data, size)) {
            return lx_false;
        }
    }

    // update offset
    stream->offset += size;
    return lx_true;
}

static lx_bool_t lx_stream_file_pwrite(lx_stream_t* self, lx_byte_t const* data, lx_size_t size, lx_size_t offset) {
    lx_stream_file_t* stream = (lx_stream_file_t*)self;
    lx_assert_and_check_return_val(stream && stream->file && data, lx_false);

    // only for write mode
    lx_assert_and_check_return_val(!stream->readable, lx_false);

    // write to the given position without buffer directly
    return size == pwrite(stream->file, data, size, offset);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_stream_ref_t lx_stream_init_file(lx_char_t const* path, lx_char_t const* mode) {
    lx_assert_and_check_return_val(path && mode, lx_null);

    // do init
    lx_bool_t           ok = lx_false;
    lx_stream_file_t*   stream = lx_null;
    do {
        stream = (lx_stream_file_t*)lx_malloc0_type(lx_stream_file_t);
        lx_assert_and_check_break(stream);

        stream->base.exit   = lx_stream_file_exit;
        stream->base.peek   = lx_stream_file_peek;
        stream->base.seek   = lx_stream_file_seek;
        stream->base.skip   = lx_stream_file_skip;
        stream->base.size   = lx_stream_file_size;
        stream->base.offset = lx_stream_file_offset;
        stream->base.write  = lx_stream_file_write;
        stream->base.pwrite = lx_stream_file_pwrite;
        stream->base.flush  = lx_stream_file_flush;

        // open file
        stream->readable = mode[0] == 'r';
        stream->file     = open(path, stream->readable? O_RDONLY : (O_WRONLY | O_CREAT | O_TRUNC), stream->readable? 0 : (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));
        lx_assert_and_check_break(stream->file > 0);

        // init file for read mode
        if (stream->readable) {
            struct stat st = {0};
            if (!fstat(stream->file, &st)) {
                stream->filesize = st.st_size;
            } else {
                break;
            }

            // map file
            stream->filebuff = (lx_byte_t const*)mmap(lx_null, stream->filesize, PROT_READ, MAP_SHARED, stream->file, 0);
            lx_assert_and_check_break(stream->filebuff);
        }

        ok = lx_true;

    } while (0);

    if (!ok && stream) {
        lx_stream_exit((lx_stream_ref_t)stream);
        stream = lx_null;
    }
    return (lx_stream_ref_t)stream;
}

