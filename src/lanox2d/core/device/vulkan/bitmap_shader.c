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
 * @file        bitmap_shader.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "bitmap_shader.h"
#include "vk.h"
#include "sampler.h"
#include "image.h"
#include "../../shader.h"
#include "../../quality.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_void_t lx_bitmap_shader_devdata_free(lx_pointer_t devdata) {
    lx_bitmap_shader_devdata_t* bitmap_devdata = (lx_bitmap_shader_devdata_t*)devdata;
    if (bitmap_devdata) {
        lx_assert(bitmap_devdata->device);
        if (bitmap_devdata->image) {
            lx_vk_image_exit(bitmap_devdata->image);
            bitmap_devdata->image = lx_null;
        }
        if (bitmap_devdata->sampler) {
            lx_vk_sampler_exit(bitmap_devdata->sampler);
            bitmap_devdata->sampler = lx_null;
        }
        lx_free(bitmap_devdata);
    }
}

static lx_bitmap_shader_devdata_t* lx_bitmap_shader_init_devdata(lx_vulkan_device_t* device, lx_bitmap_shader_t* shader) {
    lx_assert(device && shader);

    // get bitmap
    lx_bitmap_ref_t bitmap = shader->bitmap;
    lx_assert(bitmap);

    lx_bool_t ok = lx_false;
    lx_bitmap_shader_devdata_t* devdata = lx_null;
    do {

        // init bitmap shader devdata
        devdata = lx_malloc0_type(lx_bitmap_shader_devdata_t);
        lx_assert_and_check_return_val(devdata, lx_null);

        devdata->device = device->device;

        // create sampler
        static VkSamplerAddressMode address_modes[] = {
            VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
            VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
            VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            VK_SAMPLER_ADDRESS_MODE_REPEAT,
            VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT
        };
        lx_size_t tile_mode = lx_shader_tile_mode((lx_shader_ref_t)shader);
        lx_assert_and_check_break(tile_mode < lx_arrayn(address_modes));
        VkFilter filter = lx_quality() > LX_QUALITY_LOW? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
        if (!(lx_paint_flags(device->base.paint) & LX_PAINT_FLAG_FILTER_BITMAP)) {
            filter = VK_FILTER_NEAREST;
        }
        devdata->sampler = lx_vk_sampler_init(device, filter, address_modes[tile_mode], address_modes[tile_mode]);
        lx_assert_and_check_break(devdata->sampler);

        // create image
        const VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
        lx_size_t width = lx_bitmap_width(bitmap);
        lx_size_t height = lx_bitmap_height(bitmap);
        devdata->image = lx_vk_image_init(device, format, width, height,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
        lx_assert_and_check_break(devdata->image);

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
        lx_float_t sw = (lx_float_t)lx_bitmap_width(bitmap);
        lx_float_t sh = (lx_float_t)lx_bitmap_height(bitmap);
        devdata->matrix = shader->base.matrix;
        if (lx_matrix_invert(&devdata->matrix)) {
            devdata->matrix.tx /= sw;
            devdata->matrix.ty /= sh;
        }

        ok = lx_true;
    } while (0);

    if (!ok && devdata) {
        lx_bitmap_shader_devdata_free(devdata);
        devdata = lx_null;
    }
    return devdata;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bitmap_shader_devdata_t* lx_bitmap_shader_devdata(lx_vulkan_device_t* device, lx_bitmap_shader_t* shader) {
    lx_bitmap_shader_devdata_t* devdata = (lx_bitmap_shader_devdata_t*)shader->base.devdata;
    if (!devdata) {
        devdata = lx_bitmap_shader_init_devdata(device, shader);
        if (devdata) {
            shader->base.devdata_free = lx_bitmap_shader_devdata_free;
            shader->base.devdata = devdata;
        }
    }
    return devdata;
}
