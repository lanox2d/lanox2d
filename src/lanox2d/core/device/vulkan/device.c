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
#ifdef LX_CONFIG_WINDOW_HAVE_GLFW
#   include <GLFW/glfw3.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

static lx_bool_t lx_device_vulkan_draw_lock(lx_device_ref_t self) {
    return lx_true;
}

static lx_void_t lx_device_vulkan_draw_commit(lx_device_ref_t self) {
}

static lx_void_t lx_device_vulkan_draw_clear(lx_device_ref_t self, lx_color_t color) {
}

static lx_void_t lx_device_vulkan_draw_lines(lx_device_ref_t self, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
}

static lx_void_t lx_device_vulkan_draw_points(lx_device_ref_t self, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
}

static lx_void_t lx_device_vulkan_draw_polygon(lx_device_ref_t self, lx_polygon_ref_t polygon, lx_shape_ref_t hint, lx_rect_ref_t bounds) {
}

static lx_void_t lx_device_vulkan_draw_path(lx_device_ref_t self, lx_path_ref_t path) {
}

static lx_bool_t lx_device_vulkan_swapchain_init(lx_vulkan_device_t* device) {
    lx_assert_and_check_return_val(device, lx_false);

    lx_bool_t ok = lx_false;
    VkSurfaceFormatKHR* formats = lx_null;
    do {

        // get the surface capabilities
        VkSurfaceCapabilitiesKHR surface_capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->gpu_device, device->surface, &surface_capabilities);
        device->swapchain.framesize = surface_capabilities.currentExtent;

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
        device->swapchain.format = formats[chosen_format].format;

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
        if (vkCreateSwapchainKHR(device->device, &swapchain_createinfo, lx_null, &device->swapchain.swapchain) != VK_SUCCESS) {
            lx_trace_e("create swapchain failed!");
            break;
        }

        // get the length of the created swapchain
        if (vkGetSwapchainImagesKHR(device->device, device->swapchain.swapchain, &device->swapchain.swapchain_length, lx_null) != VK_SUCCESS) {
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

static lx_void_t lx_device_vulkan_swapchain_exit(lx_vulkan_device_t* device) {
    if (device && device->device && device->swapchain.swapchain) {
        vkDestroySwapchainKHR(device->device, device->swapchain.swapchain, lx_null);
    }
}

static lx_void_t lx_device_vulkan_exit(lx_device_ref_t self) {
    lx_vulkan_device_t* device = (lx_vulkan_device_t*)self;
    if (device) {
        // destroy swapchain
        lx_device_vulkan_swapchain_exit(device);

        // destroy device
        if (device->device) {
            vkDestroyDevice(device->device, lx_null);
            device->device = lx_null;
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

        device->base.draw_clear   = lx_device_vulkan_draw_clear;
        device->base.draw_lines   = lx_device_vulkan_draw_lines;
        device->base.draw_points  = lx_device_vulkan_draw_points;
        device->base.draw_polygon = lx_device_vulkan_draw_polygon;
        device->base.draw_path    = lx_device_vulkan_draw_path;
        device->base.draw_lock    = lx_device_vulkan_draw_lock;
        device->base.draw_commit  = lx_device_vulkan_draw_commit;
        device->base.exit         = lx_device_vulkan_exit;
        device->instance          = (VkInstance)vkinstance;
        device->surface           = (VkSurfaceKHR)vksurface;

        // select gpu device
        device->gpu_device = lx_vk_physical_device_select(device->instance);
        if (!device->gpu_device) {
            lx_trace_e("failed to find a suitable GPU!");
            break;
        }

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


