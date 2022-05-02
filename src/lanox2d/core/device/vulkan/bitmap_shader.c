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
#include "image_view.h"
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
            vkDestroyImage(bitmap_devdata->device, bitmap_devdata->image, lx_null);
            bitmap_devdata->image = lx_null;
        }
        if (bitmap_devdata->imagemem) {
            vkFreeMemory(bitmap_devdata->device, bitmap_devdata->imagemem, lx_null);
            bitmap_devdata->imagemem = lx_null;
        }
        if (bitmap_devdata->sampler) {
            lx_vk_sampler_exit(bitmap_devdata->sampler);
            bitmap_devdata->sampler = lx_null;
        }
        if (bitmap_devdata->imageview) {
            lx_vk_image_view_exit(bitmap_devdata->imageview);
            bitmap_devdata->imageview = lx_null;
        }
        lx_free(bitmap_devdata);
    }
}

static lx_bool_t lx_bitmap_shader_load_texture(lx_vulkan_device_t* device, lx_bitmap_shader_devdata_t* devdata,
    lx_bitmap_ref_t bitmap, VkFormat format, VkImageUsageFlags usage, VkFlags required_props) {
    lx_bool_t ok = lx_false;
    VkFence fence = VK_NULL_HANDLE;
    VkImage stage_image = VK_NULL_HANDLE;
    VkDeviceMemory stage_imagemem = VK_NULL_HANDLE;
    VkCommandPool cmd_pool = VK_NULL_HANDLE;
    VkCommandBuffer cmd = VK_NULL_HANDLE;
    do {
        // check for linear supportability
        VkFormatProperties props;
        lx_bool_t need_blit = lx_true;
        vkGetPhysicalDeviceFormatProperties(device->gpu_device, format, &props);
        lx_assert((props.linearTilingFeatures | props.optimalTilingFeatures) & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
        if (props.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) {
            // linear format supporting the required texture
            need_blit = lx_false;
        }

        // create image
        VkImageCreateInfo image_create_info = {};
        image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_create_info.pNext = lx_null;
        image_create_info.flags = 0;
        image_create_info.imageType = VK_IMAGE_TYPE_2D;
        image_create_info.format = format,
        image_create_info.extent.width = lx_bitmap_width(bitmap);
        image_create_info.extent.height = lx_bitmap_height(bitmap);
        image_create_info.extent.depth = 1;
        image_create_info.mipLevels = 1;
        image_create_info.arrayLayers = 1;
        image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_create_info.tiling = VK_IMAGE_TILING_LINEAR;
        image_create_info.usage = (need_blit ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : VK_IMAGE_USAGE_SAMPLED_BIT),
            image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_create_info.queueFamilyIndexCount = 1;
        image_create_info.pQueueFamilyIndices = &device->gpu_familyidx;
        image_create_info.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
        if (vkCreateImage(device->device, &image_create_info, lx_null, &devdata->image) != VK_SUCCESS) {
            break;
        }

        // allocate image memory
        VkMemoryRequirements mem_reqs;
        vkGetImageMemoryRequirements(device->device, devdata->image, &mem_reqs);

        VkMemoryAllocateInfo mem_alloc = {};
        mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        mem_alloc.pNext = lx_null;
        mem_alloc.memoryTypeIndex = 0;
        mem_alloc.allocationSize = mem_reqs.size;
        if (!lx_vk_allocate_memory_type_from_properties(device->gpu_memory_properties, mem_reqs.memoryTypeBits,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &mem_alloc.memoryTypeIndex)) {
            break;
        }
        if (vkAllocateMemory(device->device, &mem_alloc, lx_null, &devdata->imagemem) != VK_SUCCESS) {
            break;
        }
        if (vkBindImageMemory(device->device, devdata->image, devdata->imagemem, 0) != VK_SUCCESS) {
            break;
        }

        // fill bitmap data to image memory
        if (required_props & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
            VkImageSubresource subres = {};
            subres.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            subres.mipLevel = 0;
            subres.arrayLayer = 0;
            VkSubresourceLayout layout;
            vkGetImageSubresourceLayout(device->device, devdata->image, &subres, &layout);

            lx_pointer_t data = lx_null;
            if (vkMapMemory(device->device, devdata->imagemem, 0, mem_alloc.allocationSize, 0, &data) != VK_SUCCESS) {
                break;
            }

            // get bitmap data
            lx_size_t width = lx_bitmap_width(bitmap);
            lx_size_t height = lx_bitmap_height(bitmap);
            lx_size_t row_bytes = lx_bitmap_row_bytes(bitmap);
            lx_byte_t const* bitmap_data = lx_bitmap_data(bitmap);
            lx_assert_and_check_break(bitmap_data && width && height);

            // get pixmap
            lx_pixmap_ref_t sp = lx_pixmap(lx_bitmap_pixfmt(bitmap), 0xff);
            lx_pixmap_ref_t dp = lx_pixmap(LX_PIXFMT_RGBA8888, 0xff);
            lx_assert_and_check_break(sp && dp && format == VK_FORMAT_R8G8B8A8_UNORM);

            lx_size_t  j;
            lx_size_t  b = dp->btp;
            lx_size_t  n = layout.rowPitch;
            lx_byte_t* p = data;
            for (j = 0; j < height; j++) {
                lx_size_t  i = 0;
                lx_byte_t* d = p;
                lx_byte_t* e = p + n;
                if (dp == sp) {
                    for (i = 0; i < row_bytes && d < e; i += 4, d += b) {
                        dp->pixel_copy(d, &bitmap_data[i], 0xff);
                    }
                } else {
                    for (i = 0; i < row_bytes && d < e; i += 4, d += b) {
                        dp->color_set(d, sp->color_get(&bitmap_data[i]));
                    }
                }
                p += n;
                bitmap_data += row_bytes;
            }
            vkUnmapMemory(device->device, devdata->imagemem);
        }

        // set image layout to command pool
        VkCommandPoolCreateInfo cmd_poolinfo = {};
        cmd_poolinfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmd_poolinfo.pNext = lx_null;
        cmd_poolinfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        cmd_poolinfo.queueFamilyIndex = device->gpu_familyidx;
        if (vkCreateCommandPool(device->device, &cmd_poolinfo, lx_null, &cmd_pool) != VK_SUCCESS) {
            break;
        }

        VkCommandBufferAllocateInfo cmdinfo = {};
        cmdinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdinfo.pNext = lx_null;
        cmdinfo.commandPool = cmd_pool;
        cmdinfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmdinfo.commandBufferCount = 1;
        if (vkAllocateCommandBuffers(device->device, &cmdinfo, &cmd) != VK_SUCCESS) {
            break;
        }

        VkCommandBufferBeginInfo cmd_bufinfo = {};
        cmd_bufinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmd_bufinfo.pNext = lx_null;
        cmd_bufinfo.flags = 0;
        cmd_bufinfo.pInheritanceInfo = lx_null;
        if (vkBeginCommandBuffer(cmd, &cmd_bufinfo) != VK_SUCCESS) {
            break;
        }

        // if linear is supported, we are done
        if (!need_blit) {
            lx_vk_set_image_layout(cmd, devdata->image, VK_IMAGE_LAYOUT_PREINITIALIZED,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                VK_PIPELINE_STAGE_HOST_BIT,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
        } else {
            // save current image and mem as staging image and memory
            stage_image = devdata->image;
            stage_imagemem = devdata->imagemem;
            devdata->image = VK_NULL_HANDLE;
            devdata->imagemem = VK_NULL_HANDLE;

            // create a tile texture to blit into
            image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
            image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            if (vkCreateImage(device->device, &image_create_info, lx_null, &devdata->image) != VK_SUCCESS) {
                break;
            }
            vkGetImageMemoryRequirements(device->device, devdata->image, &mem_reqs);

            mem_alloc.allocationSize = mem_reqs.size;
            if (!lx_vk_allocate_memory_type_from_properties(device->gpu_memory_properties, mem_reqs.memoryTypeBits,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mem_alloc.memoryTypeIndex)) {
                break;
            }
            if (vkAllocateMemory(device->device, &mem_alloc, lx_null, &devdata->imagemem) != VK_SUCCESS) {
                break;
            }
            if (vkBindImageMemory(device->device, devdata->image, devdata->imagemem, 0) != VK_SUCCESS) {
                break;
            }

            // transitions image out of UNDEFINED type
            lx_vk_set_image_layout(cmd, stage_image, VK_IMAGE_LAYOUT_PREINITIALIZED,
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
            lx_vk_set_image_layout(cmd, devdata->image, VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

            VkImageCopy bltinfo = {};
            bltinfo.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            bltinfo.srcSubresource.mipLevel = 0;
            bltinfo.srcSubresource.baseArrayLayer = 0;
            bltinfo.srcSubresource.layerCount = 1;
            bltinfo.srcOffset.x = 0;
            bltinfo.srcOffset.y = 0;
            bltinfo.srcOffset.z = 0;
            bltinfo.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            bltinfo.dstSubresource.mipLevel = 0;
            bltinfo.dstSubresource.baseArrayLayer = 0;
            bltinfo.dstSubresource.layerCount = 1;
            bltinfo.dstOffset.x = 0;
            bltinfo.dstOffset.y = 0;
            bltinfo.dstOffset.z = 0;
            bltinfo.extent.width = lx_bitmap_width(bitmap);
            bltinfo.extent.height = lx_bitmap_height(bitmap);
            bltinfo.extent.depth = 1;
            vkCmdCopyImage(cmd, stage_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                devdata->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bltinfo);

            lx_vk_set_image_layout(cmd, devdata->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
        }

        if (vkEndCommandBuffer(cmd) != VK_SUCCESS) {
            break;
        }

        // submit the image layout command
        VkFenceCreateInfo fence_info = {};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.pNext = lx_null;
        fence_info.flags = 0;
        if (vkCreateFence(device->device, &fence_info, lx_null, &fence) != VK_SUCCESS) {
            break;
        }

        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pNext = lx_null;
        submit_info.waitSemaphoreCount = 0;
        submit_info.pWaitSemaphores = lx_null;
        submit_info.pWaitDstStageMask = lx_null;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &cmd;
        submit_info.signalSemaphoreCount = 0;
        submit_info.pSignalSemaphores = lx_null;
        if (vkQueueSubmit(device->queue, 1, &submit_info, fence) != VK_SUCCESS) {
            break;
        }
        if (vkWaitForFences(device->device, 1, &fence, VK_TRUE, 100000000) != VK_SUCCESS) {
            break;
        }

        ok = lx_true;
    } while (0);

    if (fence != VK_NULL_HANDLE) {
        vkDestroyFence(device->device, fence, lx_null);
        fence = VK_NULL_HANDLE;
    }
    if (cmd_pool != VK_NULL_HANDLE) {
        if (cmd != VK_NULL_HANDLE) {
            vkFreeCommandBuffers(device->device, cmd_pool, 1, &cmd);
            cmd = VK_NULL_HANDLE;
        }
        vkDestroyCommandPool(device->device, cmd_pool, lx_null);
        cmd_pool = VK_NULL_HANDLE;
    }
    if (stage_image != VK_NULL_HANDLE) {
        vkDestroyImage(device->device, stage_image, lx_null);
        vkFreeMemory(device->device, stage_imagemem, lx_null);
    }

    return ok;
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

        // load texture from bitmap
        const VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
        if (!lx_bitmap_shader_load_texture(device, devdata, bitmap, format,
                VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
            break;
        }

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

        // create image view
        devdata->imageview = lx_vk_image_view_init(device, devdata->image, format);
        lx_assert_and_check_break(devdata->imageview);

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
