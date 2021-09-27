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

static lx_void_t lx_device_vulkan_exit(lx_device_ref_t self) {
    lx_vulkan_device_t* device = (lx_vulkan_device_t*)self;
    if (device) {
        lx_free(device);
    }
}

static lx_bool_t lx_device_vulkan_is_suitable_device(VkPhysicalDevice device) {

    // get queue family count
    lx_uint32_t queuefamily_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queuefamily_count, lx_null);
    lx_assert_and_check_return_val(queuefamily_count, lx_false);

    // has suitable queue family?
    lx_bool_t found = lx_false;
    VkQueueFamilyProperties* queuefamilies = lx_nalloc0_type(queuefamily_count, VkQueueFamilyProperties);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queuefamily_count, queuefamilies);
    if (queuefamilies) {
        lx_uint32_t i;
        for (i = 0; i < queuefamily_count; i++) {
            VkQueueFamilyProperties* queuefamily = &queuefamilies[i];
            if (queuefamily->queueCount > 0 && queuefamily->queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                found = lx_true;
                break;
            }
        }
        lx_free(queuefamilies);
    }
    return found;
}

static VkPhysicalDevice lx_device_vulkan_select_device(VkInstance instance) {

    // get device count
    lx_uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, lx_null);
    lx_assert_and_check_return_val(device_count, lx_null);

    // select a suitable device
    VkPhysicalDevice selected_device = lx_null;
    VkPhysicalDevice* devices = lx_nalloc0_type(device_count, VkPhysicalDevice);
    if (devices) {
        vkEnumeratePhysicalDevices(instance, &device_count, devices);
        lx_uint32_t i;
        for (i = 0; i < device_count; i++) {
            VkPhysicalDevice device = devices[i];
            if (lx_device_vulkan_is_suitable_device(device)) {
                selected_device = device;
                break;
            }
        }
        lx_free(devices);
    }
    return selected_device;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_device_ref_t lx_device_init_from_vulkan(lx_size_t width, lx_size_t height, lx_cpointer_t devdata) {
    lx_assert_and_check_return_val(width && height && devdata, lx_null);

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
        device->instance          = (VkInstance)devdata;

        // select vulkan device
        device->device = lx_device_vulkan_select_device(device->instance);
        if (!device->device) {
            lx_trace_e("failed to find a suitable GPU!");
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


