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
 * @file        image.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "image.h"
#include "image_view.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the image type
typedef struct lx_vk_image_t {
    lx_vulkan_device_t*     device;
    VkImage                 image;
    VkDeviceMemory          memory;
    lx_size_t               memory_size;
    lx_vk_image_view_ref_t  texture_view;
    lx_vk_image_view_ref_t  framebuffer_view;
}lx_vk_image_t;

// the attachment usage flag
typedef enum lx_vk_image_attachment_usage_flags_e_ {
    LX_VK_IMAGE_ATTACHMENT_STENCIL = 0x1,
    LX_VK_IMAGE_ATTACHMENT_COLOR   = 0x2,
    LX_VK_IMAGE_ATTACHMENT_TEXTURE = 0x4
}lx_vk_image_attachment_usage_flags_e;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_vk_image_ref_t lx_vk_image_init(lx_vulkan_device_t* device, VkFormat format,
    lx_size_t width, lx_size_t height, lx_size_t attachment_usages, VkImageTiling tiling, VkImageUsageFlags usage_flags) {
    lx_assert_and_check_return_val(device, lx_null);

    lx_bool_t ok = lx_false;
    lx_vk_image_t* image = lx_null;
    do {
        image = lx_malloc0_type(lx_vk_image_t);
        lx_assert_and_check_break(image);

        image->device = device;

        // is linear?
        lx_bool_t is_linear = VK_IMAGE_TILING_LINEAR == tiling;

        // create image
        VkImageCreateInfo image_create_info = {};
        image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_create_info.pNext = lx_null;
        image_create_info.flags = 0;
        image_create_info.imageType = VK_IMAGE_TYPE_2D;
        image_create_info.format = format,
        image_create_info.extent.width = width;
        image_create_info.extent.height = height;
        image_create_info.extent.depth = 1;
        image_create_info.mipLevels = 1;
        image_create_info.arrayLayers = 1;
        image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_create_info.tiling = tiling;
        image_create_info.usage = usage_flags;
        image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_create_info.queueFamilyIndexCount = 1;
        image_create_info.pQueueFamilyIndices = &device->gpu_familyidx;
        image_create_info.initialLayout = is_linear? VK_IMAGE_LAYOUT_PREINITIALIZED : VK_IMAGE_LAYOUT_UNDEFINED;
        if (vkCreateImage(device->device, &image_create_info, lx_null, &image->image) != VK_SUCCESS) {
            break;
        }

        // allocate image memory
        VkMemoryRequirements mem_reqs;
        vkGetImageMemoryRequirements(device->device, image->image, &mem_reqs);
        image->memory_size = mem_reqs.size;

        VkMemoryAllocateInfo mem_alloc = {};
        mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        mem_alloc.pNext = lx_null;
        mem_alloc.memoryTypeIndex = 0;
        mem_alloc.allocationSize = mem_reqs.size;
        if (!lx_vk_allocate_memory_type_from_properties(device->gpu_memory_properties, mem_reqs.memoryTypeBits,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &mem_alloc.memoryTypeIndex)) {
            break;
        }
        if (vkAllocateMemory(device->device, &mem_alloc, lx_null, &image->memory) != VK_SUCCESS) {
            break;
        }
        if (vkBindImageMemory(device->device, image->image, image->memory, 0) != VK_SUCCESS) {
            break;
        }

        // create framebuffer view
        if (attachment_usages & LX_VK_IMAGE_ATTACHMENT_STENCIL || attachment_usages & LX_VK_IMAGE_ATTACHMENT_COLOR) {
            image->framebuffer_view = lx_vk_image_view_init(device, image->image, format);
            lx_assert_and_check_break(image->framebuffer_view);
        }

        // create texture view
        if (attachment_usages & LX_VK_IMAGE_ATTACHMENT_TEXTURE) {
            image->texture_view = lx_vk_image_view_init(device, image->image, format);
            lx_assert_and_check_break(image->texture_view);
        }

        ok = lx_true;
    } while (0);

    if (!ok && image) {
        lx_vk_image_exit((lx_vk_image_ref_t)image);
        image = lx_null;
    }
    return (lx_vk_image_ref_t)image;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_vk_image_ref_t lx_vk_image_init_msaa(lx_vulkan_device_t* device, VkFormat format, lx_size_t width, lx_size_t height) {
    VkImageUsageFlags usage_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                    VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                    VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    return lx_vk_image_init(device, format, width, height, LX_VK_IMAGE_ATTACHMENT_COLOR, VK_IMAGE_TILING_OPTIMAL, usage_flags);
}

lx_vk_image_ref_t lx_vk_image_init_stencil(lx_vulkan_device_t* device, VkFormat format, lx_size_t width, lx_size_t height) {
    VkImageUsageFlags usage_flags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
                                    VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    return lx_vk_image_init(device, format, width, height, LX_VK_IMAGE_ATTACHMENT_STENCIL, VK_IMAGE_TILING_OPTIMAL, usage_flags);
}

lx_vk_image_ref_t lx_vk_image_init_texture(lx_vulkan_device_t* device, VkFormat format, lx_size_t width, lx_size_t height) {
    VkImageUsageFlags usage_flags = VK_IMAGE_USAGE_SAMPLED_BIT |
                                    VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                    VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    return lx_vk_image_init(device, format, width, height, LX_VK_IMAGE_ATTACHMENT_TEXTURE, VK_IMAGE_TILING_LINEAR, usage_flags);
}

lx_vk_image_ref_t lx_vk_image_init_texture_from_bitmap(lx_vulkan_device_t* device, VkFormat format, lx_bitmap_ref_t bitmap) {
    lx_size_t width = lx_bitmap_width(bitmap);
    lx_size_t height = lx_bitmap_height(bitmap);
    lx_vk_image_t* image = (lx_vk_image_t*)lx_vk_image_init_texture(device, format, width, height);
    if (image) {
        lx_bool_t ok = lx_false;
        do {
            VkImageSubresource subres = {};
            subres.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            subres.mipLevel = 0;
            subres.arrayLayer = 0;
            VkSubresourceLayout layout;
            vkGetImageSubresourceLayout(device->device, image->image, &subres, &layout);

            lx_pointer_t data = lx_null;
            if (vkMapMemory(device->device, image->memory, 0, image->memory_size, 0, &data) != VK_SUCCESS) {
                break;
            }

            // get bitmap data
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
            vkUnmapMemory(device->device, image->memory);
            ok = lx_true;
        } while (0);
        if (!ok) {
            lx_vk_image_exit((lx_vk_image_ref_t)image);
            image = lx_null;
        }
    }
    return (lx_vk_image_ref_t)image;
}

lx_void_t lx_vk_image_exit(lx_vk_image_ref_t self) {
    lx_vk_image_t* image = (lx_vk_image_t*)self;
    if (image) {
        lx_assert(image->device && image->device->device);
        if (image->image != VK_NULL_HANDLE) {
            vkDestroyImage(image->device->device, image->image, lx_null);
            image->image = VK_NULL_HANDLE;
        }
        if (image->memory) {
            vkFreeMemory(image->device->device, image->memory, lx_null);
            image->memory = lx_null;
        }
        if (image->texture_view) {
            lx_vk_image_view_exit(image->texture_view);
            image->texture_view = lx_null;
        }
        if (image->framebuffer_view) {
            lx_vk_image_view_exit(image->framebuffer_view);
            image->framebuffer_view = lx_null;
        }
        lx_free(image);
    }
}

VkImage lx_vk_image(lx_vk_image_ref_t self) {
    lx_vk_image_t* image = (lx_vk_image_t*)self;
    return image? image->image : VK_NULL_HANDLE;
}

lx_vk_image_view_ref_t lx_vk_image_texture_view(lx_vk_image_ref_t self) {
    lx_vk_image_t* image = (lx_vk_image_t*)self;
    return image? image->texture_view : VK_NULL_HANDLE;
}

lx_vk_image_view_ref_t lx_vk_image_framebuffer_view(lx_vk_image_ref_t self) {
    lx_vk_image_t* image = (lx_vk_image_t*)self;
    return image? image->framebuffer_view : VK_NULL_HANDLE;
}
