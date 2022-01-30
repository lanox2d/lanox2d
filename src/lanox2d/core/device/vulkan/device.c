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
 * @file        device.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "device.h"
#include "pipeline.h"
#include "renderer.h"
#include "allocator.h"
#ifdef LX_CONFIG_WINDOW_HAVE_GLFW
#   include <GLFW/glfw3.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef LX_CONFIG_SMALL
#   define LX_DEVICE_VERTEX_BUFFERS_GROW      (64)
#else
#   define LX_DEVICE_VERTEX_BUFFERS_GROW      (128)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

static lx_bool_t lx_device_vulkan_draw_lock(lx_device_ref_t self) {
    return lx_vk_renderer_draw_lock((lx_vulkan_device_t*)self);
}

static lx_void_t lx_device_vulkan_draw_commit(lx_device_ref_t self) {
    lx_vk_renderer_draw_commit((lx_vulkan_device_t*)self);
}

static lx_void_t lx_device_vulkan_draw_clear(lx_device_ref_t self, lx_color_t color) {
    lx_vk_renderer_draw_clear((lx_vulkan_device_t*)self, color);
}

static lx_void_t lx_device_vulkan_draw_lines(lx_device_ref_t self, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
    lx_vk_renderer_draw_lines((lx_vulkan_device_t*)self, points, count, bounds);
}

static lx_void_t lx_device_vulkan_draw_points(lx_device_ref_t self, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
    lx_vk_renderer_draw_points((lx_vulkan_device_t*)self, points, count, bounds);
}

static lx_void_t lx_device_vulkan_draw_polygon(lx_device_ref_t self, lx_polygon_ref_t polygon, lx_shape_ref_t hint, lx_rect_ref_t bounds) {
    lx_vk_renderer_draw_polygon((lx_vulkan_device_t*)self, polygon, hint, bounds);
}

static lx_void_t lx_device_vulkan_draw_path(lx_device_ref_t self, lx_path_ref_t path) {
    lx_vk_renderer_draw_path((lx_vulkan_device_t*)self, path);
}

static lx_bool_t lx_device_vulkan_swapchain_init(lx_vulkan_device_t* device) {
    lx_assert_and_check_return_val(device, lx_false);

    lx_bool_t ok = lx_false;
    VkSurfaceFormatKHR* formats = lx_null;
    do {

        // check surface is supported
        VkBool32 supported = VK_FALSE;
        if (vkGetPhysicalDeviceSurfaceSupportKHR(device->gpu_device, device->gpu_familyidx, device->surface, &supported) == VK_SUCCESS && !supported) {
            lx_trace_e("current surface is not supported!");
            break;
        }

        // get the surface capabilities
        VkSurfaceCapabilitiesKHR surface_capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->gpu_device, device->surface, &surface_capabilities);
        device->framesize = surface_capabilities.currentExtent;

        // query the list of supported surface format and choose one we like
        lx_uint32_t format_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device->gpu_device, device->surface, &format_count, lx_null);
        lx_assert_and_check_break(format_count);

        formats = lx_nalloc0_type(format_count, VkSurfaceFormatKHR);
        lx_assert_and_check_break(formats);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device->gpu_device, device->surface, &format_count, formats);

        // select surface format
        lx_uint32_t chosen_format;
        for (chosen_format = 0; chosen_format < format_count; chosen_format++) {
            VkFormat format = formats[chosen_format].format;
            if (format == VK_FORMAT_R8G8B8A8_UNORM || format == VK_FORMAT_B8G8R8A8_UNORM) {
                break;
            }
        }
        lx_assert_and_check_break(chosen_format < format_count);
        device->format = formats[chosen_format].format;

        // create swapchain
        VkSwapchainCreateInfoKHR swapchain_createinfo = {};
        swapchain_createinfo.sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_createinfo.pNext                 = lx_null;
        swapchain_createinfo.surface               = device->surface;
        swapchain_createinfo.minImageCount         = surface_capabilities.minImageCount;
        swapchain_createinfo.imageFormat           = formats[chosen_format].format;
        swapchain_createinfo.imageColorSpace       = formats[chosen_format].colorSpace;
        swapchain_createinfo.imageExtent           = surface_capabilities.currentExtent;
        swapchain_createinfo.imageArrayLayers      = 1;
        swapchain_createinfo.imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchain_createinfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_createinfo.queueFamilyIndexCount = 1;
        swapchain_createinfo.pQueueFamilyIndices   = &device->gpu_familyidx;
        swapchain_createinfo.preTransform          = surface_capabilities.currentTransform;
#ifdef LX_CONFIG_OS_ANDROID
        swapchain_createinfo.compositeAlpha        = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
#else
        swapchain_createinfo.compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
#endif
        swapchain_createinfo.presentMode           = VK_PRESENT_MODE_FIFO_KHR;
        swapchain_createinfo.clipped               = VK_TRUE;
        swapchain_createinfo.oldSwapchain          = VK_NULL_HANDLE;
        if (vkCreateSwapchainKHR(device->device, &swapchain_createinfo, lx_null, &device->swapchain) != VK_SUCCESS) {
            lx_trace_e("create swapchain failed!");
            break;
        }

        // ok
        ok = lx_true;
    } while (0);

    if (formats) {
        lx_free(formats);
        formats = lx_null;
    }
    return ok;
}

static lx_bool_t lx_device_vulkan_imageviews_init(lx_vulkan_device_t* device) {
    lx_assert_and_check_return_val(device && device->device && device->swapchain, lx_false);

    lx_bool_t ok = lx_false;
    do {
        // get swapchain images
        vkGetSwapchainImagesKHR(device->device, device->swapchain, &device->images_count, lx_null);
        lx_assert_and_check_break(device->images_count);
        lx_uint32_t images_count = device->images_count;

        device->images = lx_nalloc0_type(images_count, VkImage);
        lx_assert_and_check_break(device->images);

        if (vkGetSwapchainImagesKHR(device->device, device->swapchain, &device->images_count, device->images) != VK_SUCCESS) {
            break;
        }

        // create image views
        device->imageviews = lx_nalloc0_type(images_count, VkImageView);
        lx_assert_and_check_break(device->imageviews);

        lx_uint32_t i;
        for (i = 0; i < images_count; i++) {
            VkImageViewCreateInfo view_createinfo = {};
            view_createinfo.sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            view_createinfo.pNext            = lx_null;
            view_createinfo.flags            = 0;
            view_createinfo.image            = device->images[i];
            view_createinfo.viewType         = VK_IMAGE_VIEW_TYPE_2D;
            view_createinfo.format           = device->format;

            VkComponentMapping components = {};
            components.r = VK_COMPONENT_SWIZZLE_R;
            components.g = VK_COMPONENT_SWIZZLE_G;
            components.b = VK_COMPONENT_SWIZZLE_B;
            components.a = VK_COMPONENT_SWIZZLE_A;
            view_createinfo.components       = components;

            VkImageSubresourceRange subresource_range = {};
            subresource_range.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            subresource_range.baseMipLevel   = 0;
            subresource_range.levelCount     = 1;
            subresource_range.baseArrayLayer = 0;
            subresource_range.layerCount     = 1;
            view_createinfo.subresourceRange = subresource_range;
            if (vkCreateImageView(device->device, &view_createinfo, lx_null, &device->imageviews[i]) != VK_SUCCESS) {
                break;
            }
        }
        lx_assert_and_check_break(i == images_count);

        ok = lx_true;
    } while (0);
    return ok;
}

static lx_bool_t lx_device_vulkan_renderpass_init(lx_vulkan_device_t* device) {
    lx_assert_and_check_return_val(device, lx_false);

    VkAttachmentDescription attachment_descriptions = {};
    attachment_descriptions.format         = device->format;
    attachment_descriptions.samples        = VK_SAMPLE_COUNT_1_BIT;
    attachment_descriptions.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment_descriptions.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    attachment_descriptions.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment_descriptions.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment_descriptions.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment_descriptions.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colour_reference = {};
    colour_reference.attachment = 0;
    colour_reference.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass_description = {};
    subpass_description.flags                   = 0;
    subpass_description.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass_description.inputAttachmentCount    = 0;
    subpass_description.pInputAttachments       = lx_null;
    subpass_description.colorAttachmentCount    = 1;
    subpass_description.pColorAttachments       = &colour_reference;
    subpass_description.pResolveAttachments     = lx_null;
    subpass_description.pDepthStencilAttachment = lx_null;
    subpass_description.preserveAttachmentCount = 0;
    subpass_description.pPreserveAttachments    = lx_null;

    VkRenderPassCreateInfo renderpass_createinfo = {};
    renderpass_createinfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderpass_createinfo.pNext           = lx_null;
    renderpass_createinfo.attachmentCount = 1;
    renderpass_createinfo.pAttachments    = &attachment_descriptions;
    renderpass_createinfo.subpassCount    = 1;
    renderpass_createinfo.pSubpasses      = &subpass_description;
    renderpass_createinfo.dependencyCount = 0;
    renderpass_createinfo.pDependencies   = lx_null;

    return vkCreateRenderPass(device->device, &renderpass_createinfo, lx_null, &device->renderpass) == VK_SUCCESS;
}

static lx_bool_t lx_device_vulkan_framebuffers_init(lx_vulkan_device_t* device) {
    lx_assert_and_check_return_val(device && device->device && device->swapchain, lx_false);

    lx_bool_t ok = lx_false;
    do {
        // create framebuffers
        lx_uint32_t images_count = device->images_count;
        device->framebuffers = lx_nalloc0_type(images_count, VkFramebuffer);
        lx_assert_and_check_break(device->framebuffers);

        lx_uint32_t i;
        for (i = 0; i < images_count; i++) {
            VkImageView attachments[1] = {device->imageviews[i]};
            VkFramebufferCreateInfo fb_createinfo = {};
            fb_createinfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            fb_createinfo.pNext           = lx_null,
            fb_createinfo.renderPass      = device->renderpass,
            fb_createinfo.attachmentCount = 1,
            fb_createinfo.pAttachments    = attachments,
            fb_createinfo.width           = (lx_uint32_t)(device->framesize.width),
            fb_createinfo.height          = (lx_uint32_t)(device->framesize.height),
            fb_createinfo.layers          = 1,
            fb_createinfo.attachmentCount = 1;

            if (vkCreateFramebuffer(device->device, &fb_createinfo, lx_null, &device->framebuffers[i]) != VK_SUCCESS) {
                break;
            }
        }

        ok = lx_true;
    } while (0);
    return ok;
}

static lx_bool_t lx_device_vulkan_commandbuffers_init(lx_vulkan_device_t* device) {
    lx_assert_and_check_return_val(device && device->device, lx_false);

    lx_bool_t ok = lx_false;
    do {
        // create a pool of command buffers to allocate command buffer from
        VkCommandPoolCreateInfo pool_createinfo = {};
        pool_createinfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_createinfo.pNext = lx_null;
        pool_createinfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        pool_createinfo.queueFamilyIndex = device->gpu_familyidx;
        if (vkCreateCommandPool(device->device, &pool_createinfo, lx_null, &device->command_pool) != VK_SUCCESS) {
            break;
        }

        // create command buffers
        device->command_buffers_count = device->images_count;
        device->command_buffers = lx_nalloc0_type(device->command_buffers_count, VkCommandBuffer);
        lx_assert_and_check_break(device->command_buffers);

        VkCommandBufferAllocateInfo buffer_createinfo = {};
        buffer_createinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        buffer_createinfo.pNext = lx_null;
        buffer_createinfo.commandPool = device->command_pool;
        buffer_createinfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        buffer_createinfo.commandBufferCount = device->command_buffers_count;
        if (vkAllocateCommandBuffers(device->device, &buffer_createinfo, device->command_buffers) != VK_SUCCESS) {
            break;
        }

        ok = lx_true;
    } while (0);
    return ok;
}

static lx_bool_t lx_device_vulkan_semaphore_init(lx_vulkan_device_t* device) {
    lx_assert_and_check_return_val(device && device->device, lx_false);

    lx_bool_t ok = lx_false;
    do {
        /* we need to create a fence to be able, in the main loop, to wait for our
         * draw command(s) to finish before swapping the framebuffers
         */
        VkFenceCreateInfo fence_createinfo = {};
        fence_createinfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_createinfo.pNext = lx_null;
        fence_createinfo.flags = 0;
        if (vkCreateFence(device->device, &fence_createinfo, lx_null, &device->fence) != VK_SUCCESS) {
            break;
        }

        /* we need to create a semaphore to be able to wait, in the main loop, for our
         * framebuffer to be available for us before drawing.
         */
        VkSemaphoreCreateInfo semaphore_createinfo = {};
        semaphore_createinfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphore_createinfo.pNext = lx_null;
        semaphore_createinfo.flags = 0;
        if (vkCreateSemaphore(device->device, &semaphore_createinfo, lx_null, &device->semaphore) != VK_SUCCESS) {
            break;
        }

        ok = lx_true;
    } while (0);
    return ok;
}

static lx_void_t lx_device_vulkan_vertex_buffer_exit(lx_pointer_t item, lx_pointer_t udata) {
    lx_vk_buffer_t* buffer = (lx_vk_buffer_t*)item;
    lx_vk_allocator_ref_t allocator = (lx_vk_allocator_ref_t)udata;
    if (allocator && buffer) {
        lx_vk_allocator_free(allocator, buffer);
    }
}

static lx_void_t lx_device_vulkan_exit(lx_device_ref_t self) {
    lx_vulkan_device_t* device = (lx_vulkan_device_t*)self;
    if (device) {

        // destroy tessellator
        if (device->tessellator) {
            lx_tessellator_exit(device->tessellator);
            device->tessellator = lx_null;
        }
        if (device->stroker) {
            lx_stroker_exit(device->stroker);
            device->stroker = lx_null;
        }

        // destroy semaphore
        if (device->semaphore) {
            vkDestroySemaphore(device->device, device->semaphore, lx_null);
            device->semaphore = 0;
        }

        // destroy fence
        if (device->fence) {
            vkDestroyFence(device->device, device->fence, lx_null);
            device->fence = 0;
        }

        // destroy pipelines
        lx_uint32_t i;
        for (i = 0; i < lx_arrayn(device->pipelines); i++) {
            if (device->pipelines[i]) {
                lx_vk_pipeline_exit(device->pipelines[i]);
                device->pipelines[i] = lx_null;
            }
        }

        // destroy vertex buffers
        if (device->vertex_buffers) {
            lx_array_exit(device->vertex_buffers);
            device->vertex_buffers = lx_null;
        }

        // destroy buffer allocator
        if (device->allocator_vertex) {
            lx_vk_allocator_exit(device->allocator_vertex);
            device->allocator_vertex = lx_null;
        }
        if (device->allocator_uniform) {
            lx_vk_allocator_exit(device->allocator_uniform);
            device->allocator_uniform = lx_null;
        }

        // destroy framebuffers
        if (device->framebuffers) {
            for (i = 0; i < device->images_count; i++) {
                vkDestroyFramebuffer(device->device, device->framebuffers[i], lx_null);
            }
            lx_free(device->framebuffers);
            device->framebuffers = lx_null;
        }

        // destroy command buffers
        if (device->command_buffers) {
            vkFreeCommandBuffers(device->device, device->command_pool, device->command_buffers_count, device->command_buffers);
            lx_free(device->command_buffers);
            device->command_buffers = lx_null;
        }
        device->command_buffers_count = 0;

        // destroy command pool
        if (device->command_pool) {
            vkDestroyCommandPool(device->device, device->command_pool, lx_null);
            device->command_pool = 0;
        }

        // destroy render pass
        if (device->renderpass) {
            vkDestroyRenderPass(device->device, device->renderpass, lx_null);
            device->renderpass = 0;
        }

        // destroy imageviews
        if (device->imageviews) {
            for (i = 0; i < device->images_count; i++) {
                vkDestroyImageView(device->device, device->imageviews[i], lx_null);
            }
            lx_free(device->imageviews);
            device->imageviews = lx_null;
        }

        // destroy swapchain
        if (device->swapchain) {
            vkDestroySwapchainKHR(device->device, device->swapchain, lx_null);
            device->swapchain = 0;
        }

        // destroy device
        if (device->device) {
            vkDestroyDevice(device->device, lx_null);
            device->device = lx_null;
        }

        // destroy stroker
        if (device->stroker) {
            lx_stroker_exit(device->stroker);
            device->stroker = lx_null;
        }
        lx_free(device);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_device_ref_t lx_device_init_from_vulkan(lx_size_t width, lx_size_t height, lx_cpointer_t vkinstance, lx_cpointer_t vksurface) {
    lx_assert_and_check_return_val(width && height && vkinstance && vksurface, lx_null);

    lx_bool_t           ok = lx_false;
    lx_vulkan_device_t* device = lx_null;
    do {

        // init device
        device = lx_malloc0_type(lx_vulkan_device_t);
        lx_assert_and_check_break(device);

        device->base.draw_lock    = lx_device_vulkan_draw_lock;
        device->base.draw_commit  = lx_device_vulkan_draw_commit;
        device->base.draw_clear   = lx_device_vulkan_draw_clear;
        device->base.draw_lines   = lx_device_vulkan_draw_lines;
        device->base.draw_points  = lx_device_vulkan_draw_points;
        device->base.draw_polygon = lx_device_vulkan_draw_polygon;
        device->base.draw_path    = lx_device_vulkan_draw_path;
        device->base.exit         = lx_device_vulkan_exit;
        device->base.width        = width;
        device->base.height       = height;
        device->instance          = (VkInstance)vkinstance;
        device->surface           = (VkSurfaceKHR)vksurface;

        // init stroker
        device->stroker = lx_stroker_init();
        lx_assert_and_check_break(device->stroker);

        // select gpu device
        device->gpu_device = lx_vk_physical_device_select(device->instance);
        if (!device->gpu_device) {
            lx_trace_e("failed to find a suitable GPU!");
            break;
        }
        vkGetPhysicalDeviceMemoryProperties(device->gpu_device, &device->gpu_memory_properties);

        // init device extensions: VK_KHR_swapchain
        lx_char_t const* swapchain_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
        lx_vk_device_extensions_add(swapchain_extensions, lx_arrayn(swapchain_extensions));

        // init device and queue
        device->device = lx_vk_device_init_gpu_device(device->gpu_device, &device->queue, &device->gpu_familyidx);
        if (!device->device || !device->queue) {
            lx_trace_e("failed to init gpu device!");
            break;
        }

        // init swapchain
        if (!lx_device_vulkan_swapchain_init(device)) {
            lx_trace_e("failed to init swapchain!");
            break;
        }

        // init image views
        if (!lx_device_vulkan_imageviews_init(device)) {
            lx_trace_e("failed to init image views!");
            break;
        }

        // init render pass
        if (!lx_device_vulkan_renderpass_init(device)) {
            lx_trace_e("failed to init render pass!");
            break;
        }

        // init framebuffers
        if (!lx_device_vulkan_framebuffers_init(device)) {
            lx_trace_e("failed to init framebuffers!");
            break;
        }

        // init commandbuffers
        if (!lx_device_vulkan_commandbuffers_init(device)) {
            lx_trace_e("failed to init commandbuffers!");
            break;
        }

        // init semaphore
        if (!lx_device_vulkan_semaphore_init(device)) {
            lx_trace_e("failed to init semaphore!");
            break;
        }

        // init buffer allocator
        device->allocator_vertex = lx_vk_allocator_init(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        device->allocator_uniform = lx_vk_allocator_init(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        lx_assert_and_check_break(device->allocator_vertex);
        lx_assert_and_check_break(device->allocator_uniform);

        // init vertex buffers
        device->vertex_buffers = lx_array_init(LX_DEVICE_VERTEX_BUFFERS_GROW,
            lx_element_mem(sizeof(lx_vk_buffer_t), lx_device_vulkan_vertex_buffer_exit, (lx_pointer_t)device->allocator_vertex));
        lx_assert_and_check_break(device->vertex_buffers);

        // init stroker
        device->stroker = lx_stroker_init();
        lx_assert_and_check_break(device->stroker);

        // init tessellator
        device->tessellator = lx_tessellator_init();
        lx_assert_and_check_break(device->tessellator);

        // init tessellator mode and flags
        lx_tessellator_mode_set(device->tessellator, LX_TESSELLATOR_MODE_TRIANGULATION);
        lx_tessellator_flags_set(device->tessellator, LX_TESSELLATOR_FLAG_AUTOCLOSED);

        // ok
        ok = lx_true;

    } while (0);

    // failed?
    if (!ok && device) {
        lx_device_exit((lx_device_ref_t)device);
        device = lx_null;
    }
    return (lx_device_ref_t)device;
}


