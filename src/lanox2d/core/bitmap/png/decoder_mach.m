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
 * @file        decoder_mach.m
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
//#define LX_TRACE_DISABLED
#import "decoder.h"
#import "../../quality.h"
#import "../../pixmap.h"
#ifdef LX_CONFIG_OS_MACOSX
#   import <AppKit/AppKit.h>
#else
#   import <UIKit/UIKit.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_bool_t lx_bitmap_png_probe(lx_stream_ref_t stream) {
    lx_byte_t const* p = lx_null;
    if (lx_stream_peek(stream, &p, 8) && p) {
        return (    p[0] == 0x89
                &&  p[1] == 'P'
                &&  p[2] == 'N'
                &&  p[3] == 'G'
                &&  p[4] == 0x0d
                &&  p[5] == 0x0a
                &&  p[6] == 0x1a
                &&  p[7] == 0x0a);
    }
    return lx_false;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bitmap_ref_t lx_bitmap_png_decode(lx_size_t pixfmt, lx_stream_ref_t stream) {
    lx_assert(stream);

    // decode image
    lx_bool_t        ok = lx_false;
    lx_bitmap_ref_t  bitmap = lx_null;
    CFDataRef        pixelData = lx_null;
    CGImageSourceRef imageSource = lx_null;
    CGImageRef       image = lx_null;
    do {

        // probe format first
        if (!lx_bitmap_png_probe(stream)) {
            break;
        }

        // get stream data
        lx_size_t size = lx_stream_size(stream);
        lx_byte_t const* data = lx_null;
        if (!lx_stream_peek(stream, &data, size) || !data) {
            break;
        }

        // decode image
        NSData* imageData = [NSData dataWithBytes:data length:size];
        CFDataRef imageDataRef = (__bridge CFDataRef)imageData;
        imageSource = CGImageSourceCreateWithData(imageDataRef, nil);
        image = CGImageSourceCreateImageAtIndex(imageSource, 0, nil);
        lx_assert_and_check_break(image);

        // get image information
        lx_size_t width    = (lx_size_t)CGImageGetWidth(image);
        lx_size_t height   = (lx_size_t)CGImageGetHeight(image);
        lx_size_t bpp      = (lx_size_t)CGImageGetBitsPerPixel(image);
        lx_size_t rowbytes = (lx_size_t)CGImageGetBytesPerRow(image);
        lx_trace_d("size: %ux%u, bpp: %u", width, height, bpp);

        // get image data
        pixelData = CGDataProviderCopyData(CGImageGetDataProvider(image));
        lx_cpointer_t pixelDataPtr = CFDataGetBytePtr(pixelData);
        lx_size_t pixelDataSize = CFDataGetLength(pixelData);
        lx_assert_and_check_break(pixelDataPtr && pixelDataSize);

        // init bitmap, default: no alpha
        bitmap = lx_bitmap_init(lx_null, pixfmt, width, height, rowbytes, lx_false);
        lx_assert_and_check_break(bitmap);

        // the bitmap data
        lx_byte_t* bitmap_data = (lx_byte_t*)lx_bitmap_data(bitmap);
        lx_size_t  bitmap_size = lx_bitmap_size(bitmap);
        lx_assert_and_check_break(bitmap_data && bitmap_size == pixelDataSize);

        // copy image data
        lx_memcpy(bitmap_data, pixelDataPtr, pixelDataSize);

        // ok
        ok = lx_true;

    } while (0);

    // free image
    if (image) {
        CFRelease(image);
        image = lx_null;
    }

    // free image source
    if (imageSource) {
        CFRelease(imageSource);
        imageSource = lx_null;
    }

    // free pixel data
    if (pixelData) {
        CFRelease(pixelData);
        pixelData = lx_null;
    }

    // decode failed
    if (!ok) {
        if (bitmap) {
            lx_bitmap_exit(bitmap);
            bitmap = lx_null;
        }
    }
    return bitmap;
}

