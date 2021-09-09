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
 * @file        BitmapShader.m
 *
 */

#import "BitmapShader.h"

#if __has_feature(objc_arc)
#   error this file need disable arc
#endif

@implementation BitmapShader

static lx_void_t lx_bitmap_shader_devdata_free(lx_shader_ref_t shader) {
    lx_assert(shader);
    BitmapShader* bitmapShader = (BitmapShader*)((lx_shader_t*)shader)->devdata;
    if (bitmapShader) {
        [bitmapShader release];
        ((lx_shader_t*)shader)->devdata = lx_null;
    }
}

- (void)dealloc {
    if (_texture) {
        [_texture release];
    }
    if (_sampler) {
        [_sampler release];
    }
    [super dealloc];
}

+ (BitmapShader*)bitmapShader:(lx_bitmap_shader_t*)shader withDevice:(id<MTLDevice>)device {
    BitmapShader* bitmapShader = (BitmapShader*)shader->base.devdata;
    if (bitmapShader == nil) {
        bitmapShader = [[BitmapShader alloc] initWithShader:shader withDevice:device];
        shader->base.devdata_free = lx_bitmap_shader_devdata_free;
        shader->base.devdata = bitmapShader;
    }
    return bitmapShader;
}

- (nonnull instancetype)initWithShader:(lx_bitmap_shader_t*)shader withDevice:(id<MTLDevice>)device {
    self = [super init];
    if (self) {

        // get bitmap
        lx_bitmap_ref_t bitmap = shader->bitmap;
        lx_assert(bitmap);

        // create texture sampler
        MTLSamplerDescriptor* samplerDescriptor = [MTLSamplerDescriptor new];
        samplerDescriptor.rAddressMode = MTLSamplerAddressModeRepeat;
        samplerDescriptor.sAddressMode = MTLSamplerAddressModeRepeat;
        samplerDescriptor.tAddressMode = MTLSamplerAddressModeRepeat;
        samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
        samplerDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
        samplerDescriptor.mipFilter = MTLSamplerMipFilterNotMipmapped;
        _sampler = [device newSamplerStateWithDescriptor:samplerDescriptor];
        [samplerDescriptor release];

        // create texture
        lx_size_t pixfmt = lx_bitmap_pixfmt(bitmap);
        lx_size_t width  = lx_bitmap_width(bitmap);
        lx_size_t height = lx_bitmap_height(bitmap);
        MTLTextureDescriptor* textureDescriptor = [[MTLTextureDescriptor alloc] init];
        switch (pixfmt) {
        case LX_PIXFMT_XRGB8888:
        case LX_PIXFMT_ARGB8888:
        case LX_PIXFMT_RGBX8888:
        case LX_PIXFMT_RGBA8888:
            // TODO
            textureDescriptor.pixelFormat = MTLPixelFormatBGRA8Unorm;
            break;
        default:
            textureDescriptor.pixelFormat = MTLPixelFormatBGRA8Unorm;
            lx_trace_e("unknown pixel format(%d)!", pixfmt);
            break;
        }
        textureDescriptor.width  = width;
        textureDescriptor.height = height;
        _texture = [device newTextureWithDescriptor:textureDescriptor];
        [textureDescriptor release];

        // init texture data
        MTLRegion    region = MTLRegionMake2D(0, 0, width, height);
        lx_pointer_t data = lx_bitmap_data(bitmap);
        lx_size_t    rowbytes = lx_bitmap_row_bytes(bitmap);
        [_texture replaceRegion:region mipmapLevel:0 withBytes:data bytesPerRow:(NSUInteger)rowbytes];

        /* convert world coordinate to camera coordinate
         *
         * before:
         *
         *
         *       bx        bounds of vertices
         *      -------V7---------------------V6------
         *  by |     /                          \     |
         *     |   /              |               \   |
         *     | /    bitmap  sw  |                 \ |
         *    V8          -----------------           V5
         *     |      sh |        |        |          |
         *     |         |        |        |          | bh
         *     |---------|--------O--------|----------|------> (bitmap matrix in world coordinate)
         *     |         |        |        |          |
         *     |         |        |        |          |
         *    V1          -----------------           V4
         *     | \                |                 / |
         *     |   \             \|/              /   |
         *     |     \                          /     |
         *      -------V2--------------------V3-------
         *                       bw
         *
         * after:
         *
         *       bx        bounds of vertices
         *      -------V7---------------------V6------
         *  by |     /                          \     |
         *     |   /              |               \   |
         *     | /    camera  sw  |                 \ |
         *    V8         O--------------------------- V5-----> (matrix in camera coordinate)
         *     |      sh |||||||| | ||||||||          |
         *     |         |||||||| | ||||||||          | bh
         *     |    -----|--------.--------|------    |
         *     |         |||||||| | ||||||||          |
         *     |         |||||||| | ||||||||          |
         *    V1         |-----------------           V4
         *     | \      \|/       |                 / |
         *     |   \              |               /   |
         *     |     \                          /     |
         *      -------V2--------------------V3-------
         *                       bw
         */
        lx_float_t sw = (lx_float_t)width;
        lx_float_t sh = (lx_float_t)height;
        _matrix = shader->base.matrix;
        if (lx_matrix_invert(&_matrix)) {
            _matrix.tx /= sw;
            _matrix.ty /= sh;
        }
    }
    return self;
}

@end
