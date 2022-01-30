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
#include "../../shader.h"

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
        lx_free(bitmap_devdata);
    }
}

static lx_bool_t lx_bitmap_shader_load_texture(lx_vulkan_device_t* device, lx_bitmap_shader_devdata_t* devdata,
    lx_bitmap_ref_t bitmap, VkImageUsageFlags usage, VkFlags required_props) {
    lx_bool_t ok = lx_false;
    do {
        // check for linear supportability
        VkFormatProperties props;
        lx_bool_t need_blit = lx_true;
        const VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
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

#if 0
            // TODO
            for (int32_t y = 0; y < imgHeight; y++) {
                unsigned char* row = (unsigned char*)((char*)data + layout.rowPitch * y);
                for (int32_t x = 0; x < imgWidth; x++) {
                    row[x * 4] = imageData[(x + y * imgWidth) * 4];
                    row[x * 4 + 1] = imageData[(x + y * imgWidth) * 4 + 1];
                    row[x * 4 + 2] = imageData[(x + y * imgWidth) * 4 + 2];
                    row[x * 4 + 3] = imageData[(x + y * imgWidth) * 4 + 3];
                }
            }
#endif
            vkUnmapMemory(device->device, devdata->imagemem);
        }

        ok = lx_true;
    } while (0);

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
        if (!lx_bitmap_shader_load_texture(device, devdata, bitmap,
                VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
            break;
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
