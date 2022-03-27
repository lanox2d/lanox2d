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
 * @file        image_view.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "image_view.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the image_view type
typedef struct lx_vk_image_view_t {
    lx_vulkan_device_t*     device;
    VkImageView             image_view;
}lx_vk_image_view_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_vk_image_view_ref_t lx_vk_image_view_init(lx_vulkan_device_t* device, VkImage image, VkFormat format) {
    lx_assert_and_check_return_val(device, lx_null);

    lx_bool_t ok = lx_false;
    lx_vk_image_view_t* image_view = lx_null;
    do {
        image_view = lx_malloc0_type(lx_vk_image_view_t);
        lx_assert_and_check_break(image_view);

        image_view->device = device;

        // create image view
        VkImageViewCreateInfo createinfo = {};
        createinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createinfo.pNext = lx_null;
        createinfo.flags = 0;
        createinfo.image = image;
        createinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createinfo.format = format;
        createinfo.components.r = VK_COMPONENT_SWIZZLE_R;
        createinfo.components.g = VK_COMPONENT_SWIZZLE_G;
        createinfo.components.b = VK_COMPONENT_SWIZZLE_B;
        createinfo.components.a = VK_COMPONENT_SWIZZLE_A;
        createinfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createinfo.subresourceRange.baseMipLevel = 0;
        createinfo.subresourceRange.levelCount = 1;
        createinfo.subresourceRange.baseArrayLayer = 0;
        createinfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(device->device, &createinfo, lx_null, &image_view->image_view) != VK_SUCCESS) {
            break;
        }

        ok = lx_true;
    } while (0);

    if (!ok && image_view) {
        lx_vk_image_view_exit((lx_vk_image_view_ref_t)image_view);
        image_view = lx_null;
    }
    return (lx_vk_image_view_ref_t)image_view;
}

lx_void_t lx_vk_image_view_exit(lx_vk_image_view_ref_t self) {
    lx_vk_image_view_t* image_view = (lx_vk_image_view_t*)self;
    if (image_view) {
        lx_assert(image_view->device && image_view->device->device);
        if (image_view->image_view != VK_NULL_HANDLE) {
            vkDestroyImageView(image_view->device->device, image_view->image_view, lx_null);
            image_view->image_view = VK_NULL_HANDLE;
        }
        lx_free(image_view);
    }
}

VkImageView lx_vk_image_view(lx_vk_image_view_ref_t self) {
    lx_vk_image_view_t* image_view = (lx_vk_image_view_t*)self;
    return image_view? image_view->image_view : VK_NULL_HANDLE;
}

