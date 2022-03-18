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
 * @file        device.h
 *
 */
#ifndef LX_CORE_DEVICE_VULKAN_DEVICE_H
#define LX_CORE_DEVICE_VULKAN_DEVICE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../tess/tess.h"
#include "vk.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the vulkan device type
typedef struct lx_vulkan_device_t_ {
    lx_device_t                         base;
    lx_window_ref_t                     window;

    // vulkan instance
    VkInstance                          instance;
    VkQueue                             queue;
    VkDevice                            device;
    VkSurfaceKHR                        surface;
    VkRenderPass                        renderpass;
    VkSemaphore                         semaphore;
    VkFence                             fence;

    // gpu device
    VkPhysicalDevice                    gpu_device;
    lx_uint32_t                         gpu_familyidx;
    VkPhysicalDeviceMemoryProperties    gpu_memory_properties;

    // swapchain
    VkSwapchainKHR                      swapchain;
    VkExtent2D                          framesize;
    VkFormat                            format;
    VkImage*                            images;
    VkImageView*                        imageviews;
    VkFramebuffer*                      framebuffers;
    lx_uint32_t                         images_count;
    lx_uint32_t                         imageindex;

    // graphics pipelines
    lx_vk_pipeline_ref_t                pipelines[LX_VK_PIPELINE_TYPE_MAXN];

    // command buffers
    VkCommandPool                       command_pool;
    VkCommandBuffer*                    command_buffers;
    lx_uint32_t                         command_buffers_count;

    // buffer allocators
    lx_vk_buffer_allocator_ref_t        allocator_vertex;
    lx_vk_buffer_allocator_ref_t        allocator_uniform;

    // descriptor sets
    lx_vk_descriptor_sets_ref_t         descriptor_sets_uniform;

    // renderer
    lx_bool_t                           renderer_prepared;
    VkCommandBuffer                     renderer_cmdbuffer;
    VkClearColorValue                   renderer_clear_color;
    lx_array_ref_t                      renderer_descriptor_sets;
    lx_array_ref_t                      vertex_buffers;
    lx_tessellator_ref_t                tessellator;
    lx_stroker_ref_t                    stroker;

}lx_vulkan_device_t;

#endif


