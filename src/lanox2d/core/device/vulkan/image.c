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

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the image type
typedef struct lx_vk_image_t {
    lx_vulkan_device_t*     device;
    VkImage                 image;
}lx_vk_image_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_vk_image_ref_t lx_vk_image_init(lx_vulkan_device_t* device, VkFormat format) {
    lx_assert_and_check_return_val(device, lx_null);

    lx_bool_t ok = lx_false;
    lx_vk_image_t* image = lx_null;
    do {
        image = lx_malloc0_type(lx_vk_image_t);
        lx_assert_and_check_break(image);

        image->device = device;

        ok = lx_true;
    } while (0);

    if (!ok && image) {
        lx_vk_image_exit((lx_vk_image_ref_t)image);
        image = lx_null;
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
        lx_free(image);
    }
}

VkImage lx_vk_image(lx_vk_image_ref_t self) {
    lx_vk_image_t* image = (lx_vk_image_t*)self;
    return image? image->image : VK_NULL_HANDLE;
}

