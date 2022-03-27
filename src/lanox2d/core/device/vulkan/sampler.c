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
 * @file        sampler.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "sampler.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the sampler type
typedef struct lx_vk_sampler_t {
    lx_vulkan_device_t*     device;
    VkSampler               sampler;
}lx_vk_sampler_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_vk_sampler_ref_t lx_vk_sampler_init(lx_vulkan_device_t* device, VkFilter filter, VkSamplerAddressMode addr_mode_x, VkSamplerAddressMode addr_mode_y) {
    lx_assert_and_check_return_val(device, lx_null);

    lx_bool_t ok = lx_false;
    lx_vk_sampler_t* sampler = lx_null;
    do {
        sampler = lx_malloc0_type(lx_vk_sampler_t);
        lx_assert_and_check_break(sampler);

        sampler->device = device;

        VkSamplerCreateInfo createinfo;
        lx_memset(&createinfo, 0, sizeof(VkSamplerCreateInfo));
        createinfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        createinfo.pNext = lx_null;
        createinfo.flags = 0;
        createinfo.magFilter = filter;
        createinfo.minFilter = filter;
        createinfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        createinfo.addressModeU = addr_mode_x;
        createinfo.addressModeV = addr_mode_y;
        createinfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        createinfo.mipLodBias = 0.0f;
        createinfo.anisotropyEnable = VK_FALSE;
        createinfo.maxAnisotropy = 1.0f;
        createinfo.compareEnable = VK_FALSE;
        createinfo.compareOp = VK_COMPARE_OP_NEVER;
        createinfo.minLod = 0.0f;
        createinfo.maxLod = 0.0f; // we do not use mipmap
        createinfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        createinfo.unnormalizedCoordinates = VK_FALSE;
        if (vkCreateSampler(device->device, &createinfo, lx_null, &sampler->sampler) != VK_SUCCESS) {
            break;
        }

        ok = lx_true;
    } while (0);

    if (!ok && sampler) {
        lx_vk_sampler_exit((lx_vk_sampler_ref_t)sampler);
        sampler = lx_null;
    }
    return (lx_vk_sampler_ref_t)sampler;
}

lx_void_t lx_vk_sampler_exit(lx_vk_sampler_ref_t self) {
    lx_vk_sampler_t* sampler = (lx_vk_sampler_t*)self;
    if (sampler) {
        lx_assert(sampler->device && sampler->device->device);
        if (sampler->sampler != VK_NULL_HANDLE) {
            vkDestroySampler(sampler->device->device, sampler->sampler, lx_null);
            sampler->sampler = VK_NULL_HANDLE;
        }
        lx_free(sampler);
    }
}

VkSampler lx_vk_sampler(lx_vk_sampler_ref_t self) {
    lx_vk_sampler_t* sampler = (lx_vk_sampler_t*)self;
    return sampler? sampler->sampler : VK_NULL_HANDLE;
}

